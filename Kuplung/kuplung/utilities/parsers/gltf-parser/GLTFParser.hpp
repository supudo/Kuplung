//
//  GLTFParser.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 07/08/17.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef GLTFParser_hpp
#define GLTFParser_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ParserUtils.hpp"
#include "kuplung/utilities/parsers/ModelObject.h"
#include <fstream>

class GLTFParser {
public:
	GLTFParser();
    ~GLTFParser();
    void init(const std::function<void(float)>& doProgress);
    std::vector<MeshModel> parse(const FBEntity& file, const std::vector<std::string>& settings);

private:
    std::function<void(float)> doProgress;
    std::vector<MeshModel> models;
};

#endif /* GLTFParser_hpp */
