//
//  Maths.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Maths_hpp
#define Maths_hpp

#include <vector>
#include <glm/glm.hpp>

class Maths {
public:
    void computeTangentBasis(
        // inputs
        std::vector<glm::vec3> &vertices,
        std::vector<glm::vec2> &uvs,
        std::vector<glm::vec3> &normals,
        // outputs
        std::vector<glm::vec3> &tangents,
        std::vector<glm::vec3> &bitangents
    );
};

#endif /* Maths_hpp */
