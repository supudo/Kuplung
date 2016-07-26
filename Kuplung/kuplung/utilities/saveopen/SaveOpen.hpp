//
//  SaveOpen.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SaveOpen_hpp
#define SaveOpen_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/utilities/saveopen/SaveOpenBinarySeq.hpp"
#include "kuplung/utilities/saveopen/SaveOpenGProtocolBufs.hpp"

class SaveOpen {
public:
    void init();
    void saveKuplungFile(FBEntity file, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> meshModelFaces);
    std::vector<ModelFaceData*> openKuplungFile(FBEntity file, ObjectsManager *managerObjects);

private:
    SaveOpenBinarySeq *entBinarySeq;
    SaveOpenGProtocolBufs *entGProtocolBufs;
};

#endif /* SaveOpen_hpp */
