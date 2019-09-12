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
		// types
		"float", "double", "int", "void", "bool", "true", "false", "mat2", "mat3", "mat4", "dmat2", "dmat3", "dmat4",
		"mat2x2", "mat2x3", "mat2x4", "dmat2x2", "dmat2x3", "dmat2x4", "mat3x2", "mat3x3", "mat3x4", "dmat3x2",
		"dmat3x3", "dmat3x4", "mat4x2", "mat4x3", "mat4x4", "dmat4x2", "dmat4x3", "dmat4x4", "vec2", "vec3", "vec4",
		"ivec2", "ivec3", "ivec4", "bvec2", "bvec3", "bvec4", "dvec2", "dvec3", "dvec4", "uint", "uvec2", "uvec3",
		"uvec4", "sampler1D", "sampler2D", "sampler3D", "samplerCube", "sampler1DShadow", "sampler2DShadow",
		"samplerCubeShadow", "sampler1DArray", "sampler2DArray", "sampler1DArrayShadow", "sampler2DArrayShadow",
		"isampler1D", "isampler2D", "isampler3D", "isamplerCube", "isampler1DArray", "isampler2DArray", "usampler1D",
		"usampler2D", "usampler3D", "usamplerCube", "usampler1DArray", "usampler2DArray", "sampler2DRect",
		"sampler2DRectShadow", "isampler2DRect", "usampler2DRect", "samplerBuffer", "isamplerBuffer", "usamplerBuffer",
		"sampler2DMS", "isampler2DMS", "usampler2DMS", "sampler2DMSArray", "isampler2DMSArray", "usampler2DMSArray",
		"samplerCubeArray", "samplerCubeArrayShadow", "isamplerCubeArray", "usamplerCubeArray", "image1D", "iimage1D",
		"uimage1D", "image2D", "iimage2D", "uimage2D", "image3D", "iimage3D", "uimage3D", "image2DRect",
		"iimage2DRect", "uimage2DRect", "imageCube", "iimageCube", "uimageCube", "imageBuffer", "iimageBuffer",
		"uimageBuffer", "image1DArray", "iimage1DArray", "uimage1DArray", "image2DArray", "iimage2DArray", "uimage2DArray",
		"imageCubeArray", "iimageCubeArray", "uimageCubeArray", "image2DMS", "iimage2DMS", "uimage2DMS", "image2DMSArray",
		"iimage2DMSArray", "uimage2DMSArray", "long", "short", "half", "fixed", "unsigned", "hvec2", "hvec3", "hvec4",
		"fvec2", "fvec3", "fvec4", "sampler3DRect",
		// modifiers
		"attribute", "const", "uniform", "varying", "buffer", "shared", "coherent", "volatile", "restrict", "readonly",
		"writeonly", "atomic_uint", "layout", "centroid", "flat", "smooth", "noperspective", "patch", "sample", "break",
		"continue", "do", "for", "while", "switch", "case", "default", "if", "else", "subroutine", "in", "out", "inout",
		"invariant", "discard", "return", "lowp", "mediump", "highp", "precision", "struct", "common", "partition",
		"active", "asm", "class", "union", "enum", "typedef", "template", "this", "packed", "resource", "goto", "inline",
		"noinline", "public", "static", "extern", "external", "interface", "superp", "input", "output", "filter",
		"sizeof", "cast", "namespace", "using", "row_major", "early_fragment_tests", "varying", "attribute",
		// deprecated
		"gl_FragColor", "gl_FragData", "gl_MaxVarying", "gl_MaxVaryingFloats", "gl_MaxVaryingComponents",
		//
		"shared", "packed", "std140", "row_major", "column_major", "ccw", "cw", "equal_spacing", "fractional_even_spacing",
		"fractional_odd_spacing", "index", "invocations", "isolines", "line_strip", "lines", "lines_adjacency", "location",
		"binding", "max_vertices", "origin_upper_left", "pixel_center_integer", "point_mode", "points", "quads", "stream",
		"triangle_strip", "triangles", "triangles_adjacency", "vertices", "local_size_x", "local_size_y", "local_size_z",
		"std430"
	};
	for (auto i : keywords)
		lang.mKeywords.insert(i);


	static const char* const identifiers[] = {
		"abs", "acos", "acosh", "all", "any", "asin", "asinh", "atan", "atanh", "atomicCounter", "atomicCounterDecrement",
		"atomicCounterIncrement", "barrier", "bitCount", "bitfieldExtract", "bitfieldInsert", "bitfieldReverse", "ceil",
		"clamp", "cos", "cosh", "cross", "degrees", "determinant", "dFdx", "dFdy", "dFdyFine", "dFdxFine", "dFdyCoarse",
		"dFdxCourse", "fwidthFine", "fwidthCoarse", "distance", "dot", "EmitStreamVertex", "EmitVertex", "EndPrimitive",
		"EndStreamPrimitive", "equal", "exp", "exp2", "faceforward", "findLSB", "findMSB", "floatBitsToInt", "floatBitsToUint",
		"floor", "fma", "fract", "frexp", "fwidth", "greaterThan", "greaterThanEqual", "imageAtomicAdd", "imageAtomicAnd",
		"imageAtomicCompSwap", "imageAtomicExchange", "imageAtomicMax", "imageAtomicMin", "imageAtomicOr", "imageAtomicXor",
		"imageLoad", "imageSize", "imageStore", "imulExtended", "intBitsToFloat", "imageSamples", "interpolateAtCentroid",
		"interpolateAtOffset", "interpolateAtSample", "inverse", "inversesqrt", "isinf", "isnan", "ldexp", "length", "lessThan",
		"lessThanEqual", "log", "log2", "matrixCompMult", "max", "memoryBarrier", "min", "mix", "mod", "modf", "noise",
		"normalize", "not", "notEqual", "outerProduct", "packDouble2x32", "packHalf2x16", "packSnorm2x16", "packSnorm4x8",
		"packUnorm2x16", "packUnorm4x8", "pow", "radians", "reflect", "refract", "round", "roundEven", "sign", "sin", "sinh",
		"smoothstep", "sqrt", "step", "Tan", "tanh", "texelFetch", "texelFetchOffset", "texture", "textureGather",
		"textureGatherOffset", "textureGatherOffsets", "textureGrad", "textureGradOffset", "textureLod", "textureLodOffset",
		"textureOffset", "textureProj", "textureProjGrad", "textureProjGradOffset", "textureProjLod", "textureProjLodOffset",
		"textureProjOffset", "textureQueryLevels", "textureQueryLod", "textureSize", "transpose", "trunc", "uaddCarry",
		"uintBitsToFloat", "umulExtended", "unpackDouble2x32", "unpackHalf2x16", "unpackSnorm2x16", "unpackSnorm4x8",
		"unpackUnorm2x16", "unpackUnorm4x8", "usubBorrow",
		// deprecated
		"texture1D", "texture1DProj", "texture1DLod", "texture1DProjLod", "texture2D", "texture2DProj", "texture2DLod", "texture2DProjLod",
		"texture2DRect", "texture2DRectProj", "texture3D", "texture3DProj", "texture3DLod", "texture3DProjLod", "shadow1D", "shadow1DProj",
		"shadow1DLod", "shadow1DProjLod","shadow2D", "shadow2DProj", "shadow2DLod", "shadow2DProjLod", "textureCube", "textureCubeLod"
	};
	for (int i=0; i<sizeof(identifiers) / sizeof(identifiers[0]); i++) {
		TextEditor::Identifier teid;
		teid.mDeclaration = "";
		lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), teid));
  }

	static const char* ppnames[] = {
		"define", "undef", "if", "ifdef", "ifndef", "else", "elif", "endif", "error", "pragma", "extension", "version", "line"
	};
	for (int i=0; i<sizeof(ppnames) / sizeof(ppnames[0]); i++) {
		TextEditor::Identifier teid;
		teid.mDeclaration = "";
		lang.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), teid));
  }

	this->kuplungEditor.SetLanguageDefinition(lang);
}

