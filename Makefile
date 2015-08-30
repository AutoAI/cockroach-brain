PROJECT_NAME = auto-baja

NVCC = nvcc
CC = g++

CUDAPATH = /usr/local/cuda-6.5
ZEDPATH = /usr/local/zed

BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

CCFLAGS = -c -std=c++11 -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
NVCCFLAGS = -c -std=c++11 -I$(CUDAPATH)/include -I$(INCLUDE_DIR)
LFLAGS = -lGL -lGLU -lglut -L$(CUDAPATH)/lib -lcuda -lcudart -lm -pthread -L$(ZEDPATH)/lib -lsl_zed -lsl_depthcore -lsl_calibration -lcudpp -lcudpp_hash -rdynamic -lnppc -lnpps -lnppi -lSM -lICE -lX11 -lXext -lXmu -lXi

OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/CloudViewer.o $(BUILD_DIR)/PointCloud.o

$(PROJECT_NAME): $(OBJECTS)
	$(CC) $(BUILD_DIR)/*.o -o $(BUILD_DIR)/$(PROJECT_NAME) $(LFLAGS)

$(BUILD_DIR)/main.o:
	$(NVCC) $(NVCCFLAGS) $(SRC_DIR)/main.cu -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/CloudViewer.o:
	$(CC) $(CCFLAGS) $(SRC_DIR)/CloudViewer.cpp -o $(BUILD_DIR)/CloudViewer.o

$(BUILD_DIR)/PointCloud.o:
	$(CC) $(CCFLAGS) $(SRC_DIR)/PointCloud.cpp -o $(BUILD_DIR)/PointCloud.o

clean:
	rm $(BUILD_DIR)/*.o

run:
	./$(BIN_DIR)/$(PROJECT_NAME)