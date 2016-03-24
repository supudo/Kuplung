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
    MENode_Color(int id, std::string name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename="", std::string textureImage="");
};

#endif /* MENode_Color_hpp */
