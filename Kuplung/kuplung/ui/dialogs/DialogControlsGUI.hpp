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
#include "kuplung/settings/SettingsStructs.h"

class DialogControlsGUI {
public:
    explicit DialogControlsGUI(ObjectsManager &managerObjects);
    void render(bool* show, bool* isFrame);
    int selectedObject, selectedObjectLight;

private:
    int selectedObjectArtefact = -1;
    int selectedTabScene, selectedTabGUICamera, selectedTabGUICameraModel, selectedTabGUIGrid, selectedTabGUILight;
    int selectedTabGUITerrain, selectedTabGUISpaceship;
    float heightTopPanel = 170.0f;

    ObjectsManager &managerObjects;
    std::unique_ptr<UIHelpers> helperUI;

    void setHeightmapImage(std::string const& heightmapImage);
    std::string heightmapImage;
    int heightmapWidth, heightmapHeight;
    GLuint vboTexHeightmap;
    bool newHeightmap = false, generateNewTerrain = false, generateNewSpaceship = false;
    bool lockCameraWithLight;
    void lockCameraOnce();
    void lockCamera();

    float lightRotateX, lightRotateY, lightRotateZ;
};

#endif /* DialogControlsGUI_hpp */
