//
//  MENode_Texture.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MENode_Texture_hpp
#define MENode_Texture_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/meshes/ModelFace.hpp"
#include "kuplung/ui/components/materialeditor/MaterialEditorData.h"

class MENode_Texture {
public:
    void init(int id, const char* name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename = "", std::string textureImage="");

    int ID;
    MaterialEditor_NodeType NodeType;
    char Name[32];
    ImVec2 Pos, Size;
    float Value;
    ImVec4 Color;
    int InputsCount, OutputsCount;
    std::string TextureFilename, TextureImage;

    ImVec2 GetInputSlotPos(int slot_no) const;
    ImVec2 GetOutputSlotPos(int slot_no) const;
};

#endif /* MENode_Texture_hpp */
