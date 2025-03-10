#
# Kuplung
# Sergey Petrov
#
# qmake
#

# produce LLVM IR code
DO_LLVM_IR = false

# address-sanitize
DO_ADDRESS_SANITIZER = false

# address-sanitize
DO_SAVE_TEMPS = false

# Cuda
DO_CUDA = false

# ========================

TEMPLATE = app

mac {
    CONFIG += c++14
    CONFIG += app_bundle
    CONFIG -= console
    CONFIG -= qt
}
win32|win64 {
    CONFIG += console c++14
    CONFIG -= app_bundle
    CONFIG -= qt
}

CONFIG(debug, debug|release) {
    DEFINES += Def_Kuplung_DEBUG_BUILD
    TARGET = kuplung
    $$DO_LLVM_IR {
        OBJECTS_DIR = debug-ll
        MOC_DIR = debug-ll
    }
    else {
        OBJECTS_DIR = debug
        MOC_DIR = debug
    }
    #QMAKE_POST_LINK = dsymutil "kuplung.app/Contents/MacOS/kuplung"
}

CONFIG(release, debug|release) {
    DEFINES += Def_Kuplung_RELEASE_BUILD
    TARGET = kuplung
    $$DO_LLVM_IR {
        OBJECTS_DIR = release-ll
        MOC_DIR = release-ll
    }
    else {
        OBJECTS_DIR = release
        MOC_DIR = release
    }
}

mac {
    DEFINES += Def_Kuplung_OpenGL_4x
    DEFINES += GLM_ENABLE_EXPERIMENTAL

    QMAKE_CXXFLAGS += -std=c++14
    QMAKE_CXXFLAGS += -stdlib=libc++

    $$DO_SAVE_TEMPS {
        QMAKE_CXXFLAGS += -S -save-temps
    }

    $$DO_LLVM_IR {
        QMAKE_EXT_OBJ = .ll
        QMAKE_CXXFLAGS += -Os -S -emit-llvm
    }

    $$DO_ADDRESS_SANITIZER {
        QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
        QMAKE_CFLAGS += -fsanitize=address -fno-omit-frame-pointer
        QMAKE_LFLAGS += -fsanitize=address
    }

    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
    QMAKE_CXXFLAGS_WARN_ON += -Wno-extern-c-compat
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
    QMAKE_CXXFLAGS_WARN_ON += -Wno-switch
    QMAKE_CXXFLAGS_WARN_ON += -Wno-multichar
    QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS_WARN_ON += -Wno-#warnings
    # Assimp-specific warning
    QMAKE_CXXFLAGS_WARN_ON += -Wno-address-of-packed-member

#    optims
#    QMAKE_CXXFLAGS_WARN_ON += -Wpadded
#    QMAKE_CXXFLAGS_WARN_ON += -Wextra
#    QMAKE_CXXFLAGS_WARN_ON += -Wshadow
#    QMAKE_CXXFLAGS_WARN_ON += -Wnon-virtual-dtor
#    QMAKE_CXXFLAGS_WARN_ON += -pedantic
#    QMAKE_CXXFLAGS_WARN_ON += -Wunused
#    QMAKE_CXXFLAGS_WARN_ON += -Wcast-align
#    QMAKE_CXXFLAGS_WARN_ON += -Wmisleading-indentation
#    QMAKE_CXXFLAGS_WARN_ON += -Weffc++
#    QMAKE_CXXFLAGS_WARN_ON += ‑fno‑elide‑constructors

    QMAKE_MAC_SDK = macosx10.12

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11

    QMAKE_INFO_PLIST = Info.plist

    plist.path = "$$DESTDIR/$$join(TARGET,,,.app)/Contents"
    plist.files = Info.plist
    INSTALLS += plist

    MediaFiles.files = resources/Kuplung_Settings.ini
    MediaFiles.files += resources/Kuplung_RecentFiles.ini
    MediaFiles.files += resources/Kuplung_RecentFilesImported.ini
    MediaFiles.files += resources/Kuplung.icns
    MediaFiles.files += resources/noise16.png
    MediaFiles.files += resources/gui
    MediaFiles.files += resources/oui
    MediaFiles.files += resources/fonts
    MediaFiles.files += resources/shaders
    MediaFiles.files += resources/shaders/stoy
    MediaFiles.files += resources/shaders/cuda
    MediaFiles.files += resources/shadertoy
    MediaFiles.files += resources/shapes
    MediaFiles.files += resources/skybox
    MediaFiles.files += resources/axis_helpers
    MediaFiles.files += resources/lua
    MediaFiles.path = Contents/Resources
    QMAKE_BUNDLE_DATA += MediaFiles

    LIBS += -framework cocoa -framework OpenGL

    LIBS += -L/usr/local/Cellar/glm/0.9.9.0/lib
    INCLUDEPATH += /usr/local/Cellar/glm/0.9.9.0/include

    LIBS += -L/usr/local/Cellar/sdl2/2.0.8/lib -lSDL2
    INCLUDEPATH += /usr/local/Cellar/sdl2/2.0.8/include

    LIBS += -L/usr/local/Cellar/glfw3/3.2.1/lib -lGLFW3
    INCLUDEPATH += /usr/local/Cellar/glfw3/3.2.1/include

    LIBS += -L/usr/local/Cellar/assimp/4.1.0/lib -lAssimp
    INCLUDEPATH += /usr/local/Cellar/assimp/4.1.0/include

    LIBS += -L/usr/local/Cellar/protobuf/3.5.1_1/lib -lprotobuf
    INCLUDEPATH += /usr/local/Cellar/protobuf/3.5.1_1/include

    LIBS += -L/usr/local/Cellar/minizip/1.2.11/lib -lz -lMinizip
    INCLUDEPATH += /usr/local/Cellar/minizip/1.2.11/include

    LIBS += -L/usr/local/Cellar/lua/5.3.4_4/lib -lz -llua.5.3.4 -lm
    INCLUDEPATH += /usr/local/Cellar/lua/5.3.4_4/include/lua

#    LIBS += -L/usr/local/Cellar/unittest-cpp/1.6.1/lib -lUnitTest++
#    INCLUDEPATH += /usr/local/Cellar/unittest-cpp/1.6.1/include

    LIBS += -L"$$PWD/../external/libnoise/lib" -lnoise
    INCLUDEPATH += "$$PWD/../external/libnoise/headers"
}

