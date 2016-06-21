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
#include "kuplung/ui/UI.hpp"
#include "kuplung/utilities/parsers/FileModelManager.hpp"
#include "kuplung/utilities/input/Controls.hpp"
#include "kuplung/utilities/font-parser/FNTParser.hpp"
#include "kuplung/meshes/Model.hpp"
#include "kuplung/meshes/ModelForward.hpp"
#include "kuplung/meshes/ModelDeferred.hpp"
#include "kuplung/meshes/RayLine.hpp"
#include "kuplung/meshes/Terrain.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/objects/RayPicking.hpp"
#include "kuplung/utilities/shapes/Shapes.h"
#include "kuplung/utilities/export/Exporter.hpp"
#include "kuplung/utilities/renderers/ImageRenderer.hpp"

class Kuplung {
public:
    ~Kuplung();
    int run();

private:
    bool init();
    void initFolders();
    void destroy();
    void onEvent(SDL_Event* ev);
    void processObjFileAsync(FBEntity file, FileBrowser_ParserType type);
    void processParsedObjFile();
    bool hasEnding(std::string const &fullString, std::string const &ending);
    void renderScene();
    void renderSceneModels();
    void setupScene(FBEntity file);
    void initSceneGUI();
    void doLog(std::string logMessage);
    void doProgress(float value);
    std::string readFile(const char *filePath);
    void processRunningThreads();
    void setShaderEditor(std::function<void(std::string)> fileShaderCompile);
    void addShape(ShapeType type);
    void addLight(LightSourceType type);
    void exportSceneAsync(FBEntity file, std::vector<Model*> meshModelFaces);
    void exportSceneFinished();
    void addTerrainModel();
    void saveScene(FBEntity file);

    void guiQuit();
    void guiProcessObjFile(FBEntity file, FileBrowser_ParserType type);
    void guiClearScreen();
    void guiEditorshaderCompiled(std::string fileName);
    void guiModelDelete(int selectedModel);
    void guiModelRename(int selectedModel, std::string newName);
    void guiSceneExport(FBEntity file);
    void guiRenderScene(FBEntity file);

    ObjectsManager *managerObjects;
    RayPicking *rayPicker;

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
    FileModelManager *parser;

    // Customs
    std::vector<FBEntity> objFiles;
    Controls *managerControls;
    UI *managerUI;
    FNTParser *fontParser;
    Exporter *managerExporter;
    ImageRenderer *imageRenderer;
    std::vector<MeshModel> meshModels, meshModelsNew;
    std::vector<Model*> meshModelFaces;
    std::vector<RayLine*> rayLines;

    Terrain *terrain;
};

#endif /* Kuplung_hpp */
