//
//  objParser2.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef objParser2_hpp
#define objParser2_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include <functional>

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

struct PackedVertex{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator<(const PackedVertex that) const{
        return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
    };
};

class objParser2 {
public:
    ~objParser2();
    void init(std::function<void(float)> doProgress);
    std::vector<MeshModel> parse(FBEntity file);
    void destroy();

private:
    FBEntity file;
    std::function<void(float)> doProgress;
    int objFileLinesCount;

    std::vector<MeshModel> models;
    std::map<std::string, MeshModelMaterial> materials;
    std::vector<glm::vec3> vectorVertices, vectorNormals;
    std::vector<glm::vec2> vectorTextureCoordinates;
    std::vector<unsigned int> vectorIndices;

    void loadMaterialFile(std::string materialFile);
    MeshMaterialTextureImage parseTextureImage(std::string textureLine);
    std::vector<std::string> splitString(const std::string &s, std::string delimiter);

    void fixIndices(
        std::vector<glm::vec3> & in_vertices,
        std::vector<glm::vec2> & in_uvs,
        std::vector<glm::vec3> & in_normals,
        std::vector<unsigned int> & out_indices,
        std::vector<glm::vec3> & out_vertices,
        std::vector<glm::vec2> & out_uvs,
        std::vector<glm::vec3> & out_normals
    );
    bool getSimilarVertexIndex(PackedVertex & packed, std::map<PackedVertex, unsigned int> & vertexToOutIndex, unsigned int & result);

    // current object name
    std::string id_objTitle;
    // vertex coordinates
    std::string id_geometricVertices;
    // texture coordinates
    std::string id_textureCoordinates;
    // normals
    std::string id_vertexNormals;
    // space vertices
    std::string id_spaceVertices;
    // polygon faces
    std::string id_face;
    // material file
    std::string id_materialFile;
    // material name for the current object
    std::string id_useMaterial;

    // material
    std::string id_materialNew;

    // To specify the ambient reflectivity of the current material, you can use the "Ka" statement,
    // the "Ka spectral" statement, or the "Ka xyz" statement.
    std::string id_materialAmbientColor;
    // To specify the diffuse reflectivity of the current material, you can use the "Kd" statement,
    // the "Kd spectral" statement, or the "Kd xyz" statement.
    std::string id_materialDiffuseColor;
    // To specify the specular reflectivity of the current material, you can use the "Ks" statement,
    // the "Ks spectral" statement, or the "Ks xyz" statement.
    std::string id_materialSpecularColor;
    // The emission constant color of the material
    std::string id_materialEmissionColor;
    // Specifies the specular exponent for the current material. This defines the focus of the specular highlight.
    std::string id_materialSpecularExp;
    // Specifies the dissolve for the current material.  Tr or d, depending on the formats. Transperancy
    std::string id_materialTransperant1;
    std::string id_materialTransperant2;
    // Specifies the optical density for the surface. This is also known as index of refraction.
    std::string id_materialOpticalDensity;
    // The "illum" statement specifies the illumination model to use in the material.
    // Illumination models are mathematical equations that represent various material lighting and shading effects.
    std::string id_materialIllumination;
    // Specifies that a color texture file or a color procedural texture file is applied to the ambient reflectivity of the material.
    // During rendering, the "map_Ka" value is multiplied by the "Ka" value.
    std::string id_materialTextureAmbient;
    // Specifies that a color texture file or color procedural texture file is linked to the diffuse reflectivity of the material.
    // During rendering, the map_Kd value is multiplied by the Kd value.
    std::string id_materialTextureDiffuse;
    // Bump map
    std::string id_materialTextureBump;
    // Displacement map
    std::string id_materialTextureDisplacement;
    // Specifies that a color texture file or color procedural texture file is linked to the specular reflectivity of the material.
    // During rendering, the map_Ks value is multiplied by the Ks value.
    std::string id_materialTextureSpecular;
    // Specifies that a scalar texture file or scalar procedural texture file is linked to the specular exponent of the material.
    // During rendering, the map_Ns value is multiplied by the Ns value.
    std::string id_materialTextureSpecularExp;
    // Specifies that a scalar texture file or scalar procedural texture file is linked to the dissolve of the material.
    // During rendering, the map_d value is multiplied by the d value.
    std::string id_materialTextureDissolve;
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

#endif /* objParser2_hpp */
