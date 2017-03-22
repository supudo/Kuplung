//
//  oceanFFT.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifdef DEF_KuplungSetting_UseCuda

#include "oceanFFT.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

////////////////////////////////////////////////////////////////////////////////
// kernels
//#include <oceanFFT_kernel.cu>

extern "C"
void cudaGenerateSpectrumKernel(float2 *d_h0,
                                float2 *d_ht,
                                unsigned int in_width,
                                unsigned int out_width,
                                unsigned int out_height,
                                float animTime,
                                float patchSize);

extern "C"
void cudaUpdateHeightmapKernel(float  *d_heightMap,
                               float2 *d_ht,
                               unsigned int width,
                               unsigned int height);

extern "C"
void cudaCalculateSlopeKernel(float *h, float2 *slopeOut,
                              unsigned int width, unsigned int height);

////////////////////////////////////////////////////////////////////////////////

void oceanFFT::init() {
    this->helperCuda = std::make_unique<CudaHelpers>();

    this->runAutoTest();
    this->runGraphicsTest();
}

void oceanFFT::initParameters() {
    this->Setting_ShowWireframes = false;
    this->Setting_DrawDots = false;
    this->Setting_Animate = true;

    this->Simm_g = 9.81f;              // gravitational constant
    this->Simm_A = 1e-7f;              // wave scale factor
    this->Simm_patchSize = 100;        // patch size
    this->Simm_windSpeed = 100.0f;
    this->Simm_windDir = CUDART_PI_F / 3.0f;
    this->Simm_dirDepend = 0.07f;

    this->meshSize = 256;
    this->spectrumW = meshSize + 4;
    this->spectrumH = meshSize + 1;
    this->heightModifier = 1.0;
}

void oceanFFT::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid) {
    this->inRendererUI = false;
    this->timerEvent();
    this->renderCuda(matrixProjection, matrixCamera, matrixGrid);
}

GLuint oceanFFT::draw(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid) {
    this->inRendererUI = true;

    this->timerEvent();

    this->createFBO();

    glBindFramebuffer(GL_FRAMEBUFFER, this->renderFBO);
    this->renderCuda(matrixProjection, matrixCamera, matrixGrid);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->renderTextureColorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return this->renderTextureColorBuffer;
}

