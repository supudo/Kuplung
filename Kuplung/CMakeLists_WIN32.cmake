# Win32 CMake file

ADD_DEFINITIONS(-DKuplung_Debug_Timings)
ADD_DEFINITIONS(-DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="${CMAKE_SOURCE_DIR}/kuplung/utilities/gl/GLIncludes.h")

# Warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4244 -wd4100 -wd4068 -wd4267 -wd4305 -wd4838 -wd4477 -wd4305 -wd4996 -wd4309")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKuplung_Debug_Timings")

# GLM
INCLUDE_DIRECTORIES("D:/Libs/glm-0.9.8.4")

# Include OpenGL
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lopengl32")

# SDL2
LINK_DIRECTORIES("D:/Libs/SDL2-2.0.5/lib/x64")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lSDL2main -lSDL2")
INCLUDE_DIRECTORIES("D:/Libs/SDL2-2.0.5/include")

# GLFW3
LINK_DIRECTORIES("D:/Libs/GLFW3/lib-vc2015")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lGLFW3")
INCLUDE_DIRECTORIES("D:/Libs/GLFW3/include")

# GLEW
LINK_DIRECTORIES("D:/Libs/glew-2.0.0-win32/lib/Release/x64")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lGLEW32")
INCLUDE_DIRECTORIES("D:/Libs/glew-2.0.0-win32/include")

# Assimp
LINK_DIRECTORIES("D:/Libs/assimp/lib")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lassimp-vc140-mt")
INCLUDE_DIRECTORIES("D:/Libs/assimp/include")

# Protobuf
LINK_DIRECTORIES("D:/Libs/protobuf/lib")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -llibprotobufd")
INCLUDE_DIRECTORIES("D:/Libs/protobuf/include")

# Lua
LINK_DIRECTORIES("D:/Libs/lua-5.3.4_Win64_vc14_lib/lib")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -llua53")
INCLUDE_DIRECTORIES("D:/Libs/lua-5.3.4_Win64_vc14_lib/include")

# libnoise
INCLUDE_DIRECTORIES(../external/libnoise/windows/include)
LINK_DIRECTORIES(../external/libnoise/windows/lib)
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lnoise")

INCLUDE_DIRECTORIES(.)

