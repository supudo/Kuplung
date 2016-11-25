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

typedef enum FileBrowser_ParserType {
    FileBrowser_ParserType_Own1 = 0,
    FileBrowser_ParserType_Own2 = 1,
    FileBrowser_ParserType_Assimp = 2
} FileBrowser_ParserType;

typedef enum InAppRendererType {
    InAppRendererType_Simple,
    InAppRendererType_Forward,
    InAppRendererType_ForwardShadowMapping,
    InAppRendererType_Deferred
} InAppRendererType;

#endif /* SettingsStructs_h */
