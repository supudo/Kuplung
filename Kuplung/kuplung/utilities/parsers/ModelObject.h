//
//  ModelObject.h
//  Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/settings/Settings.h"

#ifndef ModelObject_h
#define ModelObject_h

#include <vector>
#include <string>
#include <glm/glm.hpp>

typedef enum objMaterialImageType {
    objMaterialImageType_Ambient,
    objMaterialImageType_Diffuse,
    objMaterialImageType_Specular,
    objMaterialImageType_SpecularExp,
    objMaterialImageType_Dissolve,
    objMaterialImageType_Bump,
    objMaterialImageType_Displacement
} objMaterialImageType;

struct objMaterialImage {
    std::string filename;
    std::string image;
    int width, height;
    bool useTexture;
    std::vector<std::string> commands;
};

struct objMaterialColor {
    float r, g, b, a;
};

struct objMaterial {
    std::string materialID;

    objMaterialColor ambient, diffuse, specular, emission;

    float specularExp;
    float transparency;
    unsigned int illumination;
    float opticalDensity; // shininess

    objMaterialImage textures_ambient;
    objMaterialImage textures_diffuse;
    objMaterialImage textures_specular;
    objMaterialImage textures_specularExp;
    objMaterialImage textures_dissolve;
    objMaterialImage textures_bump;
    objMaterialImage textures_displacement;
};

struct objModelFace {
    int ID;
    int ModelID;
    std::string ModelTitle;
    std::string objFile;
    std::string materialID;
    int verticesCount;
    int textureCoordinatesCount;
    int normalsCount;
    int indicesCount;
    objMaterial faceMaterial;

    std::vector<float> vertices;
    std::vector<glm::vec3> vectors_vertices;

    std::vector<float> texture_coordinates;
    std::vector<glm::vec2> vectors_texture_coordinates;

    std::vector<float> normals;
    std::vector<glm::vec3> vectors_normals;

    std::vector<unsigned int> indices;
    std::vector<float> solidColor;
};

struct objModel {
    int ID;
    std::string modelID;
    int verticesCount;
    int textureCoordinatesCount;
    int normalsCount;
    int indicesCount;

    std::vector<objModelFace> faces;
};

struct objScene {
    int totalCountGeometricVertices;
    int totalCountTextureCoordinates;
    int totalCountNormalVertices;
    int totalCountIndices;
    int totalCountFaces;

    std::vector<objModel> models;
    std::vector<objMaterial> materials;

    std::string objFile;
};

