//
//  objParser.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef objParser_hpp
#define objParser_hpp

#include "utilities/settings/Settings.h"
#include "objObjects.h"
#include <regex>

class objParser {
public:
    ~objParser();
    void init(std::function<void(std::string)> doLog, std::function<void(float)> doProgress);
    objScene parse(FBEntity file);
    void destroy();

private:
    objScene scene;

    FBEntity file;
    std::function<void(std::string)> doLog;
    std::function<void(float)> doProgress;
    std::vector<std::string> splitString(const std::string &s, std::regex delimiter);
    std::vector<float> string2float(std::vector<std::string> strings);
    int getLineCount();

    std::vector<objMaterial> loadMaterial(std::string materialFile);
    objMaterial findMaterial(std::string materialID);
    objMaterialImage parseTextureImage(std::string textureLine);

    std::vector<float> geometricVertices, textureCoordinates, vertexNormals, spaceVertices, polygonalFaces;
    int objFileLinesCount;

    // comment line
    std::regex regex_comment;
    // whitespace
    std::regex regex_whiteSpace;

    // current object name
    std::regex regex_objTitle;
    // vertex coordinates
    std::regex regex_geometricVertices;
    // texture coordinates
    std::regex regex_textureCoordinates;
    // normals
    std::regex regex_vertexNormals;
    // space vertices
    std::regex regex_spaceVertices;
    // polygon faces
    std::regex regex_polygonalFaces;
    // face separation
    std::regex regex_polygonalFacesSingle;
    // material file
    std::regex regex_materialFile;
    // material name for the current object
    std::regex regex_useMaterial;

    // material
    std::regex regex_materialNew;

    // To specify the ambient reflectivity of the current material, you can use the "Ka" statement,
    // the "Ka spectral" statement, or the "Ka xyz" statement.
    std::regex regex_materialAmbientColor;
    // To specify the diffuse reflectivity of the current material, you can use the "Kd" statement,
    // the "Kd spectral" statement, or the "Kd xyz" statement.
    std::regex regex_materialDiffuseColor;
    // To specify the specular reflectivity of the current material, you can use the "Ks" statement,
    // the "Ks spectral" statement, or the "Ks xyz" statement.
    std::regex regex_materialSpecularColor;
    // The emission constant color of the material
    std::regex regex_materialEmissionColor;
    // Specifies the specular exponent for the current material. This defines the focus of the specular highlight.
    std::regex regex_materialShininess;
    // Specifies the dissolve for the current material.  Tr or d, depending on the formats. Transperancy
    std::regex regex_materialTransperant1;
    std::regex regex_materialTransperant2;
    // Specifies the optical density for the surface. This is also known as index of refraction.
    std::regex regex_materialOpticalDensity;
    // The "illum" statement specifies the illumination model to use in the material.
    // Illumination models are mathematical equations that represent various material lighting and shading effects.
    std::regex regex_materialIllumination;
    // Specifies that a color texture file or a color procedural texture file is applied to the ambient reflectivity of the material.
    // During rendering, the "map_Ka" value is multiplied by the "Ka" value.
    std::regex regex_materialTextureAmbient;
    // Specifies that a color texture file or color procedural texture file is linked to the diffuse reflectivity of the material.
    // During rendering, the map_Kd value is multiplied by the Kd value.
    std::regex regex_materialTextureDensity;
    // Specifies that a color texture file or color procedural texture file is linked to the specular reflectivity of the material.
    // During rendering, the map_Ks value is multiplied by the Ks value.
    std::regex regex_materialTextureSpecular;
    // Specifies that a scalar texture file or scalar procedural texture file is linked to the specular exponent of the material.
    // During rendering, the map_Ns value is multiplied by the Ns value.
    std::regex regex_materialTextureSpecularExp;
    // Specifies that a scalar texture file or scalar procedural texture file is linked to the dissolve of the material.
    // During rendering, the map_d value is multiplied by the d value.
    std::regex regex_materialTextureDissolve;
};

#endif /* objParser_hpp */
