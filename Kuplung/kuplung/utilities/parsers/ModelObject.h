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

    MeshMaterialTextureImage TextureAmbient;
    MeshMaterialTextureImage TextureDiffuse;
    MeshMaterialTextureImage TextureSpecular;
    MeshMaterialTextureImage TextureSpecularExp;
    MeshMaterialTextureImage TextureDissolve;
    MeshMaterialTextureImage TextureBump;
    MeshMaterialTextureImage TextureDisplacement;
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

        printf("model.ModelMaterial.textures_ambient.Filename = %s\n", m.ModelMaterial.TextureAmbient.Filename.c_str());
        printf("model.ModelMaterial.textures_diffuse.Filename = %s\n", m.ModelMaterial.TextureDiffuse.Filename.c_str());
        printf("model.ModelMaterial.textures_specular.Filename = %s\n", m.ModelMaterial.TextureSpecular.Filename.c_str());
        printf("model.ModelMaterial.textures_specularExp.Filename = %s\n", m.ModelMaterial.TextureSpecularExp.Filename.c_str());
        printf("model.ModelMaterial.textures_dissolve.Filename = %s\n", m.ModelMaterial.TextureDissolve.Filename.c_str());
        printf("model.ModelMaterial.textures_bump.Filename = %s\n", m.ModelMaterial.TextureBump.Filename.c_str());
        printf("model.ModelMaterial.textures_displacement.Filename = %s\n", m.ModelMaterial.TextureDisplacement.Filename.c_str());
    }
}

#endif /* ModelObject_h */
