CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

SET(PROJECT_NAME "Kuplung" CXX)
PROJECT(${PROJECT_NAME})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")

# Warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-extern-c-compat")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

IF(APPLE)
    SET(MACOSX_BUNDLE true)
    SET(MACOSX_BUNDLE_BUNDLE_NAME ${MY_APP_NAME})
    SET(MACOSX_BUNDLE_INFO_PLIST, "${CMAKE_SOURCE_DIR}/Info.plist")
ENDIF(APPLE)

# Libraries
IF(APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    MARK_AS_ADVANCED(COCOA_LIBRARY)
ELSE(APPLE)
    MESSAGE(FATAL_ERROR "Cocoa Libraries not found!")
ENDIF(APPLE)

FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
MARK_AS_ADVANCED(OpenGL_LIBRARY)
IF (OpenGL_FOUND)
    MESSAGE(FATAL_ERROR "OpenGL not found!")
ENDIF(OpenGL_FOUND)

# Boost
SET(Boost_MAJOR_VERSION 1)
SET(Boost_MINOR_VERSION 61)
SET(Boost_SUBMINOR_VERSION 0)
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
SET(Boost_VERSION "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
FIND_PACKAGE(Boost ${Boost_VERSION} COMPONENTS system filesystem REQUIRED)
IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

# GLM
INCLUDE(cmake/FindGLM.cmake)
IF(GLM_FOUND)
    INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIRS})
ELSE(GLM_FOUND)
    MESSAGE(FATAL_ERROR "GLM not found!")
ENDIF(GLM_FOUND)

# SDL2
FIND_PACKAGE(SDL2 REQUIRED)
IF(SDL2_FOUND)
    INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
ELSE(SDL2_FOUND)
    MESSAGE(FATAL_ERROR "SDL2 not found!")
ENDIF(SDL2_FOUND)

# GLFW3
FIND_PACKAGE(GLFW3 REQUIRED)
IF(GLFW3_FOUND)
    INCLUDE_DIRECTORIES(${GLFW3_INCLUDE_DIRS})
ELSE(GLFW3_FOUND)
    MESSAGE(FATAL_ERROR "GLFW3 not found!")
ENDIF(GLFW3_FOUND)

# Assimp
INCLUDE(cmake/Findassimp.cmake)
IF(assimp_FOUND)
    INCLUDE_DIRECTORIES(${assimp_INCLUDE_DIRS})
ELSE(assimp_FOUND)
    MESSAGE(FATAL_ERROR "Assimp not found!")
ENDIF(assimp_FOUND)

# Protobuf
FIND_PACKAGE(Protobuf REQUIRED)
IF(Protobuf_FOUND)
    INCLUDE_DIRECTORIES(${Protobuf_INCLUDE_DIRS})
ELSE(Protobuf_FOUND)
    MESSAGE(FATAL_ERROR "Google ProtoBuf not found!")
ENDIF(Protobuf_FOUND)

# Minizip
INCLUDE(cmake/FindMinizip.cmake)
IF(MINIZIP_FOUND)
    INCLUDE_DIRECTORIES(${MINIZIP_INCLUDE_DIR})
ELSE(MINIZIP_FOUND)
    MESSAGE(FATAL_ERROR "Minizip not found!")
ENDIF(MINIZIP_FOUND)

# libnoise
INCLUDE_DIRECTORIES(../external/libnoise/headers)
LINK_DIRECTORIES(../external/libnoise/lib)
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lnoise")

INCLUDE_DIRECTORIES(.)

