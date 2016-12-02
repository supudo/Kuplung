//
//  RenderingForwardShadowMapping.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RenderingForwardShadowMapping.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "kuplung/utilities/imgui/imguizmo/ImGuizmo.h"
#include "kuplung/utilities/stb/stb_image_write.h"

RenderingForwardShadowMapping::RenderingForwardShadowMapping(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
    this->managerObjects = managerObjects;
    this->GLSL_LightSourceNumber_Directional = 0;
    this->GLSL_LightSourceNumber_Point = 0;
    this->GLSL_LightSourceNumber_Spot = 0;
}

RenderingForwardShadowMapping::~RenderingForwardShadowMapping() {
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

    glDeleteProgram(this->shaderProgram);

    glDetachShader(this->shaderProgramShadows, this->shaderShadowsVertex);
    glDetachShader(this->shaderProgramShadows, this->shaderShadowsFragment);
    glDeleteProgram(this->shaderProgramShadows);

    glDeleteShader(this->shaderShadowsVertex);
    glDeleteShader(this->shaderShadowsFragment);

    for (size_t i=0; i<this->mfLights_Directional.size(); i++) {
        delete this->mfLights_Directional[i];
    }
    for (size_t i=0; i<this->mfLights_Point.size(); i++) {
        delete this->mfLights_Point[i];
    }
    for (size_t i=0; i<this->mfLights_Spot.size(); i++) {
        delete this->mfLights_Spot[i];
    }
    this->glUtils.reset();
}

bool RenderingForwardShadowMapping::init() {
    this->glUtils = std::make_unique<GLUtils>();

    this->GLSL_LightSourceNumber_Directional = 8;
    this->GLSL_LightSourceNumber_Point = 4;
    this->GLSL_LightSourceNumber_Spot = 4;

    bool success = true;
    success &= this->initShaderProgram();
    return success;
}

bool RenderingForwardShadowMapping::initShaderProgram() {
    bool success = true;

    // vertex shader
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    // tessellation control shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.tcs";
    std::string shaderSourceTCS = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_tess_control = shaderSourceTCS.c_str();

    // tessellation evaluation shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.tes";
    std::string shaderSourceTES = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_tess_eval = shaderSourceTES.c_str();

    // geometry shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.geom";
    std::string shaderSourceGeometry = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_geometry = shaderSourceGeometry.c_str();

    // fragment shader - parts
    std::string shaderSourceFragment;
    std::vector<std::string> fragFiles = {
        "vars",
        "effects",
        "lights",
        "shadow_mapping",
        "mapping",
        "misc"
    };
    for (size_t i=0; i<fragFiles.size(); i++) {
        shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face_" + fragFiles[i] + ".frag";
        shaderSourceFragment += this->glUtils->readFile(shaderPath.c_str());
    }
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.frag";
    shaderSourceFragment += this->glUtils->readFile(shaderPath.c_str());

    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= this->glUtils->compileShader(this->shaderProgram, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation &= this->glUtils->compileShader(this->shaderProgram, GL_TESS_CONTROL_SHADER, shader_tess_control);
    shaderCompilation &= this->glUtils->compileShader(this->shaderProgram, GL_TESS_EVALUATION_SHADER, shader_tess_eval);
    shaderCompilation &= this->glUtils->compileShader(this->shaderProgram, GL_GEOMETRY_SHADER, shader_geometry);
    shaderCompilation &= this->glUtils->compileShader(this->shaderProgram, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[RenderingForwardShadowMapping - initShaders] Error linking program " + std::to_string(this->shaderProgram) + "!");
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        glPatchParameteri(GL_PATCH_VERTICES, 3);

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
        this->glFS_GammaCoeficient = this->glUtils->glGetUniform(this->shaderProgram, "fs_gammaCoeficient");

        this->glFS_planeClose = this->glUtils->glGetUniform(this->shaderProgram, "fs_planeClose");
        this->glFS_planeFar = this->glUtils->glGetUniform(this->shaderProgram, "fs_planeFar");
        this->glFS_showDepthColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_showDepthColor");
        this->glFS_ShadowPass = this->glUtils->glGetUniform(this->shaderProgram, "fs_shadowPass");
        this->glFS_DebugShadowTexture = this->glUtils->glGetUniform(this->shaderProgram, "fs_debugShadowTexture");

        this->glVS_MVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_MVPMatrix");
        this->glFS_MMatrix = this->glUtils->glGetUniform(this->shaderProgram, "fs_ModelMatrix");
        this->glVS_WorldMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_WorldMatrix");
        this->glFS_MVMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_MVMatrix");
        this->glVS_NormalMatrix = glGetUniformLocation(this->shaderProgram, "vs_normalMatrix");

        this->glFS_ScreenResX = this->glUtils->glGetUniform(this->shaderProgram, "fs_screenResX");
        this->glFS_ScreenResY = this->glUtils->glGetUniform(this->shaderProgram, "fs_screenResY");

        this->glMaterial_ParallaxMapping = this->glUtils->glGetUniform(this->shaderProgram, "fs_userParallaxMapping");

        this->glVS_shadowModelMatrix = this->glUtils->glGetUniform(this->shaderProgram, "shadow_model");
        this->glFS_showShadows = this->glUtils->glGetUniform(this->shaderProgram, "fs_showShadows");
        this->glVS_LightSpaceMatrix = this->glUtils->glGetUniform(this->shaderProgram, "shadow_lightSpaceMatrix");
        this->glFS_SamplerShadowMap = this->glUtils->glGetUniform(this->shaderProgram, "sampler_shadowMap");

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

        // effects - tone mapping
        this->glEffect_ToneMapping_ACESFilmRec2020 = this->glUtils->glGetUniform(this->shaderProgram, "fs_ACESFilmRec2020");
    }

    success &= this->initShadows();
    success &= this->initShadowsDepth();

    return success;
}

bool RenderingForwardShadowMapping::initShadowsDepth() {
    bool success = true;
    success &= this->initShadowsDepthShader();
    return success;
}

bool RenderingForwardShadowMapping::initShadowsDepthShader() {
    bool result = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/shadows_debug_quad.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/shadows_debug_quad_depth.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgramDepth = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgramDepth, this->shaderDepthVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgramDepth, this->shaderDepthFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgramDepth);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgramDepth, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgramDepth) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgramDepth);
        return false;
    }
    else {
        this->glDepth_Plane_Close = this->glUtils->glGetUniformNoWarning(this->shaderProgramDepth, "near_plane");
        this->glDepth_Plane_Far = this->glUtils->glGetUniformNoWarning(this->shaderProgramDepth, "far_plane");
        this->glDepth_SamplerTexture = this->glUtils->glGetUniformNoWarning(this->shaderProgramDepth, "depthMap");
    }

    return result;
}

