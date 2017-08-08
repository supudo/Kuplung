//
//  ExporterGLTF.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 07/08/17.
//  Copyright © 2017 supudo.net. All rights reserved.
//

#include "ExporterGLTF.hpp"
#include <fstream>

namespace KuplungApp { namespace Utilities { namespace Export {

ExporterGLTF::~ExporterGLTF() {
}

void ExporterGLTF::init(const std::function<void(float)>& doProgress) {
    this->funcProgress = doProgress;
    this->addSuffix = false;
#ifdef _WIN32
    this->nlDelimiter = "\r\n";
#elif defined macintosh // OS 9
    this->nlDelimiter = "\r";
#else
    this->nlDelimiter = "\n";
#endif

    this->parserUtils = std::make_unique<ParserUtils>();
}

void ExporterGLTF::exportToFile(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings) {
    this->exportFile = file;
    this->objSettings = settings;
}

void ExporterGLTF::exportGeometry(const std::vector<ModelFaceBase*>& faces) {
}

void ExporterGLTF::exportMaterials(const std::vector<ModelFaceBase*>& faces) {
}

void ExporterGLTF::saveFile(const std::string& fileContents, const std::string& fileName) {
//    printf("--------------------------------------------------------\n");
//    printf("%s\n", fileName.c_str());
//    printf("%s\n", fileContents.c_str());
//    printf("--------------------------------------------------------\n");

    std::ofstream out(fileName);
    out << fileContents;
    out.close();
}

}}}