SET(SOURCE_FILES
    kuplung/meshes/artefacts/Shadertoy.cpp
    kuplung/meshes/artefacts/Shadertoy.hpp
    kuplung/meshes/artefacts/Spaceship.cpp
    kuplung/meshes/artefacts/Spaceship.hpp
    kuplung/meshes/artefacts/StructuredVolumetricSampling.cpp
    kuplung/meshes/artefacts/StructuredVolumetricSampling.hpp
    kuplung/meshes/artefacts/Terrain.cpp
    kuplung/meshes/artefacts/Terrain.hpp
    kuplung/meshes/helpers/AxisHelpers.cpp
    kuplung/meshes/helpers/AxisHelpers.hpp
    kuplung/meshes/helpers/BoundingBox.cpp
    kuplung/meshes/helpers/BoundingBox.hpp
    kuplung/meshes/helpers/Camera.cpp
    kuplung/meshes/helpers/Camera.hpp
    kuplung/meshes/helpers/CameraModel.cpp
    kuplung/meshes/helpers/CameraModel.hpp
    kuplung/meshes/helpers/Light.cpp
    kuplung/meshes/helpers/Light.hpp
    kuplung/meshes/helpers/LightRay.cpp
    kuplung/meshes/helpers/LightRay.hpp
    kuplung/meshes/helpers/MiniAxis.cpp
    kuplung/meshes/helpers/MiniAxis.hpp
    kuplung/meshes/helpers/RayLine.cpp
    kuplung/meshes/helpers/RayLine.hpp
    kuplung/meshes/helpers/Skybox.cpp
    kuplung/meshes/helpers/Skybox.hpp
    kuplung/meshes/helpers/VertexSphere.cpp
    kuplung/meshes/helpers/VertexSphere.hpp
    kuplung/meshes/helpers/WorldGrid.cpp
    kuplung/meshes/helpers/WorldGrid.hpp
    kuplung/meshes/scene/ModelFaceBase.cpp
    kuplung/meshes/scene/ModelFaceBase.hpp
    kuplung/meshes/scene/ModelFaceData.cpp
    kuplung/meshes/scene/ModelFaceData.hpp
    kuplung/meshes/scene/ModelFaceDeferredT.cpp
    kuplung/meshes/scene/ModelFaceDeferredT.hpp
    kuplung/meshes/scene/ModelFaceForward.cpp
    kuplung/meshes/scene/ModelFaceForward.hpp
    kuplung/objects/ObjectDefinitions.h
    kuplung/objects/ObjectsManager.cpp
    kuplung/objects/ObjectsManager.hpp
    kuplung/objects/RayPicking.cpp
    kuplung/objects/RayPicking.hpp
    kuplung/pcg/HeightmapGenerator.cpp
    kuplung/pcg/HeightmapGenerator.hpp
    kuplung/pcg/SpaceshipMeshGenerator.cpp
    kuplung/pcg/SpaceshipMeshGenerator.hpp
    kuplung/pcg/VoronoiGenerator.cpp
    kuplung/pcg/VoronoiGenerator.hpp
    kuplung/rendering/methods/RenderingDeferred.cpp
    kuplung/rendering/methods/RenderingDeferred.hpp
    kuplung/rendering/methods/RenderingForward.cpp
    kuplung/rendering/methods/RenderingForward.hpp
    kuplung/rendering/methods/RenderingSimple.cpp
    kuplung/rendering/methods/RenderingSimple.hpp
    kuplung/rendering/RenderingManager.cpp
    kuplung/rendering/RenderingManager.hpp
    kuplung/settings/ConfigUtils.cpp
    kuplung/settings/ConfigUtils.hpp
    kuplung/settings/FontsList.cpp
    kuplung/settings/FontsList.hpp
    kuplung/settings/Settings.cpp
    kuplung/settings/Settings.h
    kuplung/settings/SettingsStructs.h
    kuplung/ui/components/exporters/ExportOBJ.cpp
    kuplung/ui/components/exporters/ExportOBJ.hpp
    kuplung/ui/components/importers/ImportOBJ.cpp
    kuplung/ui/components/importers/ImportOBJ.hpp
    kuplung/ui/components/materialeditor/MaterialEditor.cpp
    kuplung/ui/components/materialeditor/MaterialEditor.hpp
    kuplung/ui/components/materialeditor/MaterialEditorData.h
    kuplung/ui/components/materialeditor/MELink.cpp
    kuplung/ui/components/materialeditor/MELink.hpp
    kuplung/ui/components/materialeditor/MENode.cpp
    kuplung/ui/components/materialeditor/MENode.hpp
    kuplung/ui/components/materialeditor/MENode_Color.cpp
    kuplung/ui/components/materialeditor/MENode_Color.hpp
    kuplung/ui/components/materialeditor/MENode_Combine.cpp
    kuplung/ui/components/materialeditor/MENode_Combine.hpp
    kuplung/ui/components/materialeditor/MENode_Texture.cpp
    kuplung/ui/components/materialeditor/MENode_Texture.hpp
    kuplung/ui/components/uveditor/UVEditor.cpp
    kuplung/ui/components/uveditor/UVEditor.hpp
    kuplung/ui/components/uveditor/UVPoint.cpp
    kuplung/ui/components/uveditor/UVPoint.hpp
    kuplung/ui/components/ColorPicker.cpp
    kuplung/ui/components/ColorPicker.hpp
    kuplung/ui/components/FileBrowser.cpp
    kuplung/ui/components/FileBrowser.hpp
    kuplung/ui/components/FileSaver.cpp
    kuplung/ui/components/FileSaver.hpp
    kuplung/ui/components/ImageViewer.cpp
    kuplung/ui/components/ImageViewer.hpp
    kuplung/ui/components/Log.cpp
    kuplung/ui/components/Log.hpp
    kuplung/ui/components/RendererUI.cpp
    kuplung/ui/components/RendererUI.hpp
    kuplung/ui/components/Screenshot.cpp
    kuplung/ui/components/Screenshot.hpp
    kuplung/ui/components/ShaderEditor.cpp
    kuplung/ui/components/ShaderEditor.hpp
    kuplung/ui/components/Tabs.cpp
    kuplung/ui/components/Tabs.hpp
    kuplung/ui/dialogs/DialogControlsGUI.cpp
    kuplung/ui/dialogs/DialogControlsGUI.hpp
    kuplung/ui/dialogs/DialogControlsModels.cpp
    kuplung/ui/dialogs/DialogControlsModels.hpp
    kuplung/ui/dialogs/DialogOptions.cpp
    kuplung/ui/dialogs/DialogOptions.hpp
    kuplung/ui/dialogs/DialogShadertoy.cpp
    kuplung/ui/dialogs/DialogShadertoy.hpp
    kuplung/ui/dialogs/DialogStyle.cpp
    kuplung/ui/dialogs/DialogStyle.hpp
    kuplung/ui/dialogs/DialogSVS.cpp
    kuplung/ui/dialogs/DialogSVS.hpp
    kuplung/ui/iconfonts/IconsFontAwesome.h
    kuplung/ui/iconfonts/IconsMaterialDesign.h
    kuplung/ui/implementation/SDL2OpenGL32.cpp
    kuplung/ui/implementation/SDL2OpenGL32.hpp
    kuplung/ui/Objects.h
    kuplung/ui/UI.cpp
    kuplung/ui/UI.hpp
    kuplung/ui/UIHelpers.cpp
    kuplung/ui/UIHelpers.hpp
    kuplung/utilities/consumption/Consumption.cpp
    kuplung/utilities/consumption/Consumption.hpp
    kuplung/utilities/export/Exporter.cpp
    kuplung/utilities/export/Exporter.hpp
    kuplung/utilities/export/ExporterOBJ.cpp
    kuplung/utilities/export/ExporterOBJ.hpp
    kuplung/utilities/font-parser/FNTParser.cpp
    kuplung/utilities/font-parser/FNTParser.hpp
    kuplung/utilities/gl/GLIncludes.h
    kuplung/utilities/gl/GLUtils.cpp
    kuplung/utilities/gl/GLUtils.hpp
    kuplung/utilities/imgui/imguizmo/ImGuizmo.cpp
    kuplung/utilities/imgui/imguizmo/ImGuizmo.h
    kuplung/utilities/imgui/imconfig.h
    kuplung/utilities/imgui/imgui.cpp
    kuplung/utilities/imgui/imgui.h
    kuplung/utilities/imgui/imgui_demo.cpp
    kuplung/utilities/imgui/imgui_draw.cpp
    kuplung/utilities/imgui/imgui_internal.h
    kuplung/utilities/input/Controls.cpp
    kuplung/utilities/input/Controls.hpp
    kuplung/utilities/libnoise/noiseutils.cpp
    kuplung/utilities/libnoise/noiseutils.h
    kuplung/utilities/maths/Maths.cpp
    kuplung/utilities/maths/Maths.hpp
    kuplung/utilities/minizip/KuplungMinizip.cpp
    kuplung/utilities/minizip/KuplungMinizip.hpp
    kuplung/utilities/parsers/assimp-parser/AssimpParser.cpp
    kuplung/utilities/parsers/assimp-parser/AssimpParser.hpp
    kuplung/utilities/parsers/obj-parser/objParser1.cpp
    kuplung/utilities/parsers/obj-parser/objParser1.hpp
    kuplung/utilities/parsers/obj-parser/objParser2.cpp
    kuplung/utilities/parsers/obj-parser/objParser2.hpp
    kuplung/utilities/parsers/stl-parser/STLParser.cpp
    kuplung/utilities/parsers/stl-parser/STLParser.hpp
    kuplung/utilities/parsers/FileModelManager.cpp
    kuplung/utilities/parsers/FileModelManager.hpp
    kuplung/utilities/parsers/ModelObject.h
    kuplung/utilities/parsers/ParserUtils.cpp
    kuplung/utilities/parsers/ParserUtils.hpp
    kuplung/utilities/renderers/default-forward/DefaultForwardRenderer.cpp
    kuplung/utilities/renderers/default-forward/DefaultForwardRenderer.hpp
    kuplung/utilities/renderers/scene-renderer/SceneRenderer.cpp
    kuplung/utilities/renderers/scene-renderer/SceneRenderer.hpp
    kuplung/utilities/renderers/ImageRenderer.cpp
    kuplung/utilities/renderers/ImageRenderer.hpp
    kuplung/utilities/renderers/KuplungRendererBase.cpp
    kuplung/utilities/renderers/KuplungRendererBase.hpp
    kuplung/utilities/saveopen/KuplungAppScene.pb.cc
    kuplung/utilities/saveopen/KuplungAppScene.pb.h
    kuplung/utilities/saveopen/KuplungAppSettings.pb.cc
    kuplung/utilities/saveopen/KuplungAppSettings.pb.h
    kuplung/utilities/saveopen/KuplungDefinitions.pb.cc
    kuplung/utilities/saveopen/KuplungDefinitions.pb.h
    kuplung/utilities/saveopen/SaveOpen.cpp
    kuplung/utilities/saveopen/SaveOpen.hpp
    kuplung/utilities/saveopen/SaveOpenBinarySeq.cpp
    kuplung/utilities/saveopen/SaveOpenBinarySeq.hpp
    kuplung/utilities/saveopen/SaveOpenGProtocolBufs.cpp
    kuplung/utilities/saveopen/SaveOpenGProtocolBufs.hpp
    kuplung/utilities/shapes/Shapes.h
    kuplung/utilities/stb/stb_image.h
    kuplung/utilities/stb/stb_image_write.h
    kuplung/utilities/stb/stb_rect_pack.h
    kuplung/utilities/stb/stb_textedit.h
    kuplung/utilities/stb/stb_truetype.h
    kuplung/Kuplung.cpp
    kuplung/Kuplung.hpp
    main.cpp)

