//
//  BoundingBox.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef BoundingBox_hpp
#define BoundingBox_hpp

#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include <glm/gtc/matrix_transform.hpp>

class BoundingBox {
public:
  ~BoundingBox();
  BoundingBox() noexcept;
  const bool initShaderProgram();
  void initBuffers(const MeshModel& meshModel);
  void render(const glm::mat4& matrixMVP, const glm::vec4& outlineColor);
  MeshModel meshModel;

  glm::mat4 matrixTransform;

  GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
  glm::vec3 size, center;

private:
  std::vector<GLfloat> dataVertices;
  std::vector<GLuint> dataIndices;

  GLuint shaderProgram;
  GLuint shaderVertex, shaderFragment;
  GLuint glVAO;
  GLuint vboVertices, vboIndices;

  GLint glUniformMVPMatrix, glUniformColor;
};

#endif /* BoundingBox_hpp */
