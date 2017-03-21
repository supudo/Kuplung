#include <cuda.h>
#include <builtin_types.h>

extern "C"
__global__ void vectorAdditionCUDA(const float* a, const float* b, float* c, int n)
{
    int ii = blockDim.x * blockIdx.x + threadIdx.x;
    if (ii < n)
        c[ii] = a[ii] + b[ii];
}

void vectorAddition(const float* a, const float* b, float* c, int n) {
    float *a_cuda, *b_cuda, *c_cuda;
    unsigned int nBytes = sizeof(float) * n;
    int threadsPerBlock = 256;
    int blocksPerGrid   = (n + threadsPerBlock - 1) / threadsPerBlock;

    // allocate and copy memory into the device
    cudaMalloc((void **)& a_cuda, nBytes);
    cudaMalloc((void **)& b_cuda, nBytes);
    cudaMalloc((void **)& c_cuda, nBytes);
    cudaMemcpy(a_cuda, a, nBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(b_cuda, b, nBytes, cudaMemcpyHostToDevice);

    vectorAdditionCUDA<<<blocksPerGrid, threadsPerBlock>>>(a_cuda, b_cuda, c_cuda, n);

    // load the answer back into the host
    cudaMemcpy(c, c_cuda, nBytes, cudaMemcpyDeviceToHost);

    cudaFree(a_cuda);
    cudaFree(b_cuda);
    cudaFree(c_cuda);
}
