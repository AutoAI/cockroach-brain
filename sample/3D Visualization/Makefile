PROJECT_NAME = auto-baja

NVCC = nvcc
CC = g++

CUDAPATH = /usr/loca/cuda-6.5/

BUILD_DIR = build

GCCFLAGS = -c -m32 -I$(CUDAPATH)/include
NVCCFLAGS = -c -I$(CUDAPATH)/include
LFLAGS = -m32 -L$(CUDAPATH)/lib -lcuda -lcudart -lm

all: build clean

build: build_dir gpu cpu
	$(CC) $(LFLAGS) -o $(BUILD_DIR)/$(PROJECT_NAME) *.o

build_dir:
	mkdir -p $(BUILD_DIR)

gpu:
	$(NVCC) $(NVCCFLAGS) *.cu

cpu:
	$(CC) $(GCCFLAGS) *.cpp

clean:
	rm *.o

run:
	./$(BUILD_DIR)/$(PROJECT_NAME)