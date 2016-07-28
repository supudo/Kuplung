//
//  KuplungUnzip.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>
    #include <errno.h>
    #include <fcntl.h>

    #ifdef _WIN32
    # include <direct.h>
    # include <io.h>
    #else
    # include <unistd.h>
    # include <utime.h>
    #endif


    #include <minizip/zip.h>
    #include <minizip/unzip.h>

    #define CASESENSITIVITY (0)
    #define WRITEBUFFERSIZE (8192)
    #define MAXFILENAME (256)

    #ifdef _WIN32
    #define USEWIN32IOAPI
    #include "iowin32.h"
    #endif
}

#include "KuplungUnzip.hpp"

#define dir_delimter '/'
#define MAX_FILENAME 512
#define READ_SIZE 8192

void KuplungUnzip::UnzipFile(std::string unzipFolder) {
    // Get info about the zip file
    unz_global_info global_info;
    if (unzGetGlobalInfo(this->zf, &global_info) != UNZ_OK) {
        printf("could not read file global info\n");
        unzClose(this->zf);
    }

    // Buffer to hold data read from the zip file.
    char read_buffer[READ_SIZE];

    // Loop to extract all files
    uLong i;
    for (i=0; i<global_info.number_entry; ++i) {
        // Get info about current file.
        unz_file_info file_info;
        char filename[MAX_FILENAME];
        if (unzGetCurrentFileInfo(this->zf, &file_info, filename, MAX_FILENAME, NULL, 0, NULL, 0 ) != UNZ_OK) {
            printf("could not read file info\n");
            unzClose(this->zf);
        }

        // Check if this entry is a directory or file.
        const size_t filename_length = std::strlen(filename);
        if (filename[filename_length - 1] == dir_delimter) {
            // Entry is a directory, so create it.
            printf("dir:%s\n", filename);
            boost::filesystem::path zFolder(filename);
            if (!boost::filesystem::create_directory(zFolder))
                printf("can't create folder - %s\n", zFolder.c_str());
        }
        else {
            // Entry is a file, so extract it.
            printf("file:%s\n", filename);
            if (unzOpenCurrentFile(this->zf) != UNZ_OK) {
                printf("could not open file\n");
                unzClose(this->zf);
            }

            // Open a file to write out the data.
            std::string f = unzipFolder + filename;
            FILE *out = fopen(f.c_str(), "wb");
            if (out == NULL) {
                printf("could not open destination file\n");
                unzCloseCurrentFile(this->zf);
                unzClose(this->zf);
            }

            int error = UNZ_OK;
            do {
                error = unzReadCurrentFile(this->zf, read_buffer, READ_SIZE);
                if (error < 0) {
                    printf("error %d\n", error);
                    unzCloseCurrentFile( this->zf);
                    unzClose(this->zf);
                }

                // Write data to file.
                if (error > 0)
                    fwrite(read_buffer, error, 1, out); // You should check return of fwrite...
            }
            while (error > 0);

            fclose(out);
        }

        unzCloseCurrentFile(this->zf);

        // Go the the next entry listed in the zip file.
        if ((i + 1) < global_info.number_entry) {
            if (unzGoToNextFile(this->zf) != UNZ_OK) {
                printf("cound not read next file\n");
                unzClose(this->zf);
            }
        }
    }
}

KuplungUnzip::KuplungUnzip(std::string _filename) : filename(_filename), valid(false) {
    this->zf = this->Open(_filename);
    if (!this->zf)
        return;
    valid = true;
}

void KuplungUnzip::Close(void) {
    unzClose(this->zf);
    this->zf = NULL;
    this->valid = false;
}

zipFile KuplungUnzip::Open(std::string zipfilename) {
    char filename_try[MAXFILENAME + 16] = "";
    zipFile zf = NULL;

#ifdef USEWIN32IOAPI
    zlib_filefunc64_def ffunc;
#endif

    strncpy(filename_try, zipfilename.c_str(), MAXFILENAME - 1);
    /* strncpy doesnt append the trailing NULL, of the string is too long. */
    filename_try[MAXFILENAME] = '\0';

#ifdef USEWIN32IOAPI
    fill_win32_filefunc64A(&ffunc);
    zf = unzOpen2_64(zipfilename.c_str(), &ffunc);
#else
    zf = unzOpen64(zipfilename.c_str());
#endif
    if (zf == NULL) {
        strcat(filename_try, ".zip");
#ifdef USEWIN32IOAPI
        zf = unzOpen2_64(filename_try, &ffunc);
#else
        zf = unzOpen64(filename_try);
#endif
    }

    return zf;
}

