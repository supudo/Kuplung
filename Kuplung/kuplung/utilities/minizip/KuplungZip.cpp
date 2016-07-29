//
//  KuplungZip.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/settings/Settings.h"
#include "KuplungMinizip.h"
#include <iostream>

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

#include "KuplungZip.hpp"

KuplungZip::KuplungZip(std::string _filename) : filename(_filename), valid(false) {
    this->zf = this->Open(_filename);
    if (!this->zf)
        return;
    valid = true;
}

KuplungZip::~KuplungZip(void) {
    this->Close();
}

void KuplungZip::Close(void) {
    zipClose(this->zf, NULL);
    this->zf = NULL;
    this->valid = false;
}

zipFile KuplungZip::Open(std::string zipfilename) {
    zipFile zf = NULL;
    int fileExists = 0;
    int create = 0;
    int err = ZIP_OK;
    int flags = ZipFlag_Overwrite;

    if (fileExists)
        create = (flags & ZipFlag_Overwrite) ? APPEND_STATUS_CREATE : APPEND_STATUS_ADDINZIP;
    else
        create = APPEND_STATUS_CREATE;

    zf = zipOpen64(zipfilename.c_str(), create);
    if ( err != ZIP_OK )
        return zf;
    return zf;
}

int KuplungZip::Add(std::string contentPath, std::string zipPath, int flags) {
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