bool RenderingForwardShadowMapping::initShadows() {
    bool success = true;
    success &= this->initShadowsShader();
    success &= this->initShadowsBuffers();
    return success;
}

bool RenderingForwardShadowMapping::initShadowsShader() {
    bool result = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/shadow_mapping_depth.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/shadow_mapping_depth.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgramShadows = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgramShadows, this->shaderShadowsVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgramShadows, this->shaderShadowsFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgramShadows);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgramShadows, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgramShadows) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgramShadows);
        return false;
    }
    else {
        this->glShadow_ModelMatrix = this->glUtils->glGetUniformNoWarning(this->shaderProgramShadows, "shadow_model");
        this->glShadow_LightSpaceMatrix = this->glUtils->glGetUniformNoWarning(this->shaderProgramShadows, "shadow_lightSpaceMatrix");
    }

    return result;
}

bool RenderingForwardShadowMapping::initShadowsBuffers() {
    glGenFramebuffers(1, &this->fboDepthMap);

    glGenTextures(1, &this->vboDepthMap);
    glBindTexture(GL_TEXTURE_2D, this->vboDepthMap);

    int smapWidth = 1024;//Settings::Instance()->SDL_Window_Width;
    int smapHeight = 1024;//Settings::Instance()->SDL_Window_Height;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, smapWidth, smapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fboDepthMap);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->vboDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Settings::Instance()->funcDoLog("[RenderingForwardShadowMapping] Can't create shadow framebuffer!");
        return false;
    }

    return true;
}

void RenderingForwardShadowMapping::render(std::vector<ModelFaceData*> meshModelFaces, int selectedModel) {
    this->matrixProjection = this->managerObjects.matrixProjection;
    this->matrixCamera = this->managerObjects.camera->matrixCamera;
    this->vecCameraPosition = this->managerObjects.camera->cameraPosition;
    this->uiAmbientLight = this->managerObjects.Setting_UIAmbientLight;

    this->renderShadows(meshModelFaces, selectedModel);
    this->renderModels(false, this->shaderProgram, meshModelFaces, selectedModel);
    this->renderDepth();
}

