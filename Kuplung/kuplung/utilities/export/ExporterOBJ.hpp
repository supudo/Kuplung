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
#include "kuplung/utilities/parsers/ParserUtils.hpp"
#include "kuplung/objects/ObjectsManager.hpp"

namespace KuplungApp { namespace Utilities { namespace Export {

class ExporterOBJ {
public:
    ~ExporterOBJ();
    void init(const std::function<void(float)>& doProgress);
    void exportToFile(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings, std::unique_ptr<ObjectsManager> &managerObjects);

private:
    std::function<void(float)> funcProgress;

    std::unique_ptr<ParserUtils> parserUtils;

    void exportGeometry(const std::vector<ModelFaceBase*>& faces);
    void exportMaterials(const std::vector<ModelFaceBase*>& faces);

    void saveFile(const std::string& fileContents, const std::string& fileName);
    std::string exportMesh(const ModelFaceBase& face);

    FBEntity exportFile;
    std::string nlDelimiter;
    bool addSuffix = true;

    std::vector<glm::vec3> uniqueVertices;
    std::vector<glm::vec2> uniqueTextureCoordinates;
    std::vector<glm::vec3> uniqueNormals;
    int vCounter = 1, vtCounter = 1, vnCounter = 1;

    std::vector<std::string> objSettings;
};

}}}

#endif /* ExporterOBJ_hpp */
