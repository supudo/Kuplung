TEMPLATE = app

CONFIG += c++11
CONFIG += app_bundle
CONFIG -= console
CONFIG -= qt

CONFIG(debug, debug|release) {
  TARGET = kuplung
  OBJECTS_DIR = debug
  MOC_DIR = debug
}

CONFIG(release, debug|release) {
  TARGET = kuplung
  OBJECTS_DIR = release
  MOC_DIR = release
}

mac {
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
    QMAKE_CXXFLAGS_WARN_ON += -Wno-extern-c-compat
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11

    QMAKE_INFO_PLIST = Info.plist

    plist.path = "$$DESTDIR/$$join(TARGET,,,.app)/Contents"
    plist.files = Info.plist
    INSTALLS += plist

    MediaFiles.files = resources/Kuplung_Settings.ini
    MediaFiles.files += resources/Kuplung_RecentFiles.ini
    MediaFiles.files += resources/Kuplung.icns
    MediaFiles.files += resources/gui
    MediaFiles.files += resources/fonts
    MediaFiles.files += resources/shaders
    MediaFiles.files += resources/shapes
    MediaFiles.files += resources/skybox
    MediaFiles.files += resources/axis_helpers
    MediaFiles.path = Contents/Resources
    QMAKE_BUNDLE_DATA += MediaFiles

    LIBS += -framework cocoa -framework OpenGL

    LIBS += -L/usr/local/Cellar/boost/1.60.0_2/lib -lboost_system -lboost_filesystem
    INCLUDEPATH += /usr/local/Cellar/boost/1.60.0_2/include

    LIBS += -L/usr/local/Cellar/glm/0.9.7.4/lib
    INCLUDEPATH += /usr/local/Cellar/glm/0.9.7.4/include

    LIBS += -L/usr/local/Cellar/sdl2/2.0.4/lib -lSDL2
    INCLUDEPATH += /usr/local/Cellar/sdl2/2.0.4/include

    LIBS += -L/usr/local/Cellar/glfw3/3.1.2/lib -lGLFW3
    INCLUDEPATH += /usr/local/Cellar/glfw3/3.1.2/include

    LIBS += -L/usr/local/Cellar/assimp/3.2/lib -lAssimp
    INCLUDEPATH += /usr/local/Cellar/assimp/3.2/include

    LIBS += -L"$$PWD/../external/libnoise/lib" -lnoise
    INCLUDEPATH += "$$PWD/../external/libnoise/headers"
}

win32|win64 {
#    RC_FILE += resources/Kuplung.ico
#    RC_ICONS += resources/Kuplung.ico

#    LIBS += -L"C:\Boost\lib-mingw" -llibboost_system-vc140-mt-1_60 -llibboost_filesystem-vc140-mt-gd-1_60
#    INCLUDEPATH += "C:\Boost\include\boost-1_60"

#    LIBS += -L"D:\_Work\_Misc" -llibEGLd
#    LIBS += -L"D:\_Work\_Misc" -llibGLESv2d

#    INCLUDEPATH += "D:\_Work\glm"

#    LIBS += -L"D:\_Work\SDL2\SDL2-2.0.4\i686-w64-mingw32\lib" -lSDL2
#    INCLUDEPATH += "D:\_Work\SDL2\SDL2-2.0.4\i686-w64-mingw32\include"

#    LIBS += -L"D:\_Work\GLFW\lib-mingw" -lGLFW3
#    INCLUDEPATH += "D:\_Work\GLFW\include"

#    LIBS += -L"D:\_Work\GLEW\lib\Release MX\Win32" -lglew32mx
#    INCLUDEPATH += "D:\_Work\GLEW\include"

#    LIBS += -L"D:\_Projects\Kuplung\external\libnoise\lib" -llibnoise
#    INCLUDEPATH += "D:\_Projects\Kuplung\external\libnoise\headers"
}

