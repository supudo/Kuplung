//
//  ModelFaceForward.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFaceForward.hpp"
#include <fstream>
#include <boost/filesystem.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

void ModelFaceForward::destroy() {
    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboNormals);
    glDeleteBuffers(1, &this->vboTextureCoordinates);
    glDeleteBuffers(1, &this->vboIndices);
    glDeleteBuffers(1, &this->vboTangents);
    glDeleteBuffers(1, &this->vboBitangents);

    glDeleteBuffers(1, &this->vboVerticesReflect);
    glDeleteBuffers(1, &this->vboNormalsReflect);
    glDeleteBuffers(1, &this->vboTextureCoordinatesReflect);
    glDeleteBuffers(1, &this->vboIndicesReflect);

//    if (this->vboTextureAmbient > 0)
//        glDeleteBuffers(1, &this->vboTextureAmbient);
//    if (this->vboTextureDiffuse > 0)
//        glDeleteBuffers(1, &this->vboTextureDiffuse);
//    if (this->vboTextureSpecular > 0)
//        glDeleteBuffers(1, &this->vboTextureSpecular);
//    if (this->vboTextureSpecularExp > 0)
//        glDeleteBuffers(1, &this->vboTextureSpecularExp);
//    if (this->vboTextureDissolve > 0)
//        glDeleteBuffers(1, &this->vboTextureDissolve);
//    if (this->vboTextureBump > 0)
//        glDeleteBuffers(1, &this->vboTextureBump);
//    if (this->vboTextureDisplacement > 0)
//        glDeleteBuffers(1, &this->vboTextureDisplacement);

    GLint maxColorAttachments = 1;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    GLint colorAttachment;
    GLenum att = GL_COLOR_ATTACHMENT0;
    for (colorAttachment = 0; colorAttachment < maxColorAttachments; colorAttachment++) {
        att += colorAttachment;
        GLint param;
        GLuint objName;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &param);
        if (GL_RENDERBUFFER == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = ((GLuint*)(&param))[0];
            glDeleteRenderbuffers(1, &objName);
        }
        else if (GL_TEXTURE == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = ((GLuint*)(&param))[0];
            glDeleteTextures(1, &objName);
        }
    }

    glDeleteFramebuffers(1, &this->fboDefault);
    glDeleteFramebuffers(1, &this->fboReflection);

    glDisableVertexAttribArray(this->glVS_VertexPosition);
    glDisableVertexAttribArray(this->glFS_TextureCoord);
    glDisableVertexAttribArray(this->glVS_VertexNormal);
    glDisableVertexAttribArray(this->glVS_Tangent);
    glDisableVertexAttribArray(this->glVS_Bitangent);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderTessControl);
    glDetachShader(this->shaderProgram, this->shaderTessEval);
    glDetachShader(this->shaderProgram, this->shaderGeometry);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderTessControl);
    glDeleteShader(this->shaderTessEval);
    glDeleteShader(this->shaderGeometry);
    glDeleteShader(this->shaderFragment);

    glDetachShader(this->shaderProgramReflection, this->shaderVertexReflection);
    glDetachShader(this->shaderProgramReflection, this->shaderFragmentReflection);
    glDeleteProgram(this->shaderProgramReflection);
    glDeleteShader(this->shaderVertexReflection);
    glDeleteShader(this->shaderFragmentReflection);

    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Initialization

#pragma mark - Public

void ModelFaceForward::init(MeshModel model, std::string assetsFolder) {
    ModelFaceBase::init(model, assetsFolder);
    this->initShaderProgram();
}

