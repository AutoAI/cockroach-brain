// PointCloud.cpp

#include "PointCloud.hpp"

#include <iostream>
#include <stdlib.h>

#include "HeightMap.hpp"

PointCloud::PointCloud(size_t width, size_t height) {
	Width = width;
	Height = height;
	pc = new POINT3D[Width * Height];
	hmpc = NULL;
}

PointCloud::~PointCloud() {
	free(pc);
}

void PointCloud::fill(const unsigned char* image, const float* depth_map, const sl::zed::StereoParameters *param) {
	float depth;
	float cx = param->LeftCam.cx;
	float cy = param->LeftCam.cy;
	float fx = param->LeftCam.fx;
	float fy = param->LeftCam.fy;

	int index = 0;
	for(int j = 0; j < Height; j++) {
		for(int i = 0; i < Width; i++) {
			pc[index].setColor(&image[j * (Width * 4) + i * 4]);
			depth = depth_map[j * Width + i];
			depth /= 1000.0f; // convert to meters

			pc[index].z = depth;
			pc[index].x = ((i - cx) * depth) / fx;
			pc[index].y = ((j - cy) * depth) / fy;
			index++;
		}
	}
}

void PointCloud::genHeightMap(int width, int depth) {
	HeightMap* hm = new HeightMap(width, depth);
	for(int i = 0; i < Width * Height; i++) {
		hm->insert(pc[i]);
	}
	if(hmpc == NULL) {
		hmpc = new POINT3D[width * depth];
	}

	NbPointsHM = 0;
	int x;
	int z;

	int nonZeroCount = 0;

	for(int i = 0; i < width * depth; i++) {
		if(hm->frequencies[i] == 0) {
			continue;
		}
		nonZeroCount++;

		x = i % width;
		z = i / width;

		hmpc[i].x = (x - width / 2) * VIEW_WIDTH / width;
		hmpc[i].z = z * VIEW_DEPTH / depth;
		hmpc[i].y = hm->heights[i] / HEIGHTMAP_SCALE;

		hmpc[i].r = hm->red[i];
		hmpc[i].g = hm->grn[i];
		hmpc[i].b = hm->blu[i];

		NbPointsHM++;
	}
	std::cout << "nonZeroCount: " << nonZeroCount << std::endl;
}

POINT3D PointCloud::Point(size_t i, size_t j) {
	return pc[i + Width * j];
}

POINT3D PointCloud::Point(size_t i) {
	return pc[i];
}

POINT3D PointCloud::PointHM(size_t i) {
	return hmpc[i];
}

size_t PointCloud::GetNbPoints() {
	return Width * Height;
}

size_t PointCloud::GetNbPointsHM() {
	return NbPointsHM;
}

size_t PointCloud::GetWidth() {
	return Width;
}

size_t PointCloud::GetHeight() {
	return Height;
}