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
        ModelFace *mmf = this->meshModelFaces[i];
        mmf->destroy();
    }

    this->managerUI->destroy();
    this->parser->destroy();

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
        this->managerUI->renderStart(true);
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

        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

        if (Settings::Instance()->SDL_Window_Height == 0)
            Settings::Instance()->SDL_Window_Height = current.h;
        if (Settings::Instance()->SDL_Window_Width == 0)
            Settings::Instance()->SDL_Window_Width = current.w - 100;

        this->gWindow = SDL_CreateWindow(WINDOW_TITLE, WINDOW_POSITION_X, WINDOW_POSITION_Y, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, Settings::Instance()->SDL_Window_Flags);
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
                    this->parser->init(std::bind(&Kuplung::objParserLog, this, std::placeholders::_1), std::bind(&Kuplung::doProgress, this, std::placeholders::_1));

                    this->managerObjects = new ObjectsManager(this->parser);
                    this->managerObjects->init(std::bind(&Kuplung::doProgress, this, std::placeholders::_1));

                    this->managerUI = new UI();
                    this->managerUI->init(gWindow,
                                          this->managerObjects,
                                          std::bind(&Kuplung::guiQuit, this),
                                          std::bind(&Kuplung::guiProcessObjFile, this, std::placeholders::_1),
                                          std::bind(&Kuplung::guiClearScreen, this),
                                          std::bind(&Kuplung::guiEditorshaderCompiled, this, std::placeholders::_1),
                                          std::bind(&Kuplung::addShape, this, std::placeholders::_1)
                                          );
                    this->doLog("UI initialized.");

                    this->managerObjects->loadSystemModels();

                    this->initFolders();
                    this->doLog("App initialized.");

                    this->managerControls = new Controls();
                    this->managerControls->init(std::bind(&Kuplung::objParserLog, this, std::placeholders::_1), this->gWindow);
                    this->doLog("Input Control Manager initialized.");

                    this->fontParser = new FNTParser();
                    this->fontParser->init(std::bind(&Kuplung::objParserLog, this, std::placeholders::_1));
                    this->doLog("Font Parser Initialized.");

                    this->terrain = new Terrain();

                    if (SDL_GL_SetSwapInterval(1) < 0) {
                        printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
                        success = false;
                    }

                    this->gameIsRunning = true;
                    this->sceneSelectedModelObject = -1;
                    this->selectedMaterialID = "";

                    this->initSceneGUI();
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
    Settings::Instance()->currentFolder = "/Users/supudo/Software/C++/Kuplung/_objects";
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
                Settings::Instance()->SDL_Window_Width = (int)ev->window.data1;
                Settings::Instance()->SDL_Window_Height = (int)ev->window.data2;
                Settings::Instance()->saveSettings();
                break;
            default:
                break;
        }
    }

    if (!this->managerUI->isMouseOnGUI() && !this->managerUI->isLoadingOpen) {
        // escape button
        if (this->managerControls->keyPressed_ESC) {
            this->sceneSelectedModelObject = -1;
            this->selectedMaterialID = "";
        }

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
            else if (this->managerControls->mouseGoDown)
                this->managerObjects->camera->rotateX->point += this->managerControls->yrel;
            else if (this->managerControls->mouseGoLeft)
                this->managerObjects->camera->rotateY->point += this->managerControls->xrel;
            else if (this->managerControls->mouseGoRight)
                this->managerObjects->camera->rotateY->point += this->managerControls->xrel;
        }

        // picking
        if (this->managerControls->mouseButton_LEFT) {
            int mouse_x = this->managerControls->mousePosition.x;
            int mouse_y = this->managerControls->mousePosition.y;

            glm::vec4 viewport = glm::vec4(0.0f, 0.0f, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
            glm::vec3 win_near = glm::vec3(mouse_x, mouse_y, 0.0);
            glm::vec3 win_far = glm::vec3(mouse_x, mouse_y, 1.0);

            glm::vec3 nearPoint = glm::unProject(win_near, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);
            glm::vec3 farPoint = glm::unProject(win_far, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);
            glm::vec3 direction = glm::normalize(farPoint - nearPoint);

            // http://schabby.de/picking-opengl-ray-tracing/
            // http://stackoverflow.com/questions/27891036/dragging-3-dimensional-objects-with-c-and-opengl
            float sceneClosestObject = -1;

            for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
                ModelFace *mmf = this->meshModelFaces[i];
                std::vector<float> objVertices = mmf->oFace.vertices;

                std::vector<glm::vec3> Vertices;
                for (int j=0; j<(int)objVertices.size(); j++) {
                    if ((j + 1) % 3 == 0) {
                        glm::vec3 v = glm::vec3(objVertices[j], objVertices[j - 1], objVertices[j - 2]);
                        Vertices.push_back(v);
                    }
                }

                for (int j=0; j<(int)Vertices.size(); j++) {
                    if ((j + 1) % 3 == 0) {
                        glm::vec3 face_normal = glm::normalize(glm::cross(Vertices[j-1] - Vertices[j-2], Vertices[j] - Vertices[j-2]));

                        float nDotL = glm::dot(direction, face_normal);
                        if (nDotL <= 0.0f ) {
                            float distance = glm::dot(face_normal, (Vertices[j-2] - nearPoint)) / nDotL;

                            glm::vec3 p = nearPoint + distance * direction;
                            glm::vec3 n1 = glm::cross(Vertices[j-1] - Vertices[j-2], p - Vertices[j-2]);
                            glm::vec3 n2 = glm::cross(Vertices[j] - Vertices[j-1], p - Vertices[j-1]);
                            glm::vec3 n3 = glm::cross(Vertices[j-2] - Vertices[j], p - Vertices[j]);
                            if (glm::dot(face_normal, n1) >= 0.0f && glm::dot(face_normal, n2) >= 0.0f && glm::dot(face_normal, n3) >= 0.0f) {
                                if (p.z > sceneClosestObject) {
                                    this->sceneSelectedModelObject = i;
                                    this->selectedMaterialID = mmf->oFace.materialID;
                                    this->doLog("RayCast @ [" + std::to_string(mouse_x) + ", " + std::to_string(mouse_y) + "] = [" + std::to_string(this->sceneSelectedModelObject) + "] - " + this->selectedMaterialID);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

#pragma mark - Rendering

void Kuplung::renderScene() {
    this->managerObjects->render();

    for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
        ModelFace* mmf = this->meshModelFaces[i];

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
        mmf->setOptionsSelected(this->selectedMaterialID == mmf->oFace.materialID);
        mmf->setOptionsOutlineColor(this->managerObjects->Setting_OutlineColor);
        mmf->setOptionsOutlineThickness(this->managerObjects->Setting_OutlineThickness);

        // lights
        mmf->lightSources = this->managerObjects->lightSources;

        // render
        mmf->render(this->managerObjects->matrixProjection,
                    this->managerObjects->camera->matrixCamera,
                    mtxModel,
                    this->managerObjects->camera->cameraPosition,
                    this->managerObjects->grid);
    }

    this->processRunningThreads();
}

#pragma mark - Scene GUI

void Kuplung::initSceneGUI() {
    this->managerObjects->initCamera();
    this->managerObjects->initGrid();
    this->managerObjects->initAxisSystem();
    //this->managerObjects->addLight(LightSourceType_Directional);
    this->managerObjects->addLight(LightSourceType_Point);
    //this->managerObjects->addLight(LightSourceType_Spot);
    this->managerUI->showControlsGUI = true;

    // testbed
//    //std::string testObj = "bump_cube0.obj";
//    std::string testObj = "bump_cylinder.obj";
//    FBEntity fileTestbed;
//    fileTestbed.isFile = true;
//    fileTestbed.extension = ".obj";
//    fileTestbed.title = testObj;
//    fileTestbed.path = "/Users/supudo/Software/C++/Kuplung/_objects/" + testObj;
//    this->guiProcessObjFile(fileTestbed);
    this->managerUI->showControlsModels = true;
}

void Kuplung::addShape(ShapeType type) {
    std::string shapeName = "";
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
            shapeName = "IcoSphere";
            break;
        case ShapeType_MonkeyHead:
            shapeName = "monkey_head";
            break;
        case ShapeType_Plane:
            shapeName = "plane";
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
        default:
            break;
    }
    FBEntity shapeFile;
    shapeFile.isFile = true;
    shapeFile.extension = ".obj";
    shapeFile.title = shapeName + ".obj";
    shapeFile.path = Settings::Instance()->appFolder() + "/shapes/" + shapeName + ".obj";
    this->guiProcessObjFile(shapeFile);
}

#pragma mark - App GUI

void Kuplung::processRunningThreads() {
    if (this->objParserThreadFinished && !this->objParserThreadProcessed) {
        this->managerUI->hideLoading();
        this->processParsedObjFile();
        this->objParserThreadProcessed = true;
    }
}

void Kuplung::guiProcessObjFile(FBEntity file) {
    if (this->hasEnding(file.title, ".obj")) {
        this->managerUI->showLoading();
        this->objParserThreadFinished = false;
        this->objParserThreadProcessed = false;
        std::thread objParserThread(&Kuplung::processObjFileAsync, this, file);
        objParserThread.detach();
        this->doLog("Starting parsing OBJ " + file.title);
    }
    else
        this->doLog("!!! You have to select .obj file !!!");
}

void Kuplung::processObjFileAsync(FBEntity file) {
    this->scenes.push_back(this->parser->parse(file));
    this->objFiles.push_back(file);
    this->objParserThreadFinished = true;
}

void Kuplung::processParsedObjFile() {
    this->doLog(this->objFiles[this->objFiles.size() - 1].title + " was parsed successfully.");
    this->managerUI->recentFilesAdd(this->objFiles[this->objFiles.size() - 1].title, this->objFiles[this->objFiles.size() - 1]);

    objScene scene = this->scenes[this->scenes.size() - 1];
    for (int i=0; i<(int)scene.models.size(); i++) {
        objModel model = scene.models[i];
        for (size_t j=0; j<model.faces.size(); j++) {
            ModelFace *mmf = new ModelFace();

            // relfection area
            mmf->dataVertices = this->managerObjects->grid->dataVertices;
            mmf->dataTexCoords = this->managerObjects->grid->dataTexCoords;
            mmf->dataNormals = this->managerObjects->grid->dataNormals;
            mmf->dataIndices = this->managerObjects->grid->dataIndices;

            mmf->ModelID = i;
            mmf->init();
            mmf->setModel(model.faces[j]);
            mmf->initModelProperties();
            mmf->initShaderProgram();
            mmf->initBuffers(Settings::Instance()->currentFolder);
            this->meshModelFaces.push_back(mmf);
        }
    }

    this->managerUI->meshModelFaces = &this->meshModelFaces;

    if (this->meshModelFaces.size() > 0) {
        this->managerUI->showControlsModels = true;
        //this->managerUI->showSceneStats = true;
    }

    this->managerUI->hideLoading();
}

void Kuplung::doProgress(float value) {
    this->objLoadingProgress = value;
    this->managerUI->loadingPercentage = value;
}

void Kuplung::guiQuit() {
    this->gameIsRunning = false;
}

void Kuplung::objParserLog(std::string logMessage) {
    this->doLog(logMessage);
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
    this->scenes = {};
    this->meshModelFaces = {};
    this->scenes.clear();
    this->objFiles.clear();
    for (size_t i=0; i<this->meshModelFaces.size(); i++) {
        ModelFace *mmf = this->meshModelFaces[i];
        mmf->destroy();
    }
}

void Kuplung::guiEditorshaderCompiled(std::string fileName) {
    if (fileName.compare(0, 9, "kuplung") == 0) {
    }
    else if (fileName.compare(0, 5, "light") == 0) {
//        FBEntity file;
//        file.isFile = true;
//        file.extension = ".obj";
//        file.title = "light";
//        file.path = Settings::Instance()->appFolder() + "/gui/light.obj";
//        objScene sceneGUILight = this->parser->parse(file);
//        this->meshLight->destroy();
//        this->meshLight->init();
//        this->meshLight->setModel(sceneGUILight.models[0].faces[0]);
//        this->meshLight->initShaderProgram();
//        this->meshLight->initBuffers(std::string(Settings::Instance()->appFolder()));
    }
    else if (fileName.compare(0, 4, "grid") == 0) {
//        this->sceneGridHorizontal->destroy();
//        this->sceneGridHorizontal->init();
//        this->sceneGridHorizontal->initShaderProgram();
//        this->sceneGridHorizontal->initBuffers(20, true, 1);

//        this->sceneGridVertical->destroy();
//        this->sceneGridVertical->init();
//        this->sceneGridVertical->initShaderProgram();
//        this->sceneGridVertical->initBuffers(20, false, 1);
    }
    else if (fileName.compare(0, 4, "axis") == 0) {
//        this->sceneCoordinateSystem->destroy();
//        this->sceneCoordinateSystem->init();
//        this->sceneCoordinateSystem->initShaderProgram();
//        this->sceneCoordinateSystem->initBuffers();
    }
    else if (fileName.compare(0, 4, "dots") == 0) {
//        this->lightDot->destroy();
//        this->lightDot->init();
//        this->lightDot->initShaderProgram();
    }
    else if (fileName.compare(0, 7, "terrain") == 0) {
        this->terrain->destroy();
        this->terrain->init();
        this->terrain->initShaderProgram();
        this->terrain->initBuffers(std::string(Settings::Instance()->appFolder()));
    }
}

void Kuplung::guiModelDelete(int selectedModel) {
    this->meshModelFaces.erase(this->meshModelFaces.begin() + selectedModel);
}
