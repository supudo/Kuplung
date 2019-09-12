//
//  Terrain.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Terrain_hpp
#define Terrain_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/settings/Settings.h"
#include "kuplung/pcg/HeightmapGenerator.hpp"

class Terrain {
public:
  ~Terrain();
  Terrain();
  bool initShaderProgram();
  void initBuffers(std::string const& assetsFolder, const int width, const int height);
  void render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera, const glm::mat4& matrixModel);
  std::string heightmapImage;
  std::unique_ptr<HeightmapGenerator> terrainGenerator;

  bool Setting_UseTexture, Setting_Wireframe;

private:
  GLuint shaderProgram;
  GLuint shaderVertex, shaderFragment;
  GLuint glVAO;
  GLuint vboVertices, vboTextureCoordinates, vboTextureDiffuse, vboNormals, vboColors, vboIndices;
  GLuint glUniformMVPMatrix, glAttributeVertexPosition, glAttributeVertexNormal, glAttributeColor;
  GLint glAttributeTextureCoord, glUniformHasTexture, glUniformSamplerTexture;
};

#endif /* Terrain_hpp */
