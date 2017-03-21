//
//  CudaExamples.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifdef DEF_KuplungSetting_UseCuda

#include "CudaExamples.hpp"
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/utilities/imgui/imgui_internal.h"

CudaExamples::~CudaExamples() {
}

void CudaExamples::init() {
    this->selectedCudaExample = -1;

    this->exampleOceanFFT = std::make_unique<oceanFFT>();
    this->exampleOceanFFT->init();

    this->exampleVectorAddition = std::make_unique<VectorAddition>();
    this->exampleVectorAddition->init();
}

void CudaExamples::draw(bool* p_opened, glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid) {
    ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiSetCond_FirstUseEver);

    if (ImGui::Begin("Cuda Examples", p_opened, ImGuiWindowFlags_ShowBorders)) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Select example:");
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95);
        const char* cuda_examples[] = {"Adding vectors", "Ocean FFT"};
        ImGui::Combo("##cuda_examples", &this->selectedCudaExample, cuda_examples, IM_ARRAYSIZE(cuda_examples));
        ImGui::PopItemWidth();
        ImGui::Separator();

        this->renderExample(matrixProjection, matrixCamera, matrixGrid);

        ImGui::End();
    }
}

void CudaExamples::renderExample(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid) {
    switch (this->selectedCudaExample) {
        case 0:
            this->exampleVectorAddition->doVectorAddition();
            break;
        case 1:
            this->vboTexture = this->exampleOceanFFT->draw(matrixProjection, matrixCamera, matrixGrid);
            break;
    }

    if (this->vboTexture) {
        ImGui::Image((ImTextureID)(intptr_t)this->vboTexture, ImGui::GetWindowSize(), ImVec2(0,0), ImVec2(1,1), ImVec4(1,1,1,1), ImVec4(1,1,1,1));
    }
}

#endif
