//
//  DialogControlsModels.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef DialogControlsModels_hpp
#define DialogControlsModels_hpp

#include <regex>
#include <string>
#include <vector>
#include "kuplung/utilities/ImGui/imgui.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/ui/UIHelpers.hpp"
#include "kuplung/meshes/ModelFace.hpp"

class DialogControlsModels {
public:
    void init(ObjectsManager *managerObjects, std::function<void(std::string)> doLog);
    void render(bool* show, bool* isFrame, std::vector<ModelFace*> * meshModelFaces);

private:
    std::function<void(std::string)> funcDoLog;

    void contextModelRename(std::vector<ModelFace*> * meshModelFaces);
    void contextModelDelete(std::vector<ModelFace*> * meshModelFaces);

    bool cmenu_deleteYn, cmenu_renameModel;
    char guiModelRenameText[256];
    int selectedObject;
    int selectedTabScene, selectedTabGUICamera, selectedTabGUIGrid, selectedTabGUILight;

    ObjectsManager *managerObjects;
    UIHelpers *helperUI;
};

#endif /* DialogControlsModels_hpp */
