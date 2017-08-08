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

namespace KuplungApp { namespace Utilities { namespace Export {

class ExporterGLTF {
public:
    ~ExporterGLTF();
    void init(const std::function<void(float)>& doProgress);
    void exportToFile(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings);

private:
    std::function<void(float)> funcProgress;
    std::unique_ptr<ParserUtils> parserUtils;
	std::vector<std::string> objSettings;

    void exportGeometry(const std::vector<ModelFaceBase*>& faces);
    void exportMaterials(const std::vector<ModelFaceBase*>& faces);
    void saveFile(const std::string& fileContents, const std::string& fileName);

    FBEntity exportFile;
    std::string nlDelimiter;
    bool addSuffix = true;
};

}}}

#endif /* ExporterGLTF_hpp */