void static Kuplung_printObjScene(objScene obj) {
    printf("this->scene.objFile = %s\n", obj.objFile.c_str());
    printf("this->scene.totalCountFaces = %i\n", obj.totalCountFaces);
    printf("this->scene.totalCountGeometricVertices = %i\n", obj.totalCountGeometricVertices);
    printf("this->scene.totalCountIndices = %i\n", obj.totalCountIndices);
    printf("this->scene.totalCountNormalVertices = %i\n", obj.totalCountNormalVertices);
    printf("this->scene.totalCountTextureCoordinates = %i\n", obj.totalCountTextureCoordinates);

    printf("--------------------------\n");
    printf("MATERIALS :\n");
    for (size_t i=0; i<obj.materials.size(); i++) {
        objMaterial material = obj.materials[i];
        printf("    material.ID = %s\n", material.materialID.c_str());
        printf("    material.illumination = %i\n", material.illumination);
        printf("    material.opticalDensity = %f\n", material.opticalDensity);
        printf("    material.specularExp = %f\n", material.specularExp);
        printf("    material.transparency = %f\n", material.transparency);
        printf("    material.ambient = %f, %f, %f\n", material.ambient.r, material.ambient.g, material.ambient.b);
        printf("    material.diffuse = %f, %f, %f\n", material.diffuse.r, material.diffuse.g, material.diffuse.b);
        printf("    material.emission = %f, %f, %f\n", material.emission.r, material.emission.g, material.emission.b);
        printf("    material.specular = %f, %f, %f\n", material.specular.r, material.specular.g, material.specular.b);
        printf("    material.textures_ambient.filename = %s\n", material.textures_ambient.filename.c_str());
        printf("    material.textures_ambient.image = %s\n", material.textures_ambient.image.c_str());
        printf("    material.textures_diffuse.filename = %s\n", material.textures_diffuse.filename.c_str());
        printf("    material.textures_diffuse.image = %s\n", material.textures_diffuse.image.c_str());
        printf("    material.textures_bump.filename = %s\n", material.textures_bump.filename.c_str());
        printf("    material.textures_bump.image = %s\n", material.textures_bump.image.c_str());
        printf("    material.textures_displacement.filename = %s\n", material.textures_displacement.filename.c_str());
        printf("    material.textures_displacement.image = %s\n", material.textures_displacement.image.c_str());
        printf("    material.textures_dissolve.filename = %s\n", material.textures_dissolve.filename.c_str());
        printf("    material.textures_dissolve.image = %s\n", material.textures_dissolve.image.c_str());
        printf("    material.textures_specular.filename = %s\n", material.textures_specular.filename.c_str());
        printf("    material.textures_specular.image = %s\n", material.textures_specular.image.c_str());
        printf("    material.textures_specularExp.filename = %s\n", material.textures_specularExp.filename.c_str());
        printf("    material.textures_specularExp.image = %s\n", material.textures_specularExp.image.c_str());
        printf("--------------------------\n");
    }

    printf("--------------------------\n");
    printf("MODELS :\n");
    for (size_t i=0; i<obj.models.size(); i++) {
        objModel model = obj.models[i];
        printf("    model.ID = %i\n", model.ID);
        printf("    model.indicesCount = %i\n", model.indicesCount);
        printf("    model.modelID = %s\n", model.modelID.c_str());
        printf("    model.normalsCount = %i\n", model.normalsCount);
        printf("    model.textureCoordinatesCount = %i\n", model.textureCoordinatesCount);
        printf("    model.verticesCount = %i\n", model.verticesCount);
        printf("    FACES:\n");
        for (size_t j=0; j<model.faces.size(); j++) {
            objModelFace face = model.faces[j];
            printf("        face.ID = %i\n", face.ID);
            printf("        face.indicesCount = %i\n", face.indicesCount);
            printf("        face.materialID = %s\n", face.materialID.c_str());
            printf("        face.ModelID = %i\n", face.ModelID);
            printf("        face.ModelTitle = %s\n", face.ModelTitle.c_str());
            printf("        face.normalsCount = %i\n", face.normalsCount);
            printf("        face.objFile = %s\n", face.objFile.c_str());
            printf("        face.textureCoordinatesCount = %i\n", face.textureCoordinatesCount);
            printf("        face.verticesCount = %i\n", face.verticesCount);
            printf("        face.faceMaterial.materialID = %s\n", face.faceMaterial.materialID.c_str());
            printf("        face.faceMaterial.ambient = %f, %f, %f\n", face.faceMaterial.ambient.r, face.faceMaterial.ambient.g, face.faceMaterial.ambient.b);
            printf("        face.faceMaterial.diffuse = %f, %f, %f\n", face.faceMaterial.diffuse.r, face.faceMaterial.diffuse.g, face.faceMaterial.diffuse.b);
            printf("        face.faceMaterial.emission = %f, %f, %f\n", face.faceMaterial.emission.r, face.faceMaterial.emission.g, face.faceMaterial.emission.b);
            printf("        face.faceMaterial.specular = %f, %f, %f\n", face.faceMaterial.specular.r, face.faceMaterial.specular.g, face.faceMaterial.specular.b);
            printf("        face.faceMaterial.illumination = %i\n", face.faceMaterial.illumination);
            printf("        face.faceMaterial.opticalDensity = %f\n", face.faceMaterial.opticalDensity);
            printf("        face.faceMaterial.specularExp = %f\n", face.faceMaterial.specularExp);
            printf("        face.faceMaterial.transparency = %f\n", face.faceMaterial.transparency);
            printf("        face.faceMaterial.textures_ambient.filename = %s\n", face.faceMaterial.textures_ambient.filename.c_str());
            printf("        face.faceMaterial.textures_ambient.image = %s\n", face.faceMaterial.textures_ambient.image.c_str());
            printf("        face.faceMaterial.textures_diffuse.filename = %s\n", face.faceMaterial.textures_diffuse.filename.c_str());
            printf("        face.faceMaterial.textures_diffuse.image = %s\n", face.faceMaterial.textures_diffuse.image.c_str());
            printf("        face.faceMaterial.textures_bump.filename = %s\n", face.faceMaterial.textures_bump.filename.c_str());
            printf("        face.faceMaterial.textures_bump.image = %s\n", face.faceMaterial.textures_bump.image.c_str());
            printf("        face.faceMaterial.textures_displacement.filename = %s\n", face.faceMaterial.textures_displacement.filename.c_str());
            printf("        face.faceMaterial.textures_displacement.image = %s\n", face.faceMaterial.textures_displacement.image.c_str());
            printf("        face.faceMaterial.textures_dissolve.filename = %s\n", face.faceMaterial.textures_dissolve.filename.c_str());
            printf("        face.faceMaterial.textures_dissolve.image = %s\n", face.faceMaterial.textures_dissolve.image.c_str());
            printf("        face.faceMaterial.textures_specular.filename = %s\n", face.faceMaterial.textures_specular.filename.c_str());
            printf("        face.faceMaterial.textures_specular.image = %s\n", face.faceMaterial.textures_specular.image.c_str());
            printf("        face.faceMaterial.textures_specularExp.filename = %s\n", face.faceMaterial.textures_specularExp.filename.c_str());
            printf("        face.faceMaterial.textures_specularExp.image = %s\n", face.faceMaterial.textures_specularExp.image.c_str());
        }
    }
}

struct MeshMaterialTextureImage {
    std::string Filename;
    std::string Image;
    int Width, Height;
    bool UseTexture;
    std::vector<std::string> Commands;
};