OTHER_FILES += resources/*
OTHER_FILES += resources/gui/*
OTHER_FILES += resources/fonts/*
OTHER_FILES += resources/shaders/*
OTHER_FILES += resources/shapes/*
OTHER_FILES += resources/skybox/*
OTHER_FILES += resources/axis_helpers/*

SOURCES += main.cpp \
    kuplung/Kuplung.cpp \
    kuplung/meshes/MiniAxis.cpp \
    kuplung/meshes/AxisHelpers.cpp \
    kuplung/meshes/WorldGrid.cpp \
    kuplung/meshes/Light.cpp \
    kuplung/meshes/Camera.cpp \
    kuplung/meshes/CameraModel.cpp \
    kuplung/meshes/LightRay.cpp \
    kuplung/meshes/Model.cpp \
    kuplung/meshes/ModelForward.cpp \
    kuplung/meshes/ModelDeferred.cpp \
    kuplung/meshes/BoundingBox.cpp \
    kuplung/meshes/RayLine.cpp \
    kuplung/meshes/Skybox.cpp \
    kuplung/meshes/Terrain.cpp \
    kuplung/objects/ObjectsManager.cpp \
    kuplung/objects/RayPicking.cpp \
    kuplung/settings/ConfigUtils.cpp \
    kuplung/settings/Settings.cpp \
    kuplung/settings/FontsList.cpp \
    kuplung/ui/UI.cpp \
    kuplung/ui/UIHelpers.cpp \
    kuplung/ui/implementation/SDL2OpenGL32.cpp \
    kuplung/ui/components/ShaderEditor.cpp \
    kuplung/ui/components/FileBrowser.cpp \
    kuplung/ui/components/FileSaver.cpp \
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
    kuplung/ui/dialogs/DialogStyle.cpp \
    kuplung/ui/dialogs/DialogOptions.cpp \
    kuplung/ui/dialogs/DialogControlsGUI.cpp \
    kuplung/ui/dialogs/DialogControlsModels.cpp \
    kuplung/terrain/HeightmapGenerator.cpp \
    kuplung/utilities/font-parser/FNTParser.cpp \
    kuplung/utilities/gl/GLUtils.cpp \
    kuplung/utilities/imgui/imgui.cpp \
    kuplung/utilities/imgui/imgui_demo.cpp \
    kuplung/utilities/imgui/imgui_draw.cpp \
    kuplung/utilities/input/Controls.cpp \
    kuplung/utilities/libnoise/noiseutils.cpp \
    kuplung/utilities/maths/Maths.cpp \
    kuplung/utilities/parsers/FileModelManager.cpp \
    kuplung/utilities/parsers/obj-parser/objParser1.cpp \
    kuplung/utilities/parsers/obj-parser/objParser2.cpp \
    kuplung/utilities/parsers/stl-parser/STLParser.cpp \
    kuplung/utilities/parsers/assimp-parser/AssimpParser.cpp \
    kuplung/utilities/export/Exporter.cpp \
    kuplung/utilities/export/ExporterOBJ.cpp \
    kuplung/utilities/renderers/ImageRenderer.cpp \
    kuplung/utilities/renderers/scene-renderer/SceneRenderer.cpp

HEADERS += \
    kuplung/Kuplung.hpp \
    kuplung/meshes/MiniAxis.hpp \
    kuplung/meshes/AxisHelpers.hpp \
    kuplung/meshes/WorldGrid.hpp \
    kuplung/meshes/Light.hpp \
    kuplung/meshes/Camera.hpp \
    kuplung/meshes/CameraModel.hpp \
    kuplung/meshes/LightRay.hpp \
    kuplung/meshes/Model.hpp \
    kuplung/meshes/ModelForward.hpp \
    kuplung/meshes/ModelDeferred.hpp \
    kuplung/meshes/BoundingBox.hpp \
    kuplung/meshes/RayLine.hpp \
    kuplung/meshes/Skybox.hpp \
    kuplung/meshes/Terrain.hpp \
    kuplung/objects/ObjectDefinitions.h \
    kuplung/objects/ObjectsManager.hpp \
    kuplung/objects/RayPicking.hpp \
    kuplung/settings/ConfigUtils.hpp \
    kuplung/settings/SettingsStructs.h \
    kuplung/settings/Settings.h \
    kuplung/settings/FontsList.hpp \
    kuplung/ui/Objects.h \
    kuplung/ui/UI.hpp \
    kuplung/ui/UIHelpers.hpp \
    kuplung/ui/implementation/SDL2OpenGL32.hpp \
    kuplung/ui/components/ShaderEditor.hpp \
    kuplung/ui/components/FileBrowser.hpp \
    kuplung/ui/components/FileSaver.hpp \
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
    kuplung/ui/dialogs/DialogStyle.hpp \
    kuplung/ui/dialogs/DialogOptions.hpp \
    kuplung/ui/dialogs/DialogControlsGUI.hpp \
    kuplung/ui/dialogs/DialogControlsModels.hpp \
    kuplung/ui/iconfonts/IconsFontAwesome.h \
    kuplung/ui/iconfonts/IconsMaterialDesign.h \
    kuplung/terrain/HeightmapGenerator.hpp \
    kuplung/utilities/font-parser/FNTParser.hpp \
    kuplung/utilities/gl/GLUtils.hpp \
    kuplung/utilities/imgui/imconfig.h \
    kuplung/utilities/imgui/imgui.h \
    kuplung/utilities/imgui/imgui_internal.h \
    kuplung/utilities/input/Controls.hpp \
    kuplung/utilities/libnoise/noiseutils.h \
    kuplung/utilities/maths/Maths.hpp \
    kuplung/utilities/parsers/FileModelManager.hpp \
    kuplung/utilities/parsers/ModelObject.h \
    kuplung/utilities/parsers/obj-parser/objParser1.hpp \
    kuplung/utilities/parsers/obj-parser/objParser2.hpp \
    kuplung/utilities/parsers/stl-parser/STLParser.hpp \
    kuplung/utilities/parsers/assimp-parser/AssimpParser.hpp \
    kuplung/utilities/stb/stb_image.h \
    kuplung/utilities/stb/stb_image_write.h \
    kuplung/utilities/stb/stb_rect_pack.h \
    kuplung/utilities/stb/stb_textedit.h \
    kuplung/utilities/stb/stb_truetype.h \
    kuplung/utilities/gl/GLIncludes.h \
    kuplung/utilities/shapes/Shapes.h \
    kuplung/utilities/export/Exporter.hpp \
    kuplung/utilities/export/ExporterOBJ.hpp \
    kuplung/utilities/renderers/ImageRenderer.hpp \
    kuplung/utilities/renderers/scene-renderer/SceneRenderer.hpp

DISTFILES += \
    resources/fonts/fontawesome-webfont.ttf \
    resources/fonts/material-icons-regular.ttf \
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
    resources/shapes/cone.mtl \
    resources/shapes/cone.obj \
    resources/shapes/cube.mtl \
    resources/shapes/cube.obj \
    resources/shapes/cylinder.mtl \
    resources/shapes/cylinder.obj \
    resources/shapes/grid.mtl \
    resources/shapes/grid.obj \
    resources/shapes/ico_sphere.mtl \
    resources/shapes/ico_sphere.obj \
    resources/shapes/monkey_head.mtl \
    resources/shapes/monkey_head.obj \
    resources/shapes/plane.mtl \
    resources/shapes/plane.obj \
    resources/shapes/triangle.mtl \
    resources/shapes/triangle.obj \
    resources/shapes/torus.mtl \
    resources/shapes/torus.obj \
    resources/shapes/tube.mtl \
    resources/shapes/tube.obj \
    resources/shapes/uv_sphere.mtl \
    resources/shapes/uv_sphere.obj \
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
    resources/shaders/model_face.vert \
    resources/shaders/model_face_effects.vert \
    resources/shaders/model_face_lights.vert \
    resources/shaders/model_face_mapping.vert \
    resources/shaders/model_face_misc.vert \
    resources/shaders/model_face_vars.vert \
    resources/shaders/model_face.geom \
    resources/shaders/model_face.frag \
    resources/shaders/model_face.tcs \
    resources/shaders/model_face.tes \
    resources/shaders/reflection.vert \
    resources/shaders/reflection.frag \
    resources/shaders/light.vert \
    resources/shaders/light.frag \
    resources/shaders/terrain.vert \
    resources/shaders/terrain.frag \
    resources/shaders/skybox.vert \
    resources/shaders/skybox.frag \
    resources/shaders/bounding_box.vert \
    resources/shaders/bounding_box.frag \
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
    resources/skybox/stormydays_front.jpg
