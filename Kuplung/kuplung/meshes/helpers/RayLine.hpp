//
//  RayLine.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RayLine_hpp
#define RayLine_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/objects/ObjectDefinitions.h"
#ifdef WIN32
//#include <windows.h>
//#include <windef.h>
typedef unsigned int uint;
#endif

class RayLine {
public:
  ~RayLine();
  void init();
  const bool initShaderProgram();
  void initBuffers(const glm::vec3& vecFrom, const glm::vec3& vecTo);
  void initProperties();
  void render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera);

  glm::mat4 matrixModel;

private:
  GLuint shaderProgram;
  GLuint shaderVertex, shaderFragment;
  GLuint glVAO, vboVertices, vboColors, vboIndices;
  GLint glUniformMVPMatrix, glUniformColor;

  std::vector<float> dataVertices, dataColors;
  std::vector<uint> dataIndices;
};

#endif /* RayLine_hpp */
