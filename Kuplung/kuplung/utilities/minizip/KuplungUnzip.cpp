//
//  KuplungUnzip.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/settings/Settings.h"
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



KuplungUnzip::KuplungUnzip(std::string _filename) : filename(_filename), valid(false) {
    this->zf = this->Open(_filename);
    if (!this->zf)
        return;
    valid = true;
}

KuplungUnzip::~KuplungUnzip(void) {
    this->Close();
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

bool KuplungUnzip::UnzipFile(std::string unzipFolder) {
    unz_global_info global_info;
    if (unzGetGlobalInfo(this->zf, &global_info) != UNZ_OK) {
        Settings::Instance()->funcDoLog("[KuplungUnzip] Could not read file global info!\n");
        return false;
    }

    char read_buffer[WRITEBUFFERSIZE];
    uLong i;
    for (i=0; i<global_info.number_entry; ++i) {
        unz_file_info file_info;
        char filename[MAXFILENAME];
        if (unzGetCurrentFileInfo(this->zf, &file_info, filename, MAXFILENAME, NULL, 0, NULL, 0 ) != UNZ_OK) {
            Settings::Instance()->funcDoLog("[KuplungUnzip] Could not read file info!\n");
            return false;
        }

        const size_t filename_length = std::strlen(filename);
        if (filename[filename_length - 1] == '/') {
            boost::filesystem::path zFolder(filename);
            if (!boost::filesystem::create_directory(zFolder)) {
                Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungUnzip] Can't create folder - %s\n", zFolder.c_str()));
                return false;
            }
        }
        else {
            if (unzOpenCurrentFile(this->zf) != UNZ_OK) {
                Settings::Instance()->funcDoLog("[KuplungUnzip] Could not open file!\n");
                return false;
            }

            std::string f = unzipFolder + filename;
            FILE *out = fopen(f.c_str(), "wb");
            if (out == NULL) {
                Settings::Instance()->funcDoLog("[KuplungUnzip] Could not open destination file!\n");
                unzCloseCurrentFile(this->zf);
                return false;
            }

            int error = UNZ_OK;
            do {
                error = unzReadCurrentFile(this->zf, read_buffer, WRITEBUFFERSIZE);
                if (error < 0) {
                    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungUnzip] Error Occured %d\n", error));
                    unzCloseCurrentFile( this->zf);
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
                Settings::Instance()->funcDoLog("[KuplungUnzip] Could not read next file\n");
                return false;
            }
        }
    }
    return true;
}