void RenderingForwardShadowMapping::renderShadows(std::vector<ModelFaceData*> meshModelFaces, int selectedModel) {
    if (this->managerObjects.lightSources.size() > 0) {
        glm::vec3 lightPos = glm::vec3(this->managerObjects.lightSources[0]->matrixModel[3].x, this->managerObjects.lightSources[0]->matrixModel[3].y, this->managerObjects.lightSources[0]->matrixModel[3].z);
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,
                                               this->managerObjects.Setting_PlaneClose,
                                               this->managerObjects.Setting_PlaneFar);
        glm::mat4 matrixLightView = glm::lookAt(lightPos, this->managerObjects.camera->eyeSettings->View_Center, this->managerObjects.camera->eyeSettings->View_Up);
        matrixLightView = glm::translate(this->managerObjects.camera->matrixCamera, glm::vec3(-2, 2, 2));
        this->matrixLightSpace = lightProjection * matrixLightView;

        glUseProgram(this->shaderProgramShadows);
        glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, this->fboDepthMap);
        glClear(GL_DEPTH_BUFFER_BIT);
//        this->renderModels(true, this->shaderProgramShadows, meshModelFaces, selectedModel);
        for (size_t i=0; i<meshModelFaces.size(); i++) {
            ModelFaceData *mfd = meshModelFaces[i];

            glm::mat4 matrixModel = glm::mat4(1.0);
            matrixModel *= this->managerObjects.grid->matrixModel;
            // scale
            matrixModel = glm::scale(matrixModel, glm::vec3(mfd->scaleX->point, mfd->scaleY->point, mfd->scaleZ->point));
            // translate
            matrixModel = glm::translate(matrixModel, glm::vec3(mfd->positionX->point, mfd->positionY->point, mfd->positionZ->point));
            // rotate
            matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));
            matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateX->point), glm::vec3(1, 0, 0));
            matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateY->point), glm::vec3(0, 1, 0));
            matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateZ->point), glm::vec3(0, 0, 1));
            matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));

            glUniformMatrix4fv(this->glShadow_LightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixLightSpace));
            glUniformMatrix4fv(this->glShadow_ModelMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));

            mfd->renderModel(true);
        }
        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void RenderingForwardShadowMapping::renderDepth() {
    glUseProgram(this->shaderProgramDepth);
    glUniform1f(this->glDepth_Plane_Close, this->managerObjects.Setting_PlaneClose);
    glUniform1f(this->glDepth_Plane_Far, this->managerObjects.Setting_PlaneFar);
    glUniform1i(this->glDepth_SamplerTexture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->vboDepthMap);

    if (this->depthQuadVAO == 0) {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &this->depthQuadVAO);
        glGenBuffers(1, &this->depthQuadVBO);
        glBindVertexArray(this->depthQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->depthQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(this->depthQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);
}

void RenderingForwardShadowMapping::renderModels(bool isShadowPass, GLuint sProgram, std::vector<ModelFaceData*> meshModelFaces, int selectedModel) {
    glUseProgram(sProgram);

    int selectedModelID = -1;
    for (size_t i=0; i<meshModelFaces.size(); i++) {
        ModelFaceData *mfd = meshModelFaces[i];

        if (mfd->getOptionsSelected())
            selectedModelID = i;

        glm::mat4 matrixModel = glm::mat4(1.0);
        matrixModel *= this->managerObjects.grid->matrixModel;
        // scale
        matrixModel = glm::scale(matrixModel, glm::vec3(mfd->scaleX->point, mfd->scaleY->point, mfd->scaleZ->point));
        // translate
        matrixModel = glm::translate(matrixModel, glm::vec3(mfd->positionX->point, mfd->positionY->point, mfd->positionZ->point));
        // rotate
        matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));
        matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateX->point), glm::vec3(1, 0, 0));
        matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateY->point), glm::vec3(0, 1, 0));
        matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateZ->point), glm::vec3(0, 0, 1));
        matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));

