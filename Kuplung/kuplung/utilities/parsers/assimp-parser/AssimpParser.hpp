//
//  AssimpParser.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef AssimpParser_hpp
#define AssimpParser_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssimpParser {
public:
    ~AssimpParser();
    void init(std::function<void(float)> doProgress);
    objScene parse(FBEntity file);
    void destroy();

    std::vector<float> vertices, normals;

private:
    std::function<void(float)> funcProgress;

    Assimp::Importer parser;
};

#endif /* AssimpParser_hpp */
