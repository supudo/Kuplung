//
//  HeightmapGenerator.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/21/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef HeightmapGenerator_hpp
#define HeightmapGenerator_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "noise.h"
#include "kuplung/utilities/libnoise/noiseutils.h"
#include <glm/common.hpp>

class HeightmapGenerator {
public:
    ~HeightmapGenerator();
    void initPosition();
    void generateTerrain(std::string assetsFolder, int width, int height);

    std::vector<glm::vec3> vertices, normals, colors;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;
    std::string heightmapImage;

    int Setting_Octaves;
    float Setting_Frequency, Setting_Persistence;
    bool Setting_ColorTerrain;
    float Setting_OffsetHorizontal, Setting_OffsetVertical;
    MeshModel modelTerrain;

private:
    std::string assetsFolder;
    int width, height;
    float position_x1;
    float position_x2;
    float position_y1;
    float position_y2;

    utils::NoiseMap heightMap;
    utils::Image image;

    void generateMeshModel();
    void generateGeometry2();
    void generateGeometryCubic();
};

#endif /* HeightmapGenerator_hpp */
