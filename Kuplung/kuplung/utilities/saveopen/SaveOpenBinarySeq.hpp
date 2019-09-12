//
//  SaveOpenBinarySeq.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SaveOpenBinarySeq_hpp
#define SaveOpenBinarySeq_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"

class SaveOpenBinarySeq {
public:
  void init() const;
  void saveKuplungFile(const FBEntity& file, std::unique_ptr<ObjectsManager> &managerObjects, const std::vector<ModelFaceBase*>& meshModelFaces) const;
  const std::vector<ModelFaceData*> openKuplungFile(const FBEntity& file, std::unique_ptr<ObjectsManager> &managerObjects) const;

private:
  void storeObjectsManagerSettings(std::ostream& kuplungFile, std::unique_ptr<ObjectsManager> &managerObjects) const;
  void readObjectsManagerSettings(std::istream& kuplungFile, std::unique_ptr<ObjectsManager> &managerObjects) const;

  void storeGlobalLights(std::ostream& kuplungFile, std::unique_ptr<ObjectsManager> &managerObjects) const;
  void readGlobalLights(std::istream& kuplungFile, std::unique_ptr<ObjectsManager> &managerObjects) const;

  void storeObjects(std::ostream& kuplungFile, std::vector<ModelFaceBase*> meshModelFaces) const;
  const std::vector<ModelFaceData*> readObjects(std::istream& kuplungFile, std::unique_ptr<ObjectsManager> &managerObjects) const;

  void binary_write_model(std::ostream& stream, MeshModel model) const;
  void binary_write_model_material_texture(std::ostream& stream, MeshMaterialTextureImage materialTexture) const;

  const MeshModel binary_read_model(std::istream& stream) const;
  const MeshMaterialTextureImage binary_read_model_material_texture(std::istream& stream) const;

  const bool hasEnding(std::string const &fullString, std::string const &ending) const;

  template<typename T>
  const std::ostream& binary_write(std::ostream& stream, const T& value) const;

  template<typename T>
  const std::istream& binary_read(std::istream& stream, T& value) const;

  void binary_write_string(std::ostream& stream, std::string str) const;
  const std::string binary_read_string(std::istream& stream) const;
};

#endif /* SaveOpenBinarySeq_hpp */
