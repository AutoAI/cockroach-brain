// HeightMap.cpp
// Author: Travis Vanderstad

#include "HeightMap.hpp"

HeightMap::HeightMap(int width, int depth) {
	this -> width = width;
	this -> depth = depth;

	red = new float[width * depth];
	grn = new float[width * depth];
	blu = new float[width * depth];
	heights = new float[width * depth];
	frequencies = new char[width * depth];

	for(int i = 0; i < width * depth; i++) {
		red[i] = 0;
		grn[i] = 0;
		blu[i] = 0;
		heights[i] = 0;
		frequencies[i] = 0;
	}
}

HeightMap::~HeightMap() {
	delete red;
	delete grn;
	delete blu;
	delete heights;
	delete frequencies;
}

void HeightMap::insert(POINT3D p) {
	int z_index = (int)(p.z * depth / VIEW_DEPTH);
	if(z_index < 0 || z_index >= depth) {
		return;
	}
	int x_index = (int)(p.x * width / VIEW_WIDTH + width / 2);
	if(x_index < 0 || x_index >= width) {
		return;
	}
	int index = z_index * width + x_index;

	// update height - height for a cell is always max point height
	if(p.y > heights[index]) {
		heights[index] = p.y;
	}

	// add color into the cumulative average
	red[index] = (red[index] * frequencies[index] + p.r) / (frequencies[index] + 1);
	grn[index] = (grn[index] * frequencies[index] + p.g) / (frequencies[index] + 1);
	blu[index] = (blu[index] * frequencies[index] + p.b) / (frequencies[index] + 1);
	frequencies[index]++;
}

void HeightMap::sobel() {

}