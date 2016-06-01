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
    void init(std::function<void(float)> doProgress);
    void exportToFile(FBEntity file, std::vector<ModelFace*> faces);

private:
    std::function<void(float)> funcProgress;

    void exportGeometry(std::vector<ModelFace*> faces);
    void exportMaterials(std::vector<ModelFace*> faces);
    void saveFile(std::string fileContents, std::string fileName);
    int findInMap3(std::map<int, glm::vec3> m, glm::vec3 v);
    int findInMap2(std::map<int, glm::vec2> m, glm::vec2 v);
    std::string exportMesh(ModelFace *face);

    FBEntity exportFile;
    std::string nlDelimiter;

    std::map<int, glm::vec3> uniqueVertices;
    std::map<int, glm::vec2> uniqueTextureCoordinates;
    std::map<int, glm::vec3> uniqueNormals;
    int vCounter = 1, vtCounter = 1, vnCounter = 1;
};

#endif /* ExporterOBJ_hpp */