bool ModelFaceForward::initShaderProgram() {
    bool success = true;

    // init FBO
    this->fboDefault = 0;
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // vertex shader
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.vert";
    std::string shaderSourceVertex = ModelFaceBase::readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    // tessellation control shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.tcs";
    std::string shaderSourceTCS = ModelFaceBase::readFile(shaderPath.c_str());
    const char *shader_tess_control = shaderSourceTCS.c_str();

    // tessellation evaluation shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.tes";
    std::string shaderSourceTES = ModelFaceBase::readFile(shaderPath.c_str());
    const char *shader_tess_eval = shaderSourceTES.c_str();

    // geometry shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.geom";
    std::string shaderSourceGeometry = ModelFaceBase::readFile(shaderPath.c_str());
    const char *shader_geometry = shaderSourceGeometry.c_str();

    // fragment shader - parts
    std::string shaderSourceFragment;
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face_vars.frag";
    shaderSourceFragment = ModelFaceBase::readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face_effects.frag";
    shaderSourceFragment += ModelFaceBase::readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face_lights.frag";
    shaderSourceFragment += ModelFaceBase::readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face_mapping.frag";
    shaderSourceFragment += ModelFaceBase::readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face_misc.frag";
    shaderSourceFragment += ModelFaceBase::readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.frag";
    shaderSourceFragment += ModelFaceBase::readFile(shaderPath.c_str());

    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderTessControl, GL_TESS_CONTROL_SHADER, shader_tess_control);
    shaderCompilation |= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderTessEval, GL_TESS_EVALUATION_SHADER, shader_tess_eval);
    shaderCompilation |= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderGeometry, GL_GEOMETRY_SHADER, shader_geometry);
    shaderCompilation |= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgram) + "!");
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        glValidateProgram(this->shaderProgram);
        glGetProgramiv(this->shaderProgram, GL_VALIDATE_STATUS, &programSuccess);
        if (!programSuccess) {
            GLchar ErrorLog[1024] = { 0 };
            glGetProgramInfoLog(this->shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
            Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[ModelFaceForward] Invalid shader program: '%s'\n", ErrorLog));
            return success = false;
        }
        else {
            glPatchParameteri(GL_PATCH_VERTICES, 3);

            this->glVS_VertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "vs_vertexPosition");
            this->glFS_TextureCoord = this->glUtils->glGetAttribute(this->shaderProgram, "vs_textureCoord");
            this->glVS_VertexNormal = this->glUtils->glGetAttribute(this->shaderProgram, "vs_vertexNormal");
            this->glVS_Tangent = this->glUtils->glGetAttribute(this->shaderProgram, "vs_tangent");
            this->glVS_Bitangent = this->glUtils->glGetAttribute(this->shaderProgram, "vs_bitangent");

            this->glGS_GeomDisplacementLocation = this->glUtils->glGetUniform(this->shaderProgram, "vs_displacementLocation");
            this->glTCS_UseCullFace = this->glUtils->glGetUniform(this->shaderProgram, "tcs_UseCullFace");
            this->glTCS_UseTessellation = this->glUtils->glGetUniform(this->shaderProgram, "tcs_UseTessellation");
            this->glTCS_TessellationSubdivision = this->glUtils->glGetUniform(this->shaderProgram, "tcs_TessellationSubdivision");

            this->glFS_AlphaBlending = this->glUtils->glGetUniform(this->shaderProgram, "fs_alpha");
            this->glFS_CelShading = this->glUtils->glGetUniform(this->shaderProgram, "fs_celShading");
            this->glFS_CameraPosition = this->glUtils->glGetUniform(this->shaderProgram, "fs_cameraPosition");
            this->glVS_IsBorder = this->glUtils->glGetUniform(this->shaderProgram, "vs_isBorder");
            this->glFS_OutlineColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_outlineColor");
            this->glFS_UIAmbient = this->glUtils->glGetUniform(this->shaderProgram, "fs_UIAmbient");

            this->glVS_MVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_MVPMatrix");
            this->glFS_MMatrix = this->glUtils->glGetUniform(this->shaderProgram, "fs_ModelMatrix");
            this->glVS_WorldMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_WorldMatrix");
            this->glFS_MVMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_MVMatrix");
            this->glVS_NormalMatrix = glGetUniformLocation(this->shaderProgram, "vs_normalMatrix");

            this->glFS_ScreenResX = this->glUtils->glGetUniform(this->shaderProgram, "fs_screenResX");
            this->glFS_ScreenResY = this->glUtils->glGetUniform(this->shaderProgram, "fs_screenResY");

            this->glMaterial_ParallaxMapping = this->glUtils->glGetUniform(this->shaderProgram, "fs_userParallaxMapping");

            this->gl_ModelViewSkin = this->glUtils->glGetUniform(this->shaderProgram, "fs_modelViewSkin");
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

            // light - directional
            for (int i=0; i<this->GLSL_LightSourceNumber_Directional; i++) {
                ModelFace_LightSource_Directional *f = new ModelFace_LightSource_Directional();
                f->gl_InUse = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].inUse").c_str());

                f->gl_Direction = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].direction").c_str());

                f->gl_Ambient = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].ambient").c_str());
                f->gl_Diffuse = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].diffuse").c_str());
                f->gl_Specular = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].specular").c_str());

                f->gl_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].strengthAmbient").c_str());
                f->gl_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].strengthDiffuse").c_str());
                f->gl_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].strengthSpecular").c_str());
                this->mfLights_Directional.push_back(f);
            }

            // light - point
            for (int i=0; i<this->GLSL_LightSourceNumber_Point; i++) {
                ModelFace_LightSource_Point *f = new ModelFace_LightSource_Point();
                f->gl_InUse = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].inUse").c_str());
                f->gl_Position = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].position").c_str());

                f->gl_Constant = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].constant").c_str());
                f->gl_Linear = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].linear").c_str());
                f->gl_Quadratic = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].quadratic").c_str());

                f->gl_Ambient = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].ambient").c_str());
                f->gl_Diffuse = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].diffuse").c_str());
                f->gl_Specular = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].specular").c_str());

                f->gl_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].strengthAmbient").c_str());
                f->gl_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].strengthDiffuse").c_str());
                f->gl_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram, ("pointLights[" + std::to_string(i) + "].strengthSpecular").c_str());
                this->mfLights_Point.push_back(f);
            }

            // light - spot
            for (int i=0; i<this->GLSL_LightSourceNumber_Spot; i++) {
                ModelFace_LightSource_Spot *f = new ModelFace_LightSource_Spot();
                f->gl_InUse = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].inUse").c_str());

                f->gl_Position = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].position").c_str());
                f->gl_Direction = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].direction").c_str());

                f->gl_CutOff = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].cutOff").c_str());
                f->gl_OuterCutOff = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].outerCutOff").c_str());

                f->gl_Constant = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].constant").c_str());
                f->gl_Linear = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].linear").c_str());
                f->gl_Quadratic = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].quadratic").c_str());

                f->gl_Ambient = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].ambient").c_str());
                f->gl_Diffuse = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].diffuse").c_str());
                f->gl_Specular = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].specular").c_str());

                f->gl_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].strengthAmbient").c_str());
                f->gl_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].strengthDiffuse").c_str());
                f->gl_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram, ("spotLights[" + std::to_string(i) + "].strengthSpecular").c_str());
                this->mfLights_Spot.push_back(f);
            }

            // material
            this->glMaterial_Refraction = this->glUtils->glGetUniform(this->shaderProgram, "material.refraction");
            this->glMaterial_SpecularExp = this->glUtils->glGetUniform(this->shaderProgram, "material.specularExp");
            this->glMaterial_IlluminationModel = this->glUtils->glGetUniform(this->shaderProgram, "material.illumination_model");
            this->glMaterial_HeightScale = this->glUtils->glGetUniform(this->shaderProgram, "material.heightScale");

            this->glMaterial_Ambient = this->glUtils->glGetUniform(this->shaderProgram, "material.ambient");
            this->glMaterial_Diffuse = this->glUtils->glGetUniform(this->shaderProgram, "material.diffuse");
            this->glMaterial_Specular = this->glUtils->glGetUniform(this->shaderProgram, "material.specular");
            this->glMaterial_Emission = this->glUtils->glGetUniform(this->shaderProgram, "material.emission");

            this->glMaterial_SamplerAmbient = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_ambient");
            this->glMaterial_SamplerDiffuse = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_diffuse");
            this->glMaterial_SamplerSpecular = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_specular");
            this->glMaterial_SamplerSpecularExp = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_specularExp");
            this->glMaterial_SamplerDissolve = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_dissolve");
            this->glMaterial_SamplerBump = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_bump");
            this->glMaterial_SamplerDisplacement = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_displacement");

            this->glMaterial_HasTextureAmbient = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_ambient");
            this->glMaterial_HasTextureDiffuse = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_diffuse");
            this->glMaterial_HasTextureSpecular = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_specular");
            this->glMaterial_HasTextureSpecularExp = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_specularExp");
            this->glMaterial_HasTextureDissolve = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_dissolve");
            this->glMaterial_HasTextureBump = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_bump");
            this->glMaterial_HasTextureDisplacement = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_displacement");

            // effects - gaussian blur
            this->glEffect_GB_W = this->glUtils->glGetUniform(this->shaderProgram, "effect_GBlur.gauss_w");
            this->glEffect_GB_Radius = this->glUtils->glGetUniform(this->shaderProgram, "effect_GBlur.gauss_radius");
            this->glEffect_GB_Mode = this->glUtils->glGetUniform(this->shaderProgram, "effect_GBlur.gauss_mode");

            // effects - bloom
            this->glEffect_Bloom_doBloom = this->glUtils->glGetUniform(this->shaderProgram, "effect_Bloom.doBloom");
            this->glEffect_Bloom_WeightA = this->glUtils->glGetUniform(this->shaderProgram, "effect_Bloom.bloom_WeightA");
            this->glEffect_Bloom_WeightB = this->glUtils->glGetUniform(this->shaderProgram, "effect_Bloom.bloom_WeightB");
            this->glEffect_Bloom_WeightC = this->glUtils->glGetUniform(this->shaderProgram, "effect_Bloom.bloom_WeightC");
            this->glEffect_Bloom_WeightD = this->glUtils->glGetUniform(this->shaderProgram, "effect_Bloom.bloom_WeightD");
            this->glEffect_Bloom_Vignette = this->glUtils->glGetUniform(this->shaderProgram, "effect_Bloom.bloom_Vignette");
            this->glEffect_Bloom_VignetteAtt = this->glUtils->glGetUniform(this->shaderProgram, "effect_Bloom.bloom_VignetteAtt");
        }
    }

    glBindVertexArray(0);

    success |= this->reflectionInit();

    return success;
}

