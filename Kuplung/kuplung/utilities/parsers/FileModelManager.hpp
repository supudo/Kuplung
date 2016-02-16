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
#include "kuplung/utilities/parsers/obj-parser/objParser.hpp"
#include "kuplung/utilities/parsers/stl-parser/STLParser.hpp"

class FileModelManager {
public:
    ~FileModelManager();
    void destroy();
    void init(std::function<void(std::string)> doLog, std::function<void(float)> doProgress);
    objScene parse(FBEntity file);

private:
    std::function<void(std::string)> funcLog;
    std::function<void(float)> funcProgress;
    void doProgress(float value);

    objParser *parserOBJ;
    STLParser *parserSTL;

    void logMessage(std::string message);
};

#endif /* FileModelManager_hpp */
