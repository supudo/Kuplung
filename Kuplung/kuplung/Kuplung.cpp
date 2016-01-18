//
// Kuplung.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/13/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Kuplung.hpp"
#include <fstream>
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
        MeshModelFace *mmf = this->meshModelFaces[i];
        mmf->destroy();
    }

    this->gui->destroy();
    this->parser->destroy();

    SDL_GL_DeleteContext(this->glContext);

    SDL_DestroyWindow(this->gWindow);
    this->gWindow = NULL;

    SDL_Quit();
}

#pragma mark - run

int Kuplung::run() {
    return this->run(Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
}

int Kuplung::run(int screenWidth, int screenHeight) {
    if (!this->init(screenWidth, screenHeight))
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
        this->gui->renderStart(true);
        this->renderScene();
        this->gui->renderEnd();

        SDL_GL_SwapWindow(this->gWindow);

        if (this->gameIsRunning == false)
            break;
    }

    return 0;
}

#pragma mark - Init

bool Kuplung::init(int screenWidth, int screenHeight) {
    bool success = true;

    Settings::Instance()->SDL_Window_Width = screenWidth;
    Settings::Instance()->SDL_Window_Height = screenHeight;

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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, Settings::Instance()->OpenGLMajorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, Settings::Instance()->OpenGLMinorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

        this->gWindow = SDL_CreateWindow(WINDOW_TITLE, WINDOW_POSITION_X, WINDOW_POSITION_Y, screenWidth, screenHeight, Settings::Instance()->SDL_Window_Flags);
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
                    this->gui = new GUI();
                    this->gui->init(gWindow,
                                    std::bind(&Kuplung::guiQuit, this),
                                    std::bind(&Kuplung::guiProcessObjFile, this, std::placeholders::_1),
                                    std::bind(&Kuplung::guiClearScreen, this));
                    this->doLog("Window initialized.");

                    this->initFolders();
                    this->doLog("App initialized.");

                    this->managerControls = new Controls();
                    this->managerControls->init(std::bind(&Kuplung::objParserLog, this, std::placeholders::_1), this->gWindow);

                    this->parser = new objParser();
                    this->parser->init(std::bind(&Kuplung::objParserLog, this, std::placeholders::_1), std::bind(&Kuplung::doProgress, this, std::placeholders::_1));
                    this->doLog("OBJ Parser Initialized.");

                    this->fontParser = new FNTParser();
                    this->fontParser->init(std::bind(&Kuplung::objParserLog, this, std::placeholders::_1));
                    this->doLog("Font Parser Initialized.");

                    this->terrain = new MeshTerrain();

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
    char *data_path = NULL;
    char *base_path = SDL_GetBasePath();
    if (base_path)
        data_path = base_path;
    else
        data_path = SDL_strdup("./");
    Settings::Instance()->currentFolder = data_path;
    //Settings::Instance()->currentFolder = "/Users/supudo/Software/WebGL/Hypersuit/_project/objects";
    Settings::Instance()->currentFolder = "/Users/supudo/Software/C++/Kuplung/_objects";
}

#pragma mark - Event processing