void ModelFaceForward::initBuffers() {
    glBindVertexArray(this->glVAO);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.vertices.size() * sizeof(glm::vec3), &this->meshModel.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glVS_VertexPosition);
    glVertexAttribPointer(this->glVS_VertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.normals.size() * sizeof(glm::vec3), &this->meshModel.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glVS_VertexNormal);
    glVertexAttribPointer(this->glVS_VertexNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // textures and colors
    if (this->meshModel.texture_coordinates.size() > 0) {
        glGenBuffers(1, &this->vboTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
        glBufferData(GL_ARRAY_BUFFER, this->meshModel.texture_coordinates.size() * sizeof(glm::vec2), &this->meshModel.texture_coordinates[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glFS_TextureCoord);
        glVertexAttribPointer(this->glFS_TextureCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

        // ambient texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureAmbient, objMaterialImageType_Bump, &this->vboTextureAmbient);

        // diffuse texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDiffuse, objMaterialImageType_Bump, &this->vboTextureDiffuse);

        // specular texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureSpecular, objMaterialImageType_Specular, &this->vboTextureSpecular);

        // specular-exp texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureSpecularExp, objMaterialImageType_SpecularExp, &this->vboTextureSpecularExp);

        // dissolve texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDissolve, objMaterialImageType_Dissolve, &this->vboTextureDissolve);

        // bump map texture
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureBump, objMaterialImageType_Bump, &this->vboTextureBump);

        // displacement map texture
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDisplacement, objMaterialImageType_Displacement, &this->vboTextureDisplacement);
    }

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->meshModel.countIndices * sizeof(GLuint), &this->meshModel.indices[0], GL_STATIC_DRAW);

    if (this->meshModel.ModelMaterial.TextureBump.Image != "" &&
        this->meshModel.vertices.size() > 0 &&
        this->meshModel.texture_coordinates.size() > 0 &&
        this->meshModel.normals.size() > 0) {
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        this->mathHelper->computeTangentBasis(this->meshModel.vertices, this->meshModel.texture_coordinates, this->meshModel.normals, tangents, bitangents);

        // tangents
        glGenBuffers(1, &this->vboTangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTangents);
        glBufferData(GL_ARRAY_BUFFER, (int)tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glVS_Tangent);
        glVertexAttribPointer(this->glVS_Tangent, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // bitangents
        glGenBuffers(1, &this->vboBitangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboBitangents);
        glBufferData(GL_ARRAY_BUFFER, (int)bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glVS_Bitangent);
        glVertexAttribPointer(this->glVS_Bitangent, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    }

    glBindVertexArray(0);

    this->initBuffersAgain = false;
}

#pragma mark - Render

void ModelFaceForward::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight) {
    ModelFaceBase::render(matrixProjection, matrixCamera, matrixModel, vecCameraPosition, grid, uiAmbientLight);

    if (this->initBuffersAgain)
        this->initBuffers();

    if (this->grid->actAsMirror) {
        glCullFace(GL_FRONT);
        this->renderReflectFBO();
    }

    if (!this->grid->actAsMirror) {
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glBindFramebuffer(GL_FRAMEBUFFER, this->fboDefault);
        glBindVertexArray(this->glVAO);
    }
    this->renderModel();

    if (this->grid->actAsMirror)
        this->renderMirrorSurface();

    glUseProgram(0);
    glBindVertexArray(0);

    if (Settings::Instance()->ShowBoundingBox && this->so_selectedYn) {
        glm::mat4 matrixBB = glm::mat4(1.0f);
        matrixBB *= this->matrixProjection;
        matrixBB *= this->matrixCamera;
        matrixBB *= this->matrixModel;
        this->boundingBox->render(matrixBB, this->so_outlineColor);
    }
}

void ModelFaceForward::renderModel() {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;
        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixModel));

        glm::mat4 matrixModelView = this->matrixCamera * this->matrixModel;
        glUniformMatrix4fv(this->glFS_MVMatrix, 1, GL_FALSE, glm::value_ptr(matrixModelView));

        glm::mat3 matrixNormal = glm::inverseTranspose(glm::mat3(this->matrixCamera * this->matrixModel));
        glUniformMatrix3fv(this->glVS_NormalMatrix, 1, GL_FALSE, glm::value_ptr(matrixNormal));

        glm::mat4 matrixWorld = this->matrixModel;
        glUniformMatrix4fv(this->glVS_WorldMatrix, 1, GL_FALSE, glm::value_ptr(matrixWorld));

        // blending
        if (this->meshModel.ModelMaterial.Transparency < 1.0 || this->Setting_Alpha < 1.0) {
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            if (this->meshModel.ModelMaterial.Transparency < 1.0)
                glUniform1f(this->glFS_AlphaBlending, this->meshModel.ModelMaterial.Transparency);
            else
                glUniform1f(this->glFS_AlphaBlending, this->Setting_Alpha);
        }
        else {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glUniform1f(this->glFS_AlphaBlending, 1.0);
        }

        // tessellation
        glUniform1i(this->glTCS_UseCullFace, this->Setting_UseCullFace);
        glUniform1i(this->glTCS_UseTessellation, this->Setting_UseTessellation);
        glUniform1i(this->glTCS_TessellationSubdivision, this->Setting_TessellationSubdivision);

        // cel-shading
        glUniform1i(this->glFS_CelShading, this->Setting_CelShading);

        // camera position
        glUniform3f(this->glFS_CameraPosition, this->vecCameraPosition.x, this->vecCameraPosition.y, this->vecCameraPosition.z);

        // screen size
        glUniform1f(this->glFS_ScreenResX, Settings::Instance()->SDL_Window_Width);
        glUniform1f(this->glFS_ScreenResY, Settings::Instance()->SDL_Window_Height);

        // Outline color
        glUniform3f(this->glFS_OutlineColor, this->so_outlineColor.r, this->so_outlineColor.g, this->so_outlineColor.b);

        // ambient color for editor
        glUniform3f(this->glFS_UIAmbient, this->uiAmbientLight.r, this->uiAmbientLight.g, this->uiAmbientLight.b);

        // geometry shader displacement
        glUniform3f(this->glGS_GeomDisplacementLocation, this->displaceX->point, this->displaceY->point, this->displaceZ->point);

        // mapping
        glUniform1i(this->glMaterial_ParallaxMapping, this->Setting_ParallaxMapping);

        // render skin
        glUniform1i(this->gl_ModelViewSkin, this->Setting_ModelViewSkin);
        glUniform3f(this->glFS_solidSkin_materialColor, this->solidLightSkin_MaterialColor.r, this->solidLightSkin_MaterialColor.g, this->solidLightSkin_MaterialColor.b);

        glUniform1i(this->solidLight->gl_InUse, 1);
        glUniform3f(this->solidLight->gl_Direction, 0.0f, 1.0f, 0.0f);
        glUniform3f(this->solidLight->gl_Ambient, this->solidLightSkin_Ambient.r, this->solidLightSkin_Ambient.g, this->solidLightSkin_Ambient.b);
        glUniform3f(this->solidLight->gl_Diffuse, this->solidLightSkin_Diffuse.r, this->solidLightSkin_Diffuse.g, this->solidLightSkin_Diffuse.b);
        glUniform3f(this->solidLight->gl_Specular, this->solidLightSkin_Specular.r, this->solidLightSkin_Specular.g, this->solidLightSkin_Specular.b);
        glUniform1f(this->solidLight->gl_StrengthAmbient, this->solidLightSkin_Ambient_Strength);
        glUniform1f(this->solidLight->gl_StrengthDiffuse, this->solidLightSkin_Diffuse_Strength);
        glUniform1f(this->solidLight->gl_StrengthSpecular, this->solidLightSkin_Specular_Strength);

        // lights
        int lightsCount_Directional = 0;
        int lightsCount_Point = 0;
        int lightsCount_Spot = 0;
        for (int j=0; j<(int)this->lightSources.size(); j++) {
            Light *light = this->lightSources[j];
            switch (light->type) {
                case LightSourceType_Directional: {
                    if (lightsCount_Directional < this->GLSL_LightSourceNumber_Directional) {
                        ModelFace_LightSource_Directional *f = this->mfLights_Directional[lightsCount_Directional];

                        glUniform1i(f->gl_InUse, 1);

                        // light
                        glUniform3f(f->gl_Direction, light->matrixModel[2].x, light->matrixModel[2].y, light->matrixModel[2].z);
                        //printf("%f, %f, %f\n", light->matrixModel[2].x, light->matrixModel[2].y, light->matrixModel[2].z);

                        // color
                        glUniform3f(f->gl_Ambient, light->ambient->color.r, light->ambient->color.g, light->ambient->color.b);
                        glUniform3f(f->gl_Diffuse, light->diffuse->color.r, light->diffuse->color.g, light->diffuse->color.b);
                        glUniform3f(f->gl_Specular, light->specular->color.r, light->specular->color.g, light->specular->color.b);

                        // light factors
                        glUniform1f(f->gl_StrengthAmbient, light->ambient->strength);
                        glUniform1f(f->gl_StrengthDiffuse, light->diffuse->strength);
                        glUniform1f(f->gl_StrengthSpecular, light->specular->strength);

                        lightsCount_Directional += 1;
                    }
                    break;
                }
                case LightSourceType_Point: {
                    if (lightsCount_Point < this->GLSL_LightSourceNumber_Point) {
                        ModelFace_LightSource_Point *f = this->mfLights_Point[lightsCount_Point];

                        glUniform1i(f->gl_InUse, 1);

                        // light
                        glUniform3f(f->gl_Position, light->matrixModel[3].x, light->matrixModel[3].y, light->matrixModel[3].z);

                        // factors
                        glUniform1f(f->gl_Constant, light->lConstant->point);
                        glUniform1f(f->gl_Linear, light->lLinear->point);
                        glUniform1f(f->gl_Quadratic, light->lQuadratic->point);

                        // color
                        glUniform3f(f->gl_Ambient, light->ambient->color.r, light->ambient->color.g, light->ambient->color.b);
                        glUniform3f(f->gl_Diffuse, light->diffuse->color.r, light->diffuse->color.g, light->diffuse->color.b);
                        glUniform3f(f->gl_Specular, light->specular->color.r, light->specular->color.g, light->specular->color.b);

                        // light factors
                        glUniform1f(f->gl_StrengthAmbient, light->ambient->strength);
                        glUniform1f(f->gl_StrengthDiffuse, light->diffuse->strength);
                        glUniform1f(f->gl_StrengthSpecular, light->specular->strength);

                        lightsCount_Point += 1;
                    }
                    break;
                }
                case LightSourceType_Spot: {
                    if (lightsCount_Spot < this->GLSL_LightSourceNumber_Spot) {
                        ModelFace_LightSource_Spot *f = this->mfLights_Spot[lightsCount_Spot];

                        glUniform1i(f->gl_InUse, 1);

                        // light
                        glUniform3f(f->gl_Direction, light->matrixModel[2].x, light->matrixModel[2].y, light->matrixModel[2].z);
                        glUniform3f(f->gl_Position, light->matrixModel[3].x, light->matrixModel[3].y, light->matrixModel[3].z);

                        // cutoff
                        glUniform1f(f->gl_CutOff, glm::cos(glm::radians(light->lCutOff->point)));
                        glUniform1f(f->gl_OuterCutOff, glm::cos(glm::radians(light->lOuterCutOff->point)));

                        // factors
                        glUniform1f(f->gl_Constant, light->lConstant->point);
                        glUniform1f(f->gl_Linear, light->lLinear->point);
                        glUniform1f(f->gl_Quadratic, light->lQuadratic->point);

                        // color
                        glUniform3f(f->gl_Ambient, light->ambient->color.r, light->ambient->color.g, light->ambient->color.b);
                        glUniform3f(f->gl_Diffuse, light->diffuse->color.r, light->diffuse->color.g, light->diffuse->color.b);
                        glUniform3f(f->gl_Specular, light->specular->color.r, light->specular->color.g, light->specular->color.b);

                        // light factors
                        glUniform1f(f->gl_StrengthAmbient, light->ambient->strength);
                        glUniform1f(f->gl_StrengthDiffuse, light->diffuse->strength);
                        glUniform1f(f->gl_StrengthSpecular, light->specular->strength);

                        lightsCount_Spot += 1;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        for (int j=lightsCount_Directional; j<this->GLSL_LightSourceNumber_Directional; j++) {
            glUniform1i(this->mfLights_Directional[j]->gl_InUse, 0);
        }

        for (int j=lightsCount_Point; j<this->GLSL_LightSourceNumber_Point; j++) {
            glUniform1i(this->mfLights_Point[j]->gl_InUse, 0);
        }

        for (int j=lightsCount_Spot; j<this->GLSL_LightSourceNumber_Spot; j++) {
            glUniform1i(this->mfLights_Spot[j]->gl_InUse, 0);
        }

        // material
        glUniform1f(this->glMaterial_Refraction, this->Setting_MaterialRefraction->point);
        glUniform1f(this->glMaterial_SpecularExp, this->Setting_MaterialSpecularExp->point);
        glUniform1i(this->glMaterial_IlluminationModel, this->materialIlluminationModel);
        glUniform1f(this->glMaterial_HeightScale, this->displacementHeightScale->point);
        glUniform3f(this->glMaterial_Ambient, this->materialAmbient->color.r, this->materialAmbient->color.g, this->materialAmbient->color.b);
        glUniform3f(this->glMaterial_Diffuse, this->materialDiffuse->color.r, this->materialDiffuse->color.g, this->materialDiffuse->color.b);
        glUniform3f(this->glMaterial_Specular, this->materialSpecular->color.r, this->materialSpecular->color.g, this->materialSpecular->color.b);
        glUniform3f(this->glMaterial_Emission, this->materialEmission->color.r, this->materialEmission->color.g, this->materialEmission->color.b);

        if (this->vboTextureAmbient > 0 && this->meshModel.ModelMaterial.TextureAmbient.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureAmbient, 1);
            glUniform1i(this->glMaterial_SamplerAmbient, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureAmbient);
        }
        else
            glUniform1i(this->glMaterial_HasTextureAmbient, 0);

        if (this->vboTextureDiffuse > 0 && this->meshModel.ModelMaterial.TextureDiffuse.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureDiffuse, 1);
            glUniform1i(this->glMaterial_SamplerDiffuse, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);
        }
        else
            glUniform1i(this->glMaterial_HasTextureDiffuse, 0);

        if (this->vboTextureSpecular > 0 && this->meshModel.ModelMaterial.TextureSpecular.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureSpecular, 1);
            glUniform1i(this->glMaterial_SamplerSpecular, 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecular);
        }
        else
            glUniform1i(this->glMaterial_HasTextureSpecular, 0);

        if (this->vboTextureSpecularExp > 0 && this->meshModel.ModelMaterial.TextureSpecularExp.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureSpecularExp, 1);
            glUniform1i(this->glMaterial_SamplerSpecularExp, 3);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecularExp);
        }
        else
            glUniform1i(this->glMaterial_HasTextureSpecularExp, 0);

        if (this->vboTextureDissolve > 0 && this->meshModel.ModelMaterial.TextureDissolve.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureDissolve, 1);
            glUniform1i(this->glMaterial_SamplerDissolve, 4);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDissolve);
        }
        else
            glUniform1i(this->glMaterial_HasTextureDissolve, 0);

        if (this->vboTextureBump > 0 && this->meshModel.ModelMaterial.TextureBump.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureBump, 1);
            glUniform1i(this->glMaterial_SamplerBump, 5);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureBump);
        }
        else
            glUniform1i(this->glMaterial_HasTextureBump, 0);

        if (this->vboTextureDisplacement > 0 && this->meshModel.ModelMaterial.TextureDisplacement.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureDisplacement, 1);
            glUniform1i(this->glMaterial_SamplerDisplacement, 6);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDisplacement);
        }
        else
            glUniform1i(this->glMaterial_HasTextureDisplacement, 0);

        // effects - gaussian blur
        glUniform1i(this->glEffect_GB_Mode, this->Effect_GBlur_Mode - 1);
        glUniform1f(this->glEffect_GB_W, this->Effect_GBlur_Width->point);
        glUniform1f(this->glEffect_GB_Radius, this->Effect_GBlur_Radius->point);

        // effects - bloom
        // TODO: Bloom effect
        glUniform1i(this->glEffect_Bloom_doBloom, this->Effect_Bloom_doBloom);
        glUniform1f(this->glEffect_Bloom_WeightA, this->Effect_Bloom_WeightA);
        glUniform1f(this->glEffect_Bloom_WeightB, this->Effect_Bloom_WeightB);
        glUniform1f(this->glEffect_Bloom_WeightC, this->Effect_Bloom_WeightC);
        glUniform1f(this->glEffect_Bloom_WeightD, this->Effect_Bloom_WeightD);
        glUniform1f(this->glEffect_Bloom_Vignette, this->Effect_Bloom_Vignette);
        glUniform1f(this->glEffect_Bloom_VignetteAtt, this->Effect_Bloom_VignetteAtt);

        // outlining
        this->drawOutline();

        // clear texture