KuplungUnzip::~KuplungUnzip(void) {
    this->Close();
}

int KuplungUnzip::ItemCount(void) {
    int i = 0;
    unzGoToFirstFile(this->zf);
    do {
        i++;
    }
    while((unzGoToNextFile(zf)) == UNZ_OK);
    return i;
}

void KuplungUnzip::List(void) {
    int i = 0;
    printf("Listing:\n");
    for (i=0 ; i<this->ItemCount(); i++) {
        std::string n = this->NameOfItem(i);
        long sz = this->SizeOfItem(i);
        printf("[%i] : (%ld) %s", i, sz, n.c_str());
    }
}

int KuplungUnzip::IndexOfItem(std::string itemName) {
    int i = 0;
    for (i=0 ; i<this->ItemCount(); i++) {
        std::string n = this->NameOfItem(i);
        if (!n.compare(itemName))
            return i;
    }
    return -1;
}

void KuplungUnzip::ListOfItems(std::vector<std::string>& listing) {
    unzGoToFirstFile(this->zf);
    listing.clear();
    do {
        char filename_inzip[256];
        unz_file_info64 file_info;
        int err;

        err = unzGetCurrentFileInfo64(zf, &file_info, filename_inzip,sizeof(filename_inzip), NULL, 0, NULL, 0);
        if (err != UNZ_OK)
            return;

        std::string s(filename_inzip);
        listing.push_back(s);
    }
    while ((unzGoToNextFile(this->zf)) == UNZ_OK);
}

std::string KuplungUnzip::NameOfItem(int idx) {
    std::string ret("");
    int i = 0;
    unzGoToFirstFile(this->zf);
    do {
        char filename_inzip[256];
        unz_file_info64 file_info;
        int err;
        err = unzGetCurrentFileInfo64(zf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
        if (err != UNZ_OK)
            return ret;
        if (i == idx) {
            ret.assign(filename_inzip);
            return ret;
        }
        i++;
    }
    while ((unzGoToNextFile(this->zf)) == UNZ_OK);
    return ret;
}

long KuplungUnzip::SizeOfItem(int idx) {
    int i = 0;
    unzGoToFirstFile(this->zf);
    do {
        char filename_inzip[256];
        unz_file_info64 file_info;
        int err;

        err = unzGetCurrentFileInfo64(this->zf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
        if (err != UNZ_OK)
            return 0;
        if (i == idx)
            return file_info.uncompressed_size;
        i++;
    }
    while ((unzGoToNextFile(this->zf)) == UNZ_OK);
    return 0;
}

int KuplungUnzip::ExtractToRAM( int idx, char * buf, long bufsz, std::string pw) {
    const char * password = NULL;
    int i = 0;
    int err = 0;

    if (!buf)
        return -1;
    buf[0] = '\0';

    if (pw.compare(""))
        password = pw.c_str();

    unzGoToFirstFile(this->zf);

    do {
        if (i == idx) {
            err = unzOpenCurrentFilePassword(this->zf, password);
            err = unzReadCurrentFile(this->zf, buf, bufsz);
            unzCloseCurrentFile(this->zf);
            return err;
        }
        i++;
    }
    while ((unzGoToNextFile(this->zf)) == UNZ_OK);

    return 0;
}

std::string KuplungUnzip::ExtractToString(int idx, std::string password) {
    long sz = this->SizeOfItem(idx);
    if (sz == 0)
        return "";

    sz++;
    sz *= sizeof(char);
    char * buf = (char *)malloc(sz);

    this->ExtractToRAM(idx, buf, sz, password);
    buf[sz - 1] = '\0';

    std::string ret(buf);
    return ret;
}

