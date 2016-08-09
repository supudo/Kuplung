//
//  DialogControlsGUI.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef DialogControlsGUI_hpp
#define DialogControlsGUI_hpp

#include <string>
#include <vector>
#include "kuplung/utilities/ImGui/imgui.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/ui/UIHelpers.hpp"

class DialogControlsGUI {
public:
    DialogControlsGUI(ObjectsManager &managerObjects);
    void init();
    void render(bool* show, bool* isFrame);

private:
    int selectedObject;
    int selectedObjectLight, selectedObjectArtefact;
    int selectedTabScene, selectedTabGUICamera, selectedTabGUICameraModel, selectedTabGUIGrid, selectedTabGUILight;
    int selectedTabGUITerrain, selectedTabGUISpaceship;
    float heightTopPanel = 170.0f;

    ObjectsManager &managerObjects;
    UIHelpers *helperUI;

    void setHeightmapImage(std::string heightmapImage);
    std::string heightmapImage;
    int heightmapWidth, heightmapHeight;
    GLuint vboTexHeightmap;
    bool newHeightmap, generateNewTerrain, generateNewSpaceship;
};

#endif /* DialogControlsGUI_hpp */
