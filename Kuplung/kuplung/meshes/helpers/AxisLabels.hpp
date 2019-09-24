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
  GLuint glVAO_PosX, glVAO_NegX, glVAO_PosY, glVAO_NegY, glVAO_PosZ, glVAO_NegZ;
  GLuint vboVertices_PosX, vboColors_PosX, vboIndices_PosX;
  GLuint vboVertices_NegX, vboColors_NegX, vboIndices_NegX;
  GLuint vboVertices_PosY, vboColors_PosY, vboIndices_PosY;
  GLuint vboVertices_NegY, vboColors_NegY, vboIndices_NegY;
  GLuint vboVertices_PosZ, vboColors_PosZ, vboIndices_PosZ;
  GLuint vboVertices_NegZ, vboColors_NegZ, vboIndices_NegZ;
  GLint glUniformMVPMatrix;

  int ahPosition;
};

#endif /* AxisLabels_hpp */
