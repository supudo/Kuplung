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
#include "kuplung/meshes/scene/ModelFaceBase.hpp"

class ExporterOBJ {
public:
    ~ExporterOBJ();
    void destroy();
    void init(std::function<void(float)> doProgress);
    void exportToFile(FBEntity file, std::vector<ModelFaceBase*> faces);

private:
    std::function<void(float)> funcProgress;

    void exportGeometry(std::vector<ModelFaceBase*> faces);
    void exportMaterials(std::vector<ModelFaceBase*> faces);
    void saveFile(std::string fileContents, std::string fileName);
    std::string exportMesh(ModelFaceBase *face);

    FBEntity exportFile;
    std::string nlDelimiter;

    std::vector<glm::vec3> uniqueVertices;
    std::vector<glm::vec2> uniqueTextureCoordinates;
    std::vector<glm::vec3> uniqueNormals;
    int vCounter = 1, vtCounter = 1, vnCounter = 1;
};

#endif /* ExporterOBJ_hpp */
