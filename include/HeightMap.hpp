// HeightMap.hpp
// Author: Travis Vanderstad

#pragma once

#define VIEW_DEPTH 20.0f
#define VIEW_WIDTH 10.0f

#include <stdint.h>

#include "utils.hpp"

class HeightMap {
public:
	HeightMap(int width, int depth);
	~HeightMap();
	void clear();
	void insert(POINT3D p);
	POINT3D point(size_t i);
	size_t getNumPoints();
	void calcSobel(float threshold);

	int width;
	int depth;

	char* frequencies;
	bool* sobel;
	POINT3D* pc;
};