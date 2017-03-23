//
//  STLParser.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef STLParser_hpp
#define STLParser_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"

class STLParser {
public:
    ~STLParser();
    void init(std::function<void(float)> doProgress);
    std::vector<MeshModel> parse(const FBEntity& file, const std::vector<std::string>& settings);

    std::vector<float> vertices, normals;

private:
    std::function<void(float)> funcProgress;
};

#endif /* STLParser_hpp */
