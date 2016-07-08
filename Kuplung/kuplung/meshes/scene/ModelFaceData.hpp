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
    void init(MeshModel model, std::string assetsFolder);
    void renderModel();

    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureSpecularExp, vboTextureDissolve, vboTextureBump, vboTextureDisplacement;

private:
    Maths *mathHelper;

    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices, vboTangents, vboBitangents;
};

#endif /* ModelFaceData_hpp */
