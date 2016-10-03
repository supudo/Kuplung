//
//  DialogShadertoy.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef DialogShadertoy_hpp
#define DialogShadertoy_hpp

#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/ImGui/imgui.h"
#include "kuplung/meshes/artefacts/Shadertoy.hpp"

class DialogShadertoy {
public:
    void init();
    void render(bool* p_opened);

    GLuint vboTexture;
    float windowWidth, windowHeight;
    float viewPaddingHorizontal, viewPaddingVertical;

private:
    int textureWidth, textureHeight;
    ImVec2 scrolling = ImVec2(0.0f, 0.0f);

    std::unique_ptr<Shadertoy> engineShadertoy;
    float heightTopPanel, widthTexturesPanel, buttonCompileHeight;
    char shadertoyEditorText[1024 * 16];
    void compileShader();
    int texImage0, texImage1, texImage2, texImage3;

    std::string exec(const char* cmd);
    std::string paste();
};

#endif /* DialogShadertoy_hpp */
