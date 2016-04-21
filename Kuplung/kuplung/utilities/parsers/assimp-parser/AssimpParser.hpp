//
//  AssimpParser.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef AssimpParser_hpp
#define AssimpParser_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssimpParser {
public:
    ~AssimpParser();
    void init(std::function<void(float)> doProgress);
    objScene parse(FBEntity file);
    void destroy();

    std::vector<float> vertices, normals;

private:
    std::function<void(float)> funcProgress;

    FBEntity file;
    objScene scene;
    Assimp::Importer parser;

    int indexModel = -1, indexFace = -1, indexMaterial = -1, indicesCounter = 0, modelID = 1, faceID = 1, meshCounter = 0;
    std::vector<glm::vec3> vectorsVertices, vectorsNormals;
    std::vector<glm::vec2> vectorsTextureCoordinates;
    std::vector<objMaterialImage> textures_loaded;

    void processNode(aiNode* node, const aiScene* scene);
    objModelFace processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<objMaterialImage> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
    objMaterial findMaterial(std::string materialID);
};

#endif /* AssimpParser_hpp */
