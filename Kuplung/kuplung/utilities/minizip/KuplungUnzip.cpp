//
//  KuplungUnzip.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "KuplungUnzip.hpp"
#include "KuplungMinizip.h"
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

#include <minizip/zip.h>
#include <minizip/unzip.h>

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

KuplungUnzip::~KuplungUnzip(void) {
    this->Close();
}

KuplungUnzip::KuplungUnzip(std::string _filename) : filename(_filename), valid(false) {
    this->zf = this->Open(_filename);
    if (!this->zf)
        return;
    this->valid = true;
}

void KuplungUnzip::Close(void) {
    unzClose(this->zf);
    this->zf = NULL;
    this->valid = false;
}

zipFile KuplungUnzip::Open(std::string zipfilename) {
    char filename_try[MAXFILENAME + 16] = "";
    zipFile zfi = NULL;

#ifdef USEWIN32IOAPI
    zlib_filefunc64_def ffunc;
#endif

    strncpy(filename_try, zipfilename.c_str(), MAXFILENAME - 1);
    filename_try[MAXFILENAME] = '\0';

#ifdef USEWIN32IOAPI
    fill_win32_filefunc64A(&ffunc);
    if (isLargeFile(zipfilename.c_str()))
        zfi = unzOpen2_64(zipfilename.c_str(), &ffunc);
    else
        zfi = unzOpen2(zipfilename.c_str(), &ffunc);
#else
    if (isLargeFile(zipfilename.c_str()))
        zfi = unzOpen64(zipfilename.c_str());
    else
        zfi = unzOpen(zipfilename.c_str());
#endif

    if (zfi == NULL) {
        strcat(filename_try, ".zip");
#ifdef USEWIN32IOAPI
        if (isLargeFile(zipfilename.c_str()))
            zfi = unzOpen2_64(filename_try, &ffunc);
        else
            zfi = unzOpen2(filename_try, &ffunc);
#else
        if (isLargeFile(zipfilename.c_str()))
            zfi = unzOpen64(filename_try);
        else
            zfi = unzOpen(filename_try);
#endif
    }

    return zfi;
}

bool KuplungUnzip::UnzipFile(std::string unzipFolder) {
    unz_global_info global_info;
    if (unzGetGlobalInfo(this->zf, &global_info) != UNZ_OK) {
        Settings::Instance()->funcDoLog("[KuplungUnzip] Could not read file global info!");
        return false;
    }

    char read_buffer[WRITEBUFFERSIZE];

    uLong i;
    for (i=0; i<global_info.number_entry; ++i) {
        unz_file_info file_info;
        char filename[MAXFILENAME];
        if (unzGetCurrentFileInfo(this->zf, &file_info, filename, MAXFILENAME, NULL, 0, NULL, 0 ) != UNZ_OK) {
            Settings::Instance()->funcDoLog("[KuplungUnzip] Could not read file info!");
            return false;
        }

        const size_t filename_length = std::strlen(filename);
        if (filename[filename_length - 1] == '/') {
            printf("dir:%s\n", filename);
            boost::filesystem::path zFolder(filename);
            if (!boost::filesystem::create_directory(zFolder))
                Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungUnzip] Can't create folder - %s", zFolder.c_str()));
        }
        else {
            if (unzOpenCurrentFile(this->zf) != UNZ_OK) {
                Settings::Instance()->funcDoLog("[KuplungUnzip] Could not open file!");
                return false;
            }

            std::string f = unzipFolder + filename;
            FILE *out = fopen(f.c_str(), "wb");
            if (out == NULL) {
                Settings::Instance()->funcDoLog("[KuplungUnzip] Could not open destination file!");
                unzCloseCurrentFile(this->zf);
                return false;
            }

            int error = UNZ_OK;
            do {
                error = unzReadCurrentFile(this->zf, read_buffer, WRITEBUFFERSIZE);
                if (error < 0) {
                    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungUnzip] Error %d!", error));
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
                Settings::Instance()->funcDoLog("[KuplungUnzip] Could not read next file!");
                return false;
            }
        }
    }
    return true;
}
