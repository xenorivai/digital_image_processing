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

void grayscale_histogram_equalization(uint8_t* input, const int height, const int width, uint8_t* output);
void grayscale_negative(uint8_t* input, const int height, const int width, uint8_t* output);
void negative(uint8_t *input, const int height, const int width, uint8_t *output);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cerr << "usage : ./main [path-to-file]\n";
		return 1;
	}

	///Load image
	int width, height, bpp; //bpp - bytes per pixel
	uint8_t* data = stbi_load(argv[1], &width, &height, &bpp, 3);
	uint8_t* gray_output = new uint8_t[width * height] { 0 };
	uint8_t* col_output = new uint8_t[width * height * 3] { 0 };
	if (data == NULL) { cerr << "[STB_IMAGE] FAILED TO LOAD IMAGE\n"; return 1; }
	cerr << "Image loaded!: " << argv[1] << " : " << width << " * " << height << endl;

	cout << "Choose Operation: \n(0) Nothing\n(1) Negative\n(2) Grayscale Histogram Equalization\n";
	int choice = 0;
	cin >> choice;
	if(choice == 0){//do nothing

	}
	
	else if(choice == 1){//Image Negative
		cout << "Grayscale(0) or Color(1) Negative : ";
		cin >> choice;
		if(choice == 0){//grayscale negative
			uint8_t* grayscale = new uint8_t[width * height] { 0 };
			///Convert To Grayscale
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					double r = static_cast<double>(data[(i * width + j) * 3 + 0]);
					double g = static_cast<double>(data[(i * width + j) * 3 + 1]);
					double b = static_cast<double>(data[(i * width + j) * 3 + 2]);

					grayscale[i * width + j] = static_cast<uint8_t>((r + g + b) / 3);
				}
			}
			grayscale_negative(grayscale,height,width,gray_output);
			stbi_image_free(grayscale);
		}
		else if(choice == 1){//col negative
			negative(data,height,width,col_output);
		}
	}

	else if(choice == 2){//grayscale histogram equalization
		uint8_t* grayscale = new uint8_t[width * height] { 0 };
		///Convert To Grayscale
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				double r = static_cast<double>(data[(i * width + j) * 3 + 0]);
				double g = static_cast<double>(data[(i * width + j) * 3 + 1]);
				double b = static_cast<double>(data[(i * width + j) * 3 + 2]);

				grayscale[i * width + j] = static_cast<uint8_t>((r + g + b) / 3);
			}
		}

		///Histogram Equalization
		grayscale_histogram_equalization(grayscale, height, width, gray_output);
		stbi_image_free(grayscale);
	}

	///Free up image data
	stbi_image_free(data);
	stbi_image_free(gray_output);

	return 0;
}


void grayscale_negative(uint8_t* input, const int height, const int width, uint8_t* output){
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			output[i*width + j] = 255 - input[i*width + j];
		}
	}

	///Save Images
	stbi_write_png("gray_scale.png", width, height, 1, input, width);
	stbi_write_png("negative.png", width, height, 1, output, width);

	///Plot Images
	plt::figure_size(960, 480);
	plt::title("Grayscale and Negative");
	plt::subplot2grid(1, 2, 0, 0, 1, 1);
	plt::imshow(input, height, width, 1);
	plt::subplot2grid(1, 2, 0, 1, 1, 1);
	plt::imshow(output, height, width, 1);

	plt::show();
}

void negative(uint8_t *input, const int height, const int width, uint8_t *output){
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			output[(i*width + j)*3 + 0] = 255 - input[(i*width + j)*3 + 0];
			output[(i*width + j)*3 + 1] = 255 - input[(i*width + j)*3 + 1];
			output[(i*width + j)*3 + 2] = 255 - input[(i*width + j)*3 + 2];
		}
	}

	///Save Images
	stbi_write_png("input.png", width, height, 3, input, width*3);
	stbi_write_png("negative.png", width, height, 3, output, width*3);

	///Plot Images
	plt::figure_size(960, 480);
	plt::title("Image and Negative");
	plt::subplot2grid(1, 2, 0, 0, 1, 1);
	plt::imshow(input, height, width, 3);
	plt::subplot2grid(1, 2, 0, 1, 1, 1);
	plt::imshow(output, height, width, 3);

	plt::show();
}

void grayscale_histogram_equalization(uint8_t* input, const int height, const int width, uint8_t* output)
{
	vector<int> freq(256, 0); /// frequency table for grayscale image
	vector<int> eq(256, 0);	  /// frequency table for equalized image
	double cumulative = 0;    /// stores cumuluative sum of pdf

	///Stores mapping from intensity values to a pair of their PDF and CDF
	map<int, pair<double, uint8_t>> df; // df[i] = ( pdf[i] , 255*cdf[i] );

	///Build Frequency Table
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			freq[input[(i * width + j)]]++;
		}
	}

	///Compute CDF and PDF of the grayscale image
	for (int i = 0; i < 256; i++) {
		auto y = double(freq[i]) / (double(width * height));
		cumulative += y;
		df[i] = { y, floor(double(255) * cumulative) };
	}

	///Perform Hist Equalization
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			output[(i * width + j)] = df[input[(i * width + j)]].second;
			eq[output[(i * width + j)]]++;
		}
	}

	///Save Images
	stbi_write_png("gray_scale.png", width, height, 1, input, width);
	stbi_write_png("hist_eq.png", width, height, 1, output, width);

	///Plot Hist and Images
	plt::figure_size(960, 480);
	plt::title("GrayScale Image");
	plt::xlabel("Grayscale Intensity");
	plt::ylabel("Frequency");
	plt::xlim(0, 256);

	plt::subplot2grid(2, 2, 0, 0, 1, 1);
	plt::named_plot("Original Histogram", freq, "r");
	plt::legend();
	plt::subplot2grid(2, 2, 0, 1, 1, 1);
	plt::named_plot("Equalized Histogram", eq, "b");
	plt::legend();

	plt::subplot2grid(2, 2, 1, 0, 1, 1);
	plt::imshow(input, height, width, 1);
	plt::subplot2grid(2, 2, 1, 1, 1, 1);
	plt::imshow(output, height, width, 1);
	///Display Results
	plt::show();
}