//
//  KuplungZip.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef KuplungZip_hpp
#define KuplungZip_hpp

#include <iostream>

extern "C" {
    #include <minizip/zip.h>
    #include <minizip/unzip.h>
}

#define ZipFlag_Overwrite 0x01 /* -o */
#define ZipFlag_Append 0x02 /* -a */
#define ZipFlag_Store 0x04 /* -0 */
#define ZipFlag_Faster 0x08 /* -1 */
#define ZipFlag_Better 0x10 /* -9 */
#define ZipFlag_NoPaths 0x20 /* -j */

class KuplungZip {
public:
    KuplungZip(std::string filename);
    ~KuplungZip(void);
    zipFile Open(std::string zipfilename);
    void Close(void);
    int Add(std::string contentPath, std::string zipPath="", int flags = ZipFlag_Better);

private:
    std::string filename;
    bool valid;
    zipFile zf;
};

#endif /* KuplungZip_hpp */
