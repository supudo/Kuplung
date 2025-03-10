//
//  DialogControlsGUI.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "DialogControlsGUI.hpp"
#include "kuplung/ui/components/Tabs.hpp"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"
#include "kuplung/utilities/imgui/imgui_internal.h"

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

DialogControlsGUI::DialogControlsGUI(ObjectsManager& managerObjects)
    : managerObjects(managerObjects)
    , helperUI(std::make_unique<UIHelpers>()) {

  this->selectedObject = 0;
  this->selectedObjectLight = -1;
  this->selectedTabScene = -1;
  this->selectedTabGUICamera = -1;
  this->selectedTabGUICameraModel = -1;
  this->selectedTabGUIGrid = -1;
  this->selectedTabGUILight = -1;
  this->selectedTabGUITerrain = -1;
  this->selectedTabGUISpaceship = -1;
  this->lightRotateX = 0.0f;
  this->lightRotateY = 0.0f;
  this->lightRotateZ = 0.0f;

  this->heightmapWidth = 0;
  this->heightmapHeight = 0;

  this->newHeightmap = false;
  this->generateNewTerrain = false;
  this->generateNewSpaceship = false;
  this->lockCameraWithLight = false;
}

void DialogControlsGUI::drawGlobalItems() {
  for (int i = 0; i < 8; i++) {
    switch (i) {
      case 0: {
        ImGui::Indent();
        if (ImGui::Selectable((char*)ICON_FA_ASTERISK " General", this->selectedObject == i)) {
          this->selectedObject = i;
          this->selectedObjectLight = -1;
        }
        ImGui::Unindent();
        break;
      }
      case 1: {
        ImGui::Indent();
        if (ImGui::Selectable((char*)ICON_FA_EYE " Camera", this->selectedObject == i)) {
          this->selectedObject = i;
          this->selectedObjectLight = -1;
        }
        ImGui::Unindent();
        break;
      }
      case 2: {
        ImGui::Indent();
        if (ImGui::Selectable((char*)ICON_FA_VIDEO_CAMERA " Camera ModelFace", this->selectedObject == i)) {
          this->selectedObject = i;
          this->selectedObjectLight = -1;
        }
        ImGui::Unindent();
        break;
      }
      case 3: {
        ImGui::Indent();
        if (ImGui::Selectable((char*)ICON_FA_BARS " Grid", this->selectedObject == i)) {
          this->selectedObject = i;
          this->selectedObjectLight = -1;
        }
        ImGui::Unindent();
        break;
      }
      case 4: {
        ImGui::Indent();
        if (ImGui::Selectable((char*)ICON_FA_SUN_O " Scene Lights", this->selectedObject == i)) {
          this->selectedObject = i;
          this->selectedObjectLight = -1;
        }
        ImGui::Unindent();
        break;
      }
      case 5: {
        ImGui::Indent();
        if (ImGui::Selectable((char*)ICON_FA_TREE " Skybox", this->selectedObject == i)) {
          this->selectedObject = i;
          this->selectedObjectLight = -1;
        }
        ImGui::Unindent();
        break;
      }
      case 6: {
        if (this->managerObjects.lightSources.size() == 0) {
          ImGui::Indent();
          ImGui::Text((char*)ICON_FA_LIGHTBULB_O " Lights");
          ImGui::Unindent();
        }
        else {
          if (ImGui::TreeNode((char*)ICON_FA_LIGHTBULB_O " Lights")) {
            for (size_t j = 0; j < this->managerObjects.lightSources.size(); j++) {
              ImGui::Bullet();
              if (ImGui::Selectable(this->managerObjects.lightSources[j]->title.c_str(), this->selectedObjectLight == int(j))) {
                this->selectedObjectLight = int(j);
                this->selectedObject = 6;
              }
            }
            ImGui::TreePop();
          }
        }
        break;
      }
      case 7: {
        if (ImGui::TreeNode((char*)ICON_FA_LIGHTBULB_O " Artefacts")) {
          ImGui::Bullet();
          if (ImGui::Selectable("Terrain", this->selectedObjectArtefact == 0)) {
            this->selectedObjectArtefact = 0;
            this->selectedObject = 7;
          }
          ImGui::Bullet();
          if (ImGui::Selectable("Spaceship", this->selectedObjectArtefact == 1)) {
            this->selectedObjectArtefact = 1;
            this->selectedObject = 7;
          }
#ifdef DEF_KuplungSetting_UseCuda
          ImGui::Bullet();
          if (ImGui::Selectable("Cuda - Ocean FFT", this->selectedObjectArtefact == 2)) {
            this->selectedObjectArtefact = 2;
            this->selectedObject = 7;
          }
#endif
          ImGui::TreePop();
        }
        break;
      }
      default:
        break;
    }
  }
}

