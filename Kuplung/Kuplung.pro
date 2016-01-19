TEMPLATE = app

CONFIG += c++11
CONFIG += app_bundle
CONFIG -= console
CONFIG -= qt

#QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-local-typedefs
#QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field
#QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-extern-c-compat
QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas

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
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11

  QMAKE_INFO_PLIST = Info.plist

  plist.path = "$$DESTDIR/$$join(TARGET,,,.app)/Contents"
  plist.files = Info.plist
  INSTALLS += plist

  MediaFiles.files = resources/Kuplung_Settings.ini
  MediaFiles.files += resources/Kuplung.icns
  MediaFiles.files += resources/gui
  MediaFiles.files += resources/shaders
  MediaFiles.path = Contents/Resources
  QMAKE_BUNDLE_DATA += MediaFiles

  LIBS += -framework cocoa -framework OpenGL

  LIBS += -L/usr/local/Cellar/boost/1.60.0/lib -lboost_system -lboost_filesystem
  INCLUDEPATH += /usr/local/Cellar/boost/1.60.0/include

  LIBS += -L/usr/local/Cellar/glm/0.9.7.1/lib
  INCLUDEPATH += /usr/local/Cellar/glm/0.9.7.1/include

  LIBS += -L/usr/local/Cellar/sdl2/2.0.4/lib -lSDL2
  INCLUDEPATH += /usr/local/Cellar/sdl2/2.0.4/include

  LIBS += -L/usr/local/Cellar/glfw3/3.1.2/lib -lGLFW3
  INCLUDEPATH += /usr/local/Cellar/glfw3/3.1.2/include

  LIBS += -L"$$PWD/../external/libnoise/lib" -lnoise
  INCLUDEPATH += "$$PWD/../external/libnoise/headers"
}

win32|win64 {
  #RC_FILE += resources/Kuplung.ico
  RC_ICONS += resources/Kuplung.ico

  LIBS += -L"C:\Boost\lib" -llibboost_system-vc140-mt-1_60 -llibboost_filesystem-vc140-mt-gd-1_60
  INCLUDEPATH += "C:\Boost\include\boost-1_60"

  LIBS += -L"D:\_Work\_Misc" -llibEGLd
  LIBS += -L"D:\_Work\_Misc" -llibGLESv2d

  INCLUDEPATH += "D:\_Work\glm"

  LIBS += -L"D:\_Work\SDL2\SDL2-2.0.4\i686-w64-mingw32\lib" -lSDL2
  INCLUDEPATH += "D:\_Work\SDL2\SDL2-2.0.4\i686-w64-mingw32\include"

  LIBS += -L"D:\_Work\GLFW\lib-mingw" -lGLFW3
  INCLUDEPATH += "D:\_Work\GLFW\include"

  LIBS += -L"D:\_Work\GLEW\lib\Release MX\Win32" -lglew32mx
  INCLUDEPATH += "D:\_Work\GLEW\include"

  LIBS += -L"D:\_Projects\Kuplung\external\libnoise\lib" -llibnoise
  INCLUDEPATH += "D:\_Projects\Kuplung\external\libnoise\headers"
}

OTHER_FILES += resources/*
OTHER_FILES += resources/gui/*
OTHER_FILES += resources/shaders/*

SOURCES += main.cpp \
    kuplung/Kuplung.cpp \
    kuplung/meshes/MeshCoordinateSystem.cpp \
    kuplung/meshes/MeshDot.cpp \
    kuplung/meshes/MeshGrid.cpp \
    kuplung/meshes/MeshLight.cpp \
    kuplung/meshes/MeshModelFace.cpp \
    kuplung/meshes/MeshTerrain.cpp \
    utilities/font-parser/FNTParser.cpp \
    utilities/gl/GLUtils.cpp \
    utilities/gui/ImGui/imgui.cpp \
    utilities/gui/ImGui/imgui_demo.cpp \
    utilities/gui/ImGui/imgui_draw.cpp \
    utilities/gui/GUI.cpp \
    utilities/gui/GUIEditor.cpp \
    utilities/gui/GUIFileBrowser.cpp \
    utilities/gui/GUILog.cpp \
    utilities/gui/GUIScreenshot.cpp \
    utilities/input/Controls.cpp \
    utilities/libnoise/HeightmapGenerator.cpp \
    utilities/libnoise/noiseutils.cpp \
    utilities/parsers/obj-parser/objParser.cpp \
    utilities/parsers/stl-parser/STLParser.cpp \
    utilities/settings/ConfigUtils.cpp \
    utilities/settings/Settings.cpp

HEADERS += \
    kuplung/Kuplung.hpp \
    kuplung/meshes/MeshCoordinateSystem.hpp \
    kuplung/meshes/MeshDot.hpp \
    kuplung/meshes/MeshGrid.hpp \
    kuplung/meshes/MeshLight.hpp \
    kuplung/meshes/MeshModelFace.hpp \
    kuplung/meshes/MeshTerrain.hpp \
    utilities/font-parser/FNTParser.hpp \
    utilities/gl/GLUtils.hpp \
    utilities/gui/ImGui/imconfig.h \
    utilities/gui/ImGui/imgui.h \
    utilities/gui/ImGui/imgui_internal.h \
    utilities/gui/GUI.hpp \
    utilities/gui/GUIEditor.hpp \
    utilities/gui/GUIFileBrowser.hpp \
    utilities/gui/GUILog.hpp \
    utilities/gui/GUIScreenshot.hpp \
    utilities/input/Controls.hpp \
    utilities/libnoise/HeightmapGenerator.hpp \
    utilities/libnoise/noiseutils.h \
    utilities/parsers/obj-parser/objObjects.h \
    utilities/parsers/obj-parser/objParser.hpp \
    utilities/parsers/stl-parser/STLParser.hpp \
    utilities/settings/ConfigUtils.hpp \
    utilities/settings/Settings.h \
    utilities/stb/stb_image.h \
    utilities/stb/stb_image_write.h \
    utilities/stb/stb_rect_pack.h \
    utilities/stb/stb_textedit.h \
    utilities/stb/stb_truetype.h \
    utilities/gl/GLIncludes.h

DISTFILES += \
    resources/shaders/axis.vert \
    resources/shaders/axis.frag \
    resources/shaders/dots.vert \
    resources/shaders/dots.frag \
    resources/shaders/grid.vert \
    resources/shaders/grid.frag \
    resources/shaders/kuplung.vert \
    resources/shaders/kuplung.geom \
    resources/shaders/kuplung.frag \
    resources/shaders/kuplung.tcs \
    resources/shaders/kuplung.tes \
    resources/shaders/light.vert \
    resources/shaders/light.frag \
    resources/shaders/terrain.vert \
    resources/shaders/terrain.frag
