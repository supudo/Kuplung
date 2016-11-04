//
//  ParserUtils.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef ParserUtils_hpp
#define ParserUtils_hpp

#include "kuplung/settings/Settings.h"
#include <glm/vec3.hpp>

class ParserUtils {
public:
    glm::vec3 fixVectorAxis(glm::vec3 v, int indexForward, int indexUp);
};

#endif /* ParserUtils_hpp */
