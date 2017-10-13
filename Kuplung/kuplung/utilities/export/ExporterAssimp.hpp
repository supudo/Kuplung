//
//  ExporterAssimp.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 13/09/17.
//  Copyright © 2017 supudo.net. All rights reserved.
//

#ifndef ExporterAssimp_hpp
#define ExporterAssimp_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/utilities/parsers/ParserUtils.hpp"
#include "kuplung/utilities/json/json.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace KuplungApp { namespace Utilities { namespace Export {


class ExporterAssimp {
public:
	~ExporterAssimp();
	void init(const std::function<void(float)>& doProgress);
	void exportToFile(int exportFormat, const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings, std::unique_ptr<ObjectsManager> &managerObjects);

private:
	std::function<void(float)> funcProgress;
	std::unique_ptr<ParserUtils> parserUtils;
	std::unique_ptr<Assimp::Exporter> exporter;

	void saveFile(int exportFormat, aiScene* scene);

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

#endif /* ExporterAssimp_hpp */
