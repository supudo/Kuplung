//
//  MENode_Color.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MENode_Color_hpp
#define MENode_Color_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/ui/components/materialeditor/MENode.hpp"

class MENode_Color: public MENode {
public:
  MENode_Color(int id, std::string const& name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string const& textureFilename="", std::string const& textureImage="");
  virtual void draw(ImVec2 node_rect_min, ImVec2 NODE_WINDOW_PADDING, bool showPreview, float scale);
};

#endif /* MENode_Color_hpp */