void DialogControlsGUI::drawPropertiesPane(bool* isFrame) {
  switch (this->selectedObject) {
    case 0: {
      if (ImGui::CollapsingHeader("View Options")) {
        ImGui::Indent();
        this->helperUI->addControlsSlider("Field of view", 1, 1.0f, -180.0f, 180.0f, false, NULL, &this->managerObjects.Setting_FOV, true, isFrame);
        ImGui::Separator();

        ImGui::Text("Ratio");
        if (ImGui::IsItemHovered())
          ImGui::SetTooltip("W & H");
        ImGui::SliderFloat("W##105", &this->managerObjects.Setting_RatioWidth, 0.0f, 5.0f);
        ImGui::SliderFloat("H##106", &this->managerObjects.Setting_RatioHeight, 0.0f, 5.0f);
        //                this->helperUI->addControlsFloatSliderSameLine("W", 105, 0.0f, 5.0f, &this->managerObjects.Setting_RatioWidth);
        //                this->helperUI->addControlsFloatSliderSameLine("H", 106, 0.0f, 5.0f, &this->managerObjects.Setting_RatioHeight);
        ImGui::Separator();

        ImGui::Text("Planes");
        if (ImGui::IsItemHovered())
          ImGui::SetTooltip("Far & Close");
        //                this->helperUI->addControlsFloatSliderSameLine("W", 291, 0.0f, 5.0f, &this->managerObjects.Setting_RatioWidth);
        ImGui::SliderFloat("Close##108", &this->managerObjects.Setting_PlaneClose, 0.0f, 1.0f);
        ImGui::SliderFloat("Far##107", &this->managerObjects.Setting_PlaneFar, 0.0f, 1000.0f);
        ImGui::Separator();

        ImGui::Text("Gamma");
        if (ImGui::IsItemHovered())
          ImGui::SetTooltip("Gamma Correction");
        ImGui::SliderFloat("##109", &this->managerObjects.Setting_GammaCoeficient, 1.0f, 4.0f);
        ImGui::Unindent();
      }

      if (ImGui::CollapsingHeader("Editor Artefacts")) {
        ImGui::Indent();
        ImGui::Checkbox("Axis Helpers", &this->managerObjects.Setting_ShowAxisHelpers);
        ImGui::Checkbox("Z Axis", &this->managerObjects.Settings_ShowZAxis);
        ImGui::Unindent();
      }

      if (ImGui::CollapsingHeader("Rays")) {
        ImGui::Indent();
        if (ImGui::Checkbox("Show Rays", &Settings::Instance()->showPickRays))
          Settings::Instance()->saveSettings();
        if (ImGui::Checkbox("Single Ray", &Settings::Instance()->showPickRaysSingle))
          Settings::Instance()->saveSettings();
        if (ImGui::CollapsingHeader("Add Ray", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Indent();
          ImGui::Text("Origin");
          if (ImGui::Button("Set to camera position", ImVec2(ImGui::GetWindowWidth() * 0.75f, 0))) {
            Settings::Instance()->mRayOriginX = managerObjects.camera->positionX->point;
            Settings::Instance()->mRayOriginY = managerObjects.camera->positionY->point;
            Settings::Instance()->mRayOriginZ = managerObjects.camera->positionZ->point;
          }
          ImGui::InputFloat("X##9920", &Settings::Instance()->mRayOriginX, 0, 0, 8);
          ImGui::InputFloat("Y##9921", &Settings::Instance()->mRayOriginY, 0, 0, 8);
          ImGui::InputFloat("Z##9922", &Settings::Instance()->mRayOriginZ, 0, 0, 8);
          ImGui::Checkbox("Animate", &Settings::Instance()->mRayAnimate);
          if (Settings::Instance()->mRayAnimate) {
            ImGui::Text("Direction");
            ImGui::SliderFloat("X##9930", &Settings::Instance()->mRayDirectionX, -1.0f, 1.0);
            ImGui::SliderFloat("Y##9931", &Settings::Instance()->mRayDirectionY, -1.0f, 1.0f);
            ImGui::SliderFloat("Z##9932", &Settings::Instance()->mRayDirectionZ, -1.0f, 1.0f);
          }
          else {
            ImGui::Text("Direction");
            ImGui::InputFloat("X##9930", &Settings::Instance()->mRayDirectionX, 0, 0, 8);
            ImGui::InputFloat("Y##9931", &Settings::Instance()->mRayDirectionY, 0, 0, 8);
            ImGui::InputFloat("Z##9932", &Settings::Instance()->mRayDirectionZ, 0, 0, 8);
            if (ImGui::Button("Draw", ImVec2(ImGui::GetWindowWidth() * 0.75f, 0)))
              Settings::Instance()->mRayDraw = true;
          }
          ImGui::Unindent();
        }
        ImGui::Unindent();
      }

      if (ImGui::CollapsingHeader("Bounding Box")) {
        ImGui::Indent();
        if (ImGui::Checkbox("Bounding Box", &Settings::Instance()->ShowBoundingBox))
          Settings::Instance()->saveSettings();

        if (Settings::Instance()->ShowBoundingBox) {
          if (this->helperUI->addControlsSlider("Padding", 3, 0.001f, 0.000f, 0.1f, false, NULL, &Settings::Instance()->BoundingBoxPadding, true, isFrame)) {
            Settings::Instance()->BoundingBoxRefresh = true;
            Settings::Instance()->saveSettings();
          }
          this->helperUI->addControlColor4("Color", &this->managerObjects.Setting_OutlineColor, &this->managerObjects.Setting_OutlineColorPickerOpen);
          this->helperUI->addControlsSlider("Thickness", 2, 1.01f, 0.0f, 2.0f, false, NULL, &this->managerObjects.Setting_OutlineThickness, true, isFrame);
        }
        ImGui::Unindent();
      }

      if (ImGui::CollapsingHeader("Edit Mode")) {
        ImGui::Indent();
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Manipulate mode:");
        ImGui::BeginGroup();
        float width = ImGui::GetContentRegionAvailWidth() * 0.3f;

        if (this->managerObjects.Setting_GeometryEditMode == GeometryEditMode_Vertex) {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(121 / 255.0f, 5 / 255.0f, 5 / 255.0f, 1.0f));
          if (ImGui::Button("Vertex", ImVec2(width, 0)))
            this->managerObjects.Setting_GeometryEditMode = GeometryEditMode_Vertex;
          ImGui::PopStyleColor();
        }
        else if (ImGui::Button("Vertex", ImVec2(width, 0)))
            this->managerObjects.Setting_GeometryEditMode = GeometryEditMode_Vertex;

        ImGui::SameLine();

        if (this->managerObjects.Setting_GeometryEditMode == GeometryEditMode_Line) {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(121 / 255.0f, 5 / 255.0f, 5 / 255.0f, 1.0f));
          if (ImGui::Button("Line", ImVec2(width, 0)))
            this->managerObjects.Setting_GeometryEditMode = GeometryEditMode_Line;
          ImGui::PopStyleColor();
        }
        else if (ImGui::Button("Line", ImVec2(width, 0)))
          this->managerObjects.Setting_GeometryEditMode = GeometryEditMode_Line;

        ImGui::SameLine();

        if (this->managerObjects.Setting_GeometryEditMode == GeometryEditMode_Face) {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(121 / 255.0f, 5 / 255.0f, 5 / 255.0f, 1.0f));
          if (ImGui::Button("Face", ImVec2(width, 0)))
            this->managerObjects.Setting_GeometryEditMode = GeometryEditMode_Face;
          ImGui::PopStyleColor();
        }
        else if (ImGui::Button("Face", ImVec2(width, 0)))
          this->managerObjects.Setting_GeometryEditMode = GeometryEditMode_Face;

        ImGui::EndGroup();
        ImGui::Separator();

        ImGui::Checkbox("Vertex Sphere", &this->managerObjects.Setting_VertexSphere_Visible);
        if (this->managerObjects.Setting_VertexSphere_Visible) {
          ImGui::Checkbox("Sphere", &this->managerObjects.Setting_VertexSphere_IsSphere);
          ImGui::Checkbox("Wireframes", &this->managerObjects.Setting_VertexSphere_ShowWireframes);
          this->helperUI->addControlsIntegerSlider("Segments", 67, 3, 32, &this->managerObjects.Setting_VertexSphere_Segments);
          this->helperUI->addControlsSlider("Radius", 1.0f, 0.5f, 0.0f, 2.0f, false, NULL, &this->managerObjects.Setting_VertexSphere_Radius, true, isFrame);
          this->helperUI->addControlColor4("Color", &this->managerObjects.Setting_VertexSphere_Color, &this->managerObjects.Setting_VertexSphere_ColorPickerOpen);
        }
        ImGui::Unindent();
      }

      if (ImGui::CollapsingHeader("Render Buffer", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();
        ImGui::Text("Rendering View Options");
        if (ImGui::Button("Depth Colors", ImVec2(-1, 0)))
          this->managerObjects.Setting_Rendering_Depth = !this->managerObjects.Setting_Rendering_Depth;
        if (ImGui::Button("Shadow Texture", ImVec2(-1, 0)))
          this->managerObjects.Setting_DebugShadowTexture = !this->managerObjects.Setting_DebugShadowTexture;
        ImGui::Unindent();
      }

      if (Settings::Instance()->RendererType == InAppRendererType_Deferred) {
        if (ImGui::CollapsingHeader("Deferred Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Indent();
          ImGui::Text("Deferred Rendering");

          std::vector<const char*> deferred_texture_items;
          deferred_texture_items.push_back("Lighting");
          deferred_texture_items.push_back("Position");
          deferred_texture_items.push_back("Normal");
          deferred_texture_items.push_back("Diffuse");
          deferred_texture_items.push_back("Specular");
          ImGui::Combo("##110", &this->managerObjects.Setting_LightingPass_DrawMode, &deferred_texture_items[0], int(deferred_texture_items.size()));

          ImGui::Text("Ambient Strength");
          ImGui::SliderFloat("##210", &this->managerObjects.Setting_DeferredAmbientStrength, 0.0f, 1.0f);

          ImGui::Checkbox("Test Mode", &this->managerObjects.Setting_DeferredTestMode);
          ImGui::Checkbox("Test Lights", &this->managerObjects.Setting_DeferredTestLights);
          if (ImGui::Button("Randomize Light Positions", ImVec2(-1, 0)))
            this->managerObjects.Setting_DeferredRandomizeLightPositions = true;
          ImGui::Separator();

          ImGui::Text("Number of Test Lights");
          ImGui::SliderInt("##209", &this->managerObjects.Setting_DeferredTestLightsNumber, 0, 32);
          ImGui::Unindent();
        }
      }
      break;
    }
    case 1: {
      ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.6f, 0.6f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.7f, 0.7f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f)));

      const char* tabsGUICamera[] = {
          (char*)"\n" ICON_MD_REMOVE_RED_EYE,
          (char*)"\n" ICON_MD_3D_ROTATION,
          (char*)"\n" ICON_MD_OPEN_WITH,
      };
      const char* tabsLabelsGUICamera[] = {"Look At", "Rotate", "Translate"};
      const int numTabsGUICamera = sizeof(tabsGUICamera) / sizeof(tabsGUICamera[0]);
      ImGui::TabLabels(numTabsGUICamera, tabsGUICamera, this->selectedTabGUICamera, ImVec2(30.0, 30.0), tabsLabelsGUICamera);
      ImGui::PopStyleColor(3);

      ImGui::Separator();

      switch (this->selectedTabGUICamera) {
        case 0: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Look-At matrix");
          ImGui::Separator();
          ImGui::Text("Eye");
          //                    this->helperUI->addControlsSliderSameLine("X", 1, 1.0f, -10.0f, 10.0f, false, NULL, &this->managerObjects.camera->eyeSettings->View_Eye.x, true, isFrame);
          //                    this->helperUI->addControlsSliderSameLine("Y", 2, 1.0f, this->managerObjects.Setting_PlaneClose, this->managerObjects.Setting_PlaneFar, false, NULL, &this->managerObjects.camera->eyeSettings->View_Eye.y, true, isFrame);
          //                    this->helperUI->addControlsSliderSameLine("Z", 3, 1.0f, 0.0f, 90.0f, false, NULL, &this->managerObjects.camera->eyeSettings->View_Eye.z, true, isFrame);
          this->helperUI->addControlsSliderSameLine("X", 1, 1.0f, -this->managerObjects.Setting_PlaneFar, this->managerObjects.Setting_PlaneFar, false, NULL, &this->managerObjects.camera->eyeSettings->View_Eye.x, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 2, 1.0f, -this->managerObjects.Setting_PlaneFar, this->managerObjects.Setting_PlaneFar, false, NULL, &this->managerObjects.camera->eyeSettings->View_Eye.y, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 3, 1.0f, -this->managerObjects.Setting_PlaneFar, this->managerObjects.Setting_PlaneFar, false, NULL, &this->managerObjects.camera->eyeSettings->View_Eye.z, true, isFrame);
          ImGui::Separator();
          ImGui::Text("Center");
          this->helperUI->addControlsSliderSameLine("X", 4, 1.0f, -10.0f, 10.0f, false, NULL, &this->managerObjects.camera->eyeSettings->View_Center.x, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 5, 1.0f, -10.0f, 10.0f, false, NULL, &this->managerObjects.camera->eyeSettings->View_Center.y, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 6, 1.0f, 0.0f, 45.0f, false, NULL, &this->managerObjects.camera->eyeSettings->View_Center.z, true, isFrame);
          ImGui::Separator();
          ImGui::Text("Up");
          this->helperUI->addControlsSliderSameLine("X", 7, 1.0f, -1.0f, 1.0f, false, NULL, &this->managerObjects.camera->eyeSettings->View_Up.x, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 8, 1.0f, -1.0f, 1.0f, false, NULL, &this->managerObjects.camera->eyeSettings->View_Up.y, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 9, 1.0f, -1.0f, 1.0f, false, NULL, &this->managerObjects.camera->eyeSettings->View_Up.z, true, isFrame);
          break;
        }
        case 1: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
          this->helperUI->addControlsSliderSameLine("X", 13, 1.0f, 0.0f, 360.0f, true, &this->managerObjects.camera->rotateX->animate, &this->managerObjects.camera->rotateX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 14, 1.0f, 0.0f, 360.0f, true, &this->managerObjects.camera->rotateY->animate, &this->managerObjects.camera->rotateY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 15, 1.0f, 0.0f, 360.0f, true, &this->managerObjects.camera->rotateZ->animate, &this->managerObjects.camera->rotateZ->point, true, isFrame);
          ImGui::Separator();
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around center");
          this->helperUI->addControlsSliderSameLine("X", 16, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.camera->rotateCenterX->animate, &this->managerObjects.camera->rotateCenterX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 17, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.camera->rotateCenterY->animate, &this->managerObjects.camera->rotateCenterY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 18, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.camera->rotateCenterZ->animate, &this->managerObjects.camera->rotateCenterZ->point, true, isFrame);
          break;
        }
        case 2: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
          this->helperUI->addControlsSliderSameLine("X", 19, 0.05f, -2 * this->managerObjects.Setting_GridSize, 2 * this->managerObjects.Setting_GridSize, true, &this->managerObjects.camera->positionX->animate, &this->managerObjects.camera->positionX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 20, 0.05f, -2 * this->managerObjects.Setting_GridSize, 2 * this->managerObjects.Setting_GridSize, true, &this->managerObjects.camera->positionY->animate, &this->managerObjects.camera->positionY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 21, 0.05f, -2 * this->managerObjects.Setting_GridSize, 2 * this->managerObjects.Setting_GridSize, true, &this->managerObjects.camera->positionZ->animate, &this->managerObjects.camera->positionZ->point, true, isFrame);
          break;
        }
        default:
          break;
      }
      break;
    }
    case 2: {
      ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.6f, 0.6f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.7f, 0.7f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f)));

      const char* tabsGUICameraModel[] = {
          (char*)"\n" ICON_MD_TRANSFORM,
          (char*)"\n" ICON_MD_OPEN_WITH,
          (char*)"\n" ICON_MD_3D_ROTATION,
      };
      const char* tabsLabelsGUICameraModel[] = {"General", "Position", "Rotate"};
      const int numTabsGUICameraModel = sizeof(tabsGUICameraModel) / sizeof(tabsGUICameraModel[0]);
      ImGui::TabLabels(numTabsGUICameraModel, tabsGUICameraModel, this->selectedTabGUICameraModel, ImVec2(30.0, 30.0), tabsLabelsGUICameraModel);
      ImGui::PopStyleColor(3);

      ImGui::Separator();

      switch (this->selectedTabGUICameraModel) {
        case 0: {
          ImGui::Checkbox("Camera", &this->managerObjects.cameraModel->showCameraObject);
          ImGui::Checkbox("Wire", &this->managerObjects.cameraModel->showInWire);
          ImGui::Separator();
          ImGui::Text("Inner Light Direction");
          this->helperUI->addControlsSliderSameLine("X", 1, 0.001f, -1.0f, 1.0f, true, &this->managerObjects.cameraModel->innerLightDirectionX->animate, &this->managerObjects.cameraModel->innerLightDirectionX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 2, 0.001f, -1.0f, 1.0f, true, &this->managerObjects.cameraModel->innerLightDirectionY->animate, &this->managerObjects.cameraModel->innerLightDirectionY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 3, 0.001f, -1.0f, 1.0f, true, &this->managerObjects.cameraModel->innerLightDirectionZ->animate, &this->managerObjects.cameraModel->innerLightDirectionZ->point, true, isFrame);
          ImGui::Separator();
          ImGui::Text("ModelFace Color");
          this->helperUI->addControlsSliderSameLine("X", 13, 0.01f, 0.0f, 1.0f, true, &this->managerObjects.cameraModel->colorR->animate, &this->managerObjects.cameraModel->colorR->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 14, 0.01f, 0.0f, 1.0f, true, &this->managerObjects.cameraModel->colorG->animate, &this->managerObjects.cameraModel->colorG->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 15, 0.01f, 0.0f, 1.0f, true, &this->managerObjects.cameraModel->colorB->animate, &this->managerObjects.cameraModel->colorB->point, true, isFrame);
          break;
        }
        case 1: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
          this->helperUI->addControlsSliderSameLine("X", 4, 0.05f, -2 * this->managerObjects.Setting_GridSize, 2 * this->managerObjects.Setting_GridSize, true, &this->managerObjects.cameraModel->positionX->animate, &this->managerObjects.cameraModel->positionX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 5, 0.05f, -2 * this->managerObjects.Setting_GridSize, 2 * this->managerObjects.Setting_GridSize, true, &this->managerObjects.cameraModel->positionY->animate, &this->managerObjects.cameraModel->positionY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 6, 0.05f, -2 * this->managerObjects.Setting_GridSize, 2 * this->managerObjects.Setting_GridSize, true, &this->managerObjects.cameraModel->positionZ->animate, &this->managerObjects.cameraModel->positionZ->point, true, isFrame);
          break;
        }
        case 2: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
          this->helperUI->addControlsSliderSameLine("X", 7, 1.0f, 0.0f, 360.0f, true, &this->managerObjects.cameraModel->rotateX->animate, &this->managerObjects.cameraModel->rotateX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 8, 1.0f, 0.0f, 360.0f, true, &this->managerObjects.cameraModel->rotateY->animate, &this->managerObjects.cameraModel->rotateY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 9, 1.0f, 0.0f, 360.0f, true, &this->managerObjects.cameraModel->rotateZ->animate, &this->managerObjects.cameraModel->rotateZ->point, true, isFrame);
          ImGui::Separator();
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around center");
          this->helperUI->addControlsSliderSameLine("X", 10, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.cameraModel->rotateCenterX->animate, &this->managerObjects.cameraModel->rotateCenterX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 11, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.cameraModel->rotateCenterY->animate, &this->managerObjects.cameraModel->rotateCenterY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 12, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.cameraModel->rotateCenterZ->animate, &this->managerObjects.cameraModel->rotateCenterZ->point, true, isFrame);
          break;
        }
        default:
          break;
      }
      break;
    }
    case 3: {
      ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.6f, 0.6f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.7f, 0.7f)));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f)));

      const char* tabsGUIGrid[] = {
          (char*)"\n" ICON_MD_TRANSFORM,
          (char*)"\n" ICON_MD_PHOTO_SIZE_SELECT_SMALL,
          (char*)"\n" ICON_MD_3D_ROTATION,
          (char*)"\n" ICON_MD_OPEN_WITH,
      };
      const char* tabsLabelsGUIGrid[] = {"General", "Scale", "Rotate", "Translate"};
      const int numTabsGUIGrid = sizeof(tabsGUIGrid) / sizeof(tabsGUIGrid[0]);
      ImGui::TabLabels(numTabsGUIGrid, tabsGUIGrid, this->selectedTabGUIGrid, ImVec2(30.0, 30.0), tabsLabelsGUIGrid);
      ImGui::PopStyleColor(3);

      ImGui::Separator();

      switch (this->selectedTabGUIGrid) {
        case 0: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "General grid settings");
          ImGui::Text("Grid size");
          if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Squares");
          ImGui::SliderInt("##109", &this->managerObjects.Setting_GridSize, 0, 100);
          ImGui::Separator();
          ImGui::Checkbox("Grid fixed with World", &this->managerObjects.Setting_FixedGridWorld);
          ImGui::Checkbox("Use WorldGrid", &managerObjects.Setting_UseWorldGrid);
          ImGui::Checkbox("Grid", &this->managerObjects.grid->showGrid);
          ImGui::Checkbox("Act as mirror", &this->managerObjects.grid->actAsMirror);
          if (this->managerObjects.grid->actAsMirror)
            this->helperUI->addControlsSliderSameLine("Alpha", 999, 0.01f, 0.0f, 1.0f, false, NULL, &this->managerObjects.grid->transparency, false, isFrame);
          ImGui::Separator();
          if (this->managerObjects.grid->actAsMirror) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Mirror Position");
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move mirror by axis");
            this->helperUI->addControlsSliderSameLine("X", 71, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, false, nullptr, &this->managerObjects.grid->mirrorSurface->translateX, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Y", 81, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, false, nullptr, &this->managerObjects.grid->mirrorSurface->translateY, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Z", 91, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, false, nullptr, &this->managerObjects.grid->mirrorSurface->translateZ, true, isFrame);
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate mirror around axis");
            this->helperUI->addControlsSliderSameLine("X", 41, 1.0f, -180.0f, 180.0f, false, nullptr, &this->managerObjects.grid->mirrorSurface->rotateX, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Y", 51, 1.0f, -180.0f, 180.0f, false, nullptr, &this->managerObjects.grid->mirrorSurface->rotateY, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Z", 61, 1.0f, -180.0f, 180.0f, false, nullptr, &this->managerObjects.grid->mirrorSurface->rotateZ, true, isFrame);
          }
          break;
        }
        case 1: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale object");
          this->helperUI->addControlsSliderSameLine("X", 1, 0.05f, 0.0f, 1.0f, true, &this->managerObjects.grid->scaleX->animate, &this->managerObjects.grid->scaleX->point, false, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 2, 0.05f, 0.0f, 1.0f, true, &this->managerObjects.grid->scaleY->animate, &this->managerObjects.grid->scaleY->point, false, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 3, 0.05f, 0.0f, 1.0f, true, &this->managerObjects.grid->scaleZ->animate, &this->managerObjects.grid->scaleZ->point, false, isFrame);
          break;
        }
        case 2: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
          this->helperUI->addControlsSliderSameLine("X", 4, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.grid->rotateX->animate, &this->managerObjects.grid->rotateX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 5, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.grid->rotateY->animate, &this->managerObjects.grid->rotateY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 6, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.grid->rotateZ->animate, &this->managerObjects.grid->rotateZ->point, true, isFrame);
          break;
        }
        case 3: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
          this->helperUI->addControlsSliderSameLine("X", 7, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &this->managerObjects.grid->positionX->animate, &this->managerObjects.grid->positionX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 8, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &this->managerObjects.grid->positionY->animate, &this->managerObjects.grid->positionY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 9, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &this->managerObjects.grid->positionZ->animate, &this->managerObjects.grid->positionZ->point, true, isFrame);
          break;
        }
        default:
          break;
      }
      break;
    }
    case 4: {
      ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scene Ambient Light");
      this->helperUI->addControlsSliderSameLine("X", 1, 0.001f, 0.0, 1.0, false, nullptr, &this->managerObjects.Setting_UIAmbientLight.r, true, isFrame);
      this->helperUI->addControlsSliderSameLine("Y", 2, 0.001f, 0.0, 1.0, false, nullptr, &this->managerObjects.Setting_UIAmbientLight.g, true, isFrame);
      this->helperUI->addControlsSliderSameLine("Z", 3, 0.001f, 0.0, 1.0, false, nullptr, &this->managerObjects.Setting_UIAmbientLight.b, true, isFrame);
      ImGui::Separator();

      ImGui::TextColored(ImVec4(1, 0, 0, 1), "Solid Skin Light");

      this->helperUI->addControlColor3("Ambient", &this->managerObjects.SolidLight_Ambient, &this->managerObjects.SolidLight_Ambient_ColorPicker);
      this->helperUI->addControlsSlider("Intensity", 4, 0.01f, 0.0f, 1.0f, false, NULL, &this->managerObjects.SolidLight_Ambient_Strength, true, isFrame);

      this->helperUI->addControlColor3("Diffuse", &this->managerObjects.SolidLight_Diffuse, &this->managerObjects.SolidLight_Diffuse_ColorPicker);
      this->helperUI->addControlsSlider("Intensity", 5, 0.01f, 0.0f, 1.0f, false, NULL, &this->managerObjects.SolidLight_Diffuse_Strength, true, isFrame);

      this->helperUI->addControlColor3("Specular", &this->managerObjects.SolidLight_Specular, &this->managerObjects.SolidLight_Specular_ColorPicker);
      this->helperUI->addControlsSlider("Intensity", 6, 0.01f, 0.0f, 1.0f, false, NULL, &this->managerObjects.SolidLight_Specular_Strength, true, isFrame);
      ImGui::Separator();

      this->helperUI->addControlColor3("Material Color", &this->managerObjects.SolidLight_MaterialColor, &this->managerObjects.SolidLight_MaterialColor_ColorPicker);
      ImGui::Separator();

      ImGui::Text("Direction");
      this->helperUI->addControlsSliderSameLine("X##407", 7, 0.0f, 0.0f, 10.0f, false, nullptr, &this->managerObjects.SolidLight_Direction.x, true, isFrame);
      this->helperUI->addControlsSliderSameLine("Y##408", 8, 1.0f, 0.0f, 10.0f, false, nullptr, &this->managerObjects.SolidLight_Direction.y, true, isFrame);
      this->helperUI->addControlsSliderSameLine("Z##409", 9, 0.0f, 0.0f, 10.0f, false, nullptr, &this->managerObjects.SolidLight_Direction.z, true, isFrame);
      break;
    }
    case 5: {
      ImGui::TextColored(ImVec4(1, 0, 0, 1), "Skybox");
      std::vector<const char*> skybox_items;
      for (size_t i = 0; i < this->managerObjects.skybox->skyboxItems.size(); i++)
        skybox_items.push_back(this->managerObjects.skybox->skyboxItems[i].title.c_str());
      ImGui::Combo("##987", &this->managerObjects.skybox->Setting_Skybox_Item, &skybox_items[0], int(skybox_items.size()));
      break;
    }
    case 6: {
      ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.1f / 7.0f, 0.6f, 0.6f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.1f / 7.0f, 0.7f, 0.7f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f));

      const char* tabsGUILight[] = {
          (char*)"\n" ICON_MD_TRANSFORM,
          (char*)"\n" ICON_MD_PHOTO_SIZE_SELECT_SMALL,
          (char*)"\n" ICON_MD_3D_ROTATION,
          (char*)"\n" ICON_MD_OPEN_WITH,
          (char*)"\n" ICON_MD_COLOR_LENS,
      };
      const char* tabsLabelsGUILight[] = {"General", "Scale", "Rotate", "Translate", "Colors"};
      const int numTabsGUILight = sizeof(tabsGUILight) / sizeof(tabsGUILight[0]);
      ImGui::TabLabels(numTabsGUILight, tabsGUILight, this->selectedTabGUILight, ImVec2(30.0, 30.0), tabsLabelsGUILight);
      ImGui::PopStyleColor(3);

      ImGui::Separator();

      switch (this->selectedTabGUILight) {
        case 0: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Properties");
          ImGui::Text("%s", this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->description.c_str());
          // show lamp object
          ImGui::Checkbox("Lamp", &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->showLampObject);
          ImGui::Checkbox("Direction", &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->showLampDirection);
          ImGui::Checkbox("Wire", &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->showInWire);
          ImGui::Checkbox("Lock with Camera", &this->lockCameraWithLight);
          if (ImGui::Button("View from Here", ImVec2(-1, 0)))
            this->lockCameraOnce();
          ImGui::Separator();
          if (ImGui::Button("Delete Light Source", ImVec2(-1, 0))) {
            this->selectedObject = 0;
            this->managerObjects.lightSources.erase(this->managerObjects.lightSources.begin() + this->selectedObjectLight);
            this->selectedObjectLight = -1;
          }
          break;
        }
        case 1: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale Object");
          this->helperUI->addControlsSliderSameLine("X", 10, 0.05f, 0.0f, this->managerObjects.Setting_GridSize, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->scaleX->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->scaleX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 11, 0.05f, 0.0f, this->managerObjects.Setting_GridSize, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->scaleY->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->scaleY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 12, 0.05f, 0.0f, this->managerObjects.Setting_GridSize, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->scaleZ->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->scaleZ->point, true, isFrame);
          break;
        }
        case 2: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Around Axis");
          this->helperUI->addControlsSliderSameLine("X", 4, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateCenterX->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateCenterX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 5, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateCenterY->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateCenterY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 6, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateCenterZ->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateCenterZ->point, true, isFrame);

          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Around World Center");
          //                    this->helperUI->addControlsSliderSameLine("X", 13, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateX->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateX->point, true, isFrame);
          //                    this->helperUI->addControlsSliderSameLine("Y", 14, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateY->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateY->point, true, isFrame);
          //                    this->helperUI->addControlsSliderSameLine("Z", 15, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateZ->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateZ->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("X", 13, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateX->animate, &this->lightRotateX, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 14, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateY->animate, &this->lightRotateY, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 15, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateZ->animate, &this->lightRotateZ, true, isFrame);
          break;
        }
        case 3: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move Object by Axis");
          this->helperUI->addControlsSliderSameLine("X", 16, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionX->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionX->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Y", 17, 1.0f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionY->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionY->point, true, isFrame);
          this->helperUI->addControlsSliderSameLine("Z", 18, 1.0f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionZ->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionZ->point, true, isFrame);
          break;
        }
        case 4: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Light Colors");

          this->helperUI->addControlColor3("Ambient Color", &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->ambient->color, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->ambient->colorPickerOpen);
          this->helperUI->addControlsSlider("Ambient Intensity", 19, 0.01f, 0.0f, 1.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->ambient->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->ambient->strength, true, isFrame);

          this->helperUI->addControlColor3("Diffuse Color", &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->diffuse->color, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->diffuse->colorPickerOpen);
          this->helperUI->addControlsSlider("Diffuse Intensity", 20, 0.01f, 0.0f, 1.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->diffuse->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->diffuse->strength, true, isFrame);

          this->helperUI->addControlColor3("Specular Color", &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->specular->color, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->specular->colorPickerOpen);
          this->helperUI->addControlsSlider("Specular Intensity", 21, 0.01f, 0.0f, 1.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->specular->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->specular->strength, true, isFrame);

          ImGui::Separator();

          if (this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->type != LightSourceType_Directional) {
            this->helperUI->addControlsSlider("Constant", 22, 0.01f, 0.0f, 1.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lConstant->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lConstant->point, true, isFrame);
            this->helperUI->addControlsSlider("Literal", 23, 0.01f, 0.0f, 1.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lLinear->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lLinear->point, true, isFrame);
            this->helperUI->addControlsSlider("Quadratic", 24, 0.01f, 0.0f, 1.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lQuadratic->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lQuadratic->point, true, isFrame);
          }

          int lType = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->type;
          assert(lType == LightSourceType_Directional || lType == LightSourceType_Point || lType == LightSourceType_Spot);
          switch (lType) {
          case LightSourceType_Point: {
            break;
          }
          case LightSourceType_Spot: {
            ImGui::Separator();
            this->helperUI->addControlsSlider("CutOff", 25, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lCutOff->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lCutOff->point, true, isFrame);
            this->helperUI->addControlsSlider("Outer CutOff", 26, 1.0f, -180.0f, 180.0f, true, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lOuterCutOff->animate, &this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->lOuterCutOff->point, true, isFrame);
            break;
          }
          }

          break;
        }
        default:
          break;
      }
      break;
    }
    case 7: {
      switch (this->selectedObjectArtefact) {
        case 0: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Terrain");
          ImGui::Checkbox("Terrain", &this->managerObjects.Setting_ShowTerrain);
          if (this->managerObjects.Setting_ShowTerrain) {
            ImGui::Separator();
            if (ImGui::Button("Generate Terrain", ImVec2(-1, 0)))
              this->generateNewTerrain = !this->generateNewTerrain;
            ImGui::Checkbox("Color Heightmap", &this->managerObjects.terrain->terrainGenerator->Setting_ColorTerrain);
            ImGui::Checkbox("Textured Terrain", &this->managerObjects.terrain->Setting_UseTexture);
            ImGui::Checkbox("Wireframe Terrain", &this->managerObjects.terrain->Setting_Wireframe);
            ImGui::Checkbox("Terrain ModelFace", &this->managerObjects.Setting_TerrainModel);
            // TODO: BIG memory consumption
            //ImGui::Checkbox("Animate by X", &this->managerObjects.Setting_TerrainAnimateX);
            //ImGui::Checkbox("Animate by Y", &this->managerObjects.Setting_TerrainAnimateY);
            ImGui::Separator();
            ImGui::Text("Geometry Type");
            const char* geometry_terraintype_items[] = {"Smooth", "Cubic", "Spherical"};
            ImGui::Combo("##2291", &this->managerObjects.terrain->terrainGenerator->Setting_TerrainType, geometry_terraintype_items, IM_ARRAYSIZE(geometry_terraintype_items));
          }

          if (this->generateNewTerrain) {
            this->managerObjects.generateTerrain();
            this->heightmapImage = this->managerObjects.terrain->heightmapImage;
            this->newHeightmap = true;
            this->generateNewTerrain = false;
          }

          if (this->managerObjects.Setting_ShowTerrain) {
            if (this->newHeightmap && !this->heightmapImage.empty()) {
              int tChannels;
              unsigned char* tPixels = stbi_load(this->heightmapImage.c_str(), &this->heightmapWidth, &this->heightmapHeight, &tChannels, 0);
              if (!tPixels)
                Settings::Instance()->funcDoLog("Can't load heightmap preview image - " + this->heightmapImage + " with error - " + std::string(stbi_failure_reason()));
              else {
                glGenTextures(1, &this->vboTexHeightmap);
                glBindTexture(GL_TEXTURE_2D, this->vboTexHeightmap);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                GLenum textureFormat = 0;
                switch (tChannels) {
                  case 1:
                    textureFormat = GL_LUMINANCE;
                    break;
                  case 2:
                    textureFormat = GL_LUMINANCE_ALPHA;
                    break;
                  case 3:
                    textureFormat = GL_RGB;
                    break;
                  case 4:
                    textureFormat = GL_RGBA;
                    break;
                  default:
                    textureFormat = GL_RGB;
                    break;
                }
                glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(textureFormat), this->heightmapWidth, this->heightmapHeight, 0, textureFormat, GL_UNSIGNED_BYTE, (GLvoid*)tPixels);
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(tPixels);
              }
            }
            ImGui::Separator();
            ImGui::Text("Heightmap");
            ImGui::Image(ImTextureID(intptr_t(this->vboTexHeightmap)), ImVec2(this->heightmapWidth, this->heightmapHeight), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
            this->newHeightmap = false;
          }

          if (this->managerObjects.Setting_ShowTerrain) {
            ImGui::Separator();
            ImGui::Text("Seed");
            ImGui::Checkbox("Random Seed", &this->managerObjects.terrain->terrainGenerator->Setting_SeedRandom);
#ifdef _WIN32
            this->helperUI->addControlsIntegerSliderSameLine("Max", 11, (std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), &this->managerObjects.terrain->terrainGenerator->Setting_Seed);
#else
            this->helperUI->addControlsIntegerSliderSameLine("Max", 11, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), &this->managerObjects.terrain->terrainGenerator->Setting_Seed);
#endif
            ImGui::Separator();
            ImGui::Text("Map Dimensions");
            this->helperUI->addControlsIntegerSliderSameLine("X", 1, 3, this->managerObjects.Setting_GridSize * 10, &this->managerObjects.Setting_TerrainWidth);
            this->helperUI->addControlsIntegerSliderSameLine("Y", 2, 3, this->managerObjects.Setting_GridSize * 10, &this->managerObjects.Setting_TerrainHeight);
            ImGui::Separator();
            ImGui::Text("Terrain Map Offset");
            this->helperUI->addControlsFloatSliderSameLine("X", 3, -1000.0f, 1000.0f, &this->managerObjects.terrain->terrainGenerator->Setting_OffsetHorizontal);
            this->helperUI->addControlsFloatSliderSameLine("Y", 4, -1000.0f, 1000.0f, &this->managerObjects.terrain->terrainGenerator->Setting_OffsetVertical);
            ImGui::Separator();
            ImGui::Text("Terrain Coeficients");
            this->helperUI->addControlsFloatSliderSameLine("Scale", 8, 1.0f, 100.0f, &this->managerObjects.terrain->terrainGenerator->Setting_ScaleCoeficient);
            this->helperUI->addControlsFloatSliderSameLine("Height", 9, -10.0f, 10.0f, &this->managerObjects.terrain->terrainGenerator->Setting_HeightCoeficient);
            ImGui::Separator();
            this->helperUI->addControlsIntegerSlider("Octaves", 5, 1, 24, &this->managerObjects.terrain->terrainGenerator->Setting_Octaves);
            this->helperUI->addControlsFloatSlider("Frequency", 6, 1.0f, 16.0f, &this->managerObjects.terrain->terrainGenerator->Setting_Frequency);
            this->helperUI->addControlsFloatSlider("Persistence", 7, 0.0f, 1.0f, &this->managerObjects.terrain->terrainGenerator->Setting_Persistence);
          }
          break;
        }
        case 1: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Spaceship Generator");
          ImGui::Checkbox("Spaceship", &this->managerObjects.Setting_ShowSpaceship);
          if (this->managerObjects.Setting_ShowSpaceship) {
            ImGui::Separator();
            if (ImGui::Button("Generate Spaceship", ImVec2(-1, 0)))
              this->managerObjects.Setting_GenerateSpaceship = true;
            //this->generateNewSpaceship = !this->generateNewSpaceship;
            ImGui::Checkbox("Wireframe", &this->managerObjects.spaceship->Setting_Wireframe);
          }

          //if (this->generateNewSpaceship) {
          // this->managerObjects.generateSpaceship();
          // this->generateNewSpaceship = false;
          //}
          break;
        }
