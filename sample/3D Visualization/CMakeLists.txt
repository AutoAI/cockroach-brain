SET(execName ZED_3D_Visualization)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 OLD)
	cmake_policy(SET CMP0015 OLD)  
endif(COMMAND cmake_policy)

SET(EXECUTABLE_OUTPUT_PATH ".")
SET(LINUX_ONLY_LIBRARIES "")
IF(WIN32) # Windows
SET(ZED_INCLUDE_DIRS $ENV{ZED_INCLUDE_DIRS})
	if (CMAKE_CL_64) # 64 bits
		SET(ZED_LIBRARIES $ENV{ZED_LIBRARIES_64})
	else(CMAKE_CL_64) # 32 bits
		SET(ZED_LIBRARIES $ENV{ZED_LIBRARIES_32})
		SET(GLUT_LIBRARY_DIRS $ENV{GLUT_LIBRARY_DIRS})
		SET(GLUT_LIBRARY $ENV{GLUT_LIBRARY})
	endif(CMAKE_CL_64)
SET(ZED_LIBRARY_DIR $ENV{ZED_LIBRARY_DIR})
SET(GLUT_INCLUDE_DIRS $ENV{GLUT_INCLUDE_DIRS})
ELSE() # Linux
SET(ZED_DIR "/usr/local/zed/")
find_package(ZED 1.0 REQUIRED)
find_package(GLUT)
SET(LINUX_ONLY_LIBRARIES pthread npps nppi)
ENDIF(WIN32)

# Mandatory
find_package(CUDA 6.5 EXACT REQUIRED)
find_package(OpenGL)

include_directories(${ZED_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${ZED_LIBRARY_DIR})
link_directories(${GLUT_LIBRARY_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

SET(SRC_FOLDER src)
FILE(GLOB_RECURSE SRC_FILES "${SRC_FOLDER}/*.cpp")
 
ADD_EXECUTABLE(${execName} ${SRC_FILES})
set_property(TARGET ${execName} PROPERTY OUTPUT_NAME "ZED 3D Visualization")
add_definitions(-std=c++0x)# -m64)

# Add the required libraries for linking:
TARGET_LINK_LIBRARIES(${execName}
			${ZED_LIBRARIES}
			${LINUX_ONLY_LIBRARIES}
			${OPENGL_LIBRARIES}
                        ${GLUT_LIBRARY} 
                        ${CUDA_LIBRARIES}	
                    )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3" ) # Release Perf mode
