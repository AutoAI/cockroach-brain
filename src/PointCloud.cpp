// PointCloud.cpp

#include "PointCloud.hpp"

#include <stdlib.h>

#include "HeightMap.hpp"

PointCloud::PointCloud(size_t width, size_t height) {
	Width = width;
	Height = height;
	pc = new POINT3D[Width * Height];
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
	HeightMap* result = new HeightMap(int width, int depth);
	for(int i = 0; i < Width * Height; i++) {
		result.insert(pc[i]);
	}
	POINT3D* pc_temp = new POINT3D[Width * Height + width * depth];
	std::memcpy(pc_temp, pc, Width * Height * sizeof(POINT3D));
	int x;
	int z;
	for(int i = 0; i < width * depth; i++) {
		x = i % width;
		z = i / width;

		pc_temp[Width * Height + i].x = (x - VIEW_WIDTH / 2) * width / VIEW_WIDTH;
		pc_temp[Width * Height + i].z = z * depth / VIEW_DEPTH;
		pc_temp[Width * Height + i].y = HeightMap.heights / HEIGHTMAP_SCALE;

		pc_temp[Width * Height + i].r = (HeightMap.image[i] >> 24) / 255.9f;
		pc_temp[Width * Height + i].g = ((HeightMap.image[i] & 0xFF0000) >> 16) / 255.9f;
		pc_temp[Width * Height + i].b = ((HeightMap.image[i] & 0xFF00) >> 8) / 255.9f;
	}
	free(pc);
	pc = pc_temp;
}

POINT3D PointCloud::Point(size_t i, size_t j) {
	return pc[i + Width * j];
}

POINT3D PointCloud::Point(size_t i) {
	return pc[i];
}

size_t PointCloud::GetNbPoints() {
	return Width * Height;
}

size_t PointCloud::GetWidth() {
	return Width;
}

size_t PointCloud::GetHeight() {
	return Height;
}