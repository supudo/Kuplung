//
//  Exporter.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Exporter.hpp"

namespace KuplungApp { namespace Utilities { namespace Export {

Exporter::~Exporter() {
    this->exporterOBJ.reset();
	this->exporterGLTF.reset();
}

Exporter::Exporter() {
    this->exporterOBJ = std::make_unique<ExporterOBJ>();
	this->exporterGLTF = std::make_unique<ExporterGLTF>();
}

void Exporter::init(const std::function<void(float)>& doProgress) {
    this->exporterOBJ->init(doProgress);
	this->exporterGLTF->init(doProgress);
}

void Exporter::exportScene(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings, std::unique_ptr<ObjectsManager> &managerObjects, ImportExportFormats exportFormat) {
	if (exportFormat == ImportExportFormat_OBJ)
		this->exporterOBJ->exportToFile(file, faces, settings, managerObjects);
	else if (exportFormat == ImportExportFormat_GLTF)
		this->exporterGLTF->exportToFile(file, faces, settings, managerObjects);
}

}}}
