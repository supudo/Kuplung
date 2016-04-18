//
//  ExporterOBJ.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ExporterOBJ_hpp
#define ExporterOBJ_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/ModelFace.hpp"

class ExporterOBJ {
public:
    ~ExporterOBJ();
    void destroy();
    void init();
    void exportToFile(FBEntity file, std::vector<ModelFace*> faces);

private:
    void exportGeometry(std::vector<ModelFace*> faces);
    void exportMaterials(std::vector<ModelFace*> faces);
    void saveFile(std::string fileContents, std::string fileName);

    FBEntity exportFile;
    std::string nlDelimiter;
};

#endif /* ExporterOBJ_hpp */
