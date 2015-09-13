// PointCloud.cpp

#include "PointCloud.hpp"

#include <iostream>
#include <stdlib.h>

#include "heightMap.hpp"

PointCloud::PointCloud(size_t width, size_t height) {
	width = width;
	height = height;
	pc = new POINT3D[width * height];
}

PointCloud::~PointCloud() {
	delete pc;
}

void PointCloud::fill(const unsigned char* image, const float* depth_map, const sl::zed::StereoParameters *param) {
	float depth;
	float cx = param->LeftCam.cx;
	float cy = param->LeftCam.cy;
	float fx = param->LeftCam.fx;
	float fy = param->LeftCam.fy;

	int index = 0;
	for(int j = 0; j < height; j++) {
		for(int i = 0; i < width; i++) {
			pc[index].setColor(&image[j * (width * 4) + i * 4]);
			depth = depth_map[j * width + i];
			depth /= 1000.0f; // convert to meters

			pc[index].z = depth;
			pc[index].x = ((i - cx) * depth) / fx;
			pc[index].y = ((j - cy) * depth) / fy;
			index++;
		}
	}
}

HeightMap* PointCloud::genHeightMap(int width, int depth) {
	HeightMap* hm = new HeightMap(width, depth);
	for(int i = 0; i < width * height; i++) {
		hm->insert(pc[i]);
	}

	for(int i = 0; i < width * depth; i++) {
		if(hm->frequencies[i] == 0) {
			continue;
		}

		// just trust me on these ones
		hmpc[i].x = ((i % width) - width / 2) * VIEW_WIDTH / width;
		hmpc[i].z = (i / width) * VIEW_DEPTH / depth;
		hmpc[i].y = hm->heights[i];

		hmpc[i].r = hm->red[i];
		hmpc[i].g = hm->grn[i];
		hmpc[i].b = hm->blu[i];
	}
	return hm;
}

POINT3D PointCloud::point(size_t i, size_t j) {
	return pc[i + width * j];
}

POINT3D PointCloud::point(size_t i) {
	return pc[i];
}

size_t PointCloud::getNumPoints() {
	return width * height;
}

size_t PointCloud::width() {
	return width;
}

size_t PointCloud::height() {
	return height;
}