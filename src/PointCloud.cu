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
	// allocate memory for a device copy of pc
	POINT3D* dev_pc;
	cudaMalloc((void**)&dev_pc, Width * Height * sizeof(POINT3D));
	// copy host pc to device
	cudaMemcpy(dev_pc, pc, Width * Height * sizeof(POINT3D), cudaMemcpyHostToDevice);
	// fill
	parallelFill<<< 1, Width * Height >>>(image, depth_map, &Width, &(param->LeftCam.cx), &(param->LeftCam.cy), &(param->LeftCam.fx), &(param->LeftCam.fy));
	// copy device pc to host
	cudaMemcpy(dev_pc, pc, Width * Height * sizeof(POINT3D), cudaMemcpyDeviceToHost);
}

__global__ void parallelFill(const unsigned char* image, const float* depth_map, const int* Width, const float* cx, const float* cy, const float* fx, const float* fy) {
	int t = threadIdx.x;
	int j = t / Width;
	int i = t % Width;

	pc[t].setColor(&image[j * (Width * 4) + i * 4]);

	float depth = depth_map[t];
	depth /= 1000.f; // Convert to meters;

	pc[t].z = depth;
	pc[t].x = ((i - cx) * depth) / fx;
	pc[t].y = ((j - cy) * depth) / fy;
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
