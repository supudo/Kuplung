//
//  KuplungZip.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

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

void KuplungZip::Close(void) {
    zipClose(this->zf, NULL);
    this->zf = NULL;
    this->valid = false;
}

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

int getFileCrc(const char* filenameinzip, void* buf, unsigned long size_buf, unsigned long* result_crc) {
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
                     printf("error in reading %s\n",filenameinzip);
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

int isLargeFile(const char* filename) {
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

KuplungZip::~KuplungZip(void) {
    this->Close();
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
        printf("Error allocating memory\n");
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
        printf("error in opening %s in zipfile\n", contentPath.c_str());
    else {
        fin = FOPEN_FUNC((char*)contentPath.c_str(), "rb");
        if (fin == NULL) {
            err = ZIP_ERRNO;
            printf("error in opening %s for reading\n", contentPath.c_str());
        }
    }

    if (err == ZIP_OK) {
        do {
            err = ZIP_OK;
            size_read = (int)fread(buf, 1, size_buf, fin);
            if (size_read < size_buf) {
                if (feof(fin) == 0) {
                    printf("error in reading %s\n", contentPath.c_str());
                    err = ZIP_ERRNO;
                }
            }

            if (size_read > 0) {
                err = zipWriteInFileInZip (this->zf, buf, size_read);
                if (err < 0)
                    printf("error in writing %s in the zipfile\n", contentPath.c_str());
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
