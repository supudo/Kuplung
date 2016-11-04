//
//  ParserUtils.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "ParserUtils.hpp"
#include <glm/gtx/rotate_vector.hpp>

glm::vec3 ParserUtils::fixVectorAxis(glm::vec3 v, int indexForward, int indexUp) {
    //
    //                 +Z
    //                  |
    //                  |           +Y
    //                  |         /
    //                  |       /
    //                  |     /
    //                  |   /
    //                  | /
    //  -X--------------|-----------------+X
    //                 /|
    //               /  |
    //             /    |
    //           /      |
    //         /        |
    //       /          |
    //     -Y           |
    //                 -Z
    //

    switch (indexForward) {
        case 0: { // -X Forward
            v = glm::rotateZ(v, glm::radians(-90.0f));
            break;
        }
        case 1: { // -Y Forward
            v = glm::rotateZ(v, glm::radians(180.0f));
            break;
        }
        case 2: { // -Z Forward
            v = glm::rotateX(v, glm::radians(90.0f));
            break;
        }
        case 3: { // X Forward
            v = glm::rotateZ(v, glm::radians(90.0f));
            break;
        }
        case 4: { // Y Forward
            break;
        }
        case 5: { // Z Forward
            v = glm::rotateX(v, glm::radians(-90.0f));
            break;
        }
        default:
            break;
    }

    switch (indexUp) {
        case 0: { // -X Up
            v = glm::rotateY(v, glm::radians(-90.0f));
            break;
        }
        case 1: { // -Y Up
            break;
        }
        case 2: { // -Z Up
            v = glm::rotateY(v, glm::radians(180.0f));
            break;
        }
        case 3: { // X Up
            v = glm::rotateY(v, glm::radians(90.0f));
            break;
        }
        case 4: { // Y Up
            v = glm::rotateY(v, glm::radians(180.0f));
            break;
        }
        case 5: { // Z Up
//            v = glm::rotateY(v, glm::radians(180.0f));
            break;
        }
        default:
            break;
    }

    return v;
}

