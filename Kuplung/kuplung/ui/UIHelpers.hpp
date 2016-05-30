//
//  UIHelpers.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef UIHelpers_hpp
#define UIHelpers_hpp

#include <regex>
#include <string>
#include <thread>
#include <vector>
#include <glm/glm.hpp>
#include "kuplung/utilities/ImGui/imgui.h"
#include "kuplung/ui/components/ColorPicker.hpp"

class UIHelpers {
public:
    UIHelpers();
    void render();

    void addControlsXYZ(std::string property, bool showAnimate, bool doMinus, bool* isFrame, bool* animatedFlag, float* animatedValue, float step, float min, float limit);
    bool addControlsSlider(std::string title, int idx, float step, float min, float limit, bool showAnimate, bool* animatedFlag, float* animatedValue, bool doMinus, bool* isFrame);
    bool addControlsFloatSlider(std::string title, int idx, float min, float limit, float* animatedValue);
    bool addControlsFloatSliderSameLine(std::string title, int idx, float min, float limit, float* animatedValue);
    bool addControlsIntegerSlider(std::string title, int idx, int min, int limit, int* animatedValue);
    bool addControlsSliderSameLine(std::string title, int idx, float step, float min, float limit, bool showAnimate, bool* animatedFlag, float* animatedValue, bool doMinus, bool* isFrame);
    void addControlColor3(std::string title, glm::vec3* vValue, bool* bValue);
    void addControlColor4(std::string title, glm::vec4* vValue, bool* bValue);
    bool addControlsDrag(std::string title, int idx, float step, float min, float limit, bool showAnimate, bool* animatedFlag, float* animatedValue, bool doMinus, bool* isFrame);
    bool addControlsIntegerDrag(std::string title, int idx, int min, int limit, int* animatedValue);
    bool addControlsFloatDrag(std::string title, int idx, float min, float limit, float* animatedValue);
    bool addControlsFloatDragSameLine(std::string title, int idx, float min, float limit, float* animatedValue);
    bool addControlsDragSameLine(std::string title, int idx, float step, float min, float limit, bool showAnimate, bool* animatedFlag, float* animatedValue, bool doMinus, bool* isFrame);

private:
    ColorPicker *componentColorPicker;

    void animateValue(bool* isFrame, bool* animatedFlag, float* animatedValue, float step, float limit, bool doMinus);
    void animateValueAsync(bool* isFrame, bool* animatedFlag, float* animatedValue, float step, float limit, bool doMinus);
};

#endif /* UIHelpers_hpp */