SET(Kuplung_Shapes
    resources/shapes/brick_wall.mtl
    resources/shapes/brick_wall.obj
    resources/shapes/brick_wall_diffuse.png
    resources/shapes/brick_wall_displacement.png
    resources/shapes/brick_wall_normal.png
    resources/shapes/brick_wall_spec.png
    resources/shapes/cone.mtl
    resources/shapes/cone.obj
    resources/shapes/cube.mtl
    resources/shapes/cube.obj
    resources/shapes/cylinder.mtl
    resources/shapes/cylinder.obj
    resources/shapes/grid.mtl
    resources/shapes/grid.obj
    resources/shapes/ico_sphere.mtl
    resources/shapes/ico_sphere.obj
    resources/shapes/monkey_head.mtl
    resources/shapes/monkey_head.obj
    resources/shapes/plane.mtl
    resources/shapes/plane.obj
    resources/shapes/plane_objects.mtl
    resources/shapes/plane_objects.obj
    resources/shapes/torus.mtl
    resources/shapes/torus.obj
    resources/shapes/triangle.mtl
    resources/shapes/triangle.obj
    resources/shapes/tube.mtl
    resources/shapes/tube.obj
    resources/shapes/uv_sphere.mtl
    resources/shapes/uv_sphere.obj)

SET(Kuplung_Axis_Helpers
    resources/axis_helpers/x_minus.mtl
    resources/axis_helpers/x_minus.obj
    resources/axis_helpers/x_plus.mtl
    resources/axis_helpers/x_plus.obj
    resources/axis_helpers/y_minus.mtl
    resources/axis_helpers/y_minus.obj
    resources/axis_helpers/y_plus.mtl
    resources/axis_helpers/y_plus.obj
    resources/axis_helpers/z_minus.mtl
    resources/axis_helpers/z_minus.obj
    resources/axis_helpers/z_plus.mtl
    resources/axis_helpers/z_plus.obj)