win32|win64 {
    DEFINES += Def_Kuplung_OpenGL_4x

    #QMAKE_CXXFLAGS += /std:c++latest

    QMAKE_CXXFLAGS_WARN_ON += -wd4244 -wd4100 -wd4068 -wd4267 -wd4305 -wd4838 -wd4477 -wd4305 -wd4996 -wd4309

    OBJECTS_DIR = "./objects/"

    RC_FILE += resources/kuplung.rc

    INCLUDEPATH += "D:\Libs\glm-0.9.8.4"

    LIBS += -lopengl32 -lshell32

    LIBS += -L"D:/Libs/SDL2-2.0.5/lib/x64" -lSDL2main -lSDL2
    INCLUDEPATH += "D:\Libs\SDL2-2.0.5\include"

    LIBS += -L"D:/Libs/GLFW3/lib-vc2015" -lGLFW3
    INCLUDEPATH += "D:/Libs/GLFW3/include"

    LIBS += -L"D:/Libs/glew-2.0.0-win32/lib/Release/x64" -lGLEW32
    INCLUDEPATH += "D:/Libs/glew-2.0.0-win32/include"

    LIBS += -L"D:\Libs\assimp\x64\lib" -lassimp-vc140-mt
    INCLUDEPATH += "D:\Libs\assimp\x64\include"

    LIBS += -L"D:\Libs\protobuf\lib" -llibprotobufd
    INCLUDEPATH += "D:\Libs\protobuf\include"

    LIBS += -L"D:\Libs\lua-5.3.4_Win64_vc14_lib\lib" -llua53
    INCLUDEPATH += "D:\Libs\lua-5.3.4_Win64_vc14_lib\include"

    LIBS += -L"$$PWD\..\external\libnoise\windows\lib" -lnoise
    INCLUDEPATH += "$$PWD\..\external\libnoise\windows\include"

    SOURCES += kuplung/utilities/consumption/WindowsCPUUsage.cpp
    HEADERS += kuplung/utilities/consumption/WindowsCPUUsage.hpp

    PWD_WIN = $${PWD}/resources
    DESTDIR_WIN = "./resources"
    PWD_WIN ~= s,/,\\,g
    DESTDIR_WIN ~= s,/,\\,g
    copyfiles.commands = $$quote(cmd /c xcopy /S /I /Y $${PWD_WIN} $${DESTDIR_WIN})
    QMAKE_EXTRA_TARGETS += copyfiles
    POST_TARGETDEPS += copyfiles
}

linux {
    DEFINES += Def_Kuplung_OpenGL_2x
    QMAKE_CXXFLAGS += -std=c++14

    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
    QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS_WARN_ON += -Wno-misleading-indentation
    QMAKE_CXXFLAGS_WARN_ON += -Wno-type-limits

    INCLUDEPATH += /usr/local/include
    INCLUDEPATH += /usr/include/lua5.3

    LIBS += -L/usr/lib
    LIBS += -lGLEW -lGLU -lm -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp
    LIBS += -lSDL2
    LIBS += -lglfw
    LIBS += -lprotobuf
    LIBS += -lminizip
    LIBS += -lassimp -lstdc++ -lz
    LIBS += -llua5.3
    LIBS +=  -lnoise
}

