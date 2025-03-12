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
#include <imgui.h>
#include "kuplung/ui/components/ColorPicker.hpp"

class UIHelpers {
public:
  UIHelpers();
  void render();

  void addControlsXYZ(std::string const& property, bool showAnimate, bool doMinus, bool* isFrame, bool* animatedFlag, float* animatedValue, float step, float min, float limit) const;
  const bool addControlsSlider(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame) const;
  const bool addControlsFloatSlider(std::string const& title, const int idx, const float min, const float limit, float* animatedValue) const;
  const bool addControlsFloatSliderSameLine(std::string const& title, const int idx, const float min, const float limit, float* animatedValue) const;
  const bool addControlsIntegerSlider(std::string const& title, const int idx, const int min, const int limit, int* animatedValue) const;
  const bool addControlsIntegerSliderSameLine(std::string const& title, const int idx, const int min, const int limit, int* animatedValue) const;
  const bool addControlsSliderSameLine(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame) const;
  void addControlColor3(std::string const& title, glm::vec3* vValue, bool* bValue) const;
  void addControlColor4(std::string const& title, glm::vec4* vValue, bool* bValue) const;
  const bool addControlsDrag(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame) const;
  const bool addControlsIntegerDrag(std::string const& title, const int idx, const int min, const int limit, int* animatedValue) const;
  const bool addControlsFloatDrag(std::string const& title, const int idx, const float min, const float limit, float* animatedValue) const;
  const bool addControlsFloatDragSameLine(std::string const& title, const int idx, const float min, const float limit, float* animatedValue) const;
  const bool addControlsDragSameLine(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame) const;

private:
  std::unique_ptr<ColorPicker> componentColorPicker;

  void animateValue(bool* isFrame, bool* animatedFlag, float* animatedValue, const float step, const float limit, const bool doMinus) const;
  void animateValueAsync(bool* isFrame, const bool* animatedFlag, float* animatedValue, const float step, const float limit, const bool doMinus) const;
};

#endif /* UIHelpers_hpp */