//        if (this->managerObjects.lightSources.size() > 0) {
//            this->managerObjects.matrixProjection = this->matrixLightSpace;
//            this->matrixProjection = this->matrixLightSpace;
//            glm::vec3 lightPos = glm::vec3(this->managerObjects.lightSources[0]->matrixModel[3].x, this->managerObjects.lightSources[0]->matrixModel[3].y, this->managerObjects.lightSources[0]->matrixModel[3].z);
//            matrixModel = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
//        }

        mfd->matrixGrid = this->managerObjects.grid->matrixModel;
        mfd->matrixProjection = this->matrixProjection;
        mfd->matrixCamera = this->matrixCamera;
        mfd->matrixModel = matrixModel;
        mfd->Setting_ModelViewSkin = this->managerObjects.viewModelSkin;
        mfd->lightSources = this->managerObjects.lightSources;
        mfd->setOptionsFOV(this->managerObjects.Setting_FOV);
        mfd->setOptionsOutlineColor(this->managerObjects.Setting_OutlineColor);
        mfd->setOptionsOutlineThickness(this->managerObjects.Setting_OutlineThickness);
        mfd->setOptionsSelected(int(i) == selectedModel);

        glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * matrixModel;
        if (isShadowPass)
            mvpMatrix = this->matrixLightSpace;
        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        if (isShadowPass)
            glUniformMatrix4fv(this->glVS_shadowModelMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));
        glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));

        glm::mat4 matrixModelView = this->matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glFS_MVMatrix, 1, GL_FALSE, glm::value_ptr(matrixModelView));

        glm::mat3 matrixNormal = glm::inverseTranspose(glm::mat3(this->matrixCamera * matrixModel));
        glUniformMatrix3fv(this->glVS_NormalMatrix, 1, GL_FALSE, glm::value_ptr(matrixNormal));

        glm::mat4 matrixWorld = matrixModel;
        glUniformMatrix4fv(this->glVS_WorldMatrix, 1, GL_FALSE, glm::value_ptr(matrixWorld));

        // blending
        if (mfd->meshModel.ModelMaterial.Transparency < 1.0 || mfd->Setting_Alpha < 1.0) {
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            if (mfd->meshModel.ModelMaterial.Transparency < 1.0)
                glUniform1f(this->glFS_AlphaBlending, mfd->meshModel.ModelMaterial.Transparency);
            else
                glUniform1f(this->glFS_AlphaBlending, mfd->Setting_Alpha);
        }
        else {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glUniform1f(this->glFS_AlphaBlending, 1.0);
        }

        // shadow pass
        glUniform1i(this->glFS_ShadowPass, isShadowPass);

        // depth color
        float pc = 1.0f;
        if (this->managerObjects.Setting_PlaneClose >= 1.0f)
            pc = this->managerObjects.Setting_PlaneClose;
        glUniform1f(this->glFS_planeClose, pc);
        glUniform1f(this->glFS_planeFar, this->managerObjects.Setting_PlaneFar / 100.0f);
        glUniform1i(this->glFS_showDepthColor, this->managerObjects.Setting_Rendering_Depth);
        glUniform1i(this->glFS_DebugShadowTexture, this->managerObjects.Setting_DebugShadowTexture);

        // tessellation
        glUniform1i(this->glTCS_UseCullFace, mfd->Setting_UseCullFace);
        glUniform1i(this->glTCS_UseTessellation, mfd->Setting_UseTessellation);
        glUniform1i(this->glTCS_TessellationSubdivision, mfd->Setting_TessellationSubdivision);

        // cel-shading
        glUniform1i(this->glFS_CelShading, mfd->Setting_CelShading);

        // camera position
        glUniform3f(this->glFS_CameraPosition, this->vecCameraPosition.x, this->vecCameraPosition.y, this->vecCameraPosition.z);

        // screen size
        glUniform1f(this->glFS_ScreenResX, Settings::Instance()->SDL_Window_Width);
        glUniform1f(this->glFS_ScreenResY, Settings::Instance()->SDL_Window_Height);

        // Outline color
        glUniform3f(this->glFS_OutlineColor, mfd->getOptionsOutlineColor().r, mfd->getOptionsOutlineColor().g, mfd->getOptionsOutlineColor().b);

        // ambient color for editor
        glUniform3f(this->glFS_UIAmbient, this->uiAmbientLight.r, this->uiAmbientLight.g, this->uiAmbientLight.b);

        // geometry shader displacement
        glUniform3f(this->glGS_GeomDisplacementLocation, mfd->displaceX->point, mfd->displaceY->point, mfd->displaceZ->point);

        // mapping
        glUniform1i(this->glMaterial_ParallaxMapping, mfd->Setting_ParallaxMapping);

        // gamma correction
        glUniform1f(this->glFS_GammaCoeficient, this->managerObjects.Setting_GammaCoeficient);

        // render skin
        glUniform1i(this->gl_ModelViewSkin, mfd->Setting_ModelViewSkin);
        glUniform3f(this->glFS_solidSkin_materialColor, mfd->solidLightSkin_MaterialColor.r, mfd->solidLightSkin_MaterialColor.g, mfd->solidLightSkin_MaterialColor.b);

        glUniform1i(this->solidLight->gl_InUse, 1);
        glUniform3f(this->solidLight->gl_Direction, this->managerObjects.SolidLight_Direction.x, this->managerObjects.SolidLight_Direction.y, this->managerObjects.SolidLight_Direction.z);
        glUniform3f(this->solidLight->gl_Ambient, this->managerObjects.SolidLight_Ambient.r, this->managerObjects.SolidLight_Ambient.g, this->managerObjects.SolidLight_Ambient.b);
        glUniform3f(this->solidLight->gl_Diffuse, this->managerObjects.SolidLight_Diffuse.r, this->managerObjects.SolidLight_Diffuse.g, this->managerObjects.SolidLight_Diffuse.b);
        glUniform3f(this->solidLight->gl_Specular, this->managerObjects.SolidLight_Specular.r, this->managerObjects.SolidLight_Specular.g, this->managerObjects.SolidLight_Specular.b);
        glUniform1f(this->solidLight->gl_StrengthAmbient, this->managerObjects.SolidLight_Ambient_Strength);
        glUniform1f(this->solidLight->gl_StrengthDiffuse, this->managerObjects.SolidLight_Diffuse_Strength);
        glUniform1f(this->solidLight->gl_StrengthSpecular, this->managerObjects.SolidLight_Specular_Strength);

        // lights
        int lightsCount_Directional = 0;
        int lightsCount_Point = 0;
        int lightsCount_Spot = 0;
        for (size_t j=0; j<mfd->lightSources.size(); j++) {
            Light *light = mfd->lightSources[j];
            switch (light->type) {
                case LightSourceType_Directional: {
                    if (lightsCount_Directional < this->GLSL_LightSourceNumber_Directional) {
                        ModelFace_LightSource_Directional *f = this->mfLights_Directional[lightsCount_Directional];

                        glUniform1i(f->gl_InUse, 1);

                        // light
                        glUniform3f(f->gl_Direction, light->positionX->point, light->positionY->point, light->positionZ->point);

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
                        glUniform3f(f->gl_Direction, light->positionX->point, light->positionY->point, light->positionZ->point);
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
        glUniform1f(this->glMaterial_Refraction, mfd->Setting_MaterialRefraction->point);
        glUniform1f(this->glMaterial_SpecularExp, mfd->Setting_MaterialSpecularExp->point);
        glUniform1i(this->glMaterial_IlluminationModel, mfd->materialIlluminationModel);
        glUniform1f(this->glMaterial_HeightScale, mfd->displacementHeightScale->point);
        glUniform3f(this->glMaterial_Ambient, mfd->materialAmbient->color.r, mfd->materialAmbient->color.g, mfd->materialAmbient->color.b);
        glUniform3f(this->glMaterial_Diffuse, mfd->materialDiffuse->color.r, mfd->materialDiffuse->color.g, mfd->materialDiffuse->color.b);
        glUniform3f(this->glMaterial_Specular, mfd->materialSpecular->color.r, mfd->materialSpecular->color.g, mfd->materialSpecular->color.b);
        glUniform3f(this->glMaterial_Emission, mfd->materialEmission->color.r, mfd->materialEmission->color.g, mfd->materialEmission->color.b);

        if (mfd->vboTextureAmbient > 0 && mfd->meshModel.ModelMaterial.TextureAmbient.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureAmbient, 1);
            glUniform1i(this->glMaterial_SamplerAmbient, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureAmbient);
        }
        else
            glUniform1i(this->glMaterial_HasTextureAmbient, 0);

        if (mfd->vboTextureDiffuse > 0 && mfd->meshModel.ModelMaterial.TextureDiffuse.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureDiffuse, 1);
            glUniform1i(this->glMaterial_SamplerDiffuse, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureDiffuse);
        }
        else
            glUniform1i(this->glMaterial_HasTextureDiffuse, 0);

        if (mfd->vboTextureSpecular > 0 && mfd->meshModel.ModelMaterial.TextureSpecular.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureSpecular, 1);
            glUniform1i(this->glMaterial_SamplerSpecular, 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureSpecular);
        }
        else
            glUniform1i(this->glMaterial_HasTextureSpecular, 0);

        if (mfd->vboTextureSpecularExp > 0 && mfd->meshModel.ModelMaterial.TextureSpecularExp.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureSpecularExp, 1);
            glUniform1i(this->glMaterial_SamplerSpecularExp, 3);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureSpecularExp);
        }
        else
            glUniform1i(this->glMaterial_HasTextureSpecularExp, 0);

        if (mfd->vboTextureDissolve > 0 && mfd->meshModel.ModelMaterial.TextureDissolve.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureDissolve, 1);
            glUniform1i(this->glMaterial_SamplerDissolve, 4);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureDissolve);
        }
        else
            glUniform1i(this->glMaterial_HasTextureDissolve, 0);

        if (mfd->vboTextureBump > 0 && mfd->meshModel.ModelMaterial.TextureBump.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureBump, 1);
            glUniform1i(this->glMaterial_SamplerBump, 5);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureBump);
        }
        else
            glUniform1i(this->glMaterial_HasTextureBump, 0);

        if (mfd->vboTextureDisplacement > 0 && mfd->meshModel.ModelMaterial.TextureDisplacement.UseTexture) {
            glUniform1i(this->glMaterial_HasTextureDisplacement, 1);
            glUniform1i(this->glMaterial_SamplerDisplacement, 6);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureDisplacement);
        }
        else
            glUniform1i(this->glMaterial_HasTextureDisplacement, 0);

        // effects - gaussian blur
        glUniform1i(this->glEffect_GB_Mode, mfd->Effect_GBlur_Mode - 1);
        glUniform1f(this->glEffect_GB_W, mfd->Effect_GBlur_Width->point);
        glUniform1f(this->glEffect_GB_Radius, mfd->Effect_GBlur_Radius->point);

        // effects - bloom
        // TODO: Bloom effect
        glUniform1i(this->glEffect_Bloom_doBloom, mfd->Effect_Bloom_doBloom);
        glUniform1f(this->glEffect_Bloom_WeightA, mfd->Effect_Bloom_WeightA);
        glUniform1f(this->glEffect_Bloom_WeightB, mfd->Effect_Bloom_WeightB);
        glUniform1f(this->glEffect_Bloom_WeightC, mfd->Effect_Bloom_WeightC);
        glUniform1f(this->glEffect_Bloom_WeightD, mfd->Effect_Bloom_WeightD);
        glUniform1f(this->glEffect_Bloom_Vignette, mfd->Effect_Bloom_Vignette);
        glUniform1f(this->glEffect_Bloom_VignetteAtt, mfd->Effect_Bloom_VignetteAtt);

        // effects - tone mapping
        glUniform1i(this->glEffect_ToneMapping_ACESFilmRec2020, mfd->Effect_ToneMapping_ACESFilmRec2020);

        // border
        glUniform1f(this->glVS_IsBorder, 0.0);

        glm::mat4 mtxModel;
