//
// Kuplung.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/13/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Kuplung.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>

#pragma mark - Cleanup

Kuplung::~Kuplung() {
    this->destroy();
}

void Kuplung::destroy() {
    for (size_t i=0; i<this->meshModelFaces.size(); i++) {
        Model *mmf = this->meshModelFaces[i];
        mmf->destroy();
    }

    this->managerUI->destroy();
    this->parser->destroy();
    this->managerObjects->destroy();
    for (size_t i=0; i<this->rayLines.size(); i++) {
        this->rayLines[i]->destroy();
    }

    SDL_GL_DeleteContext(this->glContext);

    SDL_DestroyWindow(this->gWindow);
    this->gWindow = NULL;

    SDL_Quit();
}

#pragma mark - run

int Kuplung::run() {
    if (!this->init())
        return 1;

    SDL_Event ev;

    while (this->gameIsRunning) {
        while (SDL_PollEvent(&ev)) {
            this->onEvent(&ev);
        }

        glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
        glClearColor(Settings::Instance()->guiClearColor.r, Settings::Instance()->guiClearColor.g, Settings::Instance()->guiClearColor.b, Settings::Instance()->guiClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // rendering
        this->managerUI->renderStart(true, &this->sceneSelectedModelObject);
        this->renderScene();
        this->managerUI->renderEnd();

        SDL_GL_SwapWindow(this->gWindow);

        if (this->gameIsRunning == false)
            break;
    }

    return 0;
}

#pragma mark - Init

bool Kuplung::init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Error: SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);//4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");

        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

        if (Settings::Instance()->SDL_Window_Height == 0)
            Settings::Instance()->SDL_Window_Height = current.h;
        if (Settings::Instance()->SDL_Window_Width == 0)
            Settings::Instance()->SDL_Window_Width = current.w - 100;

        this->gWindow = SDL_CreateWindow(WINDOW_TITLE, WINDOW_POSITION_X, WINDOW_POSITION_Y, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, Settings::Instance()->SDL_Window_Flags | SDL_WINDOW_ALLOW_HIGHDPI);
        if (this->gWindow == NULL) {
            printf("Error: Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else {
            this->glContext = SDL_GL_CreateContext(this->gWindow);
            if (!this->glContext) {
                printf("Error: Unable to create OpenGL context! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else {
                if (SDL_GL_MakeCurrent(this->gWindow, this->glContext) < 0) {
                    printf("Warning: Unable to set current context! SDL Error: %s\n", SDL_GetError());
                    success = false;
                }
                else {
                    Settings::Instance()->setLogFunc(std::bind(&Kuplung::doLog, this, std::placeholders::_1));

                    this->parser = new FileModelManager();
                    this->parser->init(std::bind(&Kuplung::doProgress, this, std::placeholders::_1));

                    this->managerObjects = new ObjectsManager(this->parser);
                    this->managerObjects->init(std::bind(&Kuplung::doProgress, this, std::placeholders::_1),
                                               std::bind(&Kuplung::addTerrainModel, this));

                    this->managerUI = new UI();
                    this->managerUI->init(gWindow,
                                          this->managerObjects,
                                          std::bind(&Kuplung::guiQuit, this),
                                          std::bind(&Kuplung::guiProcessObjFile, this, std::placeholders::_1, std::placeholders::_2),
                                          std::bind(&Kuplung::guiClearScreen, this),
                                          std::bind(&Kuplung::guiEditorshaderCompiled, this, std::placeholders::_1),
                                          std::bind(&Kuplung::addShape, this, std::placeholders::_1),
                                          std::bind(&Kuplung::addLight, this, std::placeholders::_1),
                                          std::bind(&Kuplung::guiSceneExport, this, std::placeholders::_1),
                                          std::bind(&Kuplung::guiModelDelete, this, std::placeholders::_1),
                                          std::bind(&Kuplung::guiRenderScene, this, std::placeholders::_1),
                                          std::bind(&Kuplung::saveScene, this, std::placeholders::_1)
                                          );
                    this->doLog("UI initialized.");

                    this->managerObjects->loadSystemModels();

                    this->initFolders();
                    this->doLog("App initialized.");

                    this->managerControls = new Controls();
                    this->managerControls->init(this->gWindow);
                    this->doLog("Input Control Manager initialized.");

                    this->fontParser = new FNTParser();
                    this->fontParser->init();
                    this->doLog("Font Parser Initialized.");

                    this->terrain = new Terrain();

                    if (SDL_GL_SetSwapInterval(1) < 0) {
                        printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
                        success = false;
                    }

                    this->gameIsRunning = true;
                    this->sceneSelectedModelObject = -1;

                    this->initSceneGUI();

                    this->rayPicker = new RayPicking();
                    this->rayPicker->init(this->managerObjects, this->managerControls, std::bind(&Kuplung::doLog, this, std::placeholders::_1));

                    this->managerExporter = new Exporter();
                    this->managerExporter->init(std::bind(&Kuplung::doProgress, this, std::placeholders::_1));

                    this->imageRenderer = new ImageRenderer();
                    this->imageRenderer->init();
                }
            }
        }
    }
    return success;
}

