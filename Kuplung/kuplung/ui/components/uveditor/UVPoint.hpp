//
//  UVPoint.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef UVPoint_hpp
#define UVPoint_hpp

#include "kuplung/utilities/imgui/imgui.h"

class UVPoint {
public:
  UVPoint(int id, ImVec2 position, ImColor const& color, float radius);
  void draw(ImVec2 pointRect) const;

  int ID;
  ImVec2 position;
  ImColor color;
  float radius;
  bool isDragging;
};

#endif /* UVPoint_hpp */
