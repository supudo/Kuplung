//
//  MeshLight.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MeshLight_hpp
#define MeshLight_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "utilities/gl/GLIncludes.h"
#include "utilities/parsers/obj-parser/objObjects.h"
#include "utilities/gl/GLUtils.hpp"

class MeshLight {
public:
    ~MeshLight();
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
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboTextureColor, vboIndices;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureShiness, vboTextureDissolve;
    GLuint glUniformMVPMatrix;
    GLuint glAttributeVertexPosition, glAttributeTextureCoord, glAttributeVertexNormal, glUniformSampler;
    GLuint glUniformAlphaBlending, glUniformLight_Position, glUniformLight_Intensity;
    
    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* MeshLight_hpp */
