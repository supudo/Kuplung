//
//  cuda_OBJParser.cu
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include <cuda.h>
#include <cuda_runtime.h>
#include <builtin_types.h>

extern "C"
__global__ void cudaParseOBJFile(int* obj_data,
                                 float *vertices,
                                 float *textureCoordinates,
                                 float *normals,
                                 float *indices
) {
    const unsigned int tid = threadIdx.x;
    int data = obj_data[tid];
}

void parseOBJFile(const char* obj_file_contents,
                  int obj_file_content_length,
                  float *vertices,
                  float *textureCoordinates,
                  float *normals,
                  float *indices
) {
    float *cuda_vertices, *cuda_textureCoordinates, *cuda_normals, *cuda_indices;
    const unsigned int num_threads = obj_file_content_length / 4;
    const unsigned int mem_size = sizeof(char) * obj_file_content_length;

    char *d_data;
    cudaMalloc((void **) &d_data, mem_size);
    cudaMemcpy(d_data, obj_file_contents, mem_size, cudaMemcpyHostToDevice);

    dim3 grid(1, 1, 1);
    dim3 threads(num_threads, 1, 1);
    // execute the kernel
    cudaParseOBJFile<<< grid, threads >>>((int *) d_data, vertices, textureCoordinates, normals, indices);

    cudaFree(d_data);
    cudaFree(cuda_vertices);
    cudaFree(cuda_textureCoordinates);
    cudaFree(cuda_normals);
    cudaFree(cuda_indices);
}
