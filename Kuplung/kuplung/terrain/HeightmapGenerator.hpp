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
#include <glm/common.hpp>

struct PackedTerrainPoint {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator<(const PackedTerrainPoint that) const{
        return memcmp((void*)this, (void*)&that, sizeof(PackedTerrainPoint))>0;
    };
};

class HeightmapGenerator {
public:
    ~HeightmapGenerator();
    void initPosition();
    void initSettings();
    void generateTerrain(std::string assetsFolder, double offsetHorizontal, double offsetVertical, int width, int height);

    std::vector<glm::vec3> vertices, normals, colors;
    std::vector<glm::vec2> uvs;
    std::vector<int> indices;
    std::string heightmapImage;

    int Setting_Octaves;
    float Setting_Frequency;
    float Setting_Persistence;
    bool Setting_ColorTerrain;
    MeshModel modelTerrain;

private:
    double position_x1;
    double position_x2;
    double position_y1;
    double position_y2;

    bool getSimilarVertexIndex(PackedTerrainPoint & packed, std::map<PackedTerrainPoint, unsigned int> & vertexToOutIndex, unsigned int & result);
};

#endif /* HeightmapGenerator_hpp */