void KuplungIDE::draw(const char* title, bool* p_opened, std::vector<ModelFaceBase*> const& meshModelFaces, ObjectsManager &managerObjects) {
  ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height), ImGuiSetCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiSetCond_FirstUseEver);

	auto cpos = this->kuplungEditor.GetCursorPosition();
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(.13f, .13f, .13f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	/// MIGRATE : ImGui::Begin(title, p_opened, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	ImGui::Begin(title, p_opened);

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
		this->loadSelectedShader(managerObjects);
	ImGui::Separator();

	if (ImGui::Button("Compile Shaders", ImVec2(-1.0f, 40.0))) {
		if (Settings::Instance()->RendererType == InAppRendererType_Forward) {
			if (this->selectedIndex == 0)
				(&managerObjects)->shaderSourceVertex = this->kuplungEditor.GetText();
			else if (this->selectedIndex == 1)
				(&managerObjects)->shaderSourceGeometry = this->kuplungEditor.GetText();
			else if (this->selectedIndex == 2)
				(&managerObjects)->shaderSourceTCS = this->kuplungEditor.GetText();
			else if (this->selectedIndex == 3)
				(&managerObjects)->shaderSourceTES = this->kuplungEditor.GetText();
			else if (this->selectedIndex == 4)
				(&managerObjects)->shaderSourceFragment = this->kuplungEditor.GetText();
			Settings::Instance()->shouldRecompileShaders = true;
		}
	}
	ImGui::Separator();

	if (ImGui::BeginMenuBar())
		ImGui::EndMenuBar();

	ImGui::Text("%6d/%-6d %6d lines  %s %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, this->kuplungEditor.GetTotalLines(),
		this->kuplungEditor.IsOverwrite() ? "Ovr" : "Ins",
		this->kuplungEditor.CanUndo() ? "*" : " ",
		this->kuplungEditor.GetLanguageDefinition().mName.c_str(), this->meshesShadersList[this->selectedIndex].c_str());

	ImGui::Separator();

	this->kuplungEditor.Render("Kuplung IDE");

  ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void KuplungIDE::loadSelectedShader(ObjectsManager &managerObjects) {
	std::string shaderSource = "";
	if (Settings::Instance()->RendererType == InAppRendererType_Forward) {
		if (this->selectedIndex == 0)
			shaderSource = managerObjects.shaderSourceVertex;
		else if (this->selectedIndex == 1)
			shaderSource = managerObjects.shaderSourceGeometry;
		else if (this->selectedIndex == 2)
			shaderSource = managerObjects.shaderSourceTCS;
		else if (this->selectedIndex == 3)
			shaderSource = managerObjects.shaderSourceTES;
		else if (this->selectedIndex == 4)
			shaderSource = managerObjects.shaderSourceFragment;
		this->kuplungEditor.SetText(shaderSource);
	}
}
