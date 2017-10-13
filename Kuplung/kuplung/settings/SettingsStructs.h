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
#include <vector>

struct Color {
    float r, g, b, w;
};

struct FBEntity {
    bool isFile;
    std::string path, title, extension, modifiedDate, size;
};

typedef enum Importer_ParserType {
    Importer_ParserType_Own = 0,
#ifdef DEF_KuplungSetting_UseCuda
    Importer_ParserType_Own_Cuda = 1,
    Importer_ParserType_Assimp = 2
#else
    Importer_ParserType_Assimp = 1
#endif
} Importer_ParserType;

#ifdef DEF_KuplungSetting_UseCuda
static int Importer_ParserType_Count = 3;
#else
static int Importer_ParserType_Count = 2;
#endif

typedef enum InAppRendererType {
    InAppRendererType_Simple,
    InAppRendererType_Forward,
    InAppRendererType_ForwardShadowMapping,
    InAppRendererType_Deferred
} InAppRendererType;

typedef enum ImportExportFormats {
	ImportExportFormat_UNDEFINED = -1,
	ImportExportFormat_OBJ = 0,
	ImportExportFormat_GLTF = 1,
	ImportExportFormat_STL = 2,
	ImportExportFormat_PLY = 3
} ImportExportFormats;

struct SupportedAssimpFormat {
	std::string id, description, fileExtension;
};

#endif /* SettingsStructs_h */