void Kuplung::initFolders() {
//    char *data_path = NULL;
//    char *base_path = SDL_GetBasePath();
//    if (base_path)
//        data_path = base_path;
//    else
//        data_path = SDL_strdup("./");
//    Settings::Instance()->currentFolder = data_path;

#ifdef _WIN32
    char const *hdrive = getenv("HOMEDRIVE");
    char const *hpath = getenv("HOMEPATH");
    std::string homeFolder = std::string(hdrive) + "/" + std::string(hpath);
#elif defined macintosh // OS 9
    char const *hpath = getenv("HOME");
    std::string homeFolder = std::string(hpath);
#else
    char const *hpath = getenv("HOME");
    std::string homeFolder = std::string(hpath);
#endif
    if (Settings::Instance()->currentFolder == "")
        Settings::Instance()->currentFolder = homeFolder;
}

#pragma mark - Event processing

void Kuplung::onEvent(SDL_Event *ev) {
    this->managerUI->processEvent(ev);
    this->managerControls->processEvents(ev);

    this->gameIsRunning = this->managerControls->gameIsRunning;

    // window resize
    if (ev->type == SDL_WINDOWEVENT) {
        switch (ev->window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                Settings::Instance()->SDL_Window_Width = int(ev->window.data1);
                Settings::Instance()->SDL_Window_Height = int(ev->window.data2);
                Settings::Instance()->saveSettings();
                break;
        }
    }

    if (!this->managerUI->isMouseOnGUI() && !this->managerUI->isParsingOpen && !this->managerUI->isLoadingOpen) {
        // escape button
        if (this->managerControls->keyPressed_ESC) {
            this->sceneSelectedModelObject = -1;
            this->managerUI->setSceneSelectedModelObject(-1);
            this->rayLines.clear();
            this->rayPicker->rayLines = this->rayLines;
        }

        if (this->managerControls->keyPressed_DELETE && this->sceneSelectedModelObject > -1 && this->meshModelFaces.size() > 0)
            this->guiModelDelete(this->sceneSelectedModelObject);

        // FOV & zoom
        if (this->managerControls->keyPressed_LALT) {
            if (this->managerControls->mouseWheel.y < 0)
                this->managerObjects->Setting_FOV += 4;
            if (this->managerControls->mouseWheel.y > 0)
                this->managerObjects->Setting_FOV -= 4;

            if (this->managerObjects->Setting_FOV > 180)
                this->managerObjects->Setting_FOV = 180;
            if (this->managerObjects->Setting_FOV < -180)
                this->managerObjects->Setting_FOV = -180;
        }
        else
            this->managerObjects->camera->positionZ->point += this->managerControls->mouseWheel.y;

        // pan world
        if (this->managerControls->mouseButton_MIDDLE) {
            if (this->managerControls->mouseGoUp)
                this->managerObjects->camera->rotateX->point += this->managerControls->yrel;
            if (this->managerControls->mouseGoDown)
                this->managerObjects->camera->rotateX->point += this->managerControls->yrel;
            if (this->managerObjects->camera->rotateX->point > 360.0f)
                this->managerObjects->camera->rotateX->point = 0.0f;
            if (this->managerObjects->camera->rotateX->point < 0.0f)
                this->managerObjects->camera->rotateX->point = 360.0f;

            if (this->managerControls->mouseGoLeft)
                this->managerObjects->camera->rotateY->point += this->managerControls->xrel;
            if (this->managerControls->mouseGoRight)
                this->managerObjects->camera->rotateY->point += this->managerControls->xrel;
            if (this->managerObjects->camera->rotateY->point > 360.0f)
                this->managerObjects->camera->rotateY->point = 0.0f;
            if (this->managerObjects->camera->rotateY->point < 0.0f)
                this->managerObjects->camera->rotateY->point = 360.0f;
        }

        // move camera
        if (this->managerControls->keyPressed_LSHIFT && this->managerControls->mouseButton_LEFT) {
            if (this->managerControls->mouseGoLeft)
                this->managerObjects->camera->positionX->point -= 0.1;
            else if (this->managerControls->mouseGoRight)
                this->managerObjects->camera->positionX->point += 0.1;
            else if (this->managerControls->mouseGoUp)
                this->managerObjects->camera->positionY->point += 0.1;
            else if (this->managerControls->mouseGoDown)
                this->managerObjects->camera->positionY->point -= 0.1;
        }

        // picking
        if (this->managerControls->mouseButton_LEFT) {
            this->rayPicker->setMatrices(this->managerObjects->matrixProjection, this->managerObjects->camera->matrixCamera);
            this->rayPicker->selectModel(this->meshModelFaces, &this->rayLines, &this->sceneSelectedModelObject);
            this->managerUI->setSceneSelectedModelObject(this->sceneSelectedModelObject);
        }
    }
}

