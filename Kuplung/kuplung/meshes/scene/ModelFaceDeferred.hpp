//
//  ModelFaceDeferred.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFaceDeferred_hpp
#define ModelFaceDeferred_hpp

#include "kuplung/meshes/scene/ModelFaceBase.hpp"

class ModelFaceDeferred: public ModelFaceBase {
public:
    void init(MeshModel model, std::string assetsFolder);
    void renderModel(GLuint shader);

private:
    GLUtils *glUtils;
    Maths *mathHelper;

    GLuint glVAO, vboVertices, vboNormals, vboTextureCoordinates, vboIndices, vboTangents, vboBitangents;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureSpecularExp, vboTextureDissolve, vboTextureBump, vboTextureDisplacement;

    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec;

    const GLuint NR_LIGHTS = 32;
    std::vector<glm::vec3> objectPositions;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    GLuint quadVAO = 0;
    GLuint quadVBO;
    void renderQuad();

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;
    void renderCube();

    void loadTexture(std::string assetsFolder, MeshMaterialTextureImage materialImage, objMaterialImageType type, GLuint* vboObject);
    std::string readFile(const char *filePath);
};

#endif /* ModelFaceDeferred_hpp */
