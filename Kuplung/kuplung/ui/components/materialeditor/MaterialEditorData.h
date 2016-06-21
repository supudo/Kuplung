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
#include "kuplung/meshes/scene/Model.hpp"

typedef enum MaterialEditor_NodeType {
    MaterialEditor_NodeType_Combine,
    MaterialEditor_NodeType_Color,
    MaterialEditor_NodeType_Image
} MaterialEditor_NodeType;

#endif /* MaterialEditorData_hpp */
