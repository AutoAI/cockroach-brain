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
	if(z_index <= 0 || z_index >= depth) {
		return;
	}
	int x_index = (int)(p.x * width / VIEW_WIDTH + width / 2);
	if(x_index <= 0 || x_index >= width) {
		return;
	}
	int i1 = z_index * width + x_index;
	int i2 = z_index * width + x_index - 1;
	int i3 = (z_index - 1) * width + x_index;
	int i4 = (z_index - 1) * width + x_index - 1;

	// update height - height for a cell is always max point height
	if(p.y > pc[i1].y) {
		pc[i1].y = p.y;
	}
	if(p.y > pc[i2].y) {
		pc[i2].y = p.y;
	}
	if(p.y > pc[i3].y) {
		pc[i3].y = p.y;
	}
	if(p.y > pc[i4].y) {
		pc[i4].y = p.y;
	}

	// add color into the cumulative average
	pc[i1].r = (pc[i1].r * frequencies[i1] + p.r) / (frequencies[i1] + 1);
	pc[i1].g = (pc[i1].g * frequencies[i1] + p.g) / (frequencies[i1] + 1);
	pc[i1].b = (pc[i1].b * frequencies[i1] + p.b) / (frequencies[i1] + 1);

	pc[i2].r = (pc[i2].r * frequencies[i2] + p.r) / (frequencies[i2] + 1);
	pc[i2].g = (pc[i2].g * frequencies[i2] + p.g) / (frequencies[i2] + 1);
	pc[i2].b = (pc[i2].b * frequencies[i2] + p.b) / (frequencies[i2] + 1);

	pc[i3].r = (pc[i3].r * frequencies[i3] + p.r) / (frequencies[i3] + 1);
	pc[i3].g = (pc[i3].g * frequencies[i3] + p.g) / (frequencies[i3] + 1);
	pc[i3].b = (pc[i3].b * frequencies[i3] + p.b) / (frequencies[i3] + 1);

	pc[i4].r = (pc[i4].r * frequencies[i4] + p.r) / (frequencies[i4] + 1);
	pc[i4].g = (pc[i4].g * frequencies[i4] + p.g) / (frequencies[i4] + 1);
	pc[i4].b = (pc[i4].b * frequencies[i4] + p.b) / (frequencies[i4] + 1);

	frequencies[i1]++;
	frequencies[i2]++;
	frequencies[i3]++;
	frequencies[i4]++;
}

POINT3D HeightMap::point(size_t i) {
	pc[i].x = (((float(i % width) - float(width / 2)) * VIEW_WIDTH) / width);
	pc[i].z = (i / width) * VIEW_DEPTH / depth;
	return pc[i];
}

size_t HeightMap::getNumPoints() {
	return width * depth;
}

// note: good candidate for CUDA acceleration
void HeightMap::calcSobel(float threshold) {
	int x, z;
	float a, b, c, d, f, g, h, j;
	/*
	sobel on:
	a b c
	d e f
	g h j
	*/
	float tempX, tempZ;
	for(int i = 0; i < width * depth; i++) {
		x = i % width;
		z = i / width;
		if(x == 0 || x == width - 1 || z == 0 || z == depth - 1) {
			sobel[i] = false;
			continue;
		}

		if(frequencies[i - width - 1] == 0) {
			sobel[i] = true;
			continue;
		}
		if(frequencies[i - width] == 0) {
			sobel[i] = true;
			continue;
		}
		if(frequencies[i - width + 1] == 0) {
			sobel[i] = true;
			continue;
		}
		if(frequencies[i - 1] == 0) {
			sobel[i] = true;
			continue;
		}
		if(frequencies[i + 1] == 0) {
			sobel[i] = true;
			continue;
		}
		if(frequencies[i + width - 1] == 0) {
			sobel[i] = true;
			continue;
		}
		if(frequencies[i + width] == 0) {
			sobel[i] = true;
			continue;
		}
		if(frequencies[i + width + 1] == 0) {
			sobel[i] = true;
			continue;
		}
		a = pc[i - width - 1].y;
		b = pc[i - width].y;
		c = pc[i - width + 1].y;
		d = pc[i - 1].y;
		f = pc[i + 1].y;
		g = pc[i + width - 1].y;
		h = pc[i + width].y;
		j = pc[i + width + 1].y;

		// use the sobel-feldman operator (3-10-3)
		tempX = (g * 3 + h * 10 + j * 3) - (a * 3 + b * 10 + c * 3);
		tempZ = (c * 3 + f * 10 + j * 3) - (a * 3 + d * 10 + g * 3);
		sobel[i] = sqrt(tempX * tempX + tempZ * tempZ) < threshold;
	}
}
