cmake_minimum_required(VERSION 3.6)
project(Kuplung)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")

# Warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-extern-c-compat")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

IF(APPLE)
    SET(MACOSX_BUNDLE true)
    SET(MACOSX_BUNDLE_BUNDLE_NAME ${MY_APP_NAME})
ENDIF(APPLE)

# Libraries
IF(APPLE)
    # FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    # FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -framework cocoa -framework OpenGL")

    INCLUDE_DIRECTORIES(/usr/local/Cellar/boost/1.61.0/include)
    LINK_DIRECTORIES(/usr/local/Cellar/boost/1.61.0/lib)
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lboost_system -lboost_filesystem")

    # FIND_PACKAGE(GLM REQUIRED)
    INCLUDE_DIRECTORIES(/usr/local/Cellar/glm/0.9.8.0/include)
    LINK_DIRECTORIES(/usr/local/Cellar/glm/0.9.8.0/lib)
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lSDL2")

    INCLUDE_DIRECTORIES(/usr/local/Cellar/sdl2/2.0.4/include)
    LINK_DIRECTORIES(/usr/local/Cellar/sdl2/2.0.4/lib)

    INCLUDE_DIRECTORIES(/usr/local/Cellar/glfw3/3.2.1/include)
    LINK_DIRECTORIES(/usr/local/Cellar/glfw3/3.2.1/lib)
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lGLFW3")

    INCLUDE_DIRECTORIES(/usr/local/Cellar/assimp/3.3.1/include)
    LINK_DIRECTORIES(/usr/local/Cellar/assimp/3.3.1/lib)
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lAssimp")

    INCLUDE_DIRECTORIES(/usr/local/Cellar/protobuf/3.1.0/include)
    LINK_DIRECTORIES(/usr/local/Cellar/protobuf/3.1.0/lib)
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lprotobuf")

    INCLUDE_DIRECTORIES(/usr/local/Cellar/minizip/1.1/include)
    LINK_DIRECTORIES(/usr/local/Cellar/minizip/1.1/lib)
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lMinizip")

    INCLUDE_DIRECTORIES(../_qt/external/libnoise/headers)
    LINK_DIRECTORIES(../_qt/external/libnoise/lib)
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lnoise")
ENDIF(APPLE)

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
        Resources/shapes/brick_wall.mtl
        Resources/shapes/brick_wall.obj
        Resources/shapes/brick_wall_diffuse.png
        Resources/shapes/brick_wall_displacement.png
        Resources/shapes/brick_wall_normal.png
        Resources/shapes/brick_wall_spec.png
        Resources/shapes/cone.mtl
        Resources/shapes/cone.obj
        Resources/shapes/cube.mtl
        Resources/shapes/cube.obj
        Resources/shapes/cylinder.mtl
        Resources/shapes/cylinder.obj
        Resources/shapes/grid.mtl
        Resources/shapes/grid.obj
        Resources/shapes/ico_sphere.mtl
        Resources/shapes/ico_sphere.obj
        Resources/shapes/monkey_head.mtl
        Resources/shapes/monkey_head.obj
        Resources/shapes/plane.mtl
        Resources/shapes/plane.obj
        Resources/shapes/plane_objects.mtl
        Resources/shapes/plane_objects.obj
        Resources/shapes/torus.mtl
        Resources/shapes/torus.obj
        Resources/shapes/triangle.mtl
        Resources/shapes/triangle.obj
        Resources/shapes/tube.mtl
        Resources/shapes/tube.obj
        Resources/shapes/uv_sphere.mtl
        Resources/shapes/uv_sphere.obj)

SET(Kuplung_Axis_Helpers
        Resources/axis_helpers/x_minus.mtl
        Resources/axis_helpers/x_minus.obj
        Resources/axis_helpers/x_plus.mtl
        Resources/axis_helpers/x_plus.obj
        Resources/axis_helpers/y_minus.mtl
        Resources/axis_helpers/y_minus.obj
        Resources/axis_helpers/y_plus.mtl
        Resources/axis_helpers/y_plus.obj
        Resources/axis_helpers/z_minus.mtl
        Resources/axis_helpers/z_minus.obj
        Resources/axis_helpers/z_plus.mtl
        Resources/axis_helpers/z_plus.obj)

SET(Kuplung_Fonts
        Resources/fonts/fontawesome-webfont.ttf
        Resources/fonts/material-icons-regular.ttf)

