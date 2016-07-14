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

class SaveOpen {
public:
    void init();
    void saveKuplungFile(FBEntity file, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> meshModelFaces);
    void openKuplungFile(FBEntity file, ObjectsManager *managerObjects);

private:
    bool hasEnding(std::string const &fullString, std::string const &ending);

    template<typename T>
    std::ostream& binary_write(std::ostream& stream, const T& value);

    template<typename T>
    std::istream& binary_read(std::istream& stream, T& value);

    void binary_write_model(std::ostream& stream, MeshModel model);
    void binary_write_model_material_texture(std::ostream& stream, MeshMaterialTextureImage materialTexture);

    MeshModel binary_read_model(std::istream& stream);
    MeshMaterialTextureImage binary_read_model_material_texture(std::istream& stream);
};

#endif /* SaveOpen_hpp */