SET(Kuplung_Fonts
    resources/fonts/fontawesome-webfont.ttf
    resources/fonts/material-icons-regular.ttf)

SET(Kuplung_GUI
    resources/gui/camera.mtl
    resources/gui/camera.obj
    resources/gui/light.mtl
    resources/gui/light.obj
    resources/gui/light_directional.mtl
    resources/gui/light_directional.obj
    resources/gui/light_point.mtl
    resources/gui/light_point.obj
    resources/gui/light_spot.mtl
    resources/gui/light_spot.obj
    resources/gui/light_texture.jpg)

SET(Kuplung_Images_xcassets
    resources/Images.xcassets/AppIcon.appiconset/Contents.json
    resources/Images.xcassets/AppIcon.appiconset/icon_128x128.png
    resources/Images.xcassets/AppIcon.appiconset/icon_128x128@2x.png
    resources/Images.xcassets/AppIcon.appiconset/icon_16x16.png
    resources/Images.xcassets/AppIcon.appiconset/icon_16x16@2x.png
    resources/Images.xcassets/AppIcon.appiconset/icon_256x256.png
    resources/Images.xcassets/AppIcon.appiconset/icon_256x256@2x.png
    resources/Images.xcassets/AppIcon.appiconset/icon_32x32.png
    resources/Images.xcassets/AppIcon.appiconset/icon_32x32@2x.png
    resources/Images.xcassets/AppIcon.appiconset/icon_512x512.png
    resources/Images.xcassets/AppIcon.appiconset/icon_512x512@2x.png)

