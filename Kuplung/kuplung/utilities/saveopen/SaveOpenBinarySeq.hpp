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

    void binary_write_model(std::ostream& stream, MeshModel model);
    void binary_write_model_material_texture(std::ostream& stream, MeshMaterialTextureImage materialTexture);

    MeshModel binary_read_model(std::istream& stream);
    MeshMaterialTextureImage binary_read_model_material_texture(std::istream& stream);


    bool hasEnding(std::string const &fullString, std::string const &ending);

    template<typename T>
    std::ostream& binary_write(std::ostream& stream, const T& value);

    template<typename T>
    std::istream& binary_read(std::istream& stream, T& value);

    void binary_write_string(std::ostream& stream, std::string str);
    std::string binary_read_string(std::istream& stream);
};

#endif /* SaveOpenBinarySeq_hpp */
