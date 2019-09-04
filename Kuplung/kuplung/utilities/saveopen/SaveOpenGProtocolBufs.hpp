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
  void readObjectsManagerSettings(std::unique_ptr<ObjectsManager>& managerObjects);

  void storeGlobalLights(std::unique_ptr<ObjectsManager>& managerObjects);
  void readGlobalLights(std::unique_ptr<ObjectsManager>& managerObjects);

  void storeObjects(std::vector<ModelFaceBase*> meshModelFaces);
  std::vector<ModelFaceData*> readObjects(std::unique_ptr<ObjectsManager>& managerObjects);

  KuplungApp::GUISettings bufGUISettings;
  KuplungApp::Scene bufScene;

  bool hasEnding(std::string const& fullString, std::string const& ending);

  KuplungApp::Vec4* getVec4(const glm::vec4& v);
  KuplungApp::Vec3* getVec3(const glm::vec3& v);

  glm::vec4 setVec4(const KuplungApp::Vec4& v);
  glm::vec3 setVec3(const KuplungApp::Vec3& v);

  KuplungApp::ObjectCoordinate* getObjectCoordinate(ObjectCoordinate& v);
  std::unique_ptr<ObjectCoordinate> setObjectCoordinate(const KuplungApp::ObjectCoordinate& v);

  KuplungApp::MaterialColor* getMaterialColor(MaterialColor& v);
  std::unique_ptr<MaterialColor> setMaterialColor(const KuplungApp::MaterialColor& v);

  KuplungApp::Mesh* getMesh(MeshModel ent);
  MeshModel setMesh(const KuplungApp::Mesh& ent);

  KuplungApp::MeshModelMaterial* getMeshModelMaterial(MeshModelMaterial ent);
  MeshModelMaterial setMeshModelMaterial(const KuplungApp::MeshModelMaterial& ent);

  KuplungApp::FBEntity* getFBEntity(FBEntity ent);
  FBEntity setFBEntity(const KuplungApp::FBEntity& ent);

  KuplungApp::MeshMaterialTextureImage* getMeshMaterialTextureImage(MeshMaterialTextureImage ent);
  MeshMaterialTextureImage setMeshMaterialTextureImage(const KuplungApp::MeshMaterialTextureImage& ent);
};

#endif /* SaveOpenGProtocolBufs_hpp */
