// PointCloud.cpp

#include "PointCloud.hpp"

#include <iostream>
#include <stdlib.h>

PointCloud::PointCloud(size_t width, size_t height) {
	this->width = width;
	this->height = height;
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

	printf("height = %d, width = %d\n", height, width);

	int index = 0;
	for(int j = 0; j < height; j++) {
		for(int i = 0; i < width; i++) {
			pc[index].setColor(&image[j * (width * 4) + i * 4]);
			depth = depth_map[j * width + i];
			depth /= 1000.0f; // convert to meters

			pc[index].z = depth;
			pc[index].x = ((i - cx) * depth) / fx;
			pc[index].y = ((j - cy) * depth) / fy;

			printf("fill:   pc[%d] = (%f, %f, %f)\n", index, pc[index].x, pc[index].y, pc[index].z);
			index++;
		}
	}
}

HeightMap* PointCloud::fillHeightMap(HeightMap *hm) {
	hm->clear();
	for(int i = 0; i < width * height; i++) {
		// printf("fillhm: pc[%d] = (%f, %f, %f)\n", i, pc[i].x, pc[i].y, pc[i].z);
		hm->insert(pc[i]);
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

size_t PointCloud::getWidth() {
	return width;
}

size_t PointCloud::getHeight() {
	return height;
}