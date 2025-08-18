//
//  KuplungMinizip.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef KuplungMinizip_hpp
#define KuplungMinizip_hpp

#ifdef _WIN32

#include "kuplung/settings/Settings.h"

namespace KuplungApp::Utilities::Minizip {

class KuplungMinizip {
public:
    ~KuplungMinizip(void);

    bool isValid(void) { return false; }
    void Create(std::string zipfilename);
    void Open(std::string zipfilename);
    void CloseZip(void);
    void CloseUnzip(void);
    int Add(std::string contentPath, std::string zipPath="", int flags=0);
    bool UnzipFile(std::string const& unzipFolder);
};

}

#else

#include <iostream>
#include <vector>
#include <minizip/zip.h>
#include <minizip/unzip.h>

namespace KuplungApp::Utilities::Minizip {

#define ZipFlag_Overwrite 0x01 /* -o */
#define ZipFlag_Append 0x02 /* -a */
#define ZipFlag_Store 0x04 /* -0 */
#define ZipFlag_Faster 0x08 /* -1 */
#define ZipFlag_Better 0x10 /* -9 */
#define ZipFlag_NoPaths 0x20 /* -j */

class KuplungMinizip {
public:
    ~KuplungMinizip(void);

    bool isValid(void) { return this->valid; }
    void Create(std::string zipfilename);
    void Open(std::string zipfilename);
    void CloseZip(void);
    void CloseUnzip(void);
    int Add(std::string contentPath, std::string zipPath="", int flags=ZipFlag_Better);
    bool UnzipFile(std::string const& unzipFolder);

private:
    std::string filename;
    bool valid = false;
    zipFile zf;

    int isLargeFile(const char* filename);
    int getFileCrc(const char* filenameinzip, void* buf, unsigned long size_buf, unsigned long* result_crc);
};

}

#endif

#endif /* KuplungMinizip_hpp */
