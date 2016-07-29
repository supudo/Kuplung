//
//  KuplungMinizip.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "KuplungMinizip.hpp"
#include "kuplung/settings/Settings.h"
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

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

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

#if (!defined(_WIN32)) && (!defined(WIN32)) && (!defined(__APPLE__))
    #ifndef __USE_FILE_OFFSET64
        #define __USE_FILE_OFFSET64
    #endif
    #ifndef __USE_LARGEFILE64
        #define __USE_LARGEFILE64
    #endif
    #ifndef _LARGEFILE64_SOURCE
        #define _LARGEFILE64_SOURCE
    #endif
    #ifndef _FILE_OFFSET_BIT
        #define _FILE_OFFSET_BIT 64
    #endif
#endif

#ifdef __APPLE__
// In darwin and perhaps other BSD variants off_t is a 64 bit value, hence no need for specific 64 bit functions
#define FOPEN_FUNC(filename, mode) fopen(filename, mode)
#define FTELLO_FUNC(stream) ftello(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko(stream, offset, origin)
#else
#define FOPEN_FUNC(filename, mode) fopen64(filename, mode)
#define FTELLO_FUNC(stream) ftello64(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko64(stream, offset, origin)
#endif

KuplungMinizip::~KuplungMinizip(void) {
}

KuplungMinizip::KuplungMinizip() {
}

void KuplungMinizip::CloseZip(void) {
    zipClose(this->zf, NULL);
    this->zf = NULL;
    this->valid = false;
}

void KuplungMinizip::CloseUnzip(void) {
    unzClose(this->zf);
    this->zf = NULL;
    this->valid = false;
}

void KuplungMinizip::Create(std::string zipfilename) {
    this->valid = false;
    zipFile zfi = NULL;
    int fileExists = 0;
    int create = 0;
    int err = ZIP_OK;
    int flags = ZipFlag_Overwrite;

    if (fileExists)
        create = (flags & ZipFlag_Overwrite) ? APPEND_STATUS_CREATE : APPEND_STATUS_ADDINZIP;
    else
        create = APPEND_STATUS_CREATE;

    zfi = zipOpen64(zipfilename.c_str(), create);
    if (err != ZIP_OK) {
        this->valid = true;
        this->zf = zfi;
    }

    this->valid = true;
    this->zf = zfi;
}

void KuplungMinizip::Open(std::string zipfilename) {
    char filename_try[MAXFILENAME + 16] = "";
    zipFile zfi = NULL;

#ifdef USEWIN32IOAPI
    zlib_filefunc64_def ffunc;
#endif

    strncpy(filename_try, zipfilename.c_str(), MAXFILENAME - 1);
    filename_try[MAXFILENAME] = '\0';

#ifdef USEWIN32IOAPI
    fill_win32_filefunc64A(&ffunc);
    if (this->isLargeFile(zipfilename.c_str()))
        zfi = unzOpen2_64(zipfilename.c_str(), &ffunc);
    else
        zfi = unzOpen2(zipfilename.c_str(), &ffunc);
#else
    if (this->isLargeFile(zipfilename.c_str()))
        zfi = unzOpen64(zipfilename.c_str());
    else
        zfi = unzOpen(zipfilename.c_str());
#endif

    if (zfi == NULL) {
        strcat(filename_try, ".zip");
#ifdef USEWIN32IOAPI
        if (this->isLargeFile(zipfilename.c_str()))
            zfi = unzOpen2_64(filename_try, &ffunc);
        else
            zfi = unzOpen2(filename_try, &ffunc);
#else
        if (this->isLargeFile(zipfilename.c_str()))
            zfi = unzOpen64(filename_try);
        else
            zfi = unzOpen(filename_try);
#endif
    }

    this->valid = true;
    this->zf = zfi;
}

int KuplungMinizip::Add(std::string contentPath, std::string zipPath, int flags) {
    int compressLevel = 0;
    int zip64 = 0;
    char * password = NULL;
    zip_fileinfo zi;
    FILE * fin = NULL;
    int size_read;
    int size_buf = 0;
    void* buf = NULL;
    int err;
    unsigned long crcFile = 0;

    if (!this->zf)
        return ZIP_PARAMERROR;

    size_buf = WRITEBUFFERSIZE;
    buf = (void*)malloc(size_buf);
    if (buf == NULL) {
        Settings::Instance()->funcDoLog("[KuplungZip] Error allocating memory!\n");
        return ZIP_INTERNALERROR;
    }

    if (!zipPath.compare(""))
        zipPath.assign(contentPath);

    if (flags & ZipFlag_Faster)
        compressLevel = 1;
    if (flags & ZipFlag_Better)
        compressLevel = 9;

    err = getFileCrc(contentPath.c_str(), buf, size_buf, &crcFile);
    zip64 = isLargeFile(contentPath.c_str());

    err = zipOpenNewFileInZip3_64(this->zf, zipPath.c_str(), &zi,
                                 NULL, 0, NULL, 0, NULL,
                                 (compressLevel != 0) ? Z_DEFLATED : 0,
                                 compressLevel,0,
                                 -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                 password, crcFile, zip64);

    if (err != ZIP_OK)
        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungZip] Error in opening %s in zipfile!\n", contentPath.c_str()));
    else {
        fin = FOPEN_FUNC((char*)contentPath.c_str(), "rb");
        if (fin == NULL) {
            err = ZIP_ERRNO;
            Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungZip] Error in opening %s for reading\n", contentPath.c_str()));
        }
    }

    if (err == ZIP_OK) {
        do {
            err = ZIP_OK;
            size_read = (int)fread(buf, 1, size_buf, fin);
            if (size_read < size_buf) {
                if (feof(fin) == 0) {
                    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungZip] Error in reading %s\n", contentPath.c_str()));
                    err = ZIP_ERRNO;
                }
            }

            if (size_read > 0) {
                err = zipWriteInFileInZip (this->zf, buf, size_read);
                if (err < 0)
                    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungZip] Error in writing %s in the zipfile\n", contentPath.c_str()));
            }
        }
        while ((err == ZIP_OK) && (size_read > 0));
    }

    if (fin)
        fclose(fin);

    if( err == ZIP_OK )
        err = zipCloseFileInZip(this->zf);

    free(buf);

    return ZIP_OK;
}