SET(Kuplung_ProtoBuf
    resources/protobuf/KuplungAppScene.proto
    resources/protobuf/KuplungAppSettings.proto
    resources/protobuf/KuplungDefinitions.proto)

SET(Kuplung_Shaders
    resources/shaders/stoy/4ljGW1.stoy
    resources/shaders/stoy/4tlSzl.stoy
    resources/shaders/stoy/Ms2SD1.stoy
    resources/shaders/stoy/XlfGRj.stoy
    resources/shaders/stoy/XlSSzK.stoy
    resources/shaders/axis.frag
    resources/shaders/axis.vert
    resources/shaders/axis_helpers.frag
    resources/shaders/axis_helpers.vert
    resources/shaders/bounding_box.frag
    resources/shaders/bounding_box.vert
    resources/shaders/camera.frag
    resources/shaders/camera.vert
    resources/shaders/deferred_g_buffer.frag
    resources/shaders/deferred_g_buffer.vert
    resources/shaders/deferred_light_box.frag
    resources/shaders/deferred_light_box.vert
    resources/shaders/deferred_shading.frag
    resources/shaders/deferred_shading.vert
    resources/shaders/grid.frag
    resources/shaders/grid.vert
    resources/shaders/light.frag
    resources/shaders/light.vert
    resources/shaders/light_ray.frag
    resources/shaders/light_ray.vert
    resources/shaders/model_face.frag
    resources/shaders/model_face.geom
    resources/shaders/model_face.tcs
    resources/shaders/model_face.tes
    resources/shaders/model_face.vert
    resources/shaders/model_face_effects.frag
    resources/shaders/model_face_lights.frag
    resources/shaders/model_face_mapping.frag
    resources/shaders/model_face_misc.frag
    resources/shaders/model_face_vars.frag
    resources/shaders/ray_line.frag
    resources/shaders/ray_line.vert
    resources/shaders/reflection.frag
    resources/shaders/reflection.vert
    resources/shaders/rendering_simple.frag
    resources/shaders/rendering_simple.geom
    resources/shaders/rendering_simple.tcs
    resources/shaders/rendering_simple.tes
    resources/shaders/rendering_simple.vert
    resources/shaders/shadertoy.vert
    resources/shaders/skybox.frag
    resources/shaders/skybox.vert
    resources/shaders/spaceship.frag
    resources/shaders/spaceship.vert
    resources/shaders/structured_vol_sampling.frag
    resources/shaders/structured_vol_sampling.vert
    resources/shaders/terrain.frag
    resources/shaders/terrain.vert
    resources/shaders/vertex_sphere.frag
    resources/shaders/vertex_sphere.vert
    resources/shadertoy/cube00_0.jpg
    resources/shadertoy/cube00_1.jpg
    resources/shadertoy/cube00_2.jpg
    resources/shadertoy/cube00_3.jpg
    resources/shadertoy/cube00_4.jpg
    resources/shadertoy/cube00_5.jpg
    resources/shadertoy/cube01_0.png
    resources/shadertoy/cube01_1.png
    resources/shadertoy/cube01_2.png
    resources/shadertoy/cube01_3.png
    resources/shadertoy/cube01_4.png
    resources/shadertoy/cube01_5.png
    resources/shadertoy/cube02_0.jpg
    resources/shadertoy/cube02_1.jpg
    resources/shadertoy/cube02_2.jpg
    resources/shadertoy/cube02_3.jpg
    resources/shadertoy/cube02_4.jpg
    resources/shadertoy/cube02_5.jpg
    resources/shadertoy/cube03_0.png
    resources/shadertoy/cube03_1.png
    resources/shadertoy/cube03_2.png
    resources/shadertoy/cube03_3.png
    resources/shadertoy/cube03_4.png
    resources/shadertoy/cube03_5.png
    resources/shadertoy/cube04_0.png
    resources/shadertoy/cube04_1.png
    resources/shadertoy/cube04_2.png
    resources/shadertoy/cube04_3.png
    resources/shadertoy/cube04_4.png
    resources/shadertoy/cube04_5.png
    resources/shadertoy/cube05_0.png
    resources/shadertoy/cube05_1.png
    resources/shadertoy/cube05_2.png
    resources/shadertoy/cube05_3.png
    resources/shadertoy/cube05_4.png
    resources/shadertoy/cube05_5.png
    resources/shadertoy/LICENSE.txt
    resources/shadertoy/tex00.jpg
    resources/shadertoy/tex01.jpg
    resources/shadertoy/tex02.jpg
    resources/shadertoy/tex03.jpg
    resources/shadertoy/tex04.jpg
    resources/shadertoy/tex05.jpg
    resources/shadertoy/tex06.jpg
    resources/shadertoy/tex07.jpg
    resources/shadertoy/tex08.jpg
    resources/shadertoy/tex09.jpg
    resources/shadertoy/tex10.png
    resources/shadertoy/tex11.png
    resources/shadertoy/tex12.png
    resources/shadertoy/tex13.png
    resources/shadertoy/tex14.png
    resources/shadertoy/tex15.png
    resources/shadertoy/tex16.png
    resources/shadertoy/tex17.jpg
    resources/shadertoy/tex18.jpg
    resources/shadertoy/tex19.png
    resources/shadertoy/tex20.jpg
    resources/shadertoy/webcamBig.png)