#pragma mark - Rendering

void Kuplung::addTerrainModel() {
    this->managerObjects->terrain->terrainGenerator->generateTerrain(Settings::Instance()->currentFolder, this->managerObjects->Setting_TerrainWidth, this->managerObjects->Setting_TerrainHeight);
    this->managerObjects->terrain->terrainGenerator->Setting_ColorTerrain = true;
    this->managerObjects->terrain->Setting_UseTexture = true;
    Model *mmf = new Model();
    mmf->dataVertices = this->managerObjects->grid->dataVertices;
    mmf->dataTexCoords = this->managerObjects->grid->dataTexCoords;
    mmf->dataNormals = this->managerObjects->grid->dataNormals;
    mmf->dataIndices = this->managerObjects->grid->dataIndices;

    mmf->ModelID = 1;
    mmf->init();
    mmf->setModel(this->managerObjects->terrain->terrainGenerator->modelTerrain);
    mmf->initBoundingBox();
    mmf->initModelProperties();
    mmf->initShaderProgram();
    mmf->initBuffers(Settings::Instance()->currentFolder);
    this->meshModelFaces.push_back(mmf);
    this->meshModels.push_back(this->managerObjects->terrain->terrainGenerator->modelTerrain);
    this->managerUI->meshModelFaces = &this->meshModelFaces;
}

void Kuplung::renderScene() {
    this->managerObjects->render();

    this->renderSceneModels();

    for (size_t i=0; i<this->rayLines.size(); i++) {
        this->rayLines[i]->render(this->managerObjects->matrixProjection, this->managerObjects->camera->matrixCamera);
    }

    this->processRunningThreads();
}

