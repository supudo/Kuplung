//
//  WorldGrid.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef WorldGrid_hpp
#define WorldGrid_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

struct MirrorSurface {
    float translateX,translateY, translateZ;
    float rotateX, rotateY, rotateZ;
};

class WorldGrid {
public:
    ~WorldGrid();
    void destroy();
    void init();
    bool initShaderProgram();
    void initBuffers(int gridSize, float unitSize);
    void initProperties(int size);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, bool showZAxis);
    int gridSize;
    float transparency;

    MirrorSurface *mirrorSurface;
    bool showGrid, actAsMirror;
    std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;
    std::unique_ptr<ObjectCoordinate> scaleX, scaleY, scaleZ;
    std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

    std::vector<GLfloat> dataVertices;
    std::vector<GLfloat> dataTexCoords;
    std::vector<GLfloat> dataNormals;
    std::vector<GLfloat> dataColors;
    std::vector<GLuint> dataIndices;

private:
    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboColors, vboIndices;
    GLuint glAttributeVertexPosition, glUniformMVPMatrix, glAttributeActAsMirror, glAttributeAlpha, glAttributeColor;

    bool actAsMirrorNeedsChange;
    int gridSizeVertex;
    int zIndex;

    std::string readFile(const char *filePath);
};

#endif /* WorldGrid_hpp */
