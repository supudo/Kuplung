//
//  KuplungMinizip.h
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef KuplungMinizip_h
#define KuplungMinizip_h

#include "kuplung/settings/Settings.h"
#include <stdio.h>
#include <minizip/zip.h>
#include <minizip/unzip.h>

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

inline int isLargeFile(const char* filename) {
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

inline int getFileCrc(const char* filenameinzip, void* buf, unsigned long size_buf, unsigned long* result_crc) {
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

#endif /* KuplungMinizip_h */
