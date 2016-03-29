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
#include "kuplung/ui/components/materialeditor/MENode.hpp"
#include "kuplung/ui/components/FileBrowser.hpp"

class MENode_Texture: public MENode {
public:
    MENode_Texture(int id, std::string name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename="", std::string textureImage="");
    virtual void draw(ImVec2 node_rect_min, ImVec2 NODE_WINDOW_PADDING, bool showPreview, float scale);

private:
    bool showTextureWindow, loadTexture, showFileBrowser;
    char filePath[256];
    GLuint vboBuffer;
    int textureWidth, textureHeight;

    FileBrowser *componentFileBrowser;

    void initBase(int id, std::string name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename="", std::string textureImage="");
    void showImage();
    void createTextureBuffer(int* width, int* height);
    void dialogFileBrowserProcessFile(FBEntity file);
    void doLog(std::string logMessage);
};

#endif /* MENode_Texture_hpp */
