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

#include "utilities/gl/GLIncludes.h"

#include "utilities/settings/Settings.h"
#include "utilities/gui/GUI.hpp"
#include "utilities/parsers/obj-parser/objParser.hpp"
#include "utilities/input/Controls.hpp"
#include "utilities/font-parser/FNTParser.hpp"

#include "meshes/ModelFace.hpp"
#include "meshes/Light.hpp"
#include "meshes/WorldGrid.hpp"
#include "meshes/CoordinateSystem.hpp"
#include "meshes/Dot.hpp"
#include "meshes/Terrain.hpp"

class Kuplung {
public:
    ~Kuplung();
    int run();
    int run(int width, int height);

private:
    bool init(int width, int height);
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

    // Screen dimension constants
    const char *WINDOW_TITLE = "Kuplung";
    const int WINDOW_POSITION_X = SDL_WINDOWPOS_CENTERED;
    const int WINDOW_POSITION_Y = SDL_WINDOWPOS_CENTERED;

    // SDLs
    SDL_Window *gWindow = NULL;
    SDL_GLContext glContext;

    // Variables
    bool gameIsRunning = false, objParserThreadFinished, objParserThreadProcessed;
    glm::mat4 matrixProjection, matrixCamera;
    int sceneSelectedModelObject;
    std::string selectedMaterialID;
    float objLoadingProgress;

    // Customs
    std::vector<FBEntity> objFiles;
    Controls *managerControls;
    GUI *gui;
    objParser *parser;
    FNTParser *fontParser;
    std::vector<objScene> scenes;
    std::vector<ModelFace*> meshModelFaces;

    Light* meshLight;
    WorldGrid* sceneGridHorizontal;
    WorldGrid* sceneGridVertical;
    CoordinateSystem* sceneCoordinateSystem;
    Dot* lightDot;
    Terrain *terrain;
};

#endif /* Kuplung_hpp */
