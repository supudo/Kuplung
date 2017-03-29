#
# Makefile for Kuplung Cuda (experimental)
# supudo.net, 2017
#
# All .cu files should be located in $(CUDA_FOLDER).
#

PROGRAM_NAME := KuplungCuda

NVCC := /usr/local/cuda/bin/nvcc
RM := rm -rf
NVCCFLAGS := -D_DEBUG --use_fast_math -L/usr/local/cuda/lib -lcuda -lcudart -lcufft --machine 64 -arch=compute_30 -c
CUDA_FOLDER := Kuplung/kuplung/cuda/cu/
CUDA_INCLUDE = -I /usr/local/cuda/include

CU_SOURCES += $(wildcard $(CUDA_FOLDER)*.cu)
CU_OBJS = ${CU_SOURCES:.cu=_cuda.o}

.PHONY: all clean distclean

# All Target
all: $(PROGRAM_NAME)

%_cuda.o: %.cu
	$(NVCC) $(CUDA_INCLUDE) -G -g -O0 -gencode arch=compute_30,code=sm_30  -odir "." -M -o $@ $<
	$(NVCC) $(CUDA_INCLUDE) -G -g -O0 --compile --relocatable-device-code=false -gencode arch=compute_30,code=compute_30 -gencode arch=compute_30,code=sm_30  -x cu -o  $@ $<


$(PROGRAM_NAME): $(CU_OBJS) 
	@ for cu_obj in $(subst $(CUDA_FOLDER), , $(CU_OBJS)); \
	do				\
		mv $(CUDA_FOLDER)/$$cu_obj external/cuda_build/$$cu_obj; \
	done

clean:
	@- $(RM) $(PROGRAM_NAME) $(CU_OBJS) *~ 

distclean: clean
