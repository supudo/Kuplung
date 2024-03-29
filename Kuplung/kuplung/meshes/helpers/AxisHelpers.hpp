//
//  AxisHelpers.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef AxisHelpers_hpp
#define AxisHelpers_hpp

#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/helpers/Camera.hpp"

class AxisHelpers {
public:
  ~AxisHelpers();
  AxisHelpers();
  void setModel(const MeshModel& meshModel);
  void initProperties();
  const bool initShaderProgram();
  void initBuffers();
  void render(const glm::mat4& mtxProjection, const glm::mat4& mtxCamera, const glm::vec3& position);
  MeshModel meshModel;

private:
  GLuint shaderProgram;
  GLuint shaderVertex, shaderFragment;
  GLuint glVAO;
  GLuint vboVertices, vboIndices;
  GLint glUniformMVPMatrix, glUniformColor;
};

#endif /* AxisHelpers_hpp */
