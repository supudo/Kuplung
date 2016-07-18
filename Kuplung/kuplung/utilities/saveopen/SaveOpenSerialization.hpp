//
//  SaveOpenSerialization.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SaveOpenSerialization_hpp
#define SaveOpenSerialization_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"

class SaveOpenSerialization {
public:
    void init();
    void saveKuplungFile(FBEntity file, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> meshModelFaces);
    void openKuplungFile(FBEntity file, ObjectsManager *managerObjects);

private:
    std::string fileName;

    bool hasEnding(std::string const &fullString, std::string const &ending);

    void storeObjectsManagerSettings(ObjectsManager *managerObjects);
    void readObjectsManagerSettings(ObjectsManager *managerObjects);

    void storeGlobalLights(ObjectsManager *managerObjects);
    void readGlobalLights(ObjectsManager *managerObjects);
};

#endif /* SaveOpenSerialization_hpp */