void Kuplung::onEvent(SDL_Event *ev) {
    this->gui->processEvent(ev);
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

    // escape button
    if (this->managerControls->keyPressed_ESC) {
        this->sceneSelectedModelObject = -1;
        this->selectedMaterialID = "";
    }

    // FOV & zoom
    if (this->managerControls->keyPressed_LALT) {
        if (this->managerControls->mouseWheel.y < 0)
            this->gui->so_GUI_FOV += 4;
        if (this->managerControls->mouseWheel.y > 0)
            this->gui->so_GUI_FOV -= 4;
        if (this->gui->so_GUI_FOV > 180)
            this->gui->so_GUI_FOV = 180;
        if (this->gui->so_GUI_FOV < -180)
            this->gui->so_GUI_FOV = -180;
    }
    else
        this->gui->gui_item_settings[0][17].oValue += this->managerControls->mouseWheel.y;

    // pan world
    if (this->managerControls->mouseButton_MIDDLE) {
        if (this->managerControls->mouseGoUp)
            this->gui->gui_item_settings[0][12].oValue += this->managerControls->yrel;
        else if (this->managerControls->mouseGoDown)
            this->gui->gui_item_settings[0][12].oValue += this->managerControls->yrel;
        else if (this->managerControls->mouseGoLeft)
            this->gui->gui_item_settings[0][13].oValue += this->managerControls->xrel;
        else if (this->managerControls->mouseGoRight)
            this->gui->gui_item_settings[0][13].oValue += this->managerControls->xrel;
    }

    // picking
    if (this->managerControls->mouseButton_LEFT) {
        int mouse_x = this->managerControls->mousePosition.x;
        int mouse_y = this->managerControls->mousePosition.y;

        glm::vec4 viewport = glm::vec4(0.0f, 0.0f, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
        glm::vec3 win_near = glm::vec3(mouse_x, mouse_y, 0.0);
        glm::vec3 win_far = glm::vec3(mouse_x, mouse_y, 1.0);

        glm::vec3 nearPoint = glm::unProject(win_near, this->matrixCamera, this->matrixProjection, viewport);
        glm::vec3 farPoint = glm::unProject(win_far, this->matrixCamera, this->matrixProjection, viewport);
        glm::vec3 direction = glm::normalize(farPoint - nearPoint);

        // http://schabby.de/picking-opengl-ray-tracing/
        // http://stackoverflow.com/questions/27891036/dragging-3-dimensional-objects-with-c-and-opengl
        float sceneClosestObject = -1;

        for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
            MeshModelFace *mmf = this->meshModelFaces[i];
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

#pragma mark - Rendering

void Kuplung::renderScene() {
    // projection
    this->matrixProjection = glm::perspective(glm::radians(this->gui->so_GUI_FOV), this->gui->so_GUI_ratio_w / this->gui->so_GUI_ratio_h, this->gui->so_GUI_plane_close, this->gui->so_GUI_plane_far);

//    this->matrixProjection = glm::translate(this->matrixProjection, glm::vec3(0, 0, 0));
//    this->matrixProjection = glm::rotate(this->matrixProjection, glm::radians(this->gui->gui_item_settings[0][12].oValue), glm::vec3(1, 0, 0));
//    this->matrixProjection = glm::rotate(this->matrixProjection, glm::radians(this->gui->gui_item_settings[0][13].oValue), glm::vec3(0, 1, 0));
//    this->matrixProjection = glm::rotate(this->matrixProjection, glm::radians(this->gui->gui_item_settings[0][14].oValue), glm::vec3(0, 0, 1));
//    this->matrixProjection = glm::translate(this->matrixProjection, glm::vec3(0, 0, 0));
//
//    this->matrixProjection = glm::translate(this->matrixProjection, glm::vec3(this->gui->gui_item_settings[0][15].oValue, this->gui->gui_item_settings[0][16].oValue, this->gui->gui_item_settings[0][17].oValue));

    // camera
    glm::vec3 mtxCamera_eye = glm::vec3(this->gui->gui_item_settings[0][0].oValue, this->gui->gui_item_settings[0][1].oValue, this->gui->gui_item_settings[0][2].oValue);
    glm::vec3 mtxCamera_center = glm::vec3(this->gui->gui_item_settings[0][3].oValue, this->gui->gui_item_settings[0][4].oValue, this->gui->gui_item_settings[0][5].oValue);
    glm::vec3 mtxCamera_up = glm::vec3(this->gui->gui_item_settings[0][6].oValue, this->gui->gui_item_settings[0][7].oValue, this->gui->gui_item_settings[0][8].oValue);

    if (this->gui->isProjection)
        this->matrixCamera = glm::lookAt(mtxCamera_eye, mtxCamera_center, mtxCamera_up);
    else
        this->matrixCamera = glm::ortho(0.0f, (float)Settings::Instance()->SDL_Window_Width, 0.0f, (float)Settings::Instance()->SDL_Window_Height, this->gui->so_GUI_plane_close, this->gui->so_GUI_plane_far);

    glm::mat4 mtxModelGrid = glm::mat4(1.0);
    mtxModelGrid = glm::scale(mtxModelGrid, glm::vec3(this->gui->gui_item_settings[1][9].oValue, this->gui->gui_item_settings[1][10].oValue, this->gui->gui_item_settings[1][11].oValue));
    mtxModelGrid = glm::translate(mtxModelGrid, glm::vec3(0, 0, 0));
    mtxModelGrid = glm::rotate(mtxModelGrid, glm::radians(this->gui->gui_item_settings[1][12].oValue), glm::vec3(1, 0, 0));
    mtxModelGrid = glm::rotate(mtxModelGrid, glm::radians(this->gui->gui_item_settings[1][13].oValue), glm::vec3(0, 1, 0));
    mtxModelGrid = glm::rotate(mtxModelGrid, glm::radians(this->gui->gui_item_settings[1][14].oValue), glm::vec3(0, 0, 1));
    mtxModelGrid = glm::translate(mtxModelGrid, glm::vec3(0, 0, 0));
    mtxModelGrid = glm::translate(mtxModelGrid, glm::vec3(this->gui->gui_item_settings[1][15].oValue, this->gui->gui_item_settings[1][16].oValue, this->gui->gui_item_settings[1][17].oValue));

    this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(this->gui->gui_item_settings[0][15].oValue, this->gui->gui_item_settings[0][16].oValue, this->gui->gui_item_settings[0][17].oValue));
    this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(0, 0, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->gui->gui_item_settings[0][12].oValue), glm::vec3(1, 0, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->gui->gui_item_settings[0][13].oValue), glm::vec3(0, 1, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->gui->gui_item_settings[0][14].oValue), glm::vec3(0, 0, 1));
    this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(0, 0, 0));

    glm::mat4 mtxModelLight = glm::mat4(1.0);
    if (this->gui->fixedGridWorld)
        mtxModelLight = mtxModelGrid;
    mtxModelLight = glm::scale(mtxModelLight, glm::vec3(this->gui->gui_item_settings[2][9].oValue, this->gui->gui_item_settings[2][10].oValue, this->gui->gui_item_settings[2][11].oValue));
    mtxModelLight = glm::translate(mtxModelLight, glm::vec3(0, 0, 0));
    mtxModelLight = glm::rotate(mtxModelLight, glm::radians(this->gui->gui_item_settings[2][12].oValue), glm::vec3(1, 0, 0));
    mtxModelLight = glm::rotate(mtxModelLight, glm::radians(this->gui->gui_item_settings[2][13].oValue), glm::vec3(0, 1, 0));
    mtxModelLight = glm::rotate(mtxModelLight, glm::radians(this->gui->gui_item_settings[2][14].oValue), glm::vec3(0, 0, 1));
    mtxModelLight = glm::translate(mtxModelLight, glm::vec3(0, 0, 0));
    mtxModelLight = glm::translate(mtxModelLight, glm::vec3(this->gui->gui_item_settings[2][15].oValue, this->gui->gui_item_settings[2][16].oValue, this->gui->gui_item_settings[2][17].oValue));

    glm::mat4 mtxModelTerrain = glm::mat4(1.0);
    if (this->gui->fixedGridWorld)
        mtxModelTerrain = mtxModelGrid;
    mtxModelTerrain = glm::scale(mtxModelTerrain, glm::vec3(this->gui->gui_item_settings[3][9].oValue, this->gui->gui_item_settings[3][10].oValue, this->gui->gui_item_settings[3][11].oValue));
    mtxModelTerrain = glm::translate(mtxModelTerrain, glm::vec3(0, 0, 0));
    mtxModelTerrain = glm::rotate(mtxModelTerrain, glm::radians(this->gui->gui_item_settings[3][12].oValue), glm::vec3(1, 0, 0));
    mtxModelTerrain = glm::rotate(mtxModelTerrain, glm::radians(this->gui->gui_item_settings[3][13].oValue), glm::vec3(0, 1, 0));
    mtxModelTerrain = glm::rotate(mtxModelTerrain, glm::radians(this->gui->gui_item_settings[3][14].oValue), glm::vec3(0, 0, 1));
    mtxModelTerrain = glm::translate(mtxModelTerrain, glm::vec3(0, 0, 0));
    mtxModelTerrain = glm::translate(mtxModelTerrain, glm::vec3(this->gui->gui_item_settings[3][15].oValue, this->gui->gui_item_settings[3][16].oValue, this->gui->gui_item_settings[3][17].oValue));

    // lamp
    if (Settings::Instance()->showLight)
        this->meshLight->render(this->matrixProjection, this->matrixCamera, mtxModelLight);

    // axes
    if (Settings::Instance()->showAxes) {
        float axisW = 120;
        float axisH = (Settings::Instance()->SDL_Window_Height * axisW) / Settings::Instance()->SDL_Window_Width;

        float axisX = 10;
        float axisY = 10;

        glViewport(axisX, axisY, axisW, axisH);

        glm::mat4 mtxModel = glm::mat4(1.0);
        mtxModel = glm::rotate(mtxModel, glm::radians(this->gui->gui_item_settings[1][12].oValue), glm::vec3(1, 0, 0));
        mtxModel = glm::rotate(mtxModel, glm::radians(this->gui->gui_item_settings[1][13].oValue), glm::vec3(0, 1, 0));
        mtxModel = glm::rotate(mtxModel, glm::radians(this->gui->gui_item_settings[1][14].oValue), glm::vec3(0, 0, 1));
        this->sceneCoordinateSystem->render(this->matrixProjection, this->matrixCamera, mtxModel);

        glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    }

    // terrain
    if (this->gui->showHeightmap)
        this->terrain->render(this->matrixProjection, this->matrixCamera, mtxModelTerrain);

    // light source
    float px = mtxModelLight[3].x;//this->gui->gui_item_settings[2][15].oValue;
    float py = mtxModelLight[3].y;//this->gui->gui_item_settings[2][16].oValue;
    float pz = mtxModelLight[3].z;//this->gui->gui_item_settings[2][17].oValue;

    glm::vec3 vLightPosition = glm::vec3(px, py, pz);
    glm::vec3 vLightDirection = glm::vec3(0, -2, 0);
    glm::vec3 vCameraPosition = glm::vec3(this->matrixCamera[3].x,this->matrixCamera[3].y, this->matrixCamera[3].z);

    // light dot
    glm::mat4 mtxModelDot = mtxModelLight;
    mtxModelDot = glm::rotate(mtxModelDot, glm::radians(90.0f), glm::vec3(1, 0, 0));
    this->lightDot->initBuffers(glm::vec3(0, 0, 0), vLightDirection, true);
    this->lightDot->render(this->matrixProjection, this->matrixCamera, mtxModelDot);

    // scene models
    for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
        MeshModelFace* mmf = this->meshModelFaces[i];

        int sis = mmf->ModelID;

        glm::mat4 mtxModel = glm::mat4(1.0);

        // reposition like the grid perspective
        mtxModel *= mtxModelGrid;

        mtxModel = glm::scale(mtxModel, glm::vec3(this->gui->scene_item_settings[sis][0].oValue, this->gui->scene_item_settings[sis][1].oValue, this->gui->scene_item_settings[sis][2].oValue));

        mtxModel = glm::translate(mtxModel, glm::vec3(0, 0, 0));
        mtxModel = glm::rotate(mtxModel, glm::radians(this->gui->scene_item_settings[sis][3].oValue), glm::vec3(1, 0, 0));
        mtxModel = glm::rotate(mtxModel, glm::radians(this->gui->scene_item_settings[sis][4].oValue), glm::vec3(0, 1, 0));
        mtxModel = glm::rotate(mtxModel, glm::radians(this->gui->scene_item_settings[sis][5].oValue), glm::vec3(0, 0, 1));
        mtxModel = glm::translate(mtxModel, glm::vec3(0, 0, 0));

        mtxModel = glm::translate(mtxModel, glm::vec3(this->gui->scene_item_settings[sis][6].oValue, this->gui->scene_item_settings[sis][7].oValue, this->gui->scene_item_settings[sis][8].oValue));

        // general
        mmf->setOptionsFOV(this->gui->so_GUI_FOV);
        mmf->setOptionsAlpha(this->gui->so_Alpha);
        mmf->setOptionsDisplacement(glm::vec3(this->gui->scene_item_settings[sis][9].oValue, this->gui->scene_item_settings[sis][10].oValue, this->gui->scene_item_settings[sis][11].oValue));

        // outlining
        mmf->setOptionsSelected(false);
        if (this->selectedMaterialID == mmf->oFace.materialID)
            mmf->setOptionsSelected(true);
        mmf->setOptionsOutlineColor(this->gui->so_GUI_outlineColor);
        mmf->setOptionsOutlineThickness(this->gui->so_outlineThickness);

        // light
        mmf->setOptionsLightPosition(vLightPosition);
        mmf->setOptionsLightDirection(vLightDirection);
        mmf->setOptionsLightAmbient(this->gui->so_GUI_lightAmbient);
        mmf->setOptionsLightDiffuse(this->gui->so_GUI_lightDiffuse);
        printf("diffuse - %f, %f, %f\n", this->gui->so_GUI_lightDiffuse.r, this->gui->so_GUI_lightDiffuse.g, this->gui->so_GUI_lightDiffuse.b);
        mmf->setOptionsLightStrengthAmbient(this->gui->gui_item_settings[2][18].oValue);
        mmf->setOptionsLightStrengthDiffuse(this->gui->gui_item_settings[2][19].oValue);
        mmf->setOptionsLightStrengthSpecular(this->gui->gui_item_settings[2][20].oValue);

        // material
        mmf->setOptionsMaterialRefraction(this->gui->scene_item_settings[sis][12].oValue);
        mmf->setOptionsMaterialShininess(this->gui->scene_item_settings[sis][13].oValue);
        mmf->setOptionsMaterialAmbient(this->gui->scene_item_settings[sis][14].vValue);
        mmf->setOptionsMaterialDiffuse(this->gui->scene_item_settings[sis][15].vValue);
        mmf->setOptionsMaterialSpecular(this->gui->scene_item_settings[sis][16].vValue);

        // render
        mmf->render(this->matrixProjection, this->matrixCamera, mtxModel, vCameraPosition);
    }

    // grid
    if (Settings::Instance()->showGrid) {
        if ((this->gui->so_GUI_grid_size + 1) != this->sceneGridHorizontal->gridSize) {
            this->sceneGridHorizontal->gridSize = this->gui->so_GUI_grid_size;
            this->sceneGridVertical->gridSize = this->gui->so_GUI_grid_size;
            this->sceneGridHorizontal->initBuffers(this->gui->so_GUI_grid_size + 1, true, 1);
            this->sceneGridVertical->initBuffers(this->gui->so_GUI_grid_size + 1, false, 1);
        }

        this->sceneGridHorizontal->render(this->matrixProjection, this->matrixCamera, mtxModelGrid);
        this->sceneGridVertical->render(this->matrixProjection, this->matrixCamera, mtxModelGrid);
    }

    this->processRunningThreads();
}

