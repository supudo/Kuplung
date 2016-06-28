//
//  SpaceshipMeshGenerator.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/21/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SpaceshipMeshGenerator_hpp
#define SpaceshipMeshGenerator_hpp

#include <glm/glm.hpp>
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"

class SpaceshipMeshGenerator {
public:
    ~SpaceshipMeshGenerator();
    void generate(int gridSize);

    std::vector<glm::vec3> vertices, normals, colors;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    MeshModel modelSpaceship;

private:
    int gridSize;

    void generateMeshModel();
    void generateFirstHull();
    void generateFirstHull2();
    int getRandomValue(float valueMin, float valueMax, bool zeroIsValid = true);
};

#endif /* SpaceshipMeshGenerator_hpp */
