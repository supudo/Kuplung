//
//  CudaHelpers.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifdef DEF_KuplungSetting_UseCuda

#ifndef CudaHelpers_hpp
#define CudaHelpers_hpp

#include "kuplung/settings/Settings.h"

#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <cufft.h>
#include <math_constants.h>

class CudaHelpers {
public:
    int findCudaDevice();
    int findCudaGLDevice();

private:
    int gpuGetMaxGflopsDeviceId();
    int _ConvertSMVer2Cores(int major, int minor);
};

#endif /* CudaHelpers_hpp */

#endif
