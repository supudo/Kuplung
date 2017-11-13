//
//  KuplungIDE.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef KuplungIDE_hpp
#define KuplungIDE_hpp

#include "kuplung/utilities/imgui/ImGuiColorTextEdit/TextEditor.h"
#include "kuplung/meshes/scene/ModelFaceData.hpp"

class KuplungIDE {
public:
    void init();
    void draw(const char* title, bool* p_opened, std::vector<ModelFaceBase*> const& meshModelFaces);

private:
	TextEditor kuplungEditor;
	int selectedIndex;
	std::vector<std::string> meshesShadersList;

	void loadSelectedShader();
};

#endif /* KuplungIDE_hpp */