ADD_DEFINITIONS(-DDef_Kuplung_OpenGL_4x)
ADD_DEFINITIONS(-DGLM_ENABLE_EXPERIMENTAL)

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
  	kuplung/meshes/helpers/AxisLabels.cpp
  	kuplung/meshes/helpers/AxisLabels.hpp
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
  	kuplung/meshes/helpers/Grid2D.cpp
  	kuplung/meshes/helpers/Grid2D.hpp
    kuplung/meshes/scene/ModelFaceBase.cpp
    kuplung/meshes/scene/ModelFaceBase.hpp
    kuplung/meshes/scene/ModelFaceData.cpp
    kuplung/meshes/scene/ModelFaceData.hpp
    kuplung/objects/Objects.h
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
    kuplung/rendering/methods/RenderingForwardShadowMapping.cpp
    kuplung/rendering/methods/RenderingForwardShadowMapping.hpp
  	kuplung/rendering/methods/RenderingShadowMapping.cpp
  	kuplung/rendering/methods/RenderingShadowMapping.hpp
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
    kuplung/ui/components/exporters/ExportGLTF.cpp
    kuplung/ui/components/exporters/ExportGLTF.hpp
    kuplung/ui/components/exporters/ExportFile.cpp
    kuplung/ui/components/exporters/ExportFile.hpp
    kuplung/ui/components/importers/ImportFile.cpp
    kuplung/ui/components/importers/ImportFile.hpp
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
    kuplung/ui/components/ide/KuplungIDE.cpp
    kuplung/ui/components/ide/KuplungIDE.hpp
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
    kuplung/ui/UIManager.cpp
    kuplung/ui/UIManager.hpp
    kuplung/ui/UIHelpers.cpp
    kuplung/ui/UIHelpers.hpp
    kuplung/utilities/measurement/Timings.cpp
    kuplung/utilities/measurement/Timings.hpp
    kuplung/utilities/consumption/Consumption.cpp
    kuplung/utilities/consumption/Consumption.hpp
    kuplung/utilities/consumption/WindowsCPUUsage.cpp
    kuplung/utilities/consumption/WindowsCPUUsage.hpp
    kuplung/utilities/export/Exporter.cpp
    kuplung/utilities/export/Exporter.hpp
    kuplung/utilities/export/ExporterAssimp.cpp
    kuplung/utilities/export/ExporterAssimp.hpp
    kuplung/utilities/export/ExporterOBJ.cpp
    kuplung/utilities/export/ExporterOBJ.hpp
    kuplung/utilities/export/ExporterGLTF.cpp
    kuplung/utilities/export/ExporterGLTF.hpp
    kuplung/utilities/font-parser/FNTParser.cpp
    kuplung/utilities/font-parser/FNTParser.hpp
    kuplung/utilities/gl/GLIncludes.h
    kuplung/utilities/gl/GLUtils.cpp
    kuplung/utilities/gl/GLUtils.hpp
    kuplung/utilities/imgui/ImGuiColorTextEdit/TextEditor.cpp
    kuplung/utilities/imgui/ImGuiColorTextEdit/TextEditor.h
    kuplung/utilities/imgui/imguizmo/ImGuizmo.cpp
    kuplung/utilities/imgui/imguizmo/ImGuizmo.h
    kuplung/utilities/imgui/imconfig.h
    kuplung/utilities/imgui/imgui.cpp
    kuplung/utilities/imgui/imgui.h
    kuplung/utilities/imgui/imgui_demo.cpp
    kuplung/utilities/imgui/imgui_draw.cpp
    kuplung/utilities/imgui/imgui_internal.h
  	kuplung/utilities/imgui/imgui_widgets.cpp
  	kuplung/utilities/imgui/imstb_rectpack.h
  	kuplung/utilities/imgui/imstb_textedit.h
  	kuplung/utilities/imgui/imstb_truetype.h
  	kuplung/utilities/imgui/imgui_impl_opengl3.cpp
  	kuplung/utilities/imgui/imgui_impl_opengl3.h
  	kuplung/utilities/imgui/imgui_impl_sdl.cpp
  	kuplung/utilities/imgui/imgui_impl_sdl.h
    kuplung/utilities/nanovg/fontstash.h
    kuplung/utilities/nanovg/nanovg.c
    kuplung/utilities/nanovg/nanovg.h
    kuplung/utilities/nanovg/nanovg_gl.h
    kuplung/utilities/nanovg/nanovg_gl_utils.h
    kuplung/utilities/oui/blendish.h
    kuplung/utilities/oui/oui.h
    kuplung/utilities/input/Controls.cpp
    kuplung/utilities/input/Controls.hpp
    kuplung/utilities/libnoise/noiseutils.cpp
    kuplung/utilities/libnoise/noiseutils.h
    kuplung/utilities/maths/Maths.cpp
    kuplung/utilities/maths/Maths.hpp
    kuplung/utilities/lua/LuaManager.cpp
    kuplung/utilities/lua/LuaManager.hpp
    kuplung/utilities/minizip/KuplungMinizip.cpp
    kuplung/utilities/minizip/KuplungMinizip.hpp
    kuplung/utilities/miniz/miniz.c
    kuplung/utilities/miniz/miniz.h
    kuplung/utilities/miniz/KuplungMiniz.cpp
    kuplung/utilities/miniz/KuplungMiniz.hpp
    kuplung/utilities/parsers/assimp-parser/AssimpParser.cpp
    kuplung/utilities/parsers/assimp-parser/AssimpParser.hpp
    kuplung/utilities/parsers/obj-parser/objParser1.cpp
    kuplung/utilities/parsers/obj-parser/objParser1.hpp
    kuplung/utilities/parsers/obj-parser/objParser2.cpp
    kuplung/utilities/parsers/obj-parser/objParser2.hpp
    kuplung/utilities/parsers/obj-parser/objParserCuda.cpp
    kuplung/utilities/parsers/obj-parser/objParserCuda.hpp
    kuplung/utilities/parsers/stl-parser/STLParser.cpp
    kuplung/utilities/parsers/stl-parser/STLParser.hpp
    kuplung/utilities/parsers/ply-parser/PLYParser.cpp
    kuplung/utilities/parsers/ply-parser/PLYParser.hpp
    kuplung/utilities/parsers/gltf-parser/GLTFParser.cpp
    kuplung/utilities/parsers/gltf-parser/GLTFParser.hpp
    kuplung/utilities/parsers/FileModelManager.cpp
    kuplung/utilities/parsers/FileModelManager.hpp
    kuplung/utilities/parsers/ModelObject.h
    kuplung/utilities/parsers/ParserUtils.cpp
    kuplung/utilities/parsers/ParserUtils.hpp
    kuplung/utilities/renderers/default-forward/DefaultForwardRenderer.cpp
    kuplung/utilities/renderers/default-forward/DefaultForwardRenderer.hpp
    kuplung/utilities/renderers/scene-renderer/SceneRenderer.cpp
    kuplung/utilities/renderers/scene-renderer/SceneRenderer.hpp
    kuplung/utilities/renderers/ray-tracer/RayTracerRenderer.cpp
    kuplung/utilities/renderers/ray-tracer/RayTracerRenderer.hpp
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
    kuplung/utilities/catch/catch.hpp
    kuplung/utilities/json/json.hpp
    kuplung/utilities/cpp-base64/base64.cpp
    kuplung/utilities/cpp-base64/base64.h
    # kuplung/cuda/CudaTest.cpp
    # kuplung/cuda/CudaTest.hpp
    kuplung/Kuplung.cpp
    kuplung/Kuplung.hpp
    main.cpp)

