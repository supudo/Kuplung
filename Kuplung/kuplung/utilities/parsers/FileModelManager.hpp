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
#include "kuplung/utilities/parsers/obj-parser/objParser1.hpp"
#include "kuplung/utilities/parsers/obj-parser/objParser2.hpp"
#include "kuplung/utilities/parsers/stl-parser/STLParser.hpp"
#include "kuplung/utilities/parsers/assimp-parser/AssimpParser.hpp"
#include "kuplung/ui/components/FileBrowser.hpp"
#include <functional>

class FileModelManager {
public:
    ~FileModelManager();
    FileModelManager();
    void destroy();
    void init(std::function<void(float)> doProgress);
    std::vector<MeshModel> parse(FBEntity file, FileBrowser_ParserType type);

private:
    std::function<void(float)> funcProgress;
    void doProgress(float value);

    std::unique_ptr<objParser1> parserOBJ1;
    std::unique_ptr<objParser2> parserOBJ2;
    std::unique_ptr<STLParser> parserSTL;
    std::unique_ptr<AssimpParser> parserAssimp;
};

#endif /* FileModelManager_hpp */
