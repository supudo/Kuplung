//
//  oceanFFT.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

/*
 * Copyright 1993-2015 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

/*
    FFT-based Ocean simulation
    based on original code by Yury Uralsky and Calvin Lin

    This sample demonstrates how to use CUFFT to synthesize and
    render an ocean surface in real-time.

    See Jerry Tessendorf's Siggraph course notes for more details:
    http://tessendorf.org/reports.html

    It also serves as an example of how to generate multiple vertex
    buffer streams from CUDA and render them using GLSL shaders.
*/

#ifdef DEF_KuplungSetting_UseCuda

#ifndef oceanFFT_hpp
#define oceanFFT_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/cuda/CudaHelpers.hpp"
#include "kuplung/objects/ObjectDefinitions.h"
#include <glm/glm.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <cufft.h>
#include <math_constants.h>

class oceanFFT {
public:
  ~oceanFFT();
  void init();
  void initParameters();
  GLuint draw(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid);
  void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid);

  bool Setting_ShowWireframes, Setting_DrawDots, Setting_Animate;

  // simulation parameters
  float Simm_g = 9.81f;              // gravitational constant
  float Simm_A = 1e-7f;              // wave scale factor
  float Simm_patchSize = 100;        // patch size
  float Simm_windSpeed = 100.0f;
  float Simm_windDir = CUDART_PI_F / 3.0f;
  float Simm_dirDepend = 0.07f;

  unsigned int meshSize = 256;
  unsigned int spectrumW = meshSize + 4;
  unsigned int spectrumH = meshSize + 1;
  float heightModifier = 1.0;

  bool scaleAll;
  std::unique_ptr<ObjectCoordinate> scaleX, scaleY, scaleZ;
  std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;

private:
  GLuint shaderProgram;
  GLuint glVAO;
  GLuint renderFBO, renderTextureColorBuffer, renderRBO;

  GLint gl_VS_MVPMatrix, gl_VS_MVMatrix, gl_VS_NMatrix;
  GLint gl_heightScale, gl_chopiness, gl_size;
  GLint gl_deepColor, gl_shallowColor, gl_skyColor, gl_lightDir;

  glm::mat4 matrixModel;

  std::unique_ptr<CudaHelpers> helperCuda;

  bool initShaderProgram();
  void initBuffers();
  void createFBO();
  void generateAttachmentTexture(GLboolean depth, GLboolean stencil);

  bool inRendererUI;

  // OpenGL vertex buffers
  GLuint posVertexBuffer;
  GLuint heightVertexBuffer, slopeVertexBuffer, indexBuffer;
  struct cudaGraphicsResource *cuda_posVB_resource, *cuda_heightVB_resource, *cuda_slopeVB_resource; // handles OpenGL-CUDA exchange

  // FFT data
  cufftHandle fftPlan;
  float2 *d_h0 = 0;   // heightfield at time 0
  float2 *h_h0 = 0;
  float2 *d_ht = 0;   // heightfield at time t
  float2 *d_slope = 0;

  // pointers to device object
  float *g_hptr = NULL;
  float2 *g_sptr = NULL;

  float animTime = 0.0f;
  float prevTime = 0.0f;
  float animationRate = -0.001f;

  bool runAutoTest();
  void runGraphicsTest();

  // GL functionality
  void timerEvent(int value);

  // Cuda functionality
  void runCuda();
  void runCudaTest();
  void generate_h0(float2 *h0);

  float phillips(float Kx, float Ky, float Vdir, float V, float A, float dir_depend);
  float gauss();
  float urand();
  void renderCuda(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid);

  void timerEvent();
};

#endif /* oceanFFT_hpp */

#endif
