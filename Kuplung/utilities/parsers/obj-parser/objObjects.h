//
//  objObjects.h
// Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

// https://corona-renderer.com/wiki/standalone/mtl

#ifndef objObjects_h
#define objObjects_h

#include <vector>
#include <string>

struct objMaterialImage {
    std::string image;
    std::vector<std::string> commands;
};

struct objMaterialColor {
    float r, g, b, a;
};

struct objMaterial {
    std::string materialID;

    objMaterialColor ambient, diffuse, specular, emission;

    float specularExp;
    float transparency;
    unsigned int illumination;
    float opticalDensity; // shininess

    objMaterialImage textures_ambient;
    objMaterialImage textures_diffuse;
    objMaterialImage textures_specular;
    objMaterialImage textures_specularExp;
    objMaterialImage textures_dissolve;
};

struct objModelFace {
    int ID;
    int ModelID;
    std::string materialID;
    int verticesCount;
    int textureCoordinatesCount;
    int normalsCount;
    int indicesCount;
    objMaterial faceMaterial;

    std::vector<float> vertices;
    std::vector<float> texture_coordinates;
    std::vector<float> normals;
    std::vector<unsigned int> indices;
    std::vector<float> solidColor;
};

struct objModel {
    int ID;
    std::string modelID;
    int verticesCount;
    int textureCoordinatesCount;
    int normalsCount;
    int indicesCount;

    std::vector<objModelFace> faces;
};

struct objScene {
    int totalCountGeometricVertices;
    int totalCountTextureCoordinates;
    int totalCountNormalVertices;
    int totalCountIndices;
    int totalCountFaces;

    std::vector<objModel> models;
    std::vector<objMaterial> materials;

    std::string objFile;
};

#endif /* objObjects_h */
