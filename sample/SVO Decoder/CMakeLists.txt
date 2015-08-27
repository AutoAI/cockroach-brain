SET(execName ZED_SVO_Decoder)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 OLD)
	cmake_policy(SET CMP0015 OLD)  
endif(COMMAND cmake_policy)

SET(EXECUTABLE_OUTPUT_PATH ".")

message("Boost program_options module is required to build this sample")

IF(WIN32) # Windows
SET(ZED_INCLUDE_DIRS $ENV{ZED_INCLUDE_DIRS})
	if (CMAKE_CL_64) # 64 bits
		SET(ZED_LIBRARIES $ENV{ZED_LIBRARIES_64})
		SET(Boost_LIBRARY_DIRS $ENV{BOOST_LIBRARY_DIR_64})
	else(CMAKE_CL_64) # 32 bits
		SET(ZED_LIBRARIES $ENV{ZED_LIBRARIES_32})
		SET(Boost_LIBRARY_DIRS $ENV{BOOST_LIBRARY_DIR_32})
	endif(CMAKE_CL_64)
SET(ZED_LIBRARY_DIR $ENV{ZED_LIBRARY_DIR})
SET(Boost_INCLUDE_DIRS $ENV{BOOST_INCLUDE_DIRS})
SET(OPENCV_DIR $ENV{OPENCV_DIR})
ELSE() # Linux
SET(ZED_DIR "/usr/local/zed/")
find_package(ZED 1.0 REQUIRED)
find_package(Boost 1.5 COMPONENTS program_options REQUIRED)
ENDIF(WIN32)

find_package(CUDA 6.5 EXACT REQUIRED)
find_package(OpenCV 2.4 COMPONENTS core highgui imgproc REQUIRED)

include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${ZED_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${ZED_LIBRARY_DIR})
link_directories(${CUDA_LIBRARY_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

SET(SRC_FOLDER src)
FILE(GLOB_RECURSE SRC_FILES "${SRC_FOLDER}/*.cpp")
 
ADD_EXECUTABLE(${execName} ${SRC_FILES})
set_property(TARGET ${execName} PROPERTY OUTPUT_NAME "ZED SVO Decoder")

add_definitions(-std=c++0x)# -m64) #-Wall)

# Add the required libraries for linking:
TARGET_LINK_LIBRARIES(${execName}
                        ${ZED_LIBRARIES}
                        ${CUDA_LIBRARIES}	
                        ${OpenCV_LIBS}  
                        nppi
			npps
                        ${Boost_LIBRARIES}
                    )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3" ) # Release Perf mode 
