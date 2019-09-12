//
//  SaveOpenGProtocolBufs.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SaveOpenGProtocolBufs_hpp
#define SaveOpenGProtocolBufs_hpp

#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/miniz/KuplungMiniz.hpp"
#include "kuplung/utilities/saveopen/KuplungAppScene.pb.h"
#include "kuplung/utilities/saveopen/KuplungAppSettings.pb.h"

class SaveOpenGProtocolBufs {
public:
  ~SaveOpenGProtocolBufs();
  void init();
  void saveKuplungFile(const FBEntity& file, std::unique_ptr<ObjectsManager>& managerObjects, const std::vector<ModelFaceBase*>& meshModelFaces);
  std::vector<ModelFaceData*> openKuplungFile(const FBEntity& file, std::unique_ptr<ObjectsManager>& managerObjects);

private:
  std::unique_ptr<KuplungApp::Utilities::Miniz::KuplungMiniz> managerZip;

  void storeObjectsManagerSettings(std::unique_ptr<ObjectsManager>& managerObjects);
  void readObjectsManagerSettings(std::unique_ptr<ObjectsManager>& managerObjects) const;

  void storeGlobalLights(std::unique_ptr<ObjectsManager>& managerObjects);
  void readGlobalLights(std::unique_ptr<ObjectsManager>& managerObjects) const;

  void storeObjects(std::vector<ModelFaceBase*> meshModelFaces);
  const std::vector<ModelFaceData*> readObjects(std::unique_ptr<ObjectsManager>& managerObjects) const;

  KuplungApp::GUISettings bufGUISettings;
  KuplungApp::Scene bufScene;

  const bool hasEnding(std::string const& fullString, std::string const& ending) const;

  KuplungApp::Vec4* getVec4(const glm::vec4& v) const;
  KuplungApp::Vec3* getVec3(const glm::vec3& v) const;

  glm::vec4 setVec4(const KuplungApp::Vec4& v) const;
  glm::vec3 setVec3(const KuplungApp::Vec3& v) const;

  KuplungApp::ObjectCoordinate* getObjectCoordinate(ObjectCoordinate& v) const;
  std::unique_ptr<ObjectCoordinate> setObjectCoordinate(const KuplungApp::ObjectCoordinate& v) const;

  KuplungApp::MaterialColor* getMaterialColor(MaterialColor& v) const;
  std::unique_ptr<MaterialColor> setMaterialColor(const KuplungApp::MaterialColor& v) const;

  KuplungApp::Mesh* getMesh(MeshModel ent) const;
  MeshModel setMesh(const KuplungApp::Mesh& ent) const;

  KuplungApp::MeshModelMaterial* getMeshModelMaterial(MeshModelMaterial ent) const ;
  MeshModelMaterial setMeshModelMaterial(const KuplungApp::MeshModelMaterial& ent) const ;

  KuplungApp::FBEntity* getFBEntity(FBEntity ent) const;
  FBEntity setFBEntity(const KuplungApp::FBEntity& ent) const;

  KuplungApp::MeshMaterialTextureImage* getMeshMaterialTextureImage(MeshMaterialTextureImage ent) const;
  const MeshMaterialTextureImage setMeshMaterialTextureImage(const KuplungApp::MeshMaterialTextureImage& ent) const;
};

#endif /* SaveOpenGProtocolBufs_hpp */
