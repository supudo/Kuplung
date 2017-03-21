//
//  VectorAddition.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifdef DEF_KuplungSetting_UseCuda

#ifndef VectorAddition_hpp
#define VectorAddition_hpp

#include <cuda.h>
#include <builtin_types.h>
#include <string>
#include <stdio.h>

class VectorAddition {
public:
    ~VectorAddition();
    void init();
    void doVectorAddition();

private:
    std::string printArray(const float* a, const unsigned int n);
};

#endif /* VectorAddition_hpp */

#endif
