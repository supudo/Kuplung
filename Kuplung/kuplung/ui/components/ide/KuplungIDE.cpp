//
//  KuplungIDE.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "KuplungIDE.hpp"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/settings/Settings.h"
#include <fstream>

void KuplungIDE::init() {
	this->selectedIndex = 0;

	if (Settings::Instance()->RendererType == InAppRendererType_Forward) {
		this->meshesShadersList.push_back("General - Vertex Shader");
		this->meshesShadersList.push_back("General - Geometry Shader");
		this->meshesShadersList.push_back("General - Tessellation Control Shader");
		this->meshesShadersList.push_back("General - Tessellation Evaluation Shader");
		this->meshesShadersList.push_back("General - Fragment Shader");
	}

	auto lang = TextEditor::LanguageDefinition::GLSL();

	static const char* keywords[] = {
		"attribute", "const", "uniform", "varying", "layout", "centroid", "flat", "smooth", "noperspective", "patch", "sample", "break",
		"continue", "do", "for", "while", "switch", "case", "default", "if", "else", "subroutine", "in", "out", "inout", "float", "double",
		"int", "void", "bool", "true", "false", "invariant", "discard", "return", "mat2", "mat3", "mat4", "dmat2", "dmat3", "dmat4",
		"mat2x2", "mat2x3", "mat2x4", "dmat2x2", "dmat2x3", "dmat2x4", "mat3x2", "mat3x3", "mat3x4", "dmat3x2", "dmat3x3", "dmat3x4",
		"mat4x2", "mat4x3", "mat4x4", "dmat4x2", "dmat4x3", "dmat4x4", "vec2", "vec3", "vec4", "ivec2", "ivec3", "ivec4", "bvec2",
		"bvec3", "bvec4", "dvec2", "dvec3", "dvec4", "uint", "uvec2", "uvec3", "uvec4", "lowp", "mediump", "highp", "precision",
		"sampler1D", "sampler2D", "sampler3D", "samplerCube", "sampler1DShadow", "sampler2DShadow", "samplerCubeShadow", "sampler1DArray",
		"sampler2DArray", "sampler1DArrayShadow", "sampler2DArrayShadow", "isampler1D", "isampler2D", "isampler3D", "isamplerCube",
		"isampler1DArray", "isampler2DArray", "usampler1D", "usampler2D", "usampler3D", "usamplerCube", "usampler1DArray",
		"usampler2DArray", "sampler2DRect", "sampler2DRectShadow", "isampler2DRect", "usampler2DRect", "samplerBuffer", "isamplerBuffer",
		"usamplerBuffer", "sampler2DMS", "isampler2DMS", "usampler2DMS", "sampler2DMSArray", "isampler2DMSArray", "usampler2DMSArray",
		"samplerCubeArray", "samplerCubeArrayShadow", "isamplerCubeArray", "usamplerCubeArray", "struct", "common", "partition", "active",
		"asm", "class", "union", "enum", "typedef", "template", "this", "packed", "goto", "inline", "noinline", "volatile", "public",
		"static", "extern", "external", "interface", "long", "short", "half", "fixed", "unsigned", "superp", "input", "output", "hvec2",
		"hvec3", "hvec4", "fvec2", "fvec3", "fvec4", "sampler3DRect", "filter", "image1D", "image2D", "image3D", "imageCube", "iimage1D",
		"iimage2D", "iimage3D", "iimageCube", "uimage1D", "uimage2D", "uimage3D", "uimageCube", "image1DArray", "image2DArray",
		"iimage1DArray", "iimage2DArray", "uimage1DArray", "uimage2DArray", "image1DShadow", "image2DShadow", "image1DArrayShadow",
		"image2DArrayShadow", "imageBuffer", "iimageBuffer", "uimageBuffer", "sizeof", "cast", "namespace", "using", "row_major"
	};
	for (auto i : keywords)
		lang.mKeywords.insert(i);

	this->kuplungEditor.SetLanguageDefinition(lang);
}

void KuplungIDE::draw(const char* title, bool* p_opened, std::vector<ModelFaceBase*> const& meshModelFaces) {
    ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiSetCond_FirstUseEver);

	ImGui::Begin(title, p_opened, ImGuiWindowFlags_ShowBorders);

	if (meshModelFaces.size() > 0) {
		std::vector<ModelFaceBase*>::const_iterator faceIterator;
		for (faceIterator = meshModelFaces.begin(); faceIterator != meshModelFaces.end(); ++faceIterator) {
			const ModelFaceBase& face = **faceIterator;
			std::string mt = face.meshModel.ModelTitle;
			this->meshesShadersList.push_back(mt);
		}
	}

	ImGui::Combo(
		"##001",
		&this->selectedIndex,
		[](void* vec, int idx, const char** out_text)
		{
			auto& vector = *static_cast<std::vector<std::string>*>(vec);
			if (idx < 0 || idx >= static_cast<int>(vector.size()))
				return false;
			*out_text = vector.at(idx).c_str();
			return true;
		},
		static_cast<void*>(&this->meshesShadersList),
		this->meshesShadersList.size()
	);
	ImGui::SameLine();
	if (ImGui::Button("Load Shader"))
		this->loadSelectedShader();
	ImGui::Separator();

	if (ImGui::BeginMenuBar())
		ImGui::EndMenuBar();

	auto cpos = this->kuplungEditor.GetCursorPosition();
	ImGui::Text("%6d/%-6d %6d lines  %s %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, this->kuplungEditor.GetTotalLines(),
		this->kuplungEditor.IsOverwrite() ? "Ovr" : "Ins",
		this->kuplungEditor.CanUndo() ? "*" : " ",
		this->kuplungEditor.GetLanguageDefinition().mName.c_str(), this->meshesShadersList[this->selectedIndex]);

	this->kuplungEditor.Render("Kuplung IDE");

    ImGui::End();
}

void KuplungIDE::loadSelectedShader() {
	std::string shaderFile = "";
	if (Settings::Instance()->RendererType == InAppRendererType_Forward) {
		if (this->selectedIndex == 0)
			shaderFile = Settings::Instance()->appFolder() + "/shaders/model_face.vert";
		else if (this->selectedIndex == 1)
			shaderFile = Settings::Instance()->appFolder() + "/shaders/model_face.geom";
		else if (this->selectedIndex == 2)
			shaderFile = Settings::Instance()->appFolder() + "/shaders/model_face.tcs";
		else if (this->selectedIndex == 3)
			shaderFile = Settings::Instance()->appFolder() + "/shaders/model_face.tes";
		std::ifstream t(shaderFile);
		std::string str((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		this->kuplungEditor.SetText(str);
	}
}
