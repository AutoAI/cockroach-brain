PROJECT_NAME = auto-baja

NVCC = nvcc
CC = g++

CUDAPATH = /usr/local/cuda-6.5
ZEDPATH = /usr/local/zed

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

CCFLAGS = -c -std=c++11 -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
NVCCFLAGS = -c -std=c++11 -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
LFLAGS = -lGL -lGLU -lglut -L$(CUDAPATH)/lib -lcuda -lcudart -lm -pthread -L$(ZEDPATH)/lib -lsl_zed -lsl_depthcore -lsl_calibration -lcudpp -lcudpp_hash -rdynamic -lnppc -lnpps -lnppi -lSM -lICE -lX11 -lXext -lXmu -lXi

OBJECTS = main.o CloudViewer.o PointCloud.o

vpath %.cpp $(SRC_DIR)
vpath %.cu $(SRC_DIR)
vpath %.h $(INCLUDE_DIR)
vpath %.o $(BUILD_DIR)

$(BIN_DIR)/$(PROJECT_NAME): $(OBJECTS)
	@echo "Linking..."
	@$(CC) *.o -o $(BIN_DIR)/$(PROJECT_NAME) $(LFLAGS)

main.o: main.cu
	@echo "Compiling main..."
	@$(NVCC) $(NVCCFLAGS) main.cu -o $(BUILD_DIR)/main.o

CloudViewer.o: CloudViewer.cpp
	@echo "Compiling CloudViewer..."
	@$(CC) $(CCFLAGS) CloudViewer.cpp -o $(BUILD_DIR)/CloudViewer.o

PointCloud.o: PointCloud.cpp
	@echo "Compiling PointCloud..."
	@$(CC) $(CCFLAGS) PointCloud.cpp -o $(BUILD_DIR)/PointCloud.o

clean:
	@rm $(BUILD_DIR)/*.o
	@rm $(BIN_DIR)/*

run:
	@./$(BIN_DIR)/$(PROJECT_NAME)