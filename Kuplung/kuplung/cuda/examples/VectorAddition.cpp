//
//  VectorAddition.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifdef DEF_KuplungSetting_UseCuda

#include "VectorAddition.hpp"
#include "kuplung/settings/Settings.h"

VectorAddition::~VectorAddition() {
}

void VectorAddition::init() {
}

// Forward declare the function in the .cu file
void vectorAddition(const float* a, const float* b, float* c, int n);

void VectorAddition::doVectorAddition() {
    int deviceCount = 0;
    int cudaDevice = 0;
    char cudaDeviceName [100];
    unsigned int N = 50;
    float *a, *b, *c;

    cuInit(0);
    cuDeviceGetCount(&deviceCount);
    cuDeviceGet(&cudaDevice, 0);
    cuDeviceGetName(cudaDeviceName, 100, cudaDevice);
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[CudaExamples - VectorAddition] Number of devices: %i\n", deviceCount));
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[CudaExamples - VectorAddition] Device name: %s\n", cudaDeviceName));

    a = new float [N];
    b = new float [N];
    c = new float [N];
    srand(time(0));
    for (unsigned int ii = 0; ii < N; ++ii) {
        a[ii] = ((rand() % 100) / 100.0f);
        b[ii] = ((rand() % 100) / 100.0f);
    }

    // This is the function call in which the kernel is called
    vectorAddition(a, b, c, N);

    std::string s = this->printArray(a, N);
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[CudaExamples - VectorAddition] input a : %s\n", s.c_str()));
    s = this->printArray(b, N);
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[CudaExamples - VectorAddition] input b : %s\n", s.c_str()));
    s = this->printArray(c, N);
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[CudaExamples - VectorAddition] output c : %s\n", s.c_str()));

    if (a) delete a;
    if (b) delete b;
    if (c) delete c;
}

std::string VectorAddition::printArray(const float* a, const unsigned int n) {
    std::string s = "(";
    unsigned int ii;
    for (ii = 0; ii < n - 1; ++ii)
        s += std::to_string(a[ii]) + ", ";
    s += std::to_string(a[ii]) + ")";
    return s;
}

#endif
