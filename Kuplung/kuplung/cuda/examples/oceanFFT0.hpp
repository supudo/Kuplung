//
//  oceanFFT0.hpp
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

#ifndef oceanFFT0_hpp
#define oceanFFT0_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include <glm/glm.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <helper_gl.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <cufft.h>

#include <helper_cuda.h>
#include <helper_cuda_gl.h>

#include <helper_functions.h>
#include <math_constants.h>

#include <GLUT/glut.h>

#include <rendercheck_gl.h>

class oceanFFT0 {
public:
    ~oceanFFT0();
    void init();
    GLuint draw(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid);

private:
    GLuint shaderProgram;
    GLuint glVAO;
    GLuint renderFBO, renderTextureColorBuffer, renderRBO;

    glm::mat4 matrixModel;

    bool initShaderProgram();
    void initBuffers();
    void createFBO();
    void generateAttachmentTexture(GLboolean depth, GLboolean stencil);

    ////////////////////////////////////////////////////////////////////////////////
    // constants
    const unsigned int meshSize = 256;
    const unsigned int spectrumW = meshSize + 4;
    const unsigned int spectrumH = meshSize + 1;
    const float heightModifier = 1.0;

    // OpenGL vertex buffers
    GLuint posVertexBuffer;
    GLuint heightVertexBuffer, slopeVertexBuffer, indexBuffer;
    struct cudaGraphicsResource *cuda_posVB_resource, *cuda_heightVB_resource, *cuda_slopeVB_resource; // handles OpenGL-CUDA exchange

    bool animate = true;
    bool drawPoints = false;
    bool wireFrame = false;

    // FFT data
    cufftHandle fftPlan;
    float2 *d_h0 = 0;   // heightfield at time 0
    float2 *h_h0 = 0;
    float2 *d_ht = 0;   // heightfield at time t
    float2 *d_slope = 0;

    // pointers to device object
    float *g_hptr = NULL;
    float2 *g_sptr = NULL;

    // simulation parameters
    const float g = 9.81f;              // gravitational constant
    const float A = 1e-7f;              // wave scale factor
    const float patchSize = 100;        // patch size
    float windSpeed = 100.0f;
    float windDir = CUDART_PI_F / 3.0f;
    float dirDepend = 0.07f;

    StopWatchInterface *timer = NULL;
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

#endif /* oceanFFT0_hpp */

#endif