bool KuplungMinizip::UnzipFile(std::string unzipFolder) {
    unz_global_info global_info;
    if (unzGetGlobalInfo(this->zf, &global_info) != UNZ_OK) {
        Settings::Instance()->funcDoLog("[KuplungMinizip] Could not read file global info!");
        return false;
    }

    char read_buffer[WRITEBUFFERSIZE];

    uLong i;
    for (i=0; i<global_info.number_entry; ++i) {
        unz_file_info file_info;
        char filename[MAXFILENAME];
        if (unzGetCurrentFileInfo(this->zf, &file_info, filename, MAXFILENAME, NULL, 0, NULL, 0 ) != UNZ_OK) {
            Settings::Instance()->funcDoLog("[KuplungMinizip] Could not read file info!");
            return false;
        }

        const size_t filename_length = std::strlen(filename);
        if (filename[filename_length - 1] == '/') {
            printf("dir:%s\n", filename);
            boost::filesystem::path zFolder(filename);
            if (!boost::filesystem::create_directory(zFolder))
                Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungMinizip] Can't create folder - %s", zFolder.c_str()));
        }
        else {
            if (unzOpenCurrentFile(this->zf) != UNZ_OK) {
                Settings::Instance()->funcDoLog("[KuplungMinizip] Could not open file!");
                return false;
            }

            std::string f = unzipFolder + filename;
            FILE *out = fopen(f.c_str(), "wb");
            if (out == NULL) {
                Settings::Instance()->funcDoLog("[KuplungMinizip] Could not open destination file!");
                unzCloseCurrentFile(this->zf);
                return false;
            }

            int error = UNZ_OK;
            do {
                error = unzReadCurrentFile(this->zf, read_buffer, WRITEBUFFERSIZE);
                if (error < 0) {
                    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungMinizip] Error %d!", error));
                    unzCloseCurrentFile(this->zf);
                    return false;
                }

                if (error > 0)
                    fwrite(read_buffer, error, 1, out);
            }
            while (error > 0);

            fclose(out);
        }

        unzCloseCurrentFile(this->zf);

        if ((i + 1) < global_info.number_entry) {
            if (unzGoToNextFile(this->zf) != UNZ_OK) {
                Settings::Instance()->funcDoLog("[KuplungMinizip] Could not read next file!");
                return false;
            }
        }
    }
    return true;
}

int KuplungMinizip::isLargeFile(const char* filename) {
    int largeFile = 0;
    ZPOS64_T pos = 0;
    FILE* pFile = FOPEN_FUNC(filename, "rb");
    if (pFile != NULL) {
        FSEEKO_FUNC(pFile, 0, SEEK_END);
        pos = FTELLO_FUNC(pFile);

        if (pos >= 0xffffffff)
            largeFile = 1;
        fclose(pFile);
    }
    return largeFile;
}

int KuplungMinizip::getFileCrc(const char* filenameinzip, void* buf, unsigned long size_buf, unsigned long* result_crc) {
    unsigned long calculate_crc = 0;
    int err = ZIP_OK;
    FILE * fin = FOPEN_FUNC(filenameinzip, "rb");

    unsigned long size_read = 0;
    unsigned long total_read = 0;
    if (fin == NULL)
        err = ZIP_ERRNO;

     if (err == ZIP_OK) {
         do {
             err = ZIP_OK;
             size_read = (int)fread(buf, 1, size_buf, fin);
             if (size_read < size_buf) {
                 if (feof(fin) == 0) {
                     Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungZip] Error in reading %s\n", filenameinzip));
                     err = ZIP_ERRNO;
                 }
             }

             if (size_read > 0)
                 calculate_crc = crc32(calculate_crc, (const Bytef *)buf, size_read);
             total_read += size_read;
         }
         while ((err == ZIP_OK) && (size_read > 0));
     }

     if (fin)
         fclose(fin);

     *result_crc = calculate_crc;
     return err;
}