void oceanFFT::renderCuda(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid) {
    // run CUDA kernel to generate vertex positions
    if (this->Setting_Animate)
        runCuda();

    if (this->inRendererUI) {
        glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    glUseProgram(this->shaderProgram);

    glm::mat4 matrixModel = glm::mat4(1.0);
    matrixModel = matrixModel * matrixGrid;

    glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
    glm::mat4 mvMatrix = matrixCamera * matrixModel;
    glm::mat3 nMatrix = glm::inverseTranspose(glm::mat3(matrixCamera * matrixModel));

    glUniformMatrix4fv(this->gl_VS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(this->gl_VS_MVMatrix, 1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix3fv(this->gl_VS_NMatrix, 1, GL_FALSE, glm::value_ptr(nMatrix));

    glUniform1f(this->gl_heightScale, this->heightModifier / 2);
    glUniform1f(this->gl_chopiness, 1.0f);
    glUniform2f(this->gl_size, (float) this->meshSize, float(this->meshSize));

    glUniform4f(this->gl_deepColor, 0.0f, 0.1f, 0.4f, 1.0f);
    glUniform4f(this->gl_shallowColor, 0.1f, 0.3f, 0.3f, 1.0f);
    glUniform4f(this->gl_skyColor, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform3f(this->gl_lightDir, 0.0f, 1.0f, 0.0f);

    glBindVertexArray(this->glVAO);
    if (this->Setting_DrawDots)
        glDrawArrays(GL_POINTS, 0, this->meshSize * this->meshSize);
    else {
        if (Settings::Instance()->wireframesMode || this->Setting_ShowWireframes)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLE_STRIP, ((this->meshSize * 2) + 2) * (this->meshSize - 1), GL_UNSIGNED_INT, 0);
        if (Settings::Instance()->wireframesMode || this->Setting_ShowWireframes)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glBindVertexArray(0);

    glUseProgram(0);
}

/* ===================================================================================================================
 * ===================================================================================================================
 * Cuda funcs
 * ===================================================================================================================
 * ===================================================================================================================
 */

bool oceanFFT::runAutoTest() {
    bool result = true;

    Settings::Instance()->funcDoLog("[CudaExamples - oceanFFT] Starting auto test...");

    // Cuda init
    int dev = this->helperCuda->findCudaDevice();

    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, dev);

    // create FFT plan
    cufftPlan2d(&this->fftPlan, this->meshSize, this->meshSize, CUFFT_C2C);

    // allocate memory
    int spectrumSize = this->spectrumW * this->spectrumH * sizeof(float2);
    cudaMalloc((void **)&this->d_h0, spectrumSize);
    this->h_h0 = (float2 *) malloc(spectrumSize);
    generate_h0(this->h_h0);
    cudaMemcpy(this->d_h0, this->h_h0, spectrumSize, cudaMemcpyHostToDevice);

    int outputSize =  this->meshSize * this->meshSize * sizeof(float2);
    cudaMalloc((void **)&this->d_ht, outputSize);
    cudaMalloc((void **)&this->d_slope, outputSize);

    this->prevTime = 0.0f;

    runCudaTest();

    cudaFree(this->d_ht);
    cudaFree(this->d_slope);
    cudaFree(this->d_h0);
    cufftDestroy(this->fftPlan);
    free(this->h_h0);

    return result;
}

void oceanFFT::runCudaTest() {
    cudaMalloc((void **)&this->g_hptr, this->meshSize * this->meshSize * sizeof(float));
    cudaMalloc((void **)&this->g_sptr, this->meshSize * this->meshSize * sizeof(float2));

    // generate wave spectrum in frequency domain
    cudaGenerateSpectrumKernel(this->d_h0, this->d_ht, this->spectrumW, this->meshSize, this->meshSize, this->animTime, this->Simm_patchSize);

    // execute inverse FFT to convert to spatial domain
    cufftExecC2C(this->fftPlan, this->d_ht, this->d_ht, CUFFT_INVERSE);

    // update heightmap values
    cudaUpdateHeightmapKernel(this->g_hptr, this->d_ht, this->meshSize, this->meshSize);

    // calculate slope for shading
    cudaCalculateSlopeKernel(this->g_hptr, this->g_sptr, this->meshSize, this->meshSize);

    cudaFree(this->g_hptr);
    cudaFree(this->g_sptr);
}

void oceanFFT::runGraphicsTest() {
    this->helperCuda->findCudaGLDevice();

    // create FFT plan
    cufftPlan2d(&this->fftPlan, this->meshSize, this->meshSize, CUFFT_C2C);

    // allocate memory
    int spectrumSize = this->spectrumW * this->spectrumH * sizeof(float2);
    cudaMalloc((void **)&this->d_h0, spectrumSize);
    this->h_h0 = (float2 *) malloc(spectrumSize);
    generate_h0(this->h_h0);
    cudaMemcpy(this->d_h0, this->h_h0, spectrumSize, cudaMemcpyHostToDevice);

    int outputSize =  this->meshSize * this->meshSize * sizeof(float2);
    cudaMalloc((void **)&this->d_ht, outputSize);
    cudaMalloc((void **)&this->d_slope, outputSize);

    this->prevTime = 0.0f;

    this->initShaderProgram();
    this->initBuffers();

    runCuda();
}

void oceanFFT::runCuda() {
    size_t num_bytes;

    // generate wave spectrum in frequency domain
    cudaGenerateSpectrumKernel(this->d_h0, this->d_ht, this->spectrumW, this->meshSize, this->meshSize, this->animTime, this->Simm_patchSize);

    // execute inverse FFT to convert to spatial domain
    cufftExecC2C(this->fftPlan, this->d_ht, this->d_ht, CUFFT_INVERSE);

    // update heightmap values in vertex buffer
    cudaGraphicsMapResources(1, &this->cuda_heightVB_resource, 0);
    cudaGraphicsResourceGetMappedPointer((void **)&this->g_hptr, &num_bytes, this->cuda_heightVB_resource);

    cudaUpdateHeightmapKernel(this->g_hptr, this->d_ht, this->meshSize, this->meshSize);

    // calculate slope for shading
    cudaGraphicsMapResources(1, &this->cuda_slopeVB_resource, 0);
    cudaGraphicsResourceGetMappedPointer((void **)&this->g_sptr, &num_bytes, this->cuda_slopeVB_resource);

    cudaCalculateSlopeKernel(this->g_hptr, this->g_sptr, this->meshSize, this->meshSize);

    cudaGraphicsUnmapResources(1, &this->cuda_heightVB_resource, 0);
    cudaGraphicsUnmapResources(1, &this->cuda_slopeVB_resource, 0);
}

void oceanFFT::timerEvent() {
    float time = SDL_GetTicks();
    if (this->Setting_Animate)
        this->animTime += (time - this->prevTime) * this->animationRate;
    this->prevTime = time;
}

float oceanFFT::urand() {
    return rand() / (float)RAND_MAX;
}

// Generates Gaussian random number with mean 0 and standard deviation 1.
float oceanFFT::gauss() {
    float u1 = urand();
    float u2 = urand();
    if (u1 < 1e-6f)
        u1 = 1e-6f;
    return sqrtf(-2 * logf(u1)) * cosf(2 * CUDART_PI_F * u2);
}

// Phillips spectrum
// (Kx, Ky) - normalized wave vector
// Vdir - wind angle in radians
// V - wind speed
// A - constant
float oceanFFT::phillips(float Kx, float Ky, float Vdir, float V, float A, float dir_depend) {
    float k_squared = Kx * Kx + Ky * Ky;

    if (k_squared == 0.0f)
        return 0.0f;

    // largest possible wave from constant wind of velocity v
    float L = V * V / this->Simm_g;

    float k_x = Kx / sqrtf(k_squared);
    float k_y = Ky / sqrtf(k_squared);
    float w_dot_k = k_x * cosf(Vdir) + k_y * sinf(Vdir);

    float phillips = A * expf(-1.0f / (k_squared * L * L)) / (k_squared * k_squared) * w_dot_k * w_dot_k;

    // filter out waves moving opposite to wind
    if (w_dot_k < 0.0f)
        phillips *= dir_depend;

    // damp out waves with very small length w << l
    //float w = L / 10000;
    //phillips *= expf(-k_squared * w * w);

    return phillips;
}

// Generate base heightfield in frequency space
void oceanFFT::generate_h0(float2 *h0) {
    for (unsigned int y = 0; y <= this->meshSize; y++) {
        for (unsigned int x = 0; x <= this->meshSize; x++) {
            float kx = (-(int)this->meshSize / 2.0f + x) * (2.0f * CUDART_PI_F / this->Simm_patchSize);
            float ky = (-(int)this->meshSize / 2.0f + y) * (2.0f * CUDART_PI_F / this->Simm_patchSize);

            float P = sqrtf(phillips(kx, ky, this->Simm_windDir, this->Simm_windSpeed, this->Simm_A, this->Simm_dirDepend));

            if (kx == 0.0f && ky == 0.0f)
                P = 0.0f;

            //float Er = urand()*2.0f-1.0f;
            //float Ei = urand()*2.0f-1.0f;
            float Er = gauss();
            float Ei = gauss();

            float h0_re = Er * P * CUDART_SQRT_HALF_F;
            float h0_im = Ei * P * CUDART_SQRT_HALF_F;

            int i = y * this->spectrumW + x;
            h0[i].x = h0_re;
            h0[i].y = h0_im;
        }
    }
}

/* ===================================================================================================================
 * ===================================================================================================================
 * Kuplung funcs
 * ===================================================================================================================
 * ===================================================================================================================
 */

void oceanFFT::initBuffers() {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    unsigned int plane_width = this->meshSize;
    unsigned int plane_height = this->meshSize;

    // vertices
    std::vector<glm::vec3> vertices;
    for (unsigned int y = 0; y < plane_height; y++) {
        for (unsigned int x = 0; x < plane_width; x++) {
            float u = x / float(plane_width - 1);
            float v = y / float(plane_height - 1);
            glm::vec3 vertex = glm::vec3(1.0);
            vertex.x = u * 2.0f - 1.0f;
            vertex.y = 0.0f;
            vertex.z = v * 2.0f - 1.0f;
            vertices.push_back(vertex * this->heightModifier);
        }
    }
    glGenBuffers(1, &this->posVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->posVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)), &vertices[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // height
    glGenBuffers(1, &this->heightVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->heightVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->meshSize * this->meshSize * sizeof(float), 0, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    cudaGraphicsGLRegisterBuffer(&this->cuda_heightVB_resource, this->heightVertexBuffer, cudaGraphicsMapFlagsWriteDiscard);

    // slopes
    glGenBuffers(1, &this->slopeVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->slopeVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->meshSize * this->meshSize * sizeof(float2), 0, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float2), NULL);
    cudaGraphicsGLRegisterBuffer(&this->cuda_slopeVB_resource, this->slopeVertexBuffer, cudaGraphicsMapFlagsWriteDiscard);

    // indices
    std::vector<unsigned int> indices;
    for (unsigned int y = 0; y < plane_height - 1; y++) {
        for (unsigned int x = 0; x < plane_width; x++) {
            indices.push_back(y * plane_width + x);
            indices.push_back((y + 1) * plane_width + x);
        }

        // start new strip with degenerate triangle
        indices.push_back((y + 1) * plane_width + (plane_width - 1));
        indices.push_back((y + 1) * plane_width);
    }
    glGenBuffers(1, &this->indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(indices.size()) * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

bool oceanFFT::initShaderProgram() {
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/cuda/oceanFFT.vert";
    std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/cuda/oceanFFT.frag";
    std::string shaderSourceFragment = Settings::Instance()->glUtils->readFile(shaderPath.c_str());

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= Settings::Instance()->glUtils->compileShader(this->shaderProgram, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation &= Settings::Instance()->glUtils->compileShader(this->shaderProgram, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[CudaExamples - oceanFFT 0] Error linking program " + std::to_string(this->shaderProgram) + "!");
        Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
        return false;
    }

    this->gl_VS_MVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "v_MVPMatrix");
    this->gl_VS_MVMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "v_MVMatrix");
    this->gl_VS_NMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "v_NMatrix");

    this->gl_heightScale = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "heightScale");
    this->gl_chopiness = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "chopiness");
    this->gl_size = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "size");

    this->gl_deepColor = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "deepColor");
    this->gl_shallowColor = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "shallowColor");
    this->gl_skyColor = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "skyColor");
    this->gl_lightDir = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "lightDir");

    return true;
}