struct MeshModelMaterial {
    int MaterialID;
    std::string MaterialTitle;

    glm::vec3 AmbientColor, DiffuseColor, SpecularColor, EmissionColor;
    float SpecularExp;
    float Transparency;
    unsigned int IlluminationMode;
    float OpticalDensity;

    MeshMaterialTextureImage textures_ambient;
    MeshMaterialTextureImage textures_diffuse;
    MeshMaterialTextureImage textures_specular;
    MeshMaterialTextureImage textures_specularExp;
    MeshMaterialTextureImage textures_dissolve;
    MeshMaterialTextureImage textures_bump;
    MeshMaterialTextureImage textures_displacement;
};

struct MeshModel {
    int ID;
    FBEntity File;
    std::string ModelTitle, MaterialTitle;
    int countVertices, countTextureCoordinates, countNormals, countIndices;

    MeshModelMaterial ModelMaterial;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texture_coordinates;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
};

void static Kuplung_printObjModels(std::vector<MeshModel> models) {
    for (size_t i=0; i<models.size(); i++) {
        MeshModel m = models[i];
        printf("model.ID = %i\n", m.ID);
        printf("model.countIndices = %i\n", m.countIndices);
        printf("model.countNormals = %i\n", m.countNormals);
        printf("model.countTextureCoordinates = %i\n", m.countTextureCoordinates);
        printf("model.countVertices = %i\n", m.countVertices);
        printf("model.MaterialTitle = %s\n", m.MaterialTitle.c_str());
        printf("model.ModelTitle = %s\n", m.ModelTitle.c_str());

        printf("m.geometry :\n");
        for (size_t j=0; j<m.indices.size(); j++) {
            int idx = m.indices[j];
            std::string geom = Settings::Instance()->string_format("index = %i ---> ", idx);
            geom += Settings::Instance()->string_format("vertex = [%f, %f, %f]", m.vertices[idx].x, m.vertices[idx].y, m.vertices[idx].z);
            geom += Settings::Instance()->string_format(", uv = [%f, %f]", m.texture_coordinates[idx].x, m.texture_coordinates[idx].y);
            geom += Settings::Instance()->string_format(", normal = [%f, %f, %f]", m.normals[idx].x, m.normals[idx].y, m.normals[idx].z);
            printf("%s\n", geom.c_str());
        }

        printf("model.ModelMaterial.MaterialID = %i\n", m.ModelMaterial.MaterialID);
        printf("model.ModelMaterial.MaterialTitle = %s\n", m.ModelMaterial.MaterialTitle.c_str());

        printf("model.ModelMaterial.AmbientColor = %f, %f, %f\n", m.ModelMaterial.AmbientColor.r, m.ModelMaterial.AmbientColor.g, m.ModelMaterial.AmbientColor.b);
        printf("model.ModelMaterial.DiffuseColor = %f, %f, %f\n", m.ModelMaterial.DiffuseColor.r, m.ModelMaterial.DiffuseColor.g, m.ModelMaterial.DiffuseColor.b);
        printf("model.ModelMaterial.SpecularColor = %f, %f, %f\n", m.ModelMaterial.SpecularColor.r, m.ModelMaterial.SpecularColor.g, m.ModelMaterial.SpecularColor.b);
        printf("model.ModelMaterial.EmissionColor = %f, %f, %f\n", m.ModelMaterial.EmissionColor.r, m.ModelMaterial.EmissionColor.g, m.ModelMaterial.EmissionColor.b);

        printf("model.ModelMaterial.SpecularExp = %f\n", m.ModelMaterial.SpecularExp);
        printf("model.ModelMaterial.SpecularExp = %f\n", m.ModelMaterial.Transparency);
        printf("model.ModelMaterial.OpticalDensity = %f\n", m.ModelMaterial.OpticalDensity);
        printf("model.ModelMaterial.IlluminationMode = %i\n", m.ModelMaterial.IlluminationMode);

        printf("model.ModelMaterial.textures_ambient.Filename = %s\n", m.ModelMaterial.textures_ambient.Filename.c_str());
        printf("model.ModelMaterial.textures_diffuse.Filename = %s\n", m.ModelMaterial.textures_diffuse.Filename.c_str());
        printf("model.ModelMaterial.textures_specular.Filename = %s\n", m.ModelMaterial.textures_specular.Filename.c_str());
        printf("model.ModelMaterial.textures_specularExp.Filename = %s\n", m.ModelMaterial.textures_specularExp.Filename.c_str());
        printf("model.ModelMaterial.textures_dissolve.Filename = %s\n", m.ModelMaterial.textures_dissolve.Filename.c_str());
        printf("model.ModelMaterial.textures_bump.Filename = %s\n", m.ModelMaterial.textures_bump.Filename.c_str());
        printf("model.ModelMaterial.textures_displacement.Filename = %s\n", m.ModelMaterial.textures_displacement.Filename.c_str());
    }
}

#endif /* ModelObject_h */
