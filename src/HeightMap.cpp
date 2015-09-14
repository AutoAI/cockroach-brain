// HeightMap.cpp
// Author: Travis Vanderstad

#include "HeightMap.hpp"

#include <math.h>
#include <iostream>

HeightMap::HeightMap(int width, int depth) {
	this->width = width;
	this->depth = depth;

	frequencies = new char[width * depth];
	sobel = new bool[width * depth];
	pc = new POINT3D[width * depth];

	// preprocess pc
	clear();
}

HeightMap::~HeightMap() {
	delete frequencies;
	delete sobel;
	delete pc;
}

void HeightMap::clear() {
	for(int i = 0; i < width * depth; i++) {
		// clear color values
		pc[i].r = 0;
		pc[i].g = 0;
		pc[i].b = 0;
		// x and z determined upon retrieval
		pc[i].y = 0;
		// clear counter for number of points affecting this cell
		frequencies[i] = 0;
	}
}

void HeightMap::insert(POINT3D p) {
	// make sure the point doesn't lie outside the map
	int z_index = (int)(p.z * depth / VIEW_DEPTH);
	if(z_index < 0 || z_index >= depth) {
		return;
	}
	if(p.x > 0) {
		return;
	}
	int x_index = (int)(p.x * width / VIEW_WIDTH + width / 2);
	if(x_index < 0 || x_index >= width) {
		return;
	}
	int i = z_index * width + x_index;

	// update height - height for a cell is always max point height
	if(p.y > pc[i].y) {
		pc[i].y = p.y;
	}

	// add color into the cumulative average
	pc[i].r = (pc[i].r * frequencies[i] + p.r) / (frequencies[i] + 1);
	pc[i].g = (pc[i].g * frequencies[i] + p.g) / (frequencies[i] + 1);
	pc[i].b = (pc[i].b * frequencies[i] + p.b) / (frequencies[i] + 1);
	frequencies[i]++;
}

POINT3D HeightMap::point(size_t i) {
	pc[i].x = ((i % width) - width / 2) * VIEW_WIDTH / width;
	pc[i].z = (i / width) * VIEW_DEPTH / depth;
	return pc[i];
}

size_t HeightMap::getNumPoints() {
	return width * depth;
}

// note: good candidate for CUDA acceleration
void HeightMap::calcSobel(float threshold) {
	int x, z;
	float a, b, c, d, f, g, h, i;
	float tempX, tempZ;
	for(int i = 0; i < width * depth; i++) {
		x = i % width;
		z = i / width;
		if(x == 0 || x == width - 1 || z == 0 || z == depth - 1) {
			sobel[i] = false;
			continue;
		}
		a = pc[i - width - 1].y;
		b = pc[i - width].y;
		c = pc[i - width + 1].y;
		d = pc[i - 1].y;
		f = pc[i + 1].y;
		g = pc[i + width - 1].y;
		h = pc[i + width].y;
		i = pc[i + width + 1].y;

		// use the sobel-feldman operator (3-10-3)
		tempX = (g * 3 + h * 10 + i * 3) - (a * 3 + b * 10 + c * 3);
		tempZ = (c * 3 + f * 10 + i * 3) - (a * 3 + d * 10 + g * 3);
		sobel[i] = sqrt(tempX * tempX + tempZ * tempZ) < threshold;
	}
}