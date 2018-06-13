#!/bin/sh
set -e

CudaResult=$(make -f KuplungCuda.make USE_CUDA=1)

if [ $CudaResult == "false" ]
then
  echo "Nvidia Cuda found."
  export DEF_KuplungSetting_UseCuda=1
else
  echo "Nvidia Cuda not found!"
fi