# BEGIN Cuda

$$DO_CUDA {
    mac {
        K_C_LIBS = -L/usr/local/cuda/lib -lcuda -lcudart -lcufft
        LIBS += -L/usr/local/cuda/lib -lcuda -lcudart -lcufft
        INCLUDEPATH += /usr/local/cuda/include

        CUDA_DIR = /usr/local/cuda

        CUDA_OBJECTS_DIR = cuda

        OTHER_FILES += kuplung/cuda/cu/cuda_vectorAddition.cu \
            kuplung/cuda/cu/cuda_oceanFFT_kernel.cu \
            kuplung/cuda/cu/cuda_OBJParser.cu

        CUDA_SOURCES += kuplung/cuda/cu/cuda_vectorAddition.cu \
            kuplung/cuda/cu/cuda_oceanFFT_kernel.cu \
            kuplung/cuda/cu/cuda_OBJParser.cu

        SYSTEM_NAME = x64
        SYSTEM_TYPE = 64
        CUDA_ARCH = compute_30
        NVCC_OPTIONS = --use_fast_math
        NVCC_COMM = $$CUDA_DIR/bin/nvcc

        cuda_d.input = CUDA_SOURCES
        cuda_d.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
        cuda_d.dependency_type = TYPE_C
        cuda_d.commands = $$NVCC_COMM -D_DEBUG $$NVCC_OPTIONS $$K_C_LIBS --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
        QMAKE_EXTRA_COMPILERS += cuda_d

        DEFINES += "DEF_KuplungSetting_UseCuda"
        DEFINES += "GLM_FORCE_CUDA"
    }
}

# END Cuda