void oceanFFT::createFBO() {
    glGenFramebuffers(1, &this->renderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->renderFBO);
    this->generateAttachmentTexture(false, false);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->renderTextureColorBuffer, 0);

    int screenWidth = Settings::Instance()->SDL_Window_Width;
    int screenHeight = Settings::Instance()->SDL_Window_Height;

    glGenRenderbuffers(1, &this->renderRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, this->renderRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->renderRBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Settings::Instance()->funcDoLog("[CudaExamples - oceanFFT 0] Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void oceanFFT::generateAttachmentTexture(GLboolean depth, GLboolean stencil) {
    GLenum attachment_type = GL_RGB;
    if (!depth && !stencil)
        attachment_type = GL_RGB;
    else if (depth && !stencil)
        attachment_type = GL_DEPTH_COMPONENT;
    else if (!depth && stencil)
        attachment_type = GL_STENCIL_INDEX;

    int screenWidth = Settings::Instance()->SDL_Window_Width;
    int screenHeight = Settings::Instance()->SDL_Window_Height;

    glGenTextures(1, &this->renderTextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, this->renderTextureColorBuffer);
    if (!depth && !stencil)
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(attachment_type), screenWidth, screenHeight, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screenWidth, screenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

oceanFFT::~oceanFFT() {
    cudaGraphicsUnregisterResource(this->cuda_heightVB_resource);
    cudaGraphicsUnregisterResource(this->cuda_slopeVB_resource);

    glDeleteBuffers(1, &this->posVertexBuffer);
    glDeleteBuffers(1, &this->heightVertexBuffer);
    glDeleteBuffers(1, &this->slopeVertexBuffer);

    cudaFree(this->d_h0);
    cudaFree(this->d_slope);
    cudaFree(this->d_ht);
    free(this->h_h0);
    if (this->fftPlan)
        cufftDestroy(this->fftPlan);

    GLint maxColorAttachments = 1;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    GLuint colorAttachment;
    GLenum att = GL_COLOR_ATTACHMENT0;
    for (colorAttachment = 0; colorAttachment < static_cast<GLuint>(maxColorAttachments); colorAttachment++) {
        att += colorAttachment;
        GLint param;
        GLuint objName;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &param);
        if (GL_RENDERBUFFER == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = reinterpret_cast<GLuint*>(&param)[0];
            glDeleteRenderbuffers(1, &objName);
        }
        else if (GL_TEXTURE == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = reinterpret_cast<GLuint*>(&param)[0];
            glDeleteTextures(1, &objName);
        }
    }

    glDeleteFramebuffers(1, &this->renderFBO);
    glDeleteRenderbuffers(1, &this->renderRBO);
}

#endif
