//
//  KuplungUnzip.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef KuplungUnzip_hpp
#define KuplungUnzip_hpp

#include <iostream>
#include <vector>

extern "C" {
    #include <minizip/zip.h>
}

class KuplungUnzip {
public:
    KuplungUnzip(std::string filename);
    ~KuplungUnzip(void);

    bool isValid(void) { return valid; }
    bool UnzipFile(std::string unzipFolder);

    zipFile Open(std::string zipfilename);
    void Close(void);

private:
    std::string filename;
    bool valid;
    zipFile zf;
};

#endif /* KuplungUnzip_hpp */