OTHER_FILES += resources/*
OTHER_FILES += resources/gui/*
OTHER_FILES += resources/oui/*
OTHER_FILES += resources/fonts/*
OTHER_FILES += resources/shaders/*
OTHER_FILES += resources/shaders/stoy/*
OTHER_FILES += resources/shaders/cuda/*
OTHER_FILES += resources/shapes/*
OTHER_FILES += resources/skybox/*
OTHER_FILES += resources/shadertoy/*
OTHER_FILES += resources/axis_helpers/*
OTHER_FILES += resources/lua/*

SOURCES += main.cpp \
    kuplung/Kuplung.cpp \
    kuplung/cuda/CudaExamples.cpp \
    kuplung/utilities/cuda/CudaHelpers.cpp \
    kuplung/cuda/examples/VectorAddition.cpp \
    kuplung/cuda/examples/oceanFFT.cpp \
    kuplung/rendering/RenderingManager.cpp \
    kuplung/rendering/methods/RenderingSimple.cpp \
    kuplung/rendering/methods/RenderingForward.cpp \
    kuplung/rendering/methods/RenderingForwardShadowMapping.cpp \
    kuplung/rendering/methods/RenderingDeferred.cpp \
    kuplung/meshes/scene/ModelFaceBase.cpp \
    kuplung/meshes/scene/ModelFaceData.cpp \
    kuplung/meshes/artefacts/Terrain.cpp \
    kuplung/meshes/artefacts/Spaceship.cpp \
    kuplung/meshes/artefacts/StructuredVolumetricSampling.cpp \
    kuplung/meshes/artefacts/Shadertoy.cpp \
    kuplung/meshes/helpers/MiniAxis.cpp \
    kuplung/meshes/helpers/AxisHelpers.cpp \
    kuplung/meshes/helpers/WorldGrid.cpp \
    kuplung/meshes/helpers/Light.cpp \
    kuplung/meshes/helpers/Camera.cpp \
    kuplung/meshes/helpers/CameraModel.cpp \
    kuplung/meshes/helpers/LightRay.cpp \
    kuplung/meshes/helpers/BoundingBox.cpp \
    kuplung/meshes/helpers/RayLine.cpp \
    kuplung/meshes/helpers/Skybox.cpp \
    kuplung/meshes/helpers/VertexSphere.cpp \
    kuplung/objects/ObjectsManager.cpp \
    kuplung/objects/RayPicking.cpp \
    kuplung/settings/ConfigUtils.cpp \
    kuplung/settings/Settings.cpp \
    kuplung/settings/FontsList.cpp \
    kuplung/ui/UIManager.cpp \
    kuplung/ui/UIHelpers.cpp \
    kuplung/ui/implementation/gui_oui/GUI_OUI.cpp \
    kuplung/ui/implementation/gui_oui/OuiNanoVG_Implementation.cpp \
    kuplung/ui/implementation/gui_imgui/GUI_ImGui.cpp \
    kuplung/ui/implementation/gui_imgui/ImGui_Implementation_OpenGL32.cpp \
    kuplung/ui/implementation/gui_imgui/ImGui_Implementation_SDL2.cpp \
    kuplung/ui/components/ShaderEditor.cpp \
    kuplung/ui/components/FileBrowser.cpp \
    kuplung/ui/components/importers/ImportFile.cpp \
    kuplung/ui/components/exporters/ExportFile.cpp \
    kuplung/ui/components/exporters/ExportOBJ.cpp \
    kuplung/ui/components/exporters/ExportGLTF.cpp \
    kuplung/ui/components/FileSaver.cpp \
    kuplung/ui/components/ImageViewer.cpp \
    kuplung/ui/components/RendererUI.cpp \
    kuplung/ui/components/Log.cpp \
    kuplung/ui/components/Screenshot.cpp \
    kuplung/ui/components/ColorPicker.cpp \
    kuplung/ui/components/Tabs.cpp \
    kuplung/ui/components/uveditor/UVEditor.cpp \
    kuplung/ui/components/uveditor/UVPoint.cpp \
    kuplung/ui/components/materialeditor/MaterialEditor.cpp \
    kuplung/ui/components/materialeditor/MELink.cpp \
    kuplung/ui/components/materialeditor/MENode.cpp \
    kuplung/ui/components/materialeditor/MENode_Color.cpp \
    kuplung/ui/components/materialeditor/MENode_Combine.cpp \
    kuplung/ui/components/materialeditor/MENode_Texture.cpp \
    kuplung/ui/components/ide/KuplungIDE.cpp \
    kuplung/ui/dialogs/DialogStyle.cpp \
    kuplung/ui/dialogs/DialogOptions.cpp \
    kuplung/ui/dialogs/DialogControlsGUI.cpp \
    kuplung/ui/dialogs/DialogControlsModels.cpp \
    kuplung/ui/dialogs/DialogSVS.cpp \
    kuplung/ui/dialogs/DialogShadertoy.cpp \
    kuplung/pcg/HeightmapGenerator.cpp \
    kuplung/pcg/SpaceshipMeshGenerator.cpp \
    kuplung/pcg/VoronoiGenerator.cpp \
    kuplung/utilities/font-parser/FNTParser.cpp \
    kuplung/utilities/gl/GLUtils.cpp \
    kuplung/utilities/imgui/imgui.cpp \
    kuplung/utilities/imgui/imgui_demo.cpp \
    kuplung/utilities/imgui/imgui_draw.cpp \
    kuplung/utilities/imgui/ImGuiColorTextEdit/TextEditor.cpp \
    kuplung/utilities/imgui/imguizmo/ImGuizmo.cpp \
    kuplung/utilities/nanovg/nanovg.c \
    kuplung/utilities/input/Controls.cpp \
    kuplung/utilities/libnoise/noiseutils.cpp \
    kuplung/utilities/maths/Maths.cpp \
    kuplung/utilities/parsers/FileModelManager.cpp \
    kuplung/utilities/parsers/ParserUtils.cpp \
    kuplung/utilities/parsers/obj-parser/objParser1.cpp \
    kuplung/utilities/parsers/obj-parser/objParser2.cpp \
    kuplung/utilities/parsers/obj-parser/objParserCuda.cpp \
    kuplung/utilities/parsers/stl-parser/STLParser.cpp \
    kuplung/utilities/parsers/ply-parser/PLYParser.cpp \
    kuplung/utilities/parsers/gltf-parser/GLTFParser.cpp \
    kuplung/utilities/parsers/assimp-parser/AssimpParser.cpp \
    kuplung/utilities/export/Exporter.cpp \
    kuplung/utilities/export/ExporterAssimp.cpp \
    kuplung/utilities/export/ExporterOBJ.cpp \
    kuplung/utilities/export/ExporterGLTF.cpp \
    kuplung/utilities/renderers/ImageRenderer.cpp \
    kuplung/utilities/renderers/KuplungRendererBase.cpp \
    kuplung/utilities/renderers/default-forward/DefaultForwardRenderer.cpp \
    kuplung/utilities/renderers/scene-renderer/SceneRenderer.cpp \
    kuplung/utilities/renderers/ray-tracer/RayTracerRenderer.cpp \
    kuplung/utilities/minizip/KuplungMinizip.cpp \
    kuplung/utilities/miniz/miniz.c \
    kuplung/utilities/miniz/KuplungMiniz.cpp \
    kuplung/utilities/saveopen/SaveOpen.cpp \
    kuplung/utilities/saveopen/SaveOpenBinarySeq.cpp \
    kuplung/utilities/saveopen/SaveOpenGProtocolBufs.cpp \
    kuplung/utilities/saveopen/KuplungDefinitions.pb.cc \
    kuplung/utilities/saveopen/KuplungAppSettings.pb.cc \
    kuplung/utilities/saveopen/KuplungAppScene.pb.cc \
    kuplung/utilities/consumption/Consumption.cpp \
    kuplung/utilities/measurement/Timings.cpp \
    kuplung/utilities/lua/LuaManager.cpp \
    kuplung/utilities/cpp-base64/base64.cpp

HEADERS += \
    kuplung/Kuplung.hpp \
    kuplung/cuda/CudaExamples.hpp \
    kuplung/utilities/cuda/CudaHelpers.hpp \
    kuplung/cuda/examples/VectorAddition.hpp \
    kuplung/cuda/examples/oceanFFT.hpp \
    kuplung/rendering/RenderingManager.hpp \
    kuplung/rendering/methods/RenderingSimple.hpp \
    kuplung/rendering/methods/RenderingForward.hpp \
    kuplung/rendering/methods/RenderingForwardShadowMapping.hpp \
    kuplung/rendering/methods/RenderingDeferred.hpp \
    kuplung/meshes/scene/ModelFaceBase.hpp \
    kuplung/meshes/scene/ModelFaceData.hpp \
    kuplung/meshes/artefacts/Terrain.hpp \
    kuplung/meshes/artefacts/Spaceship.hpp \
    kuplung/meshes/artefacts/StructuredVolumetricSampling.hpp \
    kuplung/meshes/artefacts/Shadertoy.hpp \
    kuplung/meshes/helpers/MiniAxis.hpp \
    kuplung/meshes/helpers/AxisHelpers.hpp \
    kuplung/meshes/helpers/WorldGrid.hpp \
    kuplung/meshes/helpers/Light.hpp \
    kuplung/meshes/helpers/Camera.hpp \
    kuplung/meshes/helpers/CameraModel.hpp \
    kuplung/meshes/helpers/LightRay.hpp \
    kuplung/meshes/helpers/BoundingBox.hpp \
    kuplung/meshes/helpers/RayLine.hpp \
    kuplung/meshes/helpers/Skybox.hpp \
    kuplung/meshes/helpers/VertexSphere.hpp \
    kuplung/objects/Objects.h \
    kuplung/objects/ObjectDefinitions.h \
    kuplung/objects/ObjectsManager.hpp \
    kuplung/objects/RayPicking.hpp \
    kuplung/settings/ConfigUtils.hpp \
    kuplung/settings/SettingsStructs.h \
    kuplung/settings/Settings.h \
    kuplung/settings/FontsList.hpp \
    kuplung/ui/UIManager.hpp \
    kuplung/ui/UIHelpers.hpp \
    kuplung/ui/implementation/gui_oui/GUI_OUI.hpp \
    kuplung/ui/implementation/gui_oui/OuiNanoVG_Implementation.hpp \
    kuplung/ui/implementation/gui_imgui/GUI_ImGui.hpp \
    kuplung/ui/implementation/gui_imgui/ImGui_Implementation_OpenGL32.hpp \
    kuplung/ui/implementation/gui_imgui/ImGui_Implementation_SDL2.hpp \
    kuplung/ui/components/ShaderEditor.hpp \
    kuplung/ui/components/FileBrowser.hpp \
    kuplung/ui/components/importers/ImportFile.hpp \
    kuplung/ui/components/exporters/ExportFile.hpp \
    kuplung/ui/components/exporters/ExportOBJ.hpp \
    kuplung/ui/components/exporters/ExportGLTF.hpp \
    kuplung/ui/components/FileSaver.hpp \
    kuplung/ui/components/ImageViewer.hpp \
    kuplung/ui/components/RendererUI.hpp \
    kuplung/ui/components/Log.hpp \
    kuplung/ui/components/Screenshot.hpp \
    kuplung/ui/components/ColorPicker.hpp \
    kuplung/ui/components/Tabs.hpp \
    kuplung/ui/components/uveditor/UVEditor.hpp \
    kuplung/ui/components/uveditor/UVPoint.hpp \
    kuplung/ui/components/materialeditor/MaterialEditorData.h \
    kuplung/ui/components/materialeditor/MaterialEditor.hpp \
    kuplung/ui/components/materialeditor/MELink.hpp \
    kuplung/ui/components/materialeditor/MENode.hpp \
    kuplung/ui/components/materialeditor/MENode_Color.hpp \
    kuplung/ui/components/materialeditor/MENode_Combine.hpp \
    kuplung/ui/components/materialeditor/MENode_Texture.hpp \
    kuplung/ui/components/ide/KuplungIDE.hpp \
    kuplung/ui/dialogs/DialogStyle.hpp \
    kuplung/ui/dialogs/DialogOptions.hpp \
    kuplung/ui/dialogs/DialogControlsGUI.hpp \
    kuplung/ui/dialogs/DialogControlsModels.hpp \
    kuplung/ui/dialogs/DialogSVS.hpp \
    kuplung/ui/dialogs/DialogShadertoy.hpp \
    kuplung/ui/iconfonts/IconsFontAwesome.h \
    kuplung/ui/iconfonts/IconsMaterialDesign.h \
    kuplung/pcg/HeightmapGenerator.hpp \
    kuplung/pcg/SpaceshipMeshGenerator.hpp \
    kuplung/pcg/VoronoiGenerator.hpp \
    kuplung/utilities/font-parser/FNTParser.hpp \
    kuplung/utilities/gl/GLUtils.hpp \
    kuplung/utilities/imgui/imconfig.h \
    kuplung/utilities/imgui/imgui.h \
    kuplung/utilities/imgui/imgui_internal.h \
    kuplung/utilities/imgui/ImGuiColorTextEdit/TextEditor.h \
    kuplung/utilities/imgui/imguizmo/ImGuizmo.h \
    kuplung/utilities/nanovg/fontstash.h \
    kuplung/utilities/nanovg/nanovg_gl_utils.h \
    kuplung/utilities/nanovg/nanovg_gl.h \
    kuplung/utilities/nanovg/nanovg.h \
    kuplung/utilities/oui/oui.h \
    kuplung/utilities/oui/blendish.h \
    kuplung/utilities/input/Controls.hpp \
    kuplung/utilities/libnoise/noiseutils.h \
    kuplung/utilities/maths/Maths.hpp \
    kuplung/utilities/parsers/FileModelManager.hpp \
    kuplung/utilities/parsers/ParserUtils.hpp \
    kuplung/utilities/parsers/ModelObject.h \
    kuplung/utilities/parsers/obj-parser/objParser1.hpp \
    kuplung/utilities/parsers/obj-parser/objParser2.hpp \
    kuplung/utilities/parsers/obj-parser/objParserCuda.hpp \
    kuplung/utilities/parsers/stl-parser/STLParser.hpp \
    kuplung/utilities/parsers/ply-parser/PLYParser.hpp \
    kuplung/utilities/parsers/gltf-parser/GLTFParser.hpp \
    kuplung/utilities/parsers/assimp-parser/AssimpParser.hpp \
    kuplung/utilities/stb/stb_image.h \
    kuplung/utilities/stb/stb_image_write.h \
    kuplung/utilities/stb/stb_rect_pack.h \
    kuplung/utilities/stb/stb_textedit.h \
    kuplung/utilities/stb/stb_truetype.h \
    kuplung/utilities/gl/GLIncludes.h \
    kuplung/utilities/shapes/Shapes.h \
    kuplung/utilities/export/Exporter.hpp \
    kuplung/utilities/export/ExporterAssimp.hpp \
    kuplung/utilities/export/ExporterOBJ.hpp \
    kuplung/utilities/export/ExporterGLTF.hpp \
    kuplung/utilities/renderers/ImageRenderer.hpp \
    kuplung/utilities/renderers/KuplungRendererBase.hpp \
    kuplung/utilities/renderers/default-forward/DefaultForwardRenderer.hpp \
    kuplung/utilities/renderers/scene-renderer/SceneRenderer.hpp \
    kuplung/utilities/renderers/ray-tracer/RayTracerRenderer.hpp \
    kuplung/utilities/minizip/KuplungMinizip.hpp \
    kuplung/utilities/miniz/miniz.h \
    kuplung/utilities/miniz/KuplungMiniz.hpp \
    kuplung/utilities/saveopen/SaveOpen.hpp \
    kuplung/utilities/saveopen/SaveOpenBinarySeq.hpp \
    kuplung/utilities/saveopen/SaveOpenGProtocolBufs.hpp \
    kuplung/utilities/saveopen/KuplungDefinitions.pb.h \
    kuplung/utilities/saveopen/KuplungAppSettings.pb.h \
    kuplung/utilities/saveopen/KuplungAppScene.pb.h \
    kuplung/utilities/consumption/Consumption.hpp \
    kuplung/utilities/measurement/Timings.hpp \
    kuplung/utilities/lua/LuaManager.hpp \
    kuplung/utilities/catch/catch.hpp \
    kuplung/utilities/json/json.hpp \
    kuplung/utilities/cpp-base64/base64.h

DISTFILES += \
# Lua
    resources/lua/test.lua \
# Fonts
    resources/fonts/fontawesome-webfont.ttf \
    resources/fonts/material-icons-regular.ttf \
# .proto files
    resources/protobuf/KuplungDefinitions.proto \
    resources/protobuf/KuplungAppSettings.proto \
    resources/protobuf/KuplungAppScene.proto \
# OUI/nanovg files
    resources/oui/blender_icons.svg \
    resources/oui/blender_icons16.png \
    resources/oui/DejaVuSans.ttf \
# Axis objects
    resources/axis_helpers/x_minus.obj \
    resources/axis_helpers/x_minus.mtl \
    resources/axis_helpers/x_plus.obj \
    resources/axis_helpers/x_plus.mtl \
    resources/axis_helpers/y_minus.obj \
    resources/axis_helpers/y_minus.mtl \
    resources/axis_helpers/y_plus.obj \
    resources/axis_helpers/y_plus.mtl \
    resources/axis_helpers/z_minus.obj \
    resources/axis_helpers/z_minus.mtl \
    resources/axis_helpers/z_plus.obj \
    resources/axis_helpers/z_plus.mtl \
# Shapes
    resources/shapes/cone.mtl \
    resources/shapes/cone.obj \
    resources/shapes/cube.mtl \
    resources/shapes/cube.obj \
    resources/shapes/cylinder.mtl \
    resources/shapes/cylinder.obj \
    resources/shapes/epcot.mtl \
    resources/shapes/epcot.obj \
    resources/shapes/grid.mtl \
    resources/shapes/grid.obj \
    resources/shapes/ico_sphere.mtl \
    resources/shapes/ico_sphere.obj \
    resources/shapes/monkey_head.mtl \
    resources/shapes/monkey_head.obj \
    resources/shapes/MaterialBall.mtl \
    resources/shapes/MaterialBall.obj \
    resources/shapes/MaterialBallBlender.mtl \
    resources/shapes/MaterialBallBlender.obj \
    resources/shapes/plane.mtl \
    resources/shapes/plane.obj \
    resources/shapes/plane_objects.mtl \
    resources/shapes/plane_objects.obj \
    resources/shapes/plane_objects_large.mtl \
    resources/shapes/plane_objects_large.obj \
    resources/shapes/triangle.mtl \
    resources/shapes/triangle.obj \
    resources/shapes/torus.mtl \
    resources/shapes/torus.obj \
    resources/shapes/tube.mtl \
    resources/shapes/tube.obj \
    resources/shapes/uv_sphere.mtl \
    resources/shapes/uv_sphere.obj \
    resources/shapes/brick_wall.mtl \
    resources/shapes/brick_wall.obj \
    resources/shapes/brick_wall_diffuse.png \
    resources/shapes/brick_wall_displacement.png \
    resources/shapes/brick_wall_normal.png \
    resources/shapes/brick_wall_spec.png \
    resources/shapes/brick_wall_occlusion.png \
# Shaders
    resources/shaders/axis.vert \
    resources/shaders/axis.frag \
    resources/shaders/axis_helpers.vert \
    resources/shaders/axis_helpers.frag \
    resources/shaders/camera.vert \
    resources/shaders/camera.frag \
    resources/shaders/light_ray.vert \
    resources/shaders/light_ray.frag \
    resources/shaders/ray_line.vert \
    resources/shaders/ray_line.frag \
    resources/shaders/grid.vert \
    resources/shaders/grid.frag \
    resources/shaders/deferred_g_buffer.vert \
    resources/shaders/deferred_g_buffer.frag \
    resources/shaders/deferred_light_box.vert \
    resources/shaders/deferred_light_box.frag \
    resources/shaders/deferred_shading.vert \
    resources/shaders/deferred_shading.frag \
    resources/shaders/rendering_simple.vert \
    resources/shaders/rendering_simple.tcs \
    resources/shaders/rendering_simple.tes \
    resources/shaders/rendering_simple.geom \
    resources/shaders/rendering_simple.frag \
    resources/shaders/model_face.vert \
    resources/shaders/model_face.frag \
    resources/shaders/model_face_effects.frag \
    resources/shaders/model_face_lights.frag \
    resources/shaders/model_face_mapping.frag \
    resources/shaders/model_face_misc.frag \
    resources/shaders/model_face_shadow_mapping.frag \
    resources/shaders/model_face_pbr.frag \
    resources/shaders/model_face_vars.frag \
    resources/shaders/model_face.geom \
    resources/shaders/model_face.tcs \
    resources/shaders/model_face.tes \
    resources/shaders/shadow_mapping_depth.vert \
    resources/shaders/shadow_mapping_depth.frag \
    resources/shaders/shadows_debug_quad.vert \
    resources/shaders/shadows_debug_quad_depth.frag \
    resources/shaders/reflection.vert \
    resources/shaders/reflection.frag \
    resources/shaders/light.vert \
    resources/shaders/light.frag \
    resources/shaders/terrain.vert \
    resources/shaders/terrain.frag \
    resources/shaders/spaceship.vert \
    resources/shaders/spaceship.frag \
    resources/shaders/skybox.vert \
    resources/shaders/skybox.frag \
    resources/shaders/bounding_box.vert \
    resources/shaders/bounding_box.frag \
    resources/shaders/vertex_sphere.vert \
    resources/shaders/vertex_sphere.frag \
    resources/shaders/shadertoy.vert \
    resources/shaders/structured_vol_sampling.vert \
    resources/shaders/structured_vol_sampling.frag \
# Shaders - shadertoy
    resources/shaders/stoy/4ljGW1.stoy \
    resources/shaders/stoy/4tlSzl.stoy \
    resources/shaders/stoy/Ms2SD1.stoy \
    resources/shaders/stoy/XlfGRj.stoy \
    resources/shaders/stoy/XlSSzK.stoy \
# Shaders - cuda
    resources/shaders/cuda/oceanFFT.vert \
    resources/shaders/cuda/oceanFFT.frag \
# Cube textures
    resources/skybox/fire_planet_back.jpg \
    resources/skybox/fire_planet_bottom.jpg \
    resources/skybox/fire_planet_front.jpg \
    resources/skybox/fire_planet_left.jpg \
    resources/skybox/fire_planet_right.jpg \
    resources/skybox/fire_planet_top.jpg \
    resources/skybox/lake_mountain_back.jpg \
    resources/skybox/lake_mountain_bottom.jpg \
    resources/skybox/lake_mountain_front.jpg \
    resources/skybox/lake_mountain_left.jpg \
    resources/skybox/lake_mountain_right.jpg \
    resources/skybox/lake_mountain_top.jpg \
    resources/skybox/stormydays_right.jpg \
    resources/skybox/stormydays_left.jpg \
    resources/skybox/stormydays_top.jpg \
    resources/skybox/stormydays_bottom.jpg \
    resources/skybox/stormydays_back.jpg \
    resources/skybox/stormydays_front.jpg \
# ShaderToy resources
    resources/shadertoy/LICENSE.txt \
    resources/shadertoy/cube00_0.jpg \
    resources/shadertoy/cube00_1.jpg \
    resources/shadertoy/cube00_2.jpg \
    resources/shadertoy/cube00_3.jpg \
    resources/shadertoy/cube00_4.jpg \
    resources/shadertoy/cube00_5.jpg \
    resources/shadertoy/cube01_0.png \
    resources/shadertoy/cube01_1.png \
    resources/shadertoy/cube01_2.png \
    resources/shadertoy/cube01_3.png \
    resources/shadertoy/cube01_4.png \
    resources/shadertoy/cube01_5.png \
    resources/shadertoy/cube02_0.jpg \
    resources/shadertoy/cube02_1.jpg \
    resources/shadertoy/cube02_2.jpg \
    resources/shadertoy/cube02_3.jpg \
    resources/shadertoy/cube02_4.jpg \
    resources/shadertoy/cube02_5.jpg \
    resources/shadertoy/cube03_0.png \
    resources/shadertoy/cube03_1.png \
    resources/shadertoy/cube03_2.png \
    resources/shadertoy/cube03_3.png \
    resources/shadertoy/cube03_4.png \
    resources/shadertoy/cube03_5.png \
    resources/shadertoy/cube04_0.png \
    resources/shadertoy/cube04_1.png \
    resources/shadertoy/cube04_2.png \
    resources/shadertoy/cube04_3.png \
    resources/shadertoy/cube04_4.png \
    resources/shadertoy/cube04_5.png \
    resources/shadertoy/cube05_0.png \
    resources/shadertoy/cube05_1.png \
    resources/shadertoy/cube05_2.png \
    resources/shadertoy/cube05_3.png \
    resources/shadertoy/cube05_4.png \
    resources/shadertoy/cube05_5.png \
    resources/shadertoy/tex00.jpg \
    resources/shadertoy/tex01.jpg \
    resources/shadertoy/tex02.jpg \
    resources/shadertoy/tex03.jpg \
    resources/shadertoy/tex04.jpg \
    resources/shadertoy/tex05.jpg \
    resources/shadertoy/tex06.jpg \
    resources/shadertoy/tex07.jpg \
    resources/shadertoy/tex08.jpg \
    resources/shadertoy/tex09.jpg \
    resources/shadertoy/tex10.png \
    resources/shadertoy/tex11.png \
    resources/shadertoy/tex12.png \
    resources/shadertoy/tex13.png \
    resources/shadertoy/tex14.png \
    resources/shadertoy/tex15.png \
    resources/shadertoy/tex16.png \
    resources/shadertoy/tex17.jpg \
    resources/shadertoy/tex18.jpg \
    resources/shadertoy/tex19.png \
    resources/shadertoy/tex20.jpg \
    resources/shadertoy/webcamBig.png \
    resources/kuplung.rc
