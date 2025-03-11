//
//  HeightmapGenerator.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/21/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef HeightmapGenerator_hpp
#define HeightmapGenerator_hpp

#ifdef _WIN32
#  if NOISE_STATIC
#    define NOISE_EXPORT
#  elif NOISE_BUILD_DLL
#    define NOISE_EXPORT __declspec(dllexport)
#  else
#    define NOISE_EXPORT __declspec(dllimport)
#  endif
#else
#define NOISE_EXPORT
#endif

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#ifdef __linux__
#include <libnoise/noise.h>
#elif _WIN32
#include <noise/noise.h>
#else
#include "noise.h"
#endif
#include "kuplung/utilities/libnoise/noiseutils.h"
#include <glm/common.hpp>

typedef enum GeometryTerrainType {
  GeometryTerrainType_Smooth,
  GeometryTerrainType_Cubic,
  GeometryTerrainType_Sphere
} GeometryTerrainType;

class HeightmapGenerator {
public:
  void initPosition();
  void generateTerrain(const std::string& assetsFolder, const int& width, const int& height);

  std::vector<glm::vec3> vertices, normals, colors;
  std::vector<glm::vec2> uvs;
  std::vector<unsigned int> indices;
  std::string heightmapImage;

  int Setting_Octaves;
  float Setting_Frequency, Setting_Persistence, Setting_ScaleCoeficient, Setting_HeightCoeficient;
  float Setting_OffsetHorizontal, Setting_OffsetVertical;
  bool Setting_ColorTerrain, Setting_SeedRandom;
  int Setting_TerrainType, Setting_Seed;
  MeshModel modelTerrain;

private:
  std::string assetsFolder;
  int width, height;
  float position_x1;
  float position_x2;
  float position_y1;
  float position_y2;

  utils::NoiseMap heightMap;
  utils::Image image;

  void generateMeshModel();
  void generatePlaneGeometrySmooth();
  void generatePlaneGeometryCubic();
  void generateSphereGeometry();
};

#endif /* HeightmapGenerator_hpp */