void Kuplung::renderSceneModels() {
    int cVertices = 0;
    int cIndices = 0;
    int cTriangles = 0;
    int cFaces = 0;
    for (size_t i=0; i<this->meshModelFaces.size(); i++) {
        Model* mmf = this->meshModelFaces[i];

        glm::mat4 mtxModel = glm::mat4(1.0);

        // reposition like the grid perspective
        if (this->managerObjects->Setting_FixedGridWorld)
            mtxModel *= this->managerObjects->grid->matrixModel;

        // scale
        mtxModel = glm::scale(mtxModel, glm::vec3(mmf->scaleX->point, mmf->scaleY->point, mmf->scaleZ->point));

        // rotate
        mtxModel = glm::translate(mtxModel, glm::vec3(0, 0, 0));
        mtxModel = glm::rotate(mtxModel, glm::radians(mmf->rotateX->point), glm::vec3(1, 0, 0));
        mtxModel = glm::rotate(mtxModel, glm::radians(mmf->rotateY->point), glm::vec3(0, 1, 0));
        mtxModel = glm::rotate(mtxModel, glm::radians(mmf->rotateZ->point), glm::vec3(0, 0, 1));
        mtxModel = glm::translate(mtxModel, glm::vec3(0, 0, 0));

        // translate
        mtxModel = glm::translate(mtxModel, glm::vec3(mmf->positionX->point, mmf->positionY->point, mmf->positionZ->point));

        // general
        mmf->setOptionsFOV(this->managerObjects->Setting_FOV);

        // outlining
        mmf->setOptionsSelected(this->sceneSelectedModelObject == int(i));
        mmf->setOptionsOutlineColor(this->managerObjects->Setting_OutlineColor);
        mmf->setOptionsOutlineThickness(this->managerObjects->Setting_OutlineThickness);

        // lights
        mmf->lightSources = this->managerObjects->lightSources;

        // skins
        mmf->Setting_ModelViewSkin = this->managerObjects->viewModelSkin;
        mmf->solidLightSkin_MaterialColor = this->managerObjects->SolidLight_MaterialColor;
        mmf->solidLightSkin_Ambient = this->managerObjects->SolidLight_Ambient;
        mmf->solidLightSkin_Diffuse = this->managerObjects->SolidLight_Diffuse;
        mmf->solidLightSkin_Specular = this->managerObjects->SolidLight_Specular;
        mmf->solidLightSkin_Ambient_Strength = this->managerObjects->SolidLight_Ambient_Strength;
        mmf->solidLightSkin_Diffuse_Strength = this->managerObjects->SolidLight_Diffuse_Strength;
        mmf->solidLightSkin_Specular_Strength = this->managerObjects->SolidLight_Specular_Strength;

        // rendering
        mmf->Setting_LightingPass_DrawMode = this->managerObjects->Setting_LightingPass_DrawMode;

        // render
        mmf->render(this->managerObjects->matrixProjection,
                    this->managerObjects->camera->matrixCamera,
                    mtxModel,
                    this->managerObjects->camera->cameraPosition,
                    this->managerObjects->grid,
                    this->managerObjects->Setting_UIAmbientLight);

        cVertices += mmf->meshModel.countVertices;
        cIndices += ((*std::max_element(mmf->meshModel.indices.begin(), mmf->meshModel.indices.end())) + 1);
        cTriangles += cVertices / 3;
        cFaces += cTriangles / 2;
    }
    Settings::Instance()->sceneCountObjects = int(this->meshModelFaces.size());
    Settings::Instance()->sceneCountVertices = cVertices;
    Settings::Instance()->sceneCountIndices = cIndices;
    Settings::Instance()->sceneCountTriangles = cTriangles;
    Settings::Instance()->sceneCountFaces = cFaces;

    if (this->managerObjects->Setting_ShowTerrain) {
        Settings::Instance()->sceneCountObjects += 1;
        Settings::Instance()->sceneCountVertices += int(this->managerObjects->terrain->terrainGenerator->vertices.size());
        Settings::Instance()->sceneCountIndices += int(this->managerObjects->terrain->terrainGenerator->indices.size());
        Settings::Instance()->sceneCountTriangles += int(this->managerObjects->terrain->terrainGenerator->vertices.size()) / 3;
        Settings::Instance()->sceneCountFaces += int(this->managerObjects->terrain->terrainGenerator->vertices.size()) / 6;
    }
}

#pragma mark - Scene GUI

void Kuplung::initSceneGUI() {
    this->managerObjects->initCamera();
    this->managerObjects->initCameraModel();
    this->managerObjects->initGrid();
    this->managerObjects->initAxisSystem();
    this->managerObjects->initAxisHelpers();
    this->managerObjects->initSkybox();
    this->managerObjects->initTerrain();
    //this->managerObjects->addLight(LightSourceType_Directional);
    //this->managerObjects->addLight(LightSourceType_Point);
    //this->managerObjects->addLight(LightSourceType_Spot);
    this->managerUI->showControlsGUI = true;
    this->managerUI->showControlsModels = true;
    this->managerUI->recentFiles = Settings::Instance()->loadRecentFiles();

    // testbed
//    std::string testObj = "brick_wall.obj";
//    FBEntity fileTestbed;
//    fileTestbed.isFile = true;
//    fileTestbed.extension = ".obj";
//    fileTestbed.title = testObj;
//    fileTestbed.path = "/Users/supudo/Software/C++/Kuplung/_objects/" + testObj;
//    this->guiProcessObjFile(fileTestbed, FileBrowser_ParserType_Own2);
}

