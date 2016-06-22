//
//  MENode.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MENode_hpp
#define MENode_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/meshes/scene/ModelFace.hpp"
#include "kuplung/ui/components/materialeditor/MaterialEditorData.h"

class MENode {
public:
    void init(int id, MaterialEditor_NodeType nodeType, std::string name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename = "", std::string textureImage="");
    virtual void draw(ImVec2 node_rect_min, ImVec2 NODE_WINDOW_PADDING, bool showPreview, float scale);

    ImVec2 GetInputSlotPos(int slot_no, float scale=1.0f) const;
    ImVec2 GetOutputSlotPos(int slot_no, float scale=1.0f) const;

    int ID, InputsCount, OutputsCount, NodeType;
    std::string Name, TextureFilename, TextureImage;
    ImVec2 Pos, Size;
    float Value;
    ImVec4 Color;
    bool IsExpanded;
};

#endif /* MENode_hpp */