#pragma mark - Scene GUI

void Kuplung::initSceneGUI() {
    std::map<int, std::vector<float>> initialSettings = {};

    // initial variables
    this->gui->so_GUI_outlineColor = glm::vec3(1.0, 0.0, 0.0);
    this->gui->so_GUI_lightAmbient = glm::vec3(1.0, 1.0, 1.0);
    this->gui->so_GUI_lightDiffuse = glm::vec3(1.0, 1.0, 1.0);
    this->gui->so_GUI_lightSpecular = glm::vec3(1.0, 1.0, 1.0);

    // camera
    initialSettings[0] = std::vector<float> {
        0, 0, 3, // eye
        0, 0, 0, // center
        0, 1, 0,  // up
        1, 1, 1,  // scale
        -71, -36, 0, // rotate
        0, 0, -10  // translate
    };

    // grid
    this->sceneGridHorizontal = new MeshGrid();
    this->sceneGridHorizontal->init(std::bind(&Kuplung::doLog, this, std::placeholders::_1), "grid", Settings::Instance()->OpenGL_GLSL_Version);
    this->sceneGridHorizontal->initShaderProgram();
    this->sceneGridHorizontal->initBuffers(20, true, 1);

    this->sceneGridVertical = new MeshGrid();
    this->sceneGridVertical->init(std::bind(&Kuplung::doLog, this, std::placeholders::_1), "grid", Settings::Instance()->OpenGL_GLSL_Version);
    this->sceneGridVertical->initShaderProgram();
    this->sceneGridVertical->initBuffers(20, false, 1);
    initialSettings[1] = std::vector<float> {
        1, 1, 1, // eye
        0, 0, 0, // center
        0, 1, 0,  // up
        1, 1, 1,  // scale
        0, 0, 0,  // rotate
        0, 0, 0  // translate
    };

    // light
    FBEntity file;
    file.isFile = true;
    file.extension = ".obj";
    file.title = "light";
    file.path = Settings::Instance()->appFolder() + "/gui/light.obj";
    objScene sceneGUI = this->parser->parse(file);
    this->meshLight = new MeshLight();
    this->meshLight->init(std::bind(&Kuplung::doLog, this, std::placeholders::_1), "light", Settings::Instance()->OpenGL_GLSL_Version);
    this->meshLight->setModel(sceneGUI.models[0].faces[0]);
    this->meshLight->initShaderProgram();
    this->meshLight->initBuffers(std::string(Settings::Instance()->appFolder()));
    initialSettings[2] = std::vector<float> {
        1, 1, 1, // eye
        0, 0, 0, // center
        0, 1, 0,  // up
        1, 1, 1,  // scale
        0, 0, 0,  // rotate
        0, 0, 5,  // translate
        1, 0, 0,  // ambient strength
        1, 0, 0,  // diffuse strength
        1, 0, 0  // specular strength
    };

    // axis
    this->sceneCoordinateSystem = new MeshCoordinateSystem();
    this->sceneCoordinateSystem->init(std::bind(&Kuplung::doLog, this, std::placeholders::_1), "axis", Settings::Instance()->OpenGL_GLSL_Version);
    this->sceneCoordinateSystem->initShaderProgram();
    this->sceneCoordinateSystem->initBuffers();

    // terrain
    this->terrain->init(std::bind(&Kuplung::doLog, this, std::placeholders::_1), "terrain", Settings::Instance()->OpenGL_GLSL_Version);
    this->terrain->initShaderProgram();
    this->terrain->initBuffers(std::string(Settings::Instance()->appFolder()));
    initialSettings[3] = std::vector<float> {
        1, 1, 1, // eye
        0, 0, 0, // center
        0, 1, 0,  // up
        1, 1, 1,  // scale
        0, 0, 0,  // rotate
        0, 0, 5  // translate
    };

    this->gui->initGUIControls(4, initialSettings);
    this->gui->showGUIControls();
    this->gui->setHeightmapImage(this->terrain->heightmapImage);

    // dot
    this->lightDot = new MeshDot();
    this->lightDot->init(std::bind(&Kuplung::doLog, this, std::placeholders::_1), "dots", Settings::Instance()->OpenGL_GLSL_Version);
    this->lightDot->initShaderProgram();

    // GUI Editor - shader compilation
    this->gui->setShaderEditor(std::bind(&Kuplung::guiEditorshaderCompiled, this, std::placeholders::_1, std::placeholders::_2));

    // font
//    FBEntity fontFile;
//    fontFile.isFile = true;
//    fontFile.extension = ".fnt";
//    fontFile.title = "ui_font";
//    fontFile.path = Settings::Instance()->appFolder() + "/ui_font.fnt";
//    FontMap fm = this->fontParser->parse(fontFile);

    // testbed
    FBEntity fileTestbed;
    fileTestbed.isFile = true;
    fileTestbed.extension = ".obj";
    fileTestbed.title = "cube0.obj";
    fileTestbed.path = "/Users/supudo/Software/C++/Kuplung/_objects/cube0.obj";
    this->guiProcessObjFile(fileTestbed);
}