void Kuplung::addShape(ShapeType type) {
    std::string shapeName = "";
    assert(type >= ShapeType_Cone && type <= ShapeType_UVSphere);
    switch (type) {
        case ShapeType_Cone:
            shapeName = "cone";
            break;
        case ShapeType_Cube:
            shapeName = "cube";
            break;
        case ShapeType_Cylinder:
            shapeName = "cylinder";
            break;
        case ShapeType_Grid:
            shapeName = "grid";
            break;
        case ShapeType_IcoSphere:
            shapeName = "ico_sphere";
            break;
        case ShapeType_MonkeyHead:
            shapeName = "monkey_head";
            break;
        case ShapeType_Plane:
            shapeName = "plane";
            break;
        case ShapeType_Triangle:
            shapeName = "triangle";
            break;
        case ShapeType_Torus:
            shapeName = "torus";
            break;
        case ShapeType_Tube:
            shapeName = "tube";
            break;
        case ShapeType_UVSphere:
            shapeName = "uv_sphere";
            break;
    }
    FileBrowser_ParserType t;
    assert(Settings::Instance()->ModelFileParser >= FileBrowser_ParserType_Own1 && Settings::Instance()->ModelFileParser <= FileBrowser_ParserType_Assimp);
    switch (Settings::Instance()->ModelFileParser) {
        case 0:
            t = FileBrowser_ParserType_Own1;
            break;
        case 1:
            t = FileBrowser_ParserType_Own2;
            break;
        case 2:
            t = FileBrowser_ParserType_Assimp;
            break;
    }
    FBEntity shapeFile;
    shapeFile.isFile = true;
    shapeFile.extension = ".obj";
    shapeFile.title = shapeName + ".obj";
    shapeFile.path = Settings::Instance()->appFolder() + "/shapes/" + shapeName + ".obj";
    this->guiProcessObjFile(shapeFile, t);
}

void Kuplung::addLight(LightSourceType type) {
    this->managerObjects->addLight(type);
}

#pragma mark - App GUI

void Kuplung::processRunningThreads() {
    if (this->objParserThreadFinished && !this->objParserThreadProcessed) {
        this->processParsedObjFile();
        this->objParserThreadProcessed = true;
    }
    if (this->exporterThreadFinished)
        this->exportSceneFinished();
}

void Kuplung::guiProcessObjFile(FBEntity file, FileBrowser_ParserType type) {
    if (this->hasEnding(file.title, ".obj")) {
        this->managerUI->showParsing();
        this->objParserThreadFinished = false;
        this->objParserThreadProcessed = false;
        std::thread objParserThread(&Kuplung::processObjFileAsync, this, file, type);
        objParserThread.detach();
        this->doLog("Starting parsing OBJ " + file.title);
    }
    else
        this->doLog("!!! You have to select .obj file !!!");
}

void Kuplung::processObjFileAsync(FBEntity file, FileBrowser_ParserType type) {
    std::vector<MeshModel> newModels = this->parser->parse(file, type);
    this->meshModelsNew.insert(end(this->meshModelsNew), begin(newModels), end(newModels));
    this->objFiles.push_back(file);
    this->objParserThreadFinished = true;
}

void Kuplung::guiSceneExport(FBEntity file) {
//    this->managerExporter->exportScene(file, this->meshModelFaces);
    this->managerUI->showExporting();
    this->exporterThreadFinished = false;
    std::thread exporterThread(&Kuplung::exportSceneAsync, this, file, this->meshModelFaces);
    exporterThread.detach();
}

void Kuplung::exportSceneAsync(FBEntity file, std::vector<Model*> meshModelFaces) {
    this->managerExporter->exportScene(file, meshModelFaces);
    this->exporterThreadFinished = true;
}

