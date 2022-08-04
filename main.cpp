#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <map>
#include <sys/ioctl.h>
#include <unistd.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "include/stb_image.h"
#include "include/stb_image_write.h"
#include "include/stb_image_resize.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cerr << "usage : ./main [path-to-file]";
		return 1;
	}

	//Load image
	int width, height, bpp; //bpp - bytes per pixel
	uint8_t* data = stbi_load(argv[1], &width, &height, &bpp, 3);
	uint8_t* data_new = new uint8_t[width*height];
	if (data == NULL) {
		cerr << "[STB_IMAGE] FAILED TO LOAD IMAGE\n";
		return 1;
	}
	cerr << "Image loaded!: " << argv[1] << " : " << width << " * " << height << endl;

	map<int,int> freq;
	map<int,pair<double,double>> df;

	//Convert to grayscale
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){

			auto r = data[(i*width + j)*3 + 0];
			auto g = data[(i*width + j)*3 + 1];
			auto b = data[(i*width + j)*3 + 2];

			data_new[(i*width + j)] = (r + g + b)/3;
			freq[data_new[(i*width + j)]]++;
		}
	}

	stbi_write_png("gray_scale.png",width,height,1,data_new,width);

	double cumulative = 0;
	for(auto &x : freq){
		auto y = double(x.second)/(double(width*height));
		cumulative += y;
		df[x.first] = {y,255*cumulative};
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			data_new[(i * width + j)] = df[data_new[(i * width + j)]].second;
		}
	}


	// cout << "Freq Table for Grayscale Image:\n------------------------------\n";
	// for (auto& x : freq) {
	// 	cout << x.first << " -> " << x.second << endl;
	// }
	// cout << "\n------------------------------\n";

	cout << "PDF for Grayscale Image:\n------------------------------\n";
	for(auto &x : df){
		cout << x.first << "\t-> " << x.second.first << "\t,\t" << x.second.second << endl;
	}
	cout << "\n------------------------------\n";


	stbi_write_png("hist_eq.png",width,height,1,data_new,width);
	stbi_image_free(data);
	return 0;
}