//
//  MaterialEditor.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MaterialEditor_hpp
#define MaterialEditor_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/meshes/ModelFace.hpp"
#include "kuplung/ui/components/materialeditor/MaterialEditorData.h"
#include "kuplung/ui/components/materialeditor/MENode.hpp"
#include "kuplung/ui/components/materialeditor/MENode_Color.hpp"
#include "kuplung/ui/components/materialeditor/MENode_Combine.hpp"
#include "kuplung/ui/components/materialeditor/MENode_Texture.hpp"

class MaterialEditor {
public:
    void init();
    void draw(ModelFace *face, bool* p_opened = NULL);

private:
    void initMaterialNodes(ModelFace *face);

    std::vector<MENode*> nodes;
    ImVector<MaterialEditor_NodeLink> links;
    bool inited = false;
    ImVec2 scrolling = ImVec2(0.0f, 0.0f);
    bool show_grid = true;
    int node_selected = -1;

    char nodeImagePathText[256];

    ImColor style_LinkColor;
    float style_LinkThickness;
    bool style_ShowImages;
    float panelWidth_Nodes;
};

#endif /* MaterialEditor_hpp */
