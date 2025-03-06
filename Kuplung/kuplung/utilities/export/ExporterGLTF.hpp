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
#include <nlohmann/json.hpp>
#include "kuplung/objects/ObjectsManager.hpp"

namespace KuplungApp { namespace Utilities { namespace Export {

class ExporterGLTF {
public:
  ~ExporterGLTF();
  void init(const std::function<void(float)>& doProgress);
  void exportToFile(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings, std::unique_ptr<ObjectsManager> &managerObjects);

	bool BufferInExternalFile;

private:
  std::function<void(float)> funcProgress;
  std::unique_ptr<ParserUtils> parserUtils;
	std::vector<std::string> objSettings;

	void prepFolderLocation();
	const nlohmann::json exportCameras(std::unique_ptr<ObjectsManager> &managerObjects) const;
	const nlohmann::json exportScenes(const std::vector<ModelFaceBase*>& faces) const;
	const nlohmann::json copyImage(std::string imagePath) const;

	const bool saveFile(const nlohmann::json& jsonObj) const;
	const bool saveBufferFile(std::string buffer) const;

  FBEntity exportFile;
  std::string nlDelimiter, exportFileFolder, defaultSceneName, defaultMaterialName;
  bool addSuffix = true;

	std::string gltfGenerator, gltfVersion;
};

}}}

#endif /* ExporterGLTF_hpp */
