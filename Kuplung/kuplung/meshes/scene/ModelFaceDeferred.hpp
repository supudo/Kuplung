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
    Maths *mathHelper;

    GLuint glVAO, vboVertices, vboNormals, vboTextureCoordinates, vboIndices, vboTangents, vboBitangents;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureSpecularExp, vboTextureDissolve, vboTextureBump, vboTextureDisplacement;
};

#endif /* ModelFaceDeferred_hpp */
