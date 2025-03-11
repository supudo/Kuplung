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
	this->exporterAssimp.reset();
  this->exporterOBJ.reset();
	this->exporterGLTF.reset();
}

Exporter::Exporter() {
	this->exporterAssimp = std::make_unique<ExporterAssimp>();
  this->exporterOBJ = std::make_unique<ExporterOBJ>();
	this->exporterGLTF = std::make_unique<ExporterGLTF>();
}

void Exporter::init(const std::function<void(float)>& doProgress) {
	this->exporterAssimp->init(doProgress);
  this->exporterOBJ->init(doProgress);
	this->exporterGLTF->init(doProgress);
}

void Exporter::exportScene(const ::FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings, std::unique_ptr<ObjectsManager> &managerObjects, ImportExportFormats exportFormat, int exportFormatAssimp) {
	if (exportFormat != ImportExportFormat_UNDEFINED) {
		if (Settings::Instance()->ModelFileParser == Importer_ParserType_Own) {
			if (exportFormat == ImportExportFormat_OBJ)
				this->exporterOBJ->exportToFile(file, faces, settings, managerObjects);
			else if (exportFormat == ImportExportFormat_GLTF)
				this->exporterGLTF->exportToFile(file, faces, settings, managerObjects);
		}
		else
			this->exporterAssimp->exportToFile(exportFormatAssimp, file, faces, settings, managerObjects);
	}
	else if (exportFormatAssimp > -1)
		this->exporterAssimp->exportToFile(exportFormatAssimp, file, faces, settings, managerObjects);
}

}}}
