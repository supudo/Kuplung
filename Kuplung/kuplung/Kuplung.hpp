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
#include "kuplung/meshes/ModelFace.hpp"
#include "kuplung/meshes/Terrain.hpp"
#include "kuplung/objects/ObjectsManager.hpp"

class Kuplung {
public:
    ~Kuplung();
    int run();

private:
    bool init();
    void initFolders();
    void destroy();
    void onEvent(SDL_Event* ev);
    void processObjFileAsync(FBEntity file);
    void processParsedObjFile();
    void objParserLog(std::string logMessage);
    bool hasEnding(std::string const &fullString, std::string const &ending);
    void renderScene();
    void setupScene(FBEntity file);
    void initSceneGUI();
    void doLog(std::string logMessage);
    void doProgress(float value);
    std::string readFile(const char *filePath);
    void processRunningThreads();
    void setShaderEditor(std::function<void(std::string)> fileShaderCompile);

    void guiQuit();
    void guiProcessObjFile(FBEntity file);
    void guiClearScreen();
    void guiEditorshaderCompiled(std::string fileName);
    void guiModelDelete(int selectedModel);
    void guiModelRename(int selectedModel, std::string newName);

    ObjectsManager *managerObjects;

    // Screen dimension constants
    const char *WINDOW_TITLE = "Kuplung";
    const int WINDOW_POSITION_X = SDL_WINDOWPOS_CENTERED;
    const int WINDOW_POSITION_Y = SDL_WINDOWPOS_CENTERED;

    // SDLs
    SDL_Window *gWindow = NULL;
    SDL_GLContext glContext;

    // Variables
    bool gameIsRunning = false, objParserThreadFinished, objParserThreadProcessed;
    int sceneSelectedModelObject;
    std::string selectedMaterialID;
    float objLoadingProgress;

    //objParser *parser;
    FileModelManager *parser;

    // Customs
    std::vector<FBEntity> objFiles;
    Controls *managerControls;
    UI *managerUI;
    FNTParser *fontParser;
    std::vector<objScene> scenes;
    std::vector<ModelFace*> meshModelFaces;

    Terrain *terrain;
};

#endif /* Kuplung_hpp */