//        if (this->vboTextureDiffuse > 0)
//            glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void ModelFaceForward::drawOutline() {
    glm::mat4 mvpMatrix, mtxModel;
    glUniform1f(this->glVS_IsBorder, 0.0);

    if (this->so_selectedYn) {
        glDisable(GL_DEPTH_TEST);
        glUniform1f(this->glVS_IsBorder, 1.0);
        mtxModel = glm::scale(this->matrixModel, glm::vec3(this->so_outlineThickness, this->so_outlineThickness, this->so_outlineThickness));
        mvpMatrix = this->matrixProjection * this->matrixCamera * mtxModel;
        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(mtxModel));
        this->drawOnly();
        glEnable(GL_DEPTH_TEST);
    }

    // model draw
    glUniform1f(this->glVS_IsBorder, 0.0);
    mtxModel = glm::scale(this->matrixModel, glm::vec3(1.0, 1.0, 1.0));
    mvpMatrix = this->matrixProjection * this->matrixCamera * mtxModel;
    glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(mtxModel));
    this->drawOnly();
}

void ModelFaceForward::drawOnly() {
    if (this->Setting_Wireframe || Settings::Instance()->wireframesMode || this->Setting_ModelViewSkin == ViewModelSkin_Wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //glDrawElements((this->Setting_UseTessellation ? GL_PATCHES : GL_TRIANGLES), this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);
    glDrawElements(GL_PATCHES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);

    if (this->Setting_Wireframe || Settings::Instance()->wireframesMode || this->Setting_ModelViewSkin == ViewModelSkin_Wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

#pragma mark - Reflection

bool ModelFaceForward::reflectionInit() {
    this->reflectWidth = 512;
    this->reflectHeight = 512;

    glGenVertexArrays(1, &this->glVAOReflect);
    glBindVertexArray(this->glVAOReflect);

    // vertices
    glGenBuffers(1, &this->vboVerticesReflect);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVerticesReflect);
    glBufferData(GL_ARRAY_BUFFER, this->dataVertices.size() * sizeof(GLfloat), &this->dataVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormalsReflect);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormalsReflect);
    glBufferData(GL_ARRAY_BUFFER, this->dataNormals.size() * sizeof(GLfloat), &this->dataNormals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // texture coordinates
    glGenBuffers(1, &this->vboTextureCoordinatesReflect);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinatesReflect);
    glBufferData(GL_ARRAY_BUFFER, this->dataTexCoords.size() * sizeof(GLfloat), &this->dataTexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

    // indices
    glGenBuffers(1, &this->vboIndicesReflect);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndicesReflect);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->dataIndices.size() * sizeof(GLuint), &this->dataIndices[0], GL_STATIC_DRAW);

    // texture
    GLuint colorTexture;
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->reflectWidth, this->reflectHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // depth buffer
    GLuint depthRenderbuffer;
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, this->reflectWidth, this->reflectHeight);

    // frame buffer
    glGenFramebuffers(1, &this->fboReflection);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fboReflection);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Settings::Instance()->funcDoLog("Failed to make complete framebuffer object " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, this->fboReflection);

    // reflection texture
    GLint iReflectTexName;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &iReflectTexName);
    this->reflectTexName = ((GLuint*)(&iReflectTexName))[0];

    // shaders
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/reflection.vert";
    std::string shaderSourceVertex = ModelFaceBase::readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/reflection.frag";
    std::string shaderSourceFragment = ModelFaceBase::readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgramReflection = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileAndAttachShader(this->shaderProgramReflection, this->shaderVertexReflection, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileAndAttachShader(this->shaderProgramReflection, this->shaderFragmentReflection, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgramReflection);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgramReflection, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking reflection program " + std::to_string(this->shaderProgramReflection) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgramReflection);
        return false;
    }
    else {
        this->reflectModelViewUniformIdx = glGetUniformLocation(this->shaderProgramReflection, "vs_modelViewMatrix");
        this->reflectProjectionUniformIdx = glGetUniformLocation(this->shaderProgramReflection, "vs_modelViewProjectionMatrix");
        this->reflectNormalMatrixUniformIdx = glGetUniformLocation(this->shaderProgramReflection, "vs_normalMatrix");
    }

    return true;
}

