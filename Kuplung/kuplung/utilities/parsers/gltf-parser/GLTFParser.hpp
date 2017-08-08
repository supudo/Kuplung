//
//  GlTFParser.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 07/08/17.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef GlTFParser_hpp
#define GlTFParser_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ParserUtils.hpp"
#include "kuplung/utilities/parsers/ModelObject.h"
#include <fstream>

class GlTFParser {
public:
	GlTFParser();
    ~GlTFParser();
    void init(const std::function<void(float)>& doProgress);
    std::vector<MeshModel> parse(const FBEntity& file, const std::vector<std::string>& settings);

private:
    std::function<void(float)> doProgress;
    std::vector<MeshModel> models;

    std::unique_ptr<ParserUtils> parserUtils;

    int Setting_Axis_Forward, Setting_Axis_Up;
};

#endif /* GlTFParser_hpp */
