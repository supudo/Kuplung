//
//  LightLamp.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef LightLamp_hpp
#define LightLamp_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class LightLamp {
public:
    ~LightLamp();
    void destroy();
    void init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion);
    void setModel(objModelFace oFace);
    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel);
    objModelFace oFace;

private:
    std::function<void(std::string)> doLogFunc;
    int glslVersion;
    std::string shaderName;

    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices;
    GLuint vboTextureDiffuse;
    GLuint glUniformMVPMatrix;
    GLuint glAttributeVertexPosition, glAttributeTextureCoord, glAttributeVertexNormal, glUniformSampler;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* LightLamp_hpp */