#pragma mark - App GUI

void Kuplung::processRunningThreads() {
    if (this->objParserThreadFinished && !this->objParserThreadProcessed) {
        this->gui->hideLoading();
        this->processParsedObjFile();
        this->objParserThreadProcessed = true;
    }
}

void Kuplung::guiProcessObjFile(FBEntity file) {
    if (this->hasEnding(file.title, ".obj")) {
        this->objFile = file;
        this->gui->showLoading();

        if (this->scene.totalCountGeometricVertices > 0)
            this->guiClearScreen();

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
    //std::this_thread::sleep_for(std::chrono::seconds(5));
    this->scene = this->parser->parse(file);
    this->objFile = file;
    this->objParserThreadFinished = true;
}

void Kuplung::processParsedObjFile() {
    this->doLog(this->objFile.title + " was parsed successfully.");
    this->gui->recentFilesAdd(this->objFile.title, this->objFile);

    std::map<int, std::string> scene_models;
    for (int i=0; i<(int)this->scene.models.size(); i++) {
        objModel model = this->scene.models[i];
        scene_models[i] = model.modelID;
        for (size_t j=0; j<model.faces.size(); j++) {
            MeshModelFace *mmf = new MeshModelFace();
            mmf->ModelID = i;
            mmf->init(std::bind(&Kuplung::doLog, this, std::placeholders::_1), Settings::Instance()->ShaderName, Settings::Instance()->OpenGL_GLSL_Version);
            mmf->setModel(model.faces[j]);
            mmf->initShaderProgram();
            mmf->initBuffers(Settings::Instance()->currentFolder);
            this->meshModelFaces.push_back(mmf);
        }
    }

    // render scene stats
    if (this->scene.models.size() > 0) {
        this->gui->showSceneSettings(scene_models);
        //this->gui->showSceneStats();
    }

    this->gui->hideLoading();
}

void Kuplung::doProgress(float value) {
    this->objLoadingProgress = value;
    this->gui->loadingPercentage = value;
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
    if (this->gui)
        this->gui->doLog(logMessage);
}

void Kuplung::guiClearScreen() {
    this->scene = {};
    this->meshModelFaces = {};
    this->gui->hideSceneSettings();
    this->gui->hideSceneStats();
    for (size_t i=0; i<this->meshModelFaces.size(); i++) {
        MeshModelFace *mmf = this->meshModelFaces[i];
        mmf->destroy();
    }
}

void Kuplung::guiEditorshaderCompiled(std::string fileName, std::string fileSource) {
    std::string filePath = Settings::Instance()->appFolder() + "/" + fileName;

#ifdef _WIN32
    std::string nlDelimiter = "\r\n";
#elif defined macintosh // OS 9
    std::string nlDelimiter = "\r";
#else
    std::string nlDelimiter = "\n";
#endif
    std::ofstream out(filePath);
    out << fileSource;
    out.close();

    if (fileName.compare(0, 9, "kuplung") == 0) {
    }
    else if (fileName.compare(0, 5, "light") == 0) {
    }
    else if (fileName.compare(0, 4, "grid") == 0) {
        this->sceneGridHorizontal = new MeshGrid();
        this->sceneGridHorizontal->init(std::bind(&Kuplung::doLog, this, std::placeholders::_1), "grid", Settings::Instance()->OpenGL_GLSL_Version);
        this->sceneGridHorizontal->initShaderProgram();
        this->sceneGridHorizontal->initBuffers(20, true, 1);

        this->sceneGridVertical = new MeshGrid();
        this->sceneGridVertical->init(std::bind(&Kuplung::doLog, this, std::placeholders::_1), "grid", Settings::Instance()->OpenGL_GLSL_Version);
        this->sceneGridVertical->initShaderProgram();
        this->sceneGridVertical->initBuffers(20, false, 1);
    }
    else if (fileName.compare(0, 4, "axis") == 0) {
    }
    else if (fileName.compare(0, 4, "dots") == 0) {
    }
    else if (fileName.compare(0, 7, "terrain") == 0) {
    }
}