void ModelFaceForward::renderReflectFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fboReflection);
    glViewport(0, 0, this->reflectWidth, this->reflectHeight);

    glm::mat4 mtxModel = this->matrixModel;

    this->matrixModel = glm::scale(this->matrixModel, glm::vec3(1, -1, -1));

    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));

    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 2.5, -2.5));

    glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;

    glUseProgram(this->shaderProgram);

    glUniformMatrix4fv(this->reflectProjectionUniformIdx, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    glBindVertexArray(this->glVAO);
    glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);
    glCullFace(GL_FRONT);
    glDrawElements(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);

    this->matrixModel = mtxModel;

    glBindFramebuffer(GL_FRAMEBUFFER, this->fboDefault);
    glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
}

void ModelFaceForward::renderMirrorSurface() {
    glBindVertexArray(this->glVAOReflect);
    glUseProgram(this->shaderProgramReflection);

    glm::mat4 mtxModel = this->matrixModel;
    mtxModel *= this->grid->matrixModel;

    // rotate
    mtxModel = glm::translate(mtxModel, glm::vec3(0, 0, 0));
    mtxModel = glm::rotate(mtxModel, glm::radians(this->grid->mirrorSurface->rotateX), glm::vec3(1, 0, 0));
    mtxModel = glm::rotate(mtxModel, glm::radians(this->grid->mirrorSurface->rotateY), glm::vec3(0, 1, 0));
    mtxModel = glm::rotate(mtxModel, glm::radians(this->grid->mirrorSurface->rotateZ), glm::vec3(0, 0, 1));
    mtxModel = glm::translate(mtxModel, glm::vec3(0, 0, 0));

    // translate - glm::vec3(0.0, -5.0, -2.5)
    mtxModel = glm::translate(mtxModel, glm::vec3(this->grid->mirrorSurface->translateX, this->grid->mirrorSurface->translateY, this->grid->mirrorSurface->translateZ));

    glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * mtxModel;
    //mvpMatrix = this->grid->matrixProjection * this->grid->matrixCamera * this->grid->matrixModel;

    glUniformMatrix4fv(this->reflectModelViewUniformIdx, 1, GL_FALSE, glm::value_ptr(mtxModel));
    glUniformMatrix4fv(this->reflectProjectionUniformIdx, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    // The inverse transpose of the top left 3x3 portion of the modelview matrix
    glm::mat3 matrixNormal = glm::mat3(mtxModel);
    glUniformMatrix3fv(this->reflectNormalMatrixUniformIdx, 1, GL_FALSE, glm::value_ptr(matrixNormal));

    glBindTexture(GL_TEXTURE_2D, this->reflectTexName);
    glGenerateMipmap(GL_TEXTURE_2D);
    glDrawElements(GL_TRIANGLES, (int)this->dataIndices.size(), GL_UNSIGNED_INT, nullptr);

    // rendering the grid again - this fixes the z-depth for transparency with the objects and the grid intersecting them ....
    // TODO: find the proper way to handle this!!!
    this->grid->render(this->matrixProjection, this->matrixCamera, false);
}
