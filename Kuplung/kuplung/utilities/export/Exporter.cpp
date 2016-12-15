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
}

Exporter::Exporter() {
    this->exporterOBJ = std::make_unique<ExporterOBJ>();
}

void Exporter::init(std::function<void(float)> doProgress) {
    this->exporterOBJ->init(doProgress);
}

void Exporter::exportScene(FBEntity const& file, std::vector<ModelFaceBase*> faces, std::vector<std::string> const& settings) {
    this->exporterOBJ->exportToFile(file, faces, settings);
}

}}}