SET(Kuplung_Skybox
    resources/skybox/fire_planet_back.jpg
    resources/skybox/fire_planet_bottom.jpg
    resources/skybox/fire_planet_front.jpg
    resources/skybox/fire_planet_left.jpg
    resources/skybox/fire_planet_right.jpg
    resources/skybox/fire_planet_top.jpg
    resources/skybox/lake_mountain_back.jpg
    resources/skybox/lake_mountain_bottom.jpg
    resources/skybox/lake_mountain_front.jpg
    resources/skybox/lake_mountain_left.jpg
    resources/skybox/lake_mountain_right.jpg
    resources/skybox/lake_mountain_top.jpg
    resources/skybox/stormydays_back.jpg
    resources/skybox/stormydays_bottom.jpg
    resources/skybox/stormydays_front.jpg
    resources/skybox/stormydays_left.jpg
    resources/skybox/stormydays_right.jpg
    resources/skybox/stormydays_top.jpg)

SET(Kuplung_Misc
    resources/Kuplung.icns
    resources/Kuplung.ico
    resources/Kuplung_RecentFiles.ini
    resources/Kuplung_RecentFilesImported.ini
    resources/Kuplung_Settings.ini
    resources/noise16.png)

IF (Boost_FOUND AND GLM_FOUND AND SDL2_FOUND AND GLFW3_FOUND AND assimp_FOUND AND Protobuf_FOUND AND MINIZIP_FOUND)
    IF(APPLE)
        SET(MACOSX_BUNDLE true)
        SET(MACOSX_BUNDLE_BUNDLE_NAME ${MY_APP_NAME})

        # Misc
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")
        FILE(GLOB Kuplung_Misc_Files "resources/*.*")
        FILE(COPY ${Kuplung_Misc_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")

        # Shapes
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/shapes/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/shapes/")
        FILE(GLOB Kuplung_Shapes_Files "resources/shapes/*.*")
        FILE(COPY ${Kuplung_Shapes_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/shapes/")

        # Axis Helpers
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/axis_helpers/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/axis_helpers/")
        FILE(GLOB Kuplung_Axis_Helpers_Files "resources/axis_helpers/*.*")
        FILE(COPY ${Kuplung_Axis_Helpers_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/axis_helpers/")

        # Fonts
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/fonts/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/fonts/")
        FILE(GLOB Kuplung_Fonts_Files "resources/fonts/*.*")
        FILE(COPY ${Kuplung_Fonts_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/fonts/")

        # GUI
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/gui/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/gui/")
        FILE(GLOB Kuplung_GUI_Files "resources/gui/*.*")
        FILE(COPY ${Kuplung_GUI_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/gui/")

        # Image Assets
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/Images.xcassets/AppIcon.appiconset/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/Images.xcassets/AppIcon.appiconset/")
        FILE(GLOB Kuplung_ImageAssets_Files "resources/Images.xcassets/AppIcon.appiconset/*.*")
        FILE(COPY ${Kuplung_ImageAssets_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/Images.xcassets/AppIcon.appiconset/")

        # ProtoBuf
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/protobuf/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/protobuf/")
        FILE(GLOB Kuplung_ProtoBuf_Files "resources/protobuf/*.*")
        FILE(COPY ${Kuplung_ProtoBuf_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/protobuf/")

        # Shaders
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/shaders/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/shaders/")
        FILE(GLOB Kuplung_Shaders_Files "resources/shaders/*.*")
        FILE(COPY ${Kuplung_Shaders_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/shaders/")

        # Shaders - ShaderToy
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/shaders/stoy/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/shaders/stoy/")
        FILE(GLOB Kuplung_Shaders_Stoy_Files "resources/shaders/stoy/*.*")
        FILE(COPY ${Kuplung_Shaders_Stoy_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/shaders/stoy/")

        # Shaders - ShaderToy Assets
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/shadertoy/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/shadertoy/")
        FILE(GLOB Kuplung_Shaders_StoyAssets_Files "resources/shadertoy/*.*")
        FILE(COPY ${Kuplung_Shaders_StoyAssets_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/shadertoy/")

        # Skybox
        EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "resources/skybox/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/skybox/")
        FILE(GLOB Kuplung_Skybox_Files "resources/skybox/*.*")
        FILE(COPY ${Kuplung_Skybox_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/skybox/")

        ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})

        SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist")

        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${COCOA_LIBRARY})
    ENDIF(APPLE)
ELSE(Boost_FOUND AND GLM_FOUND AND SDL2_FOUND AND GLFW3_FOUND AND assimp_FOUND AND Protobuf_FOUND AND MINIZIP_FOUND)
    MESSAGE(FATAL_ERROR "Some of the required libraries were not found. Please check the log.")
ENDIF(Boost_FOUND AND GLM_FOUND AND SDL2_FOUND AND GLFW3_FOUND AND assimp_FOUND AND Protobuf_FOUND AND MINIZIP_FOUND)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenGL_LIBRARY})

IF(Boost_FOUND)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})
ENDIF(Boost_FOUND)

IF(GLM_FOUND)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GLM_LIBRARIES})
ENDIF(GLM_FOUND)

IF(SDL2_FOUND)
    STRING(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES})
ENDIF(SDL2_FOUND)

IF(GLFW3_FOUND)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GLFW3_LIBRARIES})
ENDIF(GLFW3_FOUND)

IF(assimp_FOUND)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${assimp_LIBRARIES})
ENDIF(assimp_FOUND)

IF(Protobuf_FOUND)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Protobuf_LIBRARIES})
ENDIF(Protobuf_FOUND)

IF(MINIZIP_FOUND)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MINIZIP_LIBRARIES})
ENDIF(MINIZIP_FOUND)