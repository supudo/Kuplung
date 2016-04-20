//
//  SettingsStructs.h
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SettingsStructs_h
#define SettingsStructs_h

#include <string>

struct Color {
    float r, g, b, w;
};

struct FBEntity {
    bool isFile;
    std::string path, title, extension, modifiedDate, size;
};

#endif /* SettingsStructs_h */
