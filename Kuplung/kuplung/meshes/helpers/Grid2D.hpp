//
//  Grid2D.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Grid2D_hpp
#define Grid2D_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/settings/Settings.h"
#include "kuplung/objects/Objects.h"

class Grid2D {
public:
  ~Grid2D();
  Grid2D();
  const bool initShaderProgram();
  void initBuffers(const int& gridSize, const float& unitSize);
  void initProperties();
  void render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera, const bool& showZAxis);
  unsigned short int gridSize;

  bool showGrid;
  std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;
  std::unique_ptr<ObjectCoordinate> scaleX, scaleY, scaleZ;
  std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;

  glm::mat4 matrixModel;

  std::vector<GLfloat> dataVertices;
  std::vector<GLfloat> dataTexCoords;
  std::vector<GLfloat> dataNormals;
  std::vector<GLfloat> dataColors;
  std::vector<GLuint> dataIndices;

private:
  GLuint shaderProgram;
  GLuint shaderVertex, shaderFragment;
  GLuint glVAO, vboVertices, vboColors, vboIndices;
  GLint glUniformMVPMatrix;

  int gridSizeVertex;
  int zIndex;
};

#endif /* Grid2D_hpp */
