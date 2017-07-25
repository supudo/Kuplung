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
#include "kuplung/utilities/parsers/ParserUtils.hpp"
#include "kuplung/utilities/parsers/ModelObject.h"
#include <fstream>

class STLParser {
public:
	STLParser();
    ~STLParser();
    void init(const std::function<void(float)>& doProgress);
    std::vector<MeshModel> parse(const FBEntity& file, const std::vector<std::string>& settings);

    std::vector<float> vertices, normals;

private:
    std::function<void(float)> doProgress;
    std::vector<MeshModel> models;

    std::unique_ptr<ParserUtils> parserUtils;

    int Setting_Axis_Forward, Setting_Axis_Up;

    bool isBinarySTL(const char* buffer, unsigned int fileSize);
    bool isAsciiSTL(const char* buffer, unsigned int fileSize);

    template<class char_t>
    bool skipSpaces(const char_t** inout);

    template<class char_t>
    bool skipSpaces(const char_t* in, const char_t** out);

    template <class char_t>
    bool isLineEnd(char_t in);

    bool loadBinaryFile(const FBEntity& file, unsigned int fileSize);
    bool loadAsciiFile(const FBEntity& file);

    glm::vec3 parsePoint(std::ifstream& s);
    float parseFloat(std::ifstream& s);
};

#endif /* STLParser_hpp */