void Kuplung::exportSceneFinished() {
    this->managerUI->hideExporting();
}

void Kuplung::processParsedObjFile() {
    if (this->objFiles.size() > 0) {
        this->doLog(this->objFiles[this->objFiles.size() - 1].title + " was parsed successfully.");
        this->managerUI->hideParsing();
        this->managerUI->showLoading();
        this->managerUI->recentFilesAdd(this->objFiles[this->objFiles.size() - 1].title, this->objFiles[this->objFiles.size() - 1]);

        for (size_t i=0; i<this->meshModelsNew.size(); i++) {
            MeshModel model = this->meshModelsNew[i];
            Model *mmf;
            if (Settings::Instance()->DeferredRendering)
                mmf = new ModelDeferred();
            else
                mmf = new ModelForward();
            mmf->dataVertices = this->managerObjects->grid->dataVertices;
            mmf->dataTexCoords = this->managerObjects->grid->dataTexCoords;
            mmf->dataNormals = this->managerObjects->grid->dataNormals;
            mmf->dataIndices = this->managerObjects->grid->dataIndices;

            mmf->ModelID = (int)i;
            mmf->init();
            mmf->setModel(model);
            mmf->initBoundingBox();
            mmf->initModelProperties();
            mmf->initShaderProgram();
            mmf->initBuffers(Settings::Instance()->currentFolder);
            this->meshModelFaces.push_back(mmf);
            this->meshModels.push_back(model);
        }
        this->meshModelsNew.clear();

        this->managerUI->meshModelFaces = &this->meshModelFaces;

        if (this->meshModelFaces.size() > 0) {
            this->managerUI->showControlsModels = true;
            //this->managerUI->showSceneStats = true;
        }
    }
    this->managerUI->hideParsing();
    this->managerUI->hideLoading();
}

void Kuplung::doProgress(float value) {
    this->objLoadingProgress = value;
    this->managerUI->parsingPercentage = value;
}

void Kuplung::guiQuit() {
    this->gameIsRunning = false;
}

bool Kuplung::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    return false;
}

void Kuplung::doLog(std::string logMessage) {
    if (this->managerUI)
        this->managerUI->doLog(logMessage);
}

void Kuplung::guiClearScreen() {
    for (size_t i=0; i<this->meshModelFaces.size(); i++) {
        this->meshModelFaces[i]->destroy();
    }
    this->meshModels.clear();
    this->meshModelsNew.clear();
    this->meshModelFaces.clear();
    this->objFiles.clear();
    this->rayLines.clear();
    this->managerUI->setSceneSelectedModelObject(-1);
}

void Kuplung::guiEditorshaderCompiled(std::string fileName) {
    if (fileName.compare(0, 9, "kuplung") == 0) {
    }
    else if (fileName.compare(0, 5, "light") == 0) {
    }
    else if (fileName.compare(0, 4, "grid") == 0) {
    }
    else if (fileName.compare(0, 4, "axis") == 0) {
    }
    else if (fileName.compare(0, 4, "dots") == 0) {
    }
    else if (fileName.compare(0, 7, "terrain") == 0) {
    }
}

void Kuplung::guiModelDelete(int selectedModel) {
    this->meshModelFaces[selectedModel]->destroy();
    this->meshModelFaces.erase(this->meshModelFaces.begin() + selectedModel);
    this->meshModels.erase(this->meshModels.begin() + selectedModel);
    this->managerUI->meshModelFaces = &this->meshModelFaces;
    this->sceneSelectedModelObject = -1;
    this->managerUI->setSceneSelectedModelObject(-1);
}

void Kuplung::guiRenderScene(FBEntity file) {
    this->managerObjects->renderSkybox();
    this->renderSceneModels();
    SDL_GL_SwapWindow(this->gWindow);

    this->imageRenderer->renderImage(ImageRendererType_Scene, file, &this->meshModelFaces, this->managerObjects);
//    if (SDL_GL_MakeCurrent(this->gWindow, this->glContext) < 0)
//        Settings::Instance()->funcDoLog("[Renderer] Cannot get back to main context!");
}

void Kuplung::saveScene(FBEntity file) {
    // TODO: save scene in .kuplung file
}
