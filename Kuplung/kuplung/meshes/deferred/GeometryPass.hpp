//
//  GeometryPass.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GeometryPass_hpp
#define GeometryPass_hpp

#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLUtils.hpp"
#include <glm/common.hpp>

class GeometryPass {
public:
    GeometryPass();

    bool init();
    void setWVP(const glm::mat4& WVP);
    void setWorldMatrix(const glm::mat4& WVP);
    void setColorTextureUnit(int textureUnit);
    void enable();

private:
    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;

    GLint glVS_MVPMatrix, glVS_WorldMatrix, glFS_DiffuseSampler;

    GLUtils *glUtils;

    std::string readFile(const char *filePath);
};

#endif /* GeometryPass_hpp */
