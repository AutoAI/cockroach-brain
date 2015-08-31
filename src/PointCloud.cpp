// PointCloud.cu

#include "PointCloud.hpp"

#ifdef __unix
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL
#endif

PointCloud::PointCloud() {

}

PointCloud::PointCloud(size_t width, size_t height) {
	Width = width;
	Height = height;
	pc = new POINT3D[Width * Height];
}

PointCloud::~PointCloud() {
	
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
			depth /= 1000f; // convert to meters

			pc[index].z = depth;
			pc[index].x = ((i - cx) * depth) / fx;
			pc[index].y = ((j - cy) * depth) / fy;
			index++;
		}
	}
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