SET(Kuplung_Shapes
    resources/shapes/brick_wall.mtl
    resources/shapes/brick_wall.obj
    resources/shapes/brick_wall_diffuse.png
    resources/shapes/brick_wall_displacement.png
    resources/shapes/brick_wall_normal.png
    resources/shapes/brick_wall_occlusion.png
    resources/shapes/brick_wall_spec.png
    resources/shapes/cone.mtl
    resources/shapes/cone.obj
    resources/shapes/cube.mtl
    resources/shapes/cube.obj
    resources/shapes/cylinder.mtl
    resources/shapes/cylinder.obj
    resources/shapes/epcot.mtl
    resources/shapes/epcot.obj
    resources/shapes/grid.mtl
    resources/shapes/grid.obj
    resources/shapes/ico_sphere.mtl
    resources/shapes/ico_sphere.obj
    resources/shapes/monkey_head.mtl
    resources/shapes/monkey_head.obj
    resources/shapes/MaterialBall.mtl
    resources/shapes/MaterialBall.obj
    resources/shapes/MaterialBallBlender.mtl
    resources/shapes/MaterialBallBlender.obj
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
    resources/gui/light_directional.mtl
    resources/gui/light_directional.obj
    resources/gui/light_point.mtl
    resources/gui/light_point.obj
    resources/gui/light_spot.mtl
    resources/gui/light_spot.obj)

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
  	resources/shaders/axis_labels.frag
  	resources/shaders/axis_labels.vert
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
  	resources/shaders/grid2d.frag
  	resources/shaders/grid2d.vert
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
  	resources/shaders/rendering_shadow_mapping.frag
  	resources/shaders/rendering_shadow_mapping.vert
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

SET(Kuplung_OuiNanovVG
    resources/oui/blender_icons.svg
    resources/oui/blender_icons16.png
    resources/oui/DejaVuSans.ttf)

SET(Kuplung_Lua
    resources/lua/test.lua)

SET(Kuplung_RC
    resources/kuplung.rc)

# protobuf generation
FILE(GLOB PROTOBUF_DEFINITION_FILES "resources/protobuf/*.proto")
SET(PROTOBUF_INPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/resources/protobuf/")
SET(PROTOBUF_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/kuplung/utilities/saveopen/")
FOREACH(PROTO_FILE ${PROTOBUF_DEFINITION_FILES})
  EXECUTE_PROCESS(COMMAND protoc --proto_path=${PROTOBUF_INPUT_DIRECTORY} --cpp_out=${PROTOBUF_OUTPUT_DIRECTORY} ${PROTO_FILE})
ENDFOREACH()
FILE(GLOB PROTOBUF_MODELS "kuplung/utilities/saveopen/*.pb.cc" "kuplung/utilities/saveopen/*.hpp")

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES} ${PROTOBUF_MODELS} kuplung/utilities/nanovg/nanovg.c kuplung/utilities/miniz/miniz.c)

SET(KuplungResources_PWD_WIN "/resources")
SET(KuplungResources_DESTDIR_WIN "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources")
EXECUTE_PROCESS(COMMAND "cmd /c xcopy /S /I /Y ${KuplungResources_PWD_WIN} ${KuplungResources_DESTDIR_WIN}")
