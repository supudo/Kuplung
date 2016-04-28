//
//  FileModelManager.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef FileModelManager_hpp
#define FileModelManager_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/utilities/parsers/obj-parser/objParserX.hpp"
#include "kuplung/utilities/parsers/obj-parser/objParser.hpp"
#include "kuplung/utilities/parsers/stl-parser/STLParser.hpp"
#include "kuplung/utilities/parsers/assimp-parser/AssimpParser.hpp"
#include "kuplung/ui/components/FileBrowser.hpp"
#include <functional>

class FileModelManager {
public:
    ~FileModelManager();
    void destroy();
    void init(std::function<void(float)> doProgress);
    std::vector<MeshModel> parse(FBEntity file, FileBrowser_ParserType type);

private:
    std::function<void(float)> funcProgress;
    void doProgress(float value);

    objParserX *parserXOBJ;
    objParser *parserOBJ;
    STLParser *parserSTL;
    AssimpParser *parserAssimp;
};

#endif /* FileModelManager_hpp */
