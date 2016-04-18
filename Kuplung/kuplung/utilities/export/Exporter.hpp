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
#include "kuplung/meshes/ModelFace.hpp"
#include "kuplung/utilities/export/ExporterOBJ.hpp"

class Exporter {
public:
    ~Exporter();
    void init();
    void destroy();
    void exportScene(FBEntity file, std::vector<ModelFace*> faces);

private:
    ExporterOBJ* exporterOBJ;
};

#endif /* Exporter_hpp */