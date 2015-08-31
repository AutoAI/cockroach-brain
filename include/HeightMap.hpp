// HeightMap.hpp
// Author: Travis Vanderstad

#pragma once

#define VIEW_DEPTH 10.0f
#define VIEW_WIDTH 10.0f
#define HEIGHTMAP_SCALE 10.0f

#include <stdint.h>

#include "utils.hpp"

class HeightMap {
public:
	HeightMap(int width, int depth);
	~HeightMap();
	void insert(POINT3D p);
	void sobel();

	int width;
	int depth;

	uint32_t* image;
	float* heights;
	char* frequencies;
};