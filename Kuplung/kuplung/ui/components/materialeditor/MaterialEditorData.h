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

//struct MaterialEditor_NodeLink {
//    int InputIdx, InputSlot, OutputIdx, OutputSlot;
//    MaterialEditor_NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
//};

#endif /* MaterialEditorData_hpp */
