//
//  Exporter.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Exporter.hpp"

Exporter::~Exporter() {
    this->destroy();
}

Exporter::Exporter() {
    this->exporterOBJ = std::make_unique<ExporterOBJ>();
}

void Exporter::destroy() {
    this->exporterOBJ.reset();
}

void Exporter::init(std::function<void(float)> doProgress) {
    this->exporterOBJ->init(doProgress);
}

void Exporter::exportScene(FBEntity file, std::vector<ModelFaceBase*> faces) {
    this->exporterOBJ->exportToFile(file, faces);
}
