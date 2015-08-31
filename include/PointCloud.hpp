// PointCloud.hpp

#pragma once

#include <zed/utils/GlobalDefine.hpp>
#include "utils.hpp"

class PointCloud {
public:

	PointCloud();
	PointCloud(size_t width, size_t height);
	virtual ~PointCloud();

	void fill(const unsigned char* image, const float* depth, const sl::zed::StereoParameters *param);
	void genHeightMap(int width, int depth);

	POINT3D Point(size_t i, size_t j);
	POINT3D Point(size_t i);
	POINT3D PointHM(size_t i);

	size_t GetNbPoints();
	size_t GetNbPointsHM();
	size_t GetWidth();
	size_t GetHeight();

private:
	POINT3D* pc;
	POINT3D* hmpc;

	int Width;
	int Height;
	int NbPointsHM;
};