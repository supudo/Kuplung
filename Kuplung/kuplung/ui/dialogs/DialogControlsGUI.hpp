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
    void init(ObjectsManager *managerObjects, std::function<void(std::string)> doLog);
    void render(bool* show, bool* isFrame);

private:
    std::function<void(std::string)> funcDoLog;

    int selectedObject;
    int selectedObjectLight;
    int selectedTabScene, selectedTabGUICamera, selectedTabGUIGrid, selectedTabGUILight;
    float heightTopPanel = 170.0f;

    ObjectsManager *managerObjects;
    UIHelpers *helperUI;
};

#endif /* DialogControlsGUI_hpp */
