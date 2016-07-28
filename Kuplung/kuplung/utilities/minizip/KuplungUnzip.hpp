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
    void UnzipFile(std::string unzipFolder);

    zipFile Open(std::string zipfilename);
    void Close(void);

//    void Add(std::string contentPath, std::string zipPath, int flags);

    void List(void);
    int ItemCount(void);
    int IndexOfItem(std::string itemName);

    void ListOfItems(std::vector<std::string>& listing);
    std::string NameOfItem(int idx);
    long SizeOfItem(int idx);
    int ExtractToRAM(int idx, char * buf, long bufsz, std::string pw="");
    std::string ExtractToString(int idx, std::string password="");

    long SizeOfItem(std::string itemName) { return this->SizeOfItem(this->IndexOfItem(itemName)); }
    std::string ExtractToString(std::string itemName, std::string password="") { return this->ExtractToString(this->IndexOfItem(itemName), password); }

private:
    std::string filename;
    bool valid;
    zipFile zf;
};

#endif /* KuplungUnzip_hpp */
