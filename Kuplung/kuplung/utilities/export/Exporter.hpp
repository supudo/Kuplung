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

namespace KuplungApp { namespace Utilities { namespace Export {

class Exporter {
public:
    ~Exporter();
    Exporter();
    void init(const std::function<void(float)>& doProgress);
    void exportScene(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings);

private:
    std::unique_ptr<ExporterOBJ> exporterOBJ;
};

}}}

#endif /* Exporter_hpp */
