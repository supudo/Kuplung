//
//  ModelFaceData.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFaceData_hpp
#define ModelFaceData_hpp

#include "kuplung/meshes/scene/ModelFaceBase.hpp"

class ModelFaceData: public ModelFaceBase {
public:
    ~ModelFaceData();
    void destroy();
    void initBuffers();
    void init(MeshModel model, std::string assetsFolder);
    void renderModel(bool useTessellation);

    bool vertexSphereVisible, vertexSphereIsSphere, vertexSphereShowWireframes;
    float vertexSphereRadius;
    int vertexSphereSegments;
    glm::vec4 vertexSphereColor;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureSpecularExp, vboTextureDissolve, vboTextureBump, vboTextureDisplacement;

    glm::mat4 matrixGrid;

private:
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices, vboTangents, vboBitangents;
};

#endif /* ModelFaceData_hpp */