SET(Kuplung_GUI
        Resources/gui/camera.mtl
        Resources/gui/camera.obj
        Resources/gui/light.mtl
        Resources/gui/light.obj
        Resources/gui/light_directional.mtl
        Resources/gui/light_directional.obj
        Resources/gui/light_point.mtl
        Resources/gui/light_point.obj
        Resources/gui/light_spot.mtl
        Resources/gui/light_spot.obj
        Resources/gui/light_texture.jpg)

SET(Kuplung_Images_xcassets
        Resources/Images.xcassets/AppIcon.appiconset/Contents.json
        Resources/Images.xcassets/AppIcon.appiconset/icon_128x128.png
        Resources/Images.xcassets/AppIcon.appiconset/icon_128x128@2x.png
        Resources/Images.xcassets/AppIcon.appiconset/icon_16x16.png
        Resources/Images.xcassets/AppIcon.appiconset/icon_16x16@2x.png
        Resources/Images.xcassets/AppIcon.appiconset/icon_256x256.png
        Resources/Images.xcassets/AppIcon.appiconset/icon_256x256@2x.png
        Resources/Images.xcassets/AppIcon.appiconset/icon_32x32.png
        Resources/Images.xcassets/AppIcon.appiconset/icon_32x32@2x.png
        Resources/Images.xcassets/AppIcon.appiconset/icon_512x512.png
        Resources/Images.xcassets/AppIcon.appiconset/icon_512x512@2x.png)

SET(Kuplung_ProtoBuf
        Resources/protobuf/KuplungAppScene.proto
        Resources/protobuf/KuplungAppSettings.proto
        Resources/protobuf/KuplungDefinitions.proto)

SET(Kuplung_Shaders
        Resources/shaders/stoy/4ljGW1.stoy
        Resources/shaders/stoy/4tlSzl.stoy
        Resources/shaders/stoy/Ms2SD1.stoy
        Resources/shaders/stoy/XlfGRj.stoy
        Resources/shaders/stoy/XlSSzK.stoy
        Resources/shaders/axis.frag
        Resources/shaders/axis.vert
        Resources/shaders/axis_helpers.frag
        Resources/shaders/axis_helpers.vert
        Resources/shaders/bounding_box.frag
        Resources/shaders/bounding_box.vert
        Resources/shaders/camera.frag
        Resources/shaders/camera.vert
        Resources/shaders/deferred_g_buffer.frag
        Resources/shaders/deferred_g_buffer.vert
        Resources/shaders/deferred_light_box.frag
        Resources/shaders/deferred_light_box.vert
        Resources/shaders/deferred_shading.frag
        Resources/shaders/deferred_shading.vert
        Resources/shaders/grid.frag
        Resources/shaders/grid.vert
        Resources/shaders/light.frag
        Resources/shaders/light.vert
        Resources/shaders/light_ray.frag
        Resources/shaders/light_ray.vert
        Resources/shaders/model_face.frag
        Resources/shaders/model_face.geom
        Resources/shaders/model_face.tcs
        Resources/shaders/model_face.tes
        Resources/shaders/model_face.vert
        Resources/shaders/model_face_effects.frag
        Resources/shaders/model_face_lights.frag
        Resources/shaders/model_face_mapping.frag
        Resources/shaders/model_face_misc.frag
        Resources/shaders/model_face_vars.frag
        Resources/shaders/ray_line.frag
        Resources/shaders/ray_line.vert
        Resources/shaders/reflection.frag
        Resources/shaders/reflection.vert
        Resources/shaders/rendering_simple.frag
        Resources/shaders/rendering_simple.geom
        Resources/shaders/rendering_simple.tcs
        Resources/shaders/rendering_simple.tes
        Resources/shaders/rendering_simple.vert
        Resources/shaders/shadertoy.vert
        Resources/shaders/skybox.frag
        Resources/shaders/skybox.vert
        Resources/shaders/spaceship.frag
        Resources/shaders/spaceship.vert
        Resources/shaders/structured_vol_sampling.frag
        Resources/shaders/structured_vol_sampling.vert
        Resources/shaders/terrain.frag
        Resources/shaders/terrain.vert
        Resources/shaders/vertex_sphere.frag
        Resources/shaders/vertex_sphere.vert
        Resources/shadertoy/cube00_0.jpg
        Resources/shadertoy/cube00_1.jpg
        Resources/shadertoy/cube00_2.jpg
        Resources/shadertoy/cube00_3.jpg
        Resources/shadertoy/cube00_4.jpg
        Resources/shadertoy/cube00_5.jpg
        Resources/shadertoy/cube01_0.png
        Resources/shadertoy/cube01_1.png
        Resources/shadertoy/cube01_2.png
        Resources/shadertoy/cube01_3.png
        Resources/shadertoy/cube01_4.png
        Resources/shadertoy/cube01_5.png
        Resources/shadertoy/cube02_0.jpg
        Resources/shadertoy/cube02_1.jpg
        Resources/shadertoy/cube02_2.jpg
        Resources/shadertoy/cube02_3.jpg
        Resources/shadertoy/cube02_4.jpg
        Resources/shadertoy/cube02_5.jpg
        Resources/shadertoy/cube03_0.png
        Resources/shadertoy/cube03_1.png
        Resources/shadertoy/cube03_2.png
        Resources/shadertoy/cube03_3.png
        Resources/shadertoy/cube03_4.png
        Resources/shadertoy/cube03_5.png
        Resources/shadertoy/cube04_0.png
        Resources/shadertoy/cube04_1.png
        Resources/shadertoy/cube04_2.png
        Resources/shadertoy/cube04_3.png
        Resources/shadertoy/cube04_4.png
        Resources/shadertoy/cube04_5.png
        Resources/shadertoy/cube05_0.png
        Resources/shadertoy/cube05_1.png
        Resources/shadertoy/cube05_2.png
        Resources/shadertoy/cube05_3.png
        Resources/shadertoy/cube05_4.png
        Resources/shadertoy/cube05_5.png
        Resources/shadertoy/LICENSE.txt
        Resources/shadertoy/tex00.jpg
        Resources/shadertoy/tex01.jpg
        Resources/shadertoy/tex02.jpg
        Resources/shadertoy/tex03.jpg
        Resources/shadertoy/tex04.jpg
        Resources/shadertoy/tex05.jpg
        Resources/shadertoy/tex06.jpg
        Resources/shadertoy/tex07.jpg
        Resources/shadertoy/tex08.jpg
        Resources/shadertoy/tex09.jpg
        Resources/shadertoy/tex10.png
        Resources/shadertoy/tex11.png
        Resources/shadertoy/tex12.png
        Resources/shadertoy/tex13.png
        Resources/shadertoy/tex14.png
        Resources/shadertoy/tex15.png
        Resources/shadertoy/tex16.png
        Resources/shadertoy/tex17.jpg
        Resources/shadertoy/tex18.jpg
        Resources/shadertoy/tex19.png
        Resources/shadertoy/tex20.jpg
        Resources/shadertoy/webcamBig.png)

