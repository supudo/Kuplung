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
  void saveKuplungFile(const FBEntity& file, std::unique_ptr<ObjectsManager> &managerObjects, const std::vector<ModelFaceBase*>& meshModelFaces);
  const std::vector<ModelFaceData*> openKuplungFile(const FBEntity& file, std::unique_ptr<ObjectsManager> &managerObjects) const;

private:
  std::unique_ptr<SaveOpenBinarySeq> entBinarySeq;
  std::unique_ptr<SaveOpenGProtocolBufs> entGProtocolBufs;
};

#endif /* SaveOpen_hpp */
