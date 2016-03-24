//
//  MaterialEditorData.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MaterialEditorData_hpp
#define MaterialEditorData_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/meshes/ModelFace.hpp"

typedef enum MaterialEditor_NodeType {
    MaterialEditor_NodeType_Combine,
    MaterialEditor_NodeType_Color,
    MaterialEditor_NodeType_Image
} MaterialEditor_NodeType;

struct MaterialEditor_Node {
    int ID;
    MaterialEditor_NodeType NodeType;
    char Name[32];
    ImVec2 Pos, Size;
    float Value;
    ImVec4 Color;
    int InputsCount, OutputsCount;
    std::string TextureFilename, TextureImage;

    MaterialEditor_Node(int id, MaterialEditor_NodeType nodeType, const char* name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename = "", std::string textureImage="") {
        ID = id;
        NodeType = nodeType;
        strncpy(Name, name, 31);
        Name[31] = 0;
        Pos = pos;
        Value = value;
        Color = color;
        InputsCount = inputs_count;
        OutputsCount = outputs_count;
        TextureFilename = textureFilename;
        TextureImage = textureImage;
    }

    ImVec2 GetInputSlotPos(int slot_no) const   { return ImVec2(Pos.x, Pos.y + Size.y * ((float)slot_no+1) / ((float)InputsCount+1)); }
    ImVec2 GetOutputSlotPos(int slot_no) const  { return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)slot_no+1) / ((float)OutputsCount+1)); }
};

struct MaterialEditor_NodeLink {
    int InputIdx, InputSlot, OutputIdx, OutputSlot;

    MaterialEditor_NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
};

#endif /* MaterialEditorData_hpp */
