//
//  RenderingSimple.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RenderingSimple.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

RenderingSimple::RenderingSimple() {
}

RenderingSimple::~RenderingSimple() {
    this->destroy();
}

void RenderingSimple::destroy() {
}

bool RenderingSimple::init() {
    this->glUtils = new GLUtils();

    // vertex shader
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    // tessellation control shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.tcs";
    std::string shaderSourceTCS = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_tess_control = shaderSourceTCS.c_str();

    // tessellation evaluation shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.tes";
    std::string shaderSourceTES = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_tess_eval = shaderSourceTES.c_str();

    // geometry shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.geom";
    std::string shaderSourceGeometry = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_geometry = shaderSourceGeometry.c_str();

    // fragment shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, GL_TESS_CONTROL_SHADER, shader_tess_control);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, GL_TESS_EVALUATION_SHADER, shader_tess_eval);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, GL_GEOMETRY_SHADER, shader_geometry);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgram) + "!");
        this->glUtils->printProgramLog(this->shaderProgram);
        return false;
    }
    else {
        glPatchParameteri(GL_PATCH_VERTICES, 3);

        this->glVS_VertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "vs_vertexPosition");
        this->glFS_TextureCoord = this->glUtils->glGetAttribute(this->shaderProgram, "vs_textureCoord");
        this->glVS_VertexNormal = this->glUtils->glGetAttribute(this->shaderProgram, "vs_vertexNormal");
        this->glVS_Tangent = this->glUtils->glGetAttribute(this->shaderProgram, "vs_tangent");
        this->glVS_Bitangent = this->glUtils->glGetAttribute(this->shaderProgram, "vs_bitangent");

        this->glVS_MVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_MVPMatrix");
        this->glVS_WorldMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_WorldMatrix");

        this->glFS_HasSamplerTexture = this->glUtils->glGetUniform(this->shaderProgram, "has_texture");
        this->glFS_SamplerTexture = this->glUtils->glGetUniform(this->shaderProgram, "sampler_texture");

        this->glFS_CameraPosition = this->glUtils->glGetUniform(this->shaderProgram, "fs_cameraPosition");
        this->glFS_UIAmbient = this->glUtils->glGetUniform(this->shaderProgram, "fs_UIAmbient");

        this->glFS_solidSkin_materialColor = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_materialColor");

        this->solidLight = new ModelFace_LightSource_Directional();
        this->solidLight->gl_InUse = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.inUse");
        this->solidLight->gl_Direction = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.direction");
        this->solidLight->gl_Ambient = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.ambient");
        this->solidLight->gl_Diffuse = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.diffuse");
        this->solidLight->gl_Specular = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.specular");
        this->solidLight->gl_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.strengthAmbient");
        this->solidLight->gl_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.strengthDiffuse");
        this->solidLight->gl_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.strengthSpecular");
    }
    return true;
}

void RenderingSimple::render(std::vector<ModelFaceData*> meshModelFaces, ObjectsManager *managerObjects) {
    this->matrixProjection = managerObjects->matrixProjection;
    this->matrixCamera = managerObjects->camera->matrixCamera;
    this->vecCameraPosition = managerObjects->camera->cameraPosition;
    this->uiAmbientLight = managerObjects->Setting_UIAmbientLight;
    this->grid = managerObjects->grid;

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glUseProgram(this->shaderProgram);

    for (size_t i=0; i<meshModelFaces.size(); i++) {
        ModelFaceData *mfd = meshModelFaces[i];

        glm::mat4 matrixModel = glm::mat4(1.0);
        matrixModel *= this->grid->matrixModel;
        // scale
        matrixModel = glm::scale(matrixModel, glm::vec3(mfd->scaleX->point, mfd->scaleY->point, mfd->scaleZ->point));
        // rotate
        matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));
        matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateX->point), glm::vec3(1, 0, 0));
        matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateY->point), glm::vec3(0, 1, 0));
        matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateZ->point), glm::vec3(0, 0, 1));
        matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));
        // translate
        matrixModel = glm::translate(matrixModel, glm::vec3(mfd->positionX->point, mfd->positionY->point, mfd->positionZ->point));

        mfd->matrixProjection = this->matrixProjection;
        mfd->matrixCamera = this->matrixCamera;
        mfd->matrixModel = matrixModel;

        glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        glUniformMatrix4fv(this->glVS_WorldMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));

        glUniform3f(this->glFS_CameraPosition, this->vecCameraPosition.x, this->vecCameraPosition.y, this->vecCameraPosition.z);
        glUniform3f(this->glFS_UIAmbient, this->uiAmbientLight.r, this->uiAmbientLight.g, this->uiAmbientLight.b);

        glUniform3f(this->glFS_solidSkin_materialColor, mfd->solidLightSkin_MaterialColor.r, mfd->solidLightSkin_MaterialColor.g, mfd->solidLightSkin_MaterialColor.b);

        glUniform1i(this->solidLight->gl_InUse, 1);
        glUniform3f(this->solidLight->gl_Direction, 0.0f, 1.0f, 0.0f);
        glUniform3f(this->solidLight->gl_Ambient, managerObjects->SolidLight_Ambient.r, managerObjects->SolidLight_Ambient.g, managerObjects->SolidLight_Ambient.b);
        glUniform3f(this->solidLight->gl_Diffuse, managerObjects->SolidLight_Diffuse.r, managerObjects->SolidLight_Diffuse.g, managerObjects->SolidLight_Diffuse.b);
        glUniform3f(this->solidLight->gl_Specular, managerObjects->SolidLight_Specular.r, managerObjects->SolidLight_Specular.g, managerObjects->SolidLight_Specular.b);
        glUniform1f(this->solidLight->gl_StrengthAmbient, managerObjects->SolidLight_Ambient_Strength);
        glUniform1f(this->solidLight->gl_StrengthDiffuse, managerObjects->SolidLight_Diffuse_Strength);
        glUniform1f(this->solidLight->gl_StrengthSpecular, managerObjects->SolidLight_Specular_Strength);

        if (mfd->vboTextureDiffuse > 0 && mfd->meshModel.ModelMaterial.TextureDiffuse.UseTexture) {
            glUniform1i(this->glFS_HasSamplerTexture, 1);
            glUniform1i(this->glFS_SamplerTexture, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureDiffuse);
        }
        else
            glUniform1i(this->glFS_HasSamplerTexture, 0);

        mfd->renderModel(true);
    }

    glUseProgram(0);
}