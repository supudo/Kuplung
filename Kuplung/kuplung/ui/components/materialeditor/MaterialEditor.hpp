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
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/ui/components/materialeditor/MaterialEditorData.h"
#include "kuplung/ui/components/materialeditor/MELink.hpp"
#include "kuplung/ui/components/materialeditor/MENode.hpp"
#include "kuplung/ui/components/materialeditor/MENode_Color.hpp"
#include "kuplung/ui/components/materialeditor/MENode_Combine.hpp"
#include "kuplung/ui/components/materialeditor/MENode_Texture.hpp"

struct DragNode {
    ImVec2 pos;
    MENode* node;
    int inputSlotIndex, outputSlotIndex;
    DragNode() : node(NULL), inputSlotIndex(-1), outputSlotIndex(-1) {}
    bool isValid() const { return node && (inputSlotIndex >= 0 || outputSlotIndex >= 0); }
    void reset() { *this = DragNode(); }
};

class MaterialEditor {
public:
    void init();
    void draw(int selectedModelID, ModelFaceBase *face, bool* p_opened = NULL);

private:
    void initMaterialNodes(ModelFaceBase *face);

    bool inited = false;
    int selectedModelID;
    std::vector<MENode*> nodes;
    std::vector<MELink*> links;
    ImVec2 scrolling = ImVec2(0.0f, 0.0f);
    bool show_grid = true;
    int node_selected = -1;

    char nodeImagePathText[256];

    DragNode dragNode;
    ImColor style_LinkColor;
    float style_LinkThickness;
    bool style_ShowImages;
    float panelWidth_Nodes;
};

#endif /* MaterialEditor_hpp */
