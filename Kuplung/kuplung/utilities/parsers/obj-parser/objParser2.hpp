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
#include "kuplung/utilities/parsers/ParserUtils.hpp"
#include "kuplung/utilities/measurement/Timings.hpp"

struct PackedVertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator<(const PackedVertex& that) const noexcept {
        return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
    };
};

class objParser2 {
public:
    ~objParser2();
    void init(const std::function<void(float)>& doProgress);
    std::vector<MeshModel> parse(const FBEntity& fileToParse, const std::vector<std::string>& settings);

private:
    FBEntity file;
    std::function<void(float)> doProgress;
    int objFileLinesCount = 0;

    std::unique_ptr<ParserUtils> parserUtils;

    std::vector<MeshModel> models;
    std::map<std::string, MeshModelMaterial> materials;
    std::vector<glm::vec3> vectorVertices, vectorNormals;
    std::vector<glm::vec2> vectorTextureCoordinates;
    std::vector<unsigned int> vectorIndices;

    void loadMaterialFile(std::string const& materialFile);
    MeshMaterialTextureImage parseTextureImage(const std::string& textureLine);
    std::vector<std::string> splitString(const std::string& s, const std::string& delimiter);
    bool getSimilarVertexIndex(PackedVertex& packed, std::map<PackedVertex, unsigned int>& vertexToOutIndex, unsigned int& result);

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

#endif /* objParser2_hpp */
