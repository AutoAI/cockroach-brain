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
	pc.resize(Width * Height);
}

PointCloud::~PointCloud() {
	
}

void PointCloud::fill(const unsigned char* image, const float* depth_map, const sl::zed::StereoParameters *param) {
	parallelFill<<< 1, Width * Height >>>(image, depth_map, param->LeftCam.cx, param->LeftCam.cy, param->LeftCam.fx, param->LeftCam.fy);
}

__global__ void PointCloud::parallelFill(const unsigned char* image, const float* depth_map, const float* cx, const float* cy, const float* fx, const float* fy) {
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

size_t PointCloud::GetNbPoints() {
	return pc.size();
}

size_t PointCloud::GetWidth() {
	return Width;
}

size_t PointCloud::GetHeight() {
	return Height;
}

void PointCloud::WritePCDFile(std::string path, bool verbose) {

	if (path.find(".pcd") == std::string::npos)
		path.append(".pcd");

	FILE *fich;
	fopen_s(&fich, path.c_str(), "w");

	fprintf(fich, "# .PCD v.7 - Point Cloud Data file format\n");
	fprintf(fich, "VERSION .7\n");
	fprintf(fich, "FIELDS x y z rgb\n");
	fprintf(fich, "SIZE 4 4 4 4\n");
	fprintf(fich, "TYPE F F F F\n");
	fprintf(fich, "COUNT 1 1 1 1\n");
	fprintf(fich, "WIDTH %d\n", Width);
	fprintf(fich, "HEIGHT %d\n", Height);
	fprintf(fich, "POINTS %d\n", Width * Height);
	fprintf(fich, "DATA ascii\n");

	if (verbose) printf("Saving PCD File ...");

	for (auto it = this->cbegin(); it != this->cend(); ++it)
		fprintf(fich, "%f %f %f %e\n", it->x, it->y, it->z, it->getColorFloat());

	if (verbose) printf("Done\n");

	fclose(fich);
}
