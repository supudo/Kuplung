//
//  CudaExamples.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef CudaExamples_hpp
#define CudaExamples_hpp

#ifdef DEF_KuplungSetting_UseCuda

#include <cuda.h>
#include <builtin_types.h>
#include <string>
#include <stdio.h>
#include "kuplung/cuda/examples/VectorAddition.hpp"
#include "kuplung/cuda/examples/oceanFFT.hpp"
#include "kuplung/utilities/gl/GLIncludes.h"
#include <glm/glm.hpp>

class CudaExamples {
public:
    ~CudaExamples();
    void init();
    void draw(bool* p_opened, glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid);

    std::unique_ptr<oceanFFT> exampleOceanFFT;

private:
    int selectedCudaExample;
    GLuint vboTexture;

    void renderExample(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixGrid);

    std::unique_ptr<VectorAddition> exampleVectorAddition;
};

#endif

#endif /* CudaExamples_hpp */
