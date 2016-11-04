//
//  Exporter.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Exporter_hpp
#define Exporter_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/utilities/export/ExporterOBJ.hpp"

class Exporter {
public:
    ~Exporter();
    Exporter();
    void init(std::function<void(float)> doProgress);
    void exportScene(FBEntity file, std::vector<ModelFaceBase*> faces, std::vector<std::string> settings);

private:
    std::unique_ptr<ExporterOBJ> exporterOBJ;
};

#endif /* Exporter_hpp */
