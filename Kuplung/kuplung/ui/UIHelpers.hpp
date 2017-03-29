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
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/ui/components/ColorPicker.hpp"

class UIHelpers {
public:
    UIHelpers();
    void render();

    void addControlsXYZ(std::string const& property, bool showAnimate, bool doMinus, bool* isFrame, bool* animatedFlag, float* animatedValue, float step, float min, float limit);
    bool addControlsSlider(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame);
    bool addControlsFloatSlider(std::string const& title, const int idx, const float min, const float limit, float* animatedValue);
    bool addControlsFloatSliderSameLine(std::string const& title, const int idx, const float min, const float limit, float* animatedValue);
    bool addControlsIntegerSlider(std::string const& title, const int idx, const int min, const int limit, int* animatedValue);
    bool addControlsIntegerSliderSameLine(std::string const& title, const int idx, const int min, const int limit, int* animatedValue);
    bool addControlsSliderSameLine(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame);
    void addControlColor3(std::string const& title, glm::vec3* vValue, bool* bValue);
    void addControlColor4(std::string const& title, glm::vec4* vValue, bool* bValue);
    bool addControlsDrag(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame);
    bool addControlsIntegerDrag(std::string const& title, const int idx, const int min, const int limit, int* animatedValue);
    bool addControlsFloatDrag(std::string const& title, const int idx, const float min, const float limit, float* animatedValue);
    bool addControlsFloatDragSameLine(std::string const& title, const int idx, const float min, const float limit, float* animatedValue);
    bool addControlsDragSameLine(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame);

private:
    std::unique_ptr<ColorPicker> componentColorPicker;

    void animateValue(bool* isFrame, bool* animatedFlag, float* animatedValue, const float step, const float limit, const bool doMinus);
    void animateValueAsync(bool* isFrame, bool* animatedFlag, float* animatedValue, const float step, const float limit, const bool doMinus);
};

#endif /* UIHelpers_hpp */
