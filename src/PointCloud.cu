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

__global__ void parallelFill(const unsigned char* image, const float* depth_map, const int* Width, POINT3D* dev_pc, const float* cx, const float* cy, const float* fx, const float* fy) {
	int t = threadIdx.x;
	int j = t / *Width;
	int i = t % *Width;

	dev_pc[t].r = image[j * (*Width * 4) + i * 4];
	dev_pc[t].g = image[j * (*Width * 4) + i * 4 + 1];
	dev_pc[t].b = image[j * (*Width * 4) + i * 4 + 2];

	float depth = depth_map[t];
	depth /= 1000.f; // Convert to meters;

	dev_pc[t].z = depth;
	dev_pc[t].x = ((i - *cx) * depth) / *fx;
	dev_pc[t].y = ((j - *cy) * depth) / *fy;
}

void PointCloud::fill(const unsigned char* image, const float* depth_map, const sl::zed::StereoParameters *param) {
	// allocate memory for a device copy of pc
	POINT3D* dev_pc;
	cudaMalloc((void**)&dev_pc, Width * Height * sizeof(POINT3D));
	// copy host pc to device
	cudaMemcpy(dev_pc, pc, Width * Height * sizeof(POINT3D), cudaMemcpyHostToDevice);
	// fill
	parallelFill<<< 1, Width * Height >>>(image, depth_map, &Width, dev_pc, &(param->LeftCam.cx), &(param->LeftCam.cy), &(param->LeftCam.fx), &(param->LeftCam.fy));
	// copy device pc to host
	cudaMemcpy(dev_pc, pc, Width * Height * sizeof(POINT3D), cudaMemcpyDeviceToHost);
	// free allocated device memory
	cudaFree((void*)dev_pc);

	for(int j = 0; j < Height; j++) {
		for(int i = 0; i < Height; i++) {
			std::cout << pc[i + Width * j]->r << '\t' << pc[i + Width * j]->g << '\t' << pc[i + Width * j]->b << '\t' << pc[i + Width * j]->x << '\t' << pc[i + Width * j]->y << '\t' << pc[i + Width * j]->z << '\t' << std::endl;
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
