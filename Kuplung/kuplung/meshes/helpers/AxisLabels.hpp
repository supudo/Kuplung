//
//  AxisLabels.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef AxisLabels_hpp
#define AxisLabels_hpp

#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/helpers/Camera.hpp"

class AxisLabels {
public:
  ~AxisLabels();
  AxisLabels();
  void setModels(const MeshModel& meshModel_PosX, const MeshModel& meshModel_NegX, const MeshModel& meshModel_PosY,
                 const MeshModel& meshModel_NegY, const MeshModel& meshModel_PosZ, const MeshModel& meshModel_NegZ,
                 int position);
  void initProperties();
  const bool initShaderProgram();
  void initBuffers();
  void render(const glm::mat4& mtxProjection, const glm::mat4& mtxCamera, int position);
  MeshModel meshModel_PosX, meshModel_NegX, meshModel_PosY, meshModel_NegY, meshModel_PosZ, meshModel_NegZ;

private:
  GLuint shaderProgram;
  GLuint shaderVertex, shaderFragment;
  GLuint glVAO;
  GLuint vboVertices, vboColors, vboIndices;
  GLint glUniformMVPMatrix;

  std::vector<glm::vec3> dataVertices;
  std::vector<glm::vec3> dataColors;
  std::vector<unsigned int> dataIndices;

  int ahPosition;
};

#endif /* AxisLabels_hpp */
