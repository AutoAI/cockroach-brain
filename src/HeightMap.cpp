// HeightMap.cpp
// Author: Travis Vanderstad

#include "HeightMap.hpp"

HeightMap::HeightMap(int width, int depth) {
	this -> width = width;
	this -> depth = depth;

	image = new uint32_t[width * depth];
	heights = new float[width * depth];
	frequencies = new char[width * depth];

	for(int i = 0; i < width * depth; i++) {
		image[i] = 0;
		heights[i] = 0;
		frequencies[i] = 0;
	}
}

HeightMap::~HeightMap() {
	free(heights);
	free(frequencies);
}

void HeightMap::insert(POINT3D p) {
	int z_index = (int)(p.z * VIEW_DEPTH / depth);
	if(z_index < 0 || z_index >= depth) {
		return;
	}
	int x_index = (int)(p.x * VIEW_WIDTH / width + VIEW_WIDTH / 2);
	if(x_index < 0 || x_index >= width) {
		return;
	}
	int index = z_index * width + x_index;

	// update height - height for a cell is always max point height
	if(p.y > heights[index]) {
		heights[index] = p.y;
	}

	// grab color, unpack it into color components
	uint32_t pixel = image[index];
	uint8_t red = (uint8_t)(pixel >> 24);
	uint8_t grn = (uint8_t)((pixel & 0xFF0000) >> 16);
	uint8_t blu = (uint8_t)((pixel & 0xFF00) >> 8);

	// add color into the cumulative average (p.color is float in range [0, 1])
	red = (red * frequencies[index] + p.r * 255.9f) / (frequencies[index] + 1);
	grn = (grn * frequencies[index] + p.g * 255.9f) / (frequencies[index] + 1);
	blu = (blu * frequencies[index] + p.b * 255.9f) / (frequencies[index] + 1);
	frequencies[index]++;

	// put that color back where it came from
	uint32_t red32 = red << 24;
	uint32_t grn32 = grn << 16;
	uint32_t blu32 = blu << 8;
	image[index] = red32 | grn32 | blu32;
}

void HeightMap::sobel() {

}