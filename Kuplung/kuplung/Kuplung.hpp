//
// Kuplung.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/13/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Kuplung_hpp
#define Kuplung_hpp

#include <thread>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/settings/Settings.h"
#include "kuplung/ui/UIManager.hpp"
#include "kuplung/utilities/parsers/FileModelManager.hpp"
#include "kuplung/utilities/input/Controls.hpp"
#include "kuplung/utilities/font-parser/FNTParser.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/rendering/RenderingManager.hpp"
#include "kuplung/meshes/helpers/RayLine.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/objects/RayPicking.hpp"
#include "kuplung/utilities/shapes/Shapes.h"
#include "kuplung/utilities/export/Exporter.hpp"
#include "kuplung/utilities/renderers/ImageRenderer.hpp"
#include "kuplung/utilities/saveopen/SaveOpen.hpp"

class Kuplung {
public:
	Kuplung();
    ~Kuplung();
    int run();

private:
    bool init();
    void initFolders();
    void onEvent(SDL_Event* ev);
    void processImportFileAsync(const FBEntity& file, const std::vector<std::string> &settings);
    void processParsedImportedFile();
    void renderScene();
    void renderSceneModels();
    void initSceneGUI();
    void doLog(std::string const& logMessage);
    void doProgress(float value);
    void processRunningThreads();
    void addShape(const ShapeType type);
    void addLight(const LightSourceType type);
    void exportSceneAsync(const FBEntity& file, std::vector<ModelFaceBase*> const& meshModelFaces, const std::vector<std::string> &settings, ImportExportFormats exportFormat);
    void exportSceneFinished();
    void addTerrainModel();
    void addSpaceshipModel();
    void saveScene(const FBEntity& file);
    void openScene(const FBEntity& file);

    void guiQuit();
    void guiProcessImportedFile(const FBEntity& file, const std::vector<std::string> &settings);
    void guiClearScreen();
    void guiEditorshaderCompiled(std::string const& fileName);
    void guiModelDelete(const int selectedModel);
    void guiSceneExport(const FBEntity& file, const std::vector<std::string> &settings, ImportExportFormats exportFormat);
    void guiRenderScene(const FBEntity& file);

    // Screen dimension constants
    const char *WINDOW_TITLE = "Kuplung";
    const int WINDOW_POSITION_X = SDL_WINDOWPOS_CENTERED;
    const int WINDOW_POSITION_Y = SDL_WINDOWPOS_CENTERED;

    // SDLs
    SDL_Window *gWindow = NULL;
    SDL_GLContext glContext;

    // Variables
    bool gameIsRunning = false, objParserThreadFinished, objParserThreadProcessed, exporterThreadFinished;
    int sceneSelectedModelObject;
    float objLoadingProgress;

    //objParser *parser;
    std::unique_ptr<FileModelManager> parser;
    std::unique_ptr<SaveOpen> managerSaveOpen;

    // Customs
    std::unique_ptr<KuplungApp::Utilities::Input::Controls> managerControls;
    std::unique_ptr<UIManager> managerUI;
    std::unique_ptr<KuplungApp::Utilities::FontParser::FNTParser> fontParser;
    std::unique_ptr<KuplungApp::Utilities::Export::Exporter> managerExporter;
    std::unique_ptr<ImageRenderer> imageRenderer;
    std::unique_ptr<ObjectsManager> managerObjects;
    std::unique_ptr<RayPicking> rayPicker;
    std::unique_ptr<RenderingManager> managerRendering;

    std::vector<FBEntity> objFiles;
    std::vector<MeshModel> meshModels, meshModelsNew;
    std::vector<ModelFaceBase*> meshModelFaces;
    std::vector<RayLine*> rayLines;
};

#endif /* Kuplung_hpp */
