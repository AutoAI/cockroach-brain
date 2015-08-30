PROJECT_NAME = auto-baja

NVCC = nvcc
CC = g++

CUDAPATH = /usr/loca/cuda-6.5/

BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

GCCFLAGS = -c -m32 -std=c++11 -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
NVCCFLAGS = -c -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
LFLAGS = -m32 -L$(CUDAPATH)/lib -lcuda -lcudart -lm

all: build clean

build: build_dir src_dir gpu cpu
	$(CC) $(LFLAGS) -o $(BUILD_DIR)/$(PROJECT_NAME) *.o

build_dir:
	mkdir -p $(BUILD_DIR)

src_dir:
	mkdir -p $(SRC_DIR)

gpu:
	$(NVCC) $(NVCCFLAGS) $(SRC_DIR)/*.cu

cpu:
	$(CC) $(GCCFLAGS) $(SRC_DIR)/*.cpp

clean:
	rm *.o

run:
	./$(BUILD_DIR)/$(PROJECT_NAME)