SET(Kuplung_Skybox
        Resources/skybox/fire_planet_back.jpg
        Resources/skybox/fire_planet_bottom.jpg
        Resources/skybox/fire_planet_front.jpg
        Resources/skybox/fire_planet_left.jpg
        Resources/skybox/fire_planet_right.jpg
        Resources/skybox/fire_planet_top.jpg
        Resources/skybox/lake_mountain_back.jpg
        Resources/skybox/lake_mountain_bottom.jpg
        Resources/skybox/lake_mountain_front.jpg
        Resources/skybox/lake_mountain_left.jpg
        Resources/skybox/lake_mountain_right.jpg
        Resources/skybox/lake_mountain_top.jpg
        Resources/skybox/stormydays_back.jpg
        Resources/skybox/stormydays_bottom.jpg
        Resources/skybox/stormydays_front.jpg
        Resources/skybox/stormydays_left.jpg
        Resources/skybox/stormydays_right.jpg
        Resources/skybox/stormydays_top.jpg)

SET(Kuplung_Misc
        Resources/Kuplung.icns
        Resources/Kuplung.ico
        Resources/Kuplung_RecentFiles.ini
        Resources/Kuplung_RecentFilesImported.ini
        Resources/Kuplung_Settings.ini
        Resources/noise16.png)