#ifdef DEF_KuplungSetting_UseCuda
        case 2: {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cuda Example - Ocean FFT");
          ImGui::Checkbox("Show Ocean", &this->managerObjects.Setting_Cuda_ShowOceanFFT);

          if (this->managerObjects.Setting_Cuda_ShowOceanFFT) {
            if (ImGui::Button("Regenerate", ImVec2(-1, 0)))
              this->cudaOceanFFT->init();
            ImGui::Separator();
            ImGui::Checkbox("Wireframe", &this->cudaOceanFFT->Setting_ShowWireframes);
            ImGui::Checkbox("Draw in Points", &this->cudaOceanFFT->Setting_DrawDots);
            ImGui::Checkbox("Animate", &this->cudaOceanFFT->Setting_Animate);
            ImGui::Separator();
            this->helperUI->addControlsSlider("Gravity", 456, 0.01f, 0.0f, 100.0f, false, NULL, &this->cudaOceanFFT->Simm_g, true, isFrame);
            this->helperUI->addControlsSlider("Patch Size", 457, 1, 0.0f, 1000.0f, false, NULL, &this->cudaOceanFFT->Simm_patchSize, true, isFrame);
            this->helperUI->addControlsSlider("Wind Speed", 458, 1, 0.0f, 1000.0f, false, NULL, &this->cudaOceanFFT->Simm_windSpeed, true, isFrame);
            this->helperUI->addControlsSlider("Wind Direction", 459, 1, 0.0f, M_PI, false, NULL, &this->cudaOceanFFT->Simm_windDir, true, isFrame);
            ImGui::Separator();
            this->helperUI->addControlsSlider("Height Modifier", 460, 1, 1, 100, false, NULL, &this->cudaOceanFFT->heightModifier, true, isFrame);
            ImGui::Separator();
            ImGui::Checkbox("Scale all", &this->cudaOceanFFT->scaleAll);
            if (this->cudaOceanFFT->scaleAll) {
              ImGui::SliderFloat("##001", &this->cudaOceanFFT->scaleX->point, 1.0f, 100.0f);
              ImGui::SameLine();
              ImGui::Text("X");
              ImGui::SliderFloat("##001", &this->cudaOceanFFT->scaleY->point, 1.0f, 100.0f);
              ImGui::SameLine();
              ImGui::Text("Y");
              ImGui::SliderFloat("##001", &this->cudaOceanFFT->scaleZ->point, 1.0f, 100.0f);
              ImGui::SameLine();
              ImGui::Text("Z");
            }
            else {
              this->helperUI->addControlsSliderSameLine("X", 1, 0.1f, 1.0f, 100.0f, true, &this->cudaOceanFFT->scaleX->animate, &this->cudaOceanFFT->scaleX->point, false, isFrame);
              this->helperUI->addControlsSliderSameLine("Y", 2, 0.1f, 1.0f, 100.0f, true, &this->cudaOceanFFT->scaleY->animate, &this->cudaOceanFFT->scaleY->point, false, isFrame);
              this->helperUI->addControlsSliderSameLine("Z", 3, 0.1f, 1.0f, 100.0f, true, &this->cudaOceanFFT->scaleZ->animate, &this->cudaOceanFFT->scaleZ->point, false, isFrame);
            }
            ImGui::Separator();
            this->helperUI->addControlsSliderSameLine("X", 4, 0.05f, -2 * this->managerObjects.Setting_GridSize, 2 * this->managerObjects.Setting_GridSize, true, &this->cudaOceanFFT->positionX->animate, &this->cudaOceanFFT->positionX->point, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Y", 5, 0.05f, -2 * this->managerObjects.Setting_GridSize, 2 * this->managerObjects.Setting_GridSize, true, &this->cudaOceanFFT->positionY->animate, &this->cudaOceanFFT->positionY->point, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Z", 6, 0.05f, -2 * this->managerObjects.Setting_GridSize, 2 * this->managerObjects.Setting_GridSize, true, &this->cudaOceanFFT->positionZ->animate, &this->cudaOceanFFT->positionZ->point, true, isFrame);
          }
          break;
        }
