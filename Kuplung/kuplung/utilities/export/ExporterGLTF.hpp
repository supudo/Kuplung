//
//  ExporterGLTF.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 07/08/17.
//  Copyright © 2017 supudo.net. All rights reserved.
//

#ifndef ExporterGLTF_hpp
#define ExporterGLTF_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/utilities/parsers/ParserUtils.hpp"
#include "kuplung/utilities/json/json.hpp"
#include "kuplung/objects/ObjectsManager.hpp"

namespace KuplungApp { namespace Utilities { namespace Export {

class ExporterGLTF {
public:
    ~ExporterGLTF();
    void init(const std::function<void(float)>& doProgress);
    void exportToFile(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings, std::unique_ptr<ObjectsManager> &managerObjects);

private:
    std::function<void(float)> funcProgress;
    std::unique_ptr<ParserUtils> parserUtils;
	std::vector<std::string> objSettings;

	void prepFolderLocation();
	nlohmann::json exportCameras(std::unique_ptr<ObjectsManager> &managerObjects);
	nlohmann::json exportScenes(const std::vector<ModelFaceBase*>& faces);
	nlohmann::json exportNodes(const std::vector<ModelFaceBase*>& faces);
	nlohmann::json exportMeshes(const std::vector<ModelFaceBase*>& faces);
	nlohmann::json exportAccessors(const std::vector<ModelFaceBase*>& faces);
	nlohmann::json exportMaterials(const std::vector<ModelFaceBase*>& faces);
	nlohmann::json exportTextures(const std::vector<ModelFaceBase*>& faces);
	nlohmann::json exportImages(const std::vector<ModelFaceBase*>& faces);
	nlohmann::json copyImage(std::string imagePath);
	nlohmann::json exportSamplers(const std::vector<ModelFaceBase*>& faces);
	nlohmann::json exportBufferViews(const std::vector<ModelFaceBase*>& faces);
	nlohmann::json exportBuffers(const std::vector<ModelFaceBase*>& faces, const FBEntity& file);

	void saveFile(const nlohmann::json& jsonObj);

    FBEntity exportFile;
    std::string nlDelimiter, exportFileFolder;
    bool addSuffix = true;
};

}}}

#endif /* ExporterGLTF_hpp */
