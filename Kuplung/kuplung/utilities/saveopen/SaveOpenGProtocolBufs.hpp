//
//  SaveOpenGProtocolBufs.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SaveOpenGProtocolBufs_hpp
#define SaveOpenGProtocolBufs_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/utilities/saveopen/KuplungAppSettings.pb.h"
#include "kuplung/utilities/saveopen/KuplungAppScene.pb.h"

class SaveOpenGProtocolBufs {
public:
    void init();
    void saveKuplungFile(FBEntity file, std::unique_ptr<ObjectsManager> &managerObjects, std::vector<ModelFaceBase*> meshModelFaces);
    std::vector<ModelFaceData*> openKuplungFile(FBEntity file, std::unique_ptr<ObjectsManager> &managerObjects);

private:
    void storeObjectsManagerSettings(std::unique_ptr<ObjectsManager> &managerObjects);
    void readObjectsManagerSettings(std::unique_ptr<ObjectsManager> &managerObjects);

    void storeGlobalLights(std::unique_ptr<ObjectsManager> &managerObjects);
    void readGlobalLights(std::unique_ptr<ObjectsManager> &managerObjects);

    void storeObjects(std::vector<ModelFaceBase*> meshModelFaces);
    std::vector<ModelFaceData*> readObjects(std::unique_ptr<ObjectsManager> &managerObjects);

    KuplungApp::GUISettings bufGUISettings;
    KuplungApp::Scene bufScene;

    bool hasEnding(std::string const &fullString, std::string const &ending);

    KuplungApp::Vec4* getVec4(glm::vec4 v);
    KuplungApp::Vec3* getVec3(glm::vec3 v);
    KuplungApp::Vec2* getVec2(glm::vec2 v);

    glm::vec4 setVec4(const KuplungApp::Vec4& v);
    glm::vec3 setVec3(const KuplungApp::Vec3& v);
    glm::vec2 setVec2(const KuplungApp::Vec2& v);

    KuplungApp::ObjectCoordinate* getObjectCoordinate(ObjectCoordinate* v);
    KuplungApp::ObjectCoordinate* getObjectCoordinate2(ObjectCoordinate& v);
    ObjectCoordinate* setObjectCoordinate(const KuplungApp::ObjectCoordinate& v);
    std::unique_ptr<ObjectCoordinate> setObjectCoordinate2(const KuplungApp::ObjectCoordinate& v);

    KuplungApp::MaterialColor* getMaterialColor(MaterialColor* v);
    MaterialColor* setMaterialColor(const KuplungApp::MaterialColor& v);

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
