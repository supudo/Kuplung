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

typedef enum MaterialEditor_TextureType {
    MaterialEditor_TextureType_Ambient,
    MaterialEditor_TextureType_Diffuse,
    MaterialEditor_TextureType_Dissolve,
    MaterialEditor_TextureType_Bump,
    MaterialEditor_TextureType_Specular,
    MaterialEditor_TextureType_SpecularExp,
    MaterialEditor_TextureType_Displacement
} MaterialEditor_TextureType;

#endif /* MaterialEditorData_hpp */