#endif
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
}

void DialogControlsGUI::render(bool* show, bool* isFrame) {
  ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(Settings::Instance()->SDL_Window_Width - 310, 28), ImGuiCond_FirstUseEver);
  /// MIGRATE : ImGui::Begin("GUI Controls", show, ImGuiWindowFlags_ShowBorders);
  ImGui::Begin("GUI Controls", show);

  ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.1f / 7.0f, 0.6f, 0.6f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.1f / 7.0f, 0.7f, 0.7f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f));
  if (ImGui::Button("Reset values to default", ImVec2(-1, 0))) {
    this->managerObjects.resetPropertiesSystem();
    if (this->selectedObjectLight > -1) {
      this->lightRotateX = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateX->point;
      this->lightRotateY = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateY->point;
      this->lightRotateZ = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateZ->point;
    }
#ifdef DEF_KuplungSetting_UseCuda
    this->cudaOceanFFT->initParameters();
#endif
  }
  ImGui::PopStyleColor(3);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(255, 0, 0));
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
  ImGui::BeginChild("Global Items", ImVec2(0, this->heightTopPanel), true);
  this->drawGlobalItems();
  ImGui::EndChild();
  ImGui::PopItemWidth();
  ImGui::PopStyleColor();
  ImGui::PopStyleVar();

  ImGui::GetIO().MouseDrawCursor = true;
  ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(89, 91, 94)));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(119, 122, 124)));
  ImGui::PushStyleColor(ImGuiCol_Border, static_cast<ImVec4>(ImColor(0, 0, 0)));
  ImGui::Button("###splitterGUI", ImVec2(-1, 8.0f));
  ImGui::PopStyleColor(3);
  if (ImGui::IsItemActive())
    this->heightTopPanel += ImGui::GetIO().MouseDelta.y;
  if (ImGui::IsItemHovered())
    ImGui::SetMouseCursor(3);
  else
    ImGui::GetIO().MouseDrawCursor = false;

  ImGui::BeginChild("Properties Pane", ImVec2(0, 0), false);

  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.75f);
  this->drawPropertiesPane(isFrame);
  ImGui::PopItemWidth();
  ImGui::EndChild();
  ImGui::End();

  this->setHeightmapImage(this->managerObjects.heightmapImage);
  this->lockCamera();

  if (this->managerObjects.lightSources.size() > 0 && this->selectedObjectLight > -1 && int(this->managerObjects.lightSources.size()) > this->selectedObjectLight) {
    float slp_x = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateX->point;
    float slp_y = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateY->point;
    float slp_z = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateZ->point;
    if (this->lightRotateX < slp_x || this->lightRotateX > slp_x || this->lightRotateY < slp_y || this->lightRotateY > slp_y || this->lightRotateZ < slp_z || this->lightRotateZ > slp_z) {
      this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateX->point = this->lightRotateX;
      this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateY->point = this->lightRotateY;
      this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateZ->point = this->lightRotateZ;

      // glm::vec3 newRotation = glm::vec3(this->lightRotateX, this->lightRotateY, this->lightRotateZ);
      // glm::vec3 newPosition = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->getNewPositionAfterRotation(newRotation);

      // this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->turnOff_Position = true;
      // this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionX->point = newPosition.x;
      // this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionY->point = newPosition.y;
      // this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionZ->point = newPosition.z;
    }
  }
}
#ifdef DEF_KuplungSetting_UseCuda
void DialogControlsGUI::setCudaOceanFFT(oceanFFT* component) {
  this->cudaOceanFFT = component;
}
#endif

void DialogControlsGUI::setHeightmapImage(std::string const& heightmapImage) {
  if (this->heightmapImage != heightmapImage) {
    this->heightmapImage = heightmapImage;
    this->newHeightmap = true;
  }
}

void DialogControlsGUI::lockCameraOnce() {
  this->lockCameraWithLight = true;
  this->lockCamera();
  this->lockCameraWithLight = false;
}

void DialogControlsGUI::lockCamera() {
  if (this->lockCameraWithLight) {
    this->managerObjects.camera->positionX->point = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionX->point;
    this->managerObjects.camera->positionY->point = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionY->point;
    this->managerObjects.camera->positionZ->point = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->positionZ->point;
    this->managerObjects.camera->rotateX->point = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateX->point + 90.0f;
    this->managerObjects.camera->rotateY->point = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateY->point + 180.0f;
    this->managerObjects.camera->rotateZ->point = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->rotateZ->point;
    this->managerObjects.camera->cameraPosition = glm::vec3(this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->matrixModel[3].x, this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->matrixModel[3].y, this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->matrixModel[3].z);
    this->managerObjects.camera->matrixCamera = this->managerObjects.lightSources[size_t(this->selectedObjectLight)]->matrixModel;
  }
}
