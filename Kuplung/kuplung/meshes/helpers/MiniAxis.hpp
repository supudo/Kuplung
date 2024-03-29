//
//  MiniAxis.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MiniAxis_hpp
#define MiniAxis_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"

class MiniAxis {
public:
  ~MiniAxis();
  MiniAxis();
  void initProperties();
  const bool initShaderProgram();
  void initBuffers(); // 0 = x, 1 = y, 2 = z
  void render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera);

  bool showAxis;
  std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;
  glm::mat4 matrixModel;

private:
  int axisSize;

  GLuint shaderProgram;
  GLuint shaderVertex, shaderFragment;
  GLuint glVAO, vboVertices, vboColors;
  GLint glUniformMVPMatrix;
};

#endif /* MiniAxis_hpp */
