//
//  objParserCuda.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifdef DEF_KuplungSetting_UseCuda

#ifndef objParserCuda_hpp
#define objParserCuda_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"

class objParserCuda {
public:
    ~objParserCuda();
    void init(std::function<void(float)> doProgress);
    std::vector<MeshModel> parse(const FBEntity& file, const std::vector<std::string>& settings);

private:
    FBEntity file;
    std::function<void(float)> doProgress;
    int objFileLinesCount = 0;
    std::vector<MeshModel> models;

    void doParse(std::string const& obj_file_contents, int length);
};

#endif /* objParserCuda_hpp */

#endif