//        if (mfd->getOptionsSelected()) {
//            glDisable(GL_DEPTH_TEST);
//            glUniform1f(this->glVS_IsBorder, 1.0);
//            mtxModel = glm::scale(matrixModel, glm::vec3(mfd->getOptionsOutlineThickness()));
//            mvpMatrix = this->matrixProjection * this->matrixCamera * mtxModel;
//            glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
//            glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(mtxModel));
//            mfd->renderModel(true);
//            glEnable(GL_DEPTH_TEST);
//        }

        // model draw
        glUniform1f(this->glVS_IsBorder, 0.0);
        mtxModel = glm::scale(matrixModel, glm::vec3(1.0, 1.0, 1.0));
        glm::mat4 mvpMatrixDraw = this->matrixProjection * this->matrixCamera * mtxModel;
        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrixDraw));
        glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(mtxModel));

        // shadows
        glUniform1i(this->glFS_showShadows, mfd->Setting_ShowShadows);
        glUniformMatrix4fv(this->glShadow_ModelMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));
        glUniformMatrix4fv(this->glShadow_LightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixLightSpace));
        glUniformMatrix4fv(this->glVS_shadowModelMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));
        glUniformMatrix4fv(this->glVS_LightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixLightSpace));
        if (!isShadowPass && this->vboDepthMap > 0) {
            glUniform1i(this->glFS_SamplerShadowMap, 7);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, this->vboDepthMap);
        }

        mfd->vertexSphereVisible = this->managerObjects.Setting_VertexSphere_Visible;
        mfd->vertexSphereRadius = this->managerObjects.Setting_VertexSphere_Radius;
        mfd->vertexSphereSegments = this->managerObjects.Setting_VertexSphere_Segments;
        mfd->vertexSphereColor = this->managerObjects.Setting_VertexSphere_Color;
        mfd->vertexSphereIsSphere = this->managerObjects.Setting_VertexSphere_IsSphere;
        mfd->vertexSphereShowWireframes = this->managerObjects.Setting_VertexSphere_ShowWireframes;

        mfd->renderModel(true);

        // edit mode wireframe
        if (mfd->getOptionsSelected() && mfd->Setting_EditMode) {
            mfd->Setting_Wireframe = true;
            matrixModel = glm::scale(matrixModel, glm::vec3(mfd->scaleX->point + 0.01, mfd->scaleY->point + 0.01, mfd->scaleZ->point + 0.01));

            mvpMatrix = this->matrixProjection * this->matrixCamera * matrixModel;
            matrixModelView = this->matrixCamera * matrixModel;
            matrixNormal = glm::inverseTranspose(glm::mat3(this->matrixCamera * matrixModel));
            matrixWorld = matrixModel;
            glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));
            glUniformMatrix4fv(this->glFS_MVMatrix, 1, GL_FALSE, glm::value_ptr(matrixModelView));
            glUniformMatrix3fv(this->glVS_NormalMatrix, 1, GL_FALSE, glm::value_ptr(matrixNormal));
            glUniformMatrix4fv(this->glVS_WorldMatrix, 1, GL_FALSE, glm::value_ptr(matrixWorld));

            glUniform1i(this->gl_ModelViewSkin, ViewModelSkin_Solid);
            //TODO: put in settings
            glUniform3f(this->glFS_solidSkin_materialColor, 1.0f, 0.522f, 0.0f);

            mfd->renderModel(true);
            mfd->Setting_Wireframe = false;
        }
    }

    // edit mode
    if (this->managerObjects.VertexEditorMode != glm::vec3(0.0) && selectedModelID > -1) {
        ImGuizmo::Enable(true);

        ModelFaceData *mfd = meshModelFaces[selectedModelID];

        glm::vec4 v0 = glm::vec4(this->managerObjects.VertexEditorMode, 1.0);
        v0 = mfd->matrixModel * v0;
        glm::mat4 matrixVertex = mfd->matrixModel;
        matrixVertex[3] = v0;

        glm::mat4 mtx = glm::mat4(1.0);

        ImGuizmo::Manipulate(glm::value_ptr(this->managerObjects.camera->matrixCamera),
                             glm::value_ptr(this->managerObjects.matrixProjection),
                             ImGuizmo::TRANSLATE,
                             ImGuizmo::WORLD,
                             glm::value_ptr(matrixVertex),
                             glm::value_ptr(mtx)
                            );

        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(mtx, scale, rotation, translation, skew, perspective);

        glm::vec3 v = this->managerObjects.VertexEditorMode;
        this->managerObjects.VertexEditorMode.x += translation.x;
        this->managerObjects.VertexEditorMode.y += -1.0 * translation.y;
        this->managerObjects.VertexEditorMode.z += translation.z;
        if (this->managerObjects.Setting_GeometryEditMode == GeometryEditMode_Vertex)
            mfd->meshModel.vertices[this->managerObjects.VertexEditorModeID] = this->managerObjects.VertexEditorMode;
        else if (this->managerObjects.Setting_GeometryEditMode == GeometryEditMode_Line) {
        }
        else if (this->managerObjects.Setting_GeometryEditMode == GeometryEditMode_Face) {
            for (size_t i=0; i<mfd->meshModel.vertices.size(); i++) {
                if (mfd->meshModel.vertices[i] == v)
                    mfd->meshModel.vertices[i] = v;
            }
        }
        //TODO: not good for drawing... reuploading the buffers again .... should find a better way - immediate draw, GL_STREAM_DRAW?
        mfd->initBuffers();
        mfd->Setting_EditMode = true;
    }

    glUseProgram(0);
}
