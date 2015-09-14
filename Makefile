# Makefile for auto-baja, an open-source autonomy system for the baja go kart
# Author: Travis Vanderstad

PROJECT_NAME = auto-baja

NVCC = nvcc
CC = g++

CUDAPATH = /usr/local/cuda-6.5
ZEDPATH = /usr/local/zed

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

CCFLAGS = -c -g -std=c++11 -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
NVCCFLAGS = -c -g -std=c++11 -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
LFLAGS = -lGL -lGLU -lglut -L$(CUDAPATH)/lib -lcuda -lcudart -lm -pthread -L$(ZEDPATH)/lib -lsl_zed -lsl_depthcore -lsl_calibration -lcudpp -lcudpp_hash -rdynamic -lnppc -lnpps -lnppi -lSM -lICE -lX11 -lXext -lXmu -lXi

OBJECTS = main.o CloudViewer.o PointCloud.o HeightMap.o

vpath %.cpp $(SRC_DIR)
vpath %.cu $(SRC_DIR)
vpath %.hpp $(INCLUDE_DIR)
vpath %.o $(BUILD_DIR)

# If the first argument is "run"...
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

$(BIN_DIR)/$(PROJECT_NAME): $(OBJECTS)
	@echo "Linking..."
	@$(CC) $(BUILD_DIR)/*.o -o $(BIN_DIR)/$(PROJECT_NAME) $(LFLAGS)

main.o: main.cpp PointCloud.hpp CloudViewer.hpp HeightMap.hpp
	@echo "Compiling main..."
	@$(CC) $(CCFLAGS) $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

CloudViewer.o: CloudViewer.cpp CloudViewer.hpp PointCloud.hpp HeightMap.hpp utils.hpp
	@echo "Compiling CloudViewer..."
	@$(CC) $(CCFLAGS) $(SRC_DIR)/CloudViewer.cpp -o $(BUILD_DIR)/CloudViewer.o

PointCloud.o: PointCloud.cpp PointCloud.hpp utils.hpp HeightMap.hpp
	@echo "Compiling PointCloud..."
	@$(CC) $(CCFLAGS) $(SRC_DIR)/PointCloud.cpp -o $(BUILD_DIR)/PointCloud.o

HeightMap.o: HeightMap.cpp HeightMap.hpp utils.hpp
	@echo "Compiling HeightMap..."
	@$(CC) $(CCFLAGS) $(SRC_DIR)/HeightMap.cpp -o $(BUILD_DIR)/HeightMap.o

clean:
	@rm $(BUILD_DIR)/*.o
	@rm $(BIN_DIR)/*

run:
	@./$(BIN_DIR)/$(PROJECT_NAME) $(RUN_ARGS)