IF(APPLE)
    SET(MACOSX_BUNDLE true)
    SET(MACOSX_BUNDLE_BUNDLE_NAME ${MY_APP_NAME})

    # Shapes
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/shapes/" "Kuplung.app/Contents/Resources/shapes/")
    FILE(GLOB Kuplung_Shapes_Files "Resources/shapes/*.*")
    FILE(COPY ${Kuplung_Shapes_Files} DESTINATION "Kuplung.app/Contents/Resources/shapes/")

    # Axis Helpers
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/axis_helpers/" "Kuplung.app/Contents/Resources/axis_helpers/")
    FILE(GLOB Kuplung_Axis_Helpers_Files "Resources/axis_helpers/*.*")
    FILE(COPY ${Kuplung_Axis_Helpers_Files} DESTINATION "Kuplung.app/Contents/Resources/axis_helpers/")

    # Fonts
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/fonts/" "Kuplung.app/Contents/Resources/fonts/")
    FILE(GLOB Kuplung_Fonts_Files "Resources/fonts/*.*")
    FILE(COPY ${Kuplung_Fonts_Files} DESTINATION "Kuplung.app/Contents/Resources/fonts/")

    # GUI
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/gui/" "Kuplung.app/Contents/Resources/gui/")
    FILE(GLOB Kuplung_GUI_Files "Resources/gui/*.*")
    FILE(COPY ${Kuplung_GUI_Files} DESTINATION "Kuplung.app/Contents/Resources/gui/")

    # Image Assets
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/Images.xcassets/AppIcon.appiconset/" "Kuplung.app/Contents/Resources/Images.xcassets/AppIcon.appiconset/")
    FILE(GLOB Kuplung_ImageAssets_Files "Resources/Images.xcassets/AppIcon.appiconset/*.*")
    FILE(COPY ${Kuplung_ImageAssets_Files} DESTINATION "Kuplung.app/Contents/Resources/Images.xcassets/AppIcon.appiconset/")

    # ProtoBuf
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/protobuf/" "Kuplung.app/Contents/Resources/protobuf/")
    FILE(GLOB Kuplung_ProtoBuf_Files "Resources/protobuf/*.*")
    FILE(COPY ${Kuplung_ProtoBuf_Files} DESTINATION "Kuplung.app/Contents/Resources/protobuf/")

    # Shaders
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/shaders/" "Kuplung.app/Contents/Resources/shaders/")
    FILE(GLOB Kuplung_Shaders_Files "Resources/shaders/*.*")
    FILE(COPY ${Kuplung_Shaders_Files} DESTINATION "Kuplung.app/Contents/Resources/shaders/")

    # Shaders - ShaderToy
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/shaders/stoy/" "Kuplung.app/Contents/Resources/shaders/stoy/")
    FILE(GLOB Kuplung_Shaders_Stoy_Files "Resources/shaders/stoy/*.*")
    FILE(COPY ${Kuplung_Shaders_Stoy_Files} DESTINATION "Kuplung.app/Contents/Resources/shaders/stoy/")

    # Shaders - ShaderToy Assets
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/shadertoy/" "Kuplung.app/Contents/Resources/shadertoy/")
    FILE(GLOB Kuplung_Shaders_StoyAssets_Files "Resources/shadertoy/*.*")
    FILE(COPY ${Kuplung_Shaders_StoyAssets_Files} DESTINATION "Kuplung.app/Contents/Resources/shadertoy/")

    # Skybox
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "Resources/skybox/" "Kuplung.app/Contents/Resources/skybox/")
    FILE(GLOB Kuplung_Skybox_Files "Resources/skybox/*.*")
    FILE(COPY ${Kuplung_Skybox_Files} DESTINATION "Kuplung.app/Contents/Resources/skybox/")

    ADD_EXECUTABLE(Kuplung MACOSX_BUNDLE ${SOURCE_FILES})

    SET_TARGET_PROPERTIES(Kuplung PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist")
    #SET_TARGET_PROPERTIES(Kuplung PROPERTIES RESOURCE "${Kuplung_Shapes}")
    #SET_TARGET_PROPERTIES(Kuplung PROPERTIES RESOURCE "${Kuplung_Axis_Helpers}")
    #SET_TARGET_PROPERTIES(Kuplung PROPERTIES RESOURCE "${Kuplung_Fonts}")
    #SET_TARGET_PROPERTIES(Kuplung PROPERTIES RESOURCE "${Kuplung_GUI}")
    #SET_TARGET_PROPERTIES(Kuplung PROPERTIES RESOURCE "${Kuplung_Images_xcassets}")
    #SET_TARGET_PROPERTIES(Kuplung PROPERTIES RESOURCE "${Kuplung_ProtoBuf}")
    #SET_TARGET_PROPERTIES(Kuplung PROPERTIES RESOURCE "${Kuplung_Shaders}")
    #SET_TARGET_PROPERTIES(Kuplung PROPERTIES RESOURCE "${Kuplung_Skybox}")
    #SET_TARGET_PROPERTIES(Kuplung PROPERTIES RESOURCE "${Kuplung_Misc}")
ENDIF(APPLE)
