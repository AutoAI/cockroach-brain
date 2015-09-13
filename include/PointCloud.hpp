// PointCloud.hpp

#pragma once

#include <zed/utils/GlobalDefine.hpp>

#include "HeightMap.hpp"
#include "utils.hpp"

class PointCloud {
public:
	PointCloud(size_t width, size_t height);
	~PointCloud();

	void fill(const unsigned char* image, const float* depth, const sl::zed::StereoParameters *param);
	HeightMap* genHeightMap(int width, int depth);

	POINT3D point(size_t i, size_t j);
	POINT3D point(size_t i);

	size_t getNumPoints();
	size_t getWidth();
	size_t getHeight();

private:
	POINT3D* pc;

	int width;
	int height;
};