//
//  objParserCuda.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifdef DEF_KuplungSetting_UseCuda

#include "objParserCuda.hpp"
#include <fstream>
#include <numeric>
#include <sstream>

#include <cuda.h>
#include <builtin_types.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

// ----------------------------------------------------
// ---- Cuda prototype
// ----------------------------------------------------
void parseOBJFile(const char* obj_file_contents,
                  int obj_file_content_length,
                  float *vertices,
                  float *textureCoordinates,
                  float *normals,
                  float *indices);
// ----------------------------------------------------

objParserCuda::~objParserCuda() {
}

void objParserCuda::init(const std::function<void(float)>& doProgress) {
    this->doProgress = doProgress;
    this->objFileLinesCount = 0;
}

std::vector<MeshModel> objParserCuda::parse(const FBEntity& file, const std::vector<std::string>& settings) {
    this->file = file;
    this->models = {};

    std::ifstream ifs(this->file.path.c_str());
    if (!ifs.is_open()) {
        Settings::Instance()->funcDoLog("Cannot open .obj file" + this->file.path + "!");
        ifs.close();
        return {};
    }
    ifs.close();

    std::string objFileContents;
    auto fp = fopen(this->file.path.c_str(), "rb");
    if (!f) {
      Settings::Instance()->funcDoLog("[Kuplung] Could not parse obj file!");
      return false;
    }
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    char *buf = new char[file_size + 1];
    fseek(fp, 0, SEEK_SET);
    fread(buf, sizeof(char), file_size, fp);
    fclose(fp);
    buf[file_size] = '\0';
    objFileContents = buf;
    delete[] buf;

    this->doParse(objFileContents, file_size);

    return this->models;
}

void objParserCuda::doParse(std::string const& obj_file_contents, int length) {
    int deviceCount = 0;
    int cudaDevice = 0;
    char cudaDeviceName [100];

    cuInit(0);
    cuDeviceGetCount(&deviceCount);
    cuDeviceGet(&cudaDevice, 0);
    cuDeviceGetName(cudaDeviceName, 100, cudaDevice);
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[Cuda OBJ Parser] Number of devices: %i\n", deviceCount));
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[Cuda OBJ Parser] Device name: %s\n", cudaDeviceName));

    float *vertices, *textureCoordinates, *normals, *indices;

    parseOBJFile(obj_file_contents.c_str(), length, vertices, textureCoordinates, normals, indices);

    if (vertices) delete vertices;
    if (textureCoordinates) delete textureCoordinates;
    if (normals) delete normals;
    if (indices) delete indices;
}

#endif
