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

class SaveOpenGProtocolBufs {
public:
    void init();
    void saveKuplungFile(FBEntity file, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> meshModelFaces);
    std::vector<ModelFaceData*> openKuplungFile(FBEntity file, ObjectsManager *managerObjects);

private:
    void storeObjectsManagerSettings(std::ostream& kuplungFile, ObjectsManager *managerObjects);
    void readObjectsManagerSettings(std::istream& kuplungFile, ObjectsManager *managerObjects);

    void storeGlobalLights(std::ostream& kuplungFile, ObjectsManager *managerObjects);
    void readGlobalLights(std::istream& kuplungFile, ObjectsManager *managerObjects);

    void storeObjects(std::ostream& kuplungFile, std::vector<ModelFaceBase*> meshModelFaces);
    std::vector<ModelFaceData*> readObjects(std::istream& kuplungFile, ObjectsManager *managerObjects);

    bool hasEnding(std::string const &fullString, std::string const &ending);

    KuplungApp::Vec4* getVec4(glm::vec4 v);
    KuplungApp::Vec3* getVec3(glm::vec3 v);
    KuplungApp::Vec2* getVec2(glm::vec2 v);

    glm::vec4 setVec4(const KuplungApp::Vec4& v);
    glm::vec3 setVec3(const KuplungApp::Vec3& v);
    glm::vec2 setVec2(const KuplungApp::Vec2& v);

//    KuplungApp::FloatMatrix* getMatrix(glm::mat4 m);
//    glm::mat4 setMatrix(const KuplungApp::FloatMatrix& m);

    KuplungApp::ObjectCoordinate* getObjectCoordinate(ObjectCoordinate* v);
    ObjectCoordinate* setObjectCoordinate(const KuplungApp::ObjectCoordinate& v);
};

#endif /* SaveOpenGProtocolBufs_hpp */
