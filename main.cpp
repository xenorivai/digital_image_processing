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

#include "include/matplotlibcpp.h" /// https://github.com/lava/matplotlib-cpp

namespace plt = matplotlibcpp;
using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cerr << "usage : ./main [path-to-file]";
		return 1;
	}

///Load image
	int width, height, bpp; //bpp - bytes per pixel
	uint8_t* data = stbi_load(argv[1], &width, &height, &bpp, 3);
	uint8_t* grayscale = new uint8_t[width*height] {0};
	uint8_t* equalized = new uint8_t[width*height] {0};
	if (data == NULL) {
		cerr << "[STB_IMAGE] FAILED TO LOAD IMAGE\n";
		return 1;
	}
	cerr << "Image loaded!: " << argv[1] << " : " << width << " * " << height << endl;


	vector<int> freq(256,0); // frequency table for grayscale image
	vector<int> eq(256, 0);	 // frequency table for equalized image

///Stores mapping from intensity values to their PDF and CDF
	map<int,pair<double,uint8_t>> df; // df[i] = ( pdf[i] , 255*cdf[i] );

///Convert to grayscale
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){

			double r = static_cast<double>(data[(i*width + j)*3 + 0]);
			double g = static_cast<double>(data[(i*width + j)*3 + 1]);
			double b = static_cast<double>(data[(i*width + j)*3 + 2]);

			grayscale[(i*width + j)] = (r + g + b)/3;

			freq[grayscale[(i*width + j)]]++;
		}
	}

	double cumulative = 0; /// stores cumuluative sum of pdf

///Compute CDF and PDF of grayscale image
	for(int i = 0; i < 256; i++){
		auto y = double(freq[i])/(double(width*height));
		cumulative += y;
		df[i] = {y,floor(double(255)*cumulative)};
	}

///Perform Hist Equalization
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			
			equalized[(i * width + j)] = df[grayscale[(i * width + j)]].second;

			eq[equalized[(i * width + j)]]++;
		}
	}
///Save Images
	stbi_write_png("gray_scale.png",width,height,1,grayscale,width);
	stbi_write_png("hist_eq.png",width,height,1,equalized,width);

///Plot Hist and Images
	plt::figure_size(960,480);
	plt::title("GrayScale Image");
	plt::xlabel("Grayscale Intensity");
	plt::ylabel("Frequency");
	plt::xlim(0,256);

	plt::subplot2grid(2,2,0,0,1,1);
	plt::named_plot("Original Histogram",freq,"r");
	plt::legend();
	plt::subplot2grid(2,2,0,1,1,1);
	plt::named_plot("Equalized Histogram",eq,"b");
	plt::legend();
	
	plt::subplot2grid(2,2,1,0,1,1);
	plt::imshow(grayscale,height,width,1);
	plt::subplot2grid(2,2,1,1,1,1);
	plt::imshow(equalized,height,width,1);
	
	plt::show();

///Output statistis to stdout, pipe this to some file say "stats.txt"
	cout << "Statistics:\n------------------------------\n";
	cout << "\n\ni -- freq -- eq -- pdf -- 255*cdf\n\n\n";
	for(int i = 0; i < 256; i++){
		// cout << x.first << "\t-> " << eq[x.first] <<"\t,\t" <<x.second.first << "\t,\t" << x.second.second << endl;
		printf("%d , \t %5d,\t %5d,\t %0.5f,\t %d\n",i,freq[i],eq[i],df[i].first,df[i].second);
	}
	cout << "\n------------------------------\n";

///Free up image data
	stbi_image_free(data);
	stbi_image_free(grayscale);
	stbi_image_free(equalized);

	return 0;
}