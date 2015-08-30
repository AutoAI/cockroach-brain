PROJECT_NAME = auto-baja

NVCC = nvcc
CC = g++

CUDAPATH = /usr/local/cuda-6.5
ZEDPATH = /usr/local/zed

BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

GCCFLAGS = -c -std=c++11 -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
NVCCFLAGS = -c -std=c++11 -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
LFLAGS = -lGL -lGLU -lglut -L$(CUDAPATH)/lib -lcuda -lcudart -lm -pthread -L$(ZEDPATH)/lib -lsl_zed -lsl_depthcore -lsl_calibration -lcudpp -lcudpp_hash-rdynamic -lnpps -lnppi -lSM -lICE -lX11 -lXext -lXmu -lXi

all: build clean

build: build_dir src_dir gpu cpu
	$(CC) *.o -o $(BUILD_DIR)/$(PROJECT_NAME) $(LFLAGS)

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