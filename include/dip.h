#ifndef DIP_H
#define DIP_H

#include "util.h"
#include "image.h"

namespace plt =  matplotlibcpp;

namespace dip {
	Image grayscale_histogram_equalization(Image& input);
	Image grayscale_negative(Image& input);
	Image negative(Image& input);
	Image correlate(Image& input, filter& w);	
	void log_transform(uint8_t* input, const int height, const int width, uint8_t* output, const double factor);
}

void dip::log_transform(uint8_t* input, const int height, const int width, uint8_t* output, const double factor) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			output[i * width + j] = min(uint8_t(255), static_cast<uint8_t>(factor * log(uint8_t(1) + input[i * width + j])));
		}
	}

	///Save Images
	stbi_write_png("gray_scale.png", width, height, 1, input, width);
	stbi_write_png("log_transformed.png", width, height, 1, output, width);

	///Plot Images
	plt::figure_size(960, 480);
	plt::title("Grayscale and Negative");
	plt::subplot2grid(1, 2, 0, 0, 1, 1);
	plt::imshow(input, height, width, 1);
	plt::subplot2grid(1, 2, 0, 1, 1, 1);
	plt::imshow(output, height, width, 1);

	plt::show();
}

Image dip::correlate(Image& input, filter& filt){
	Image output(input.width,input.height,input.bpp);

	output = input.correlate(filt);

	///Save image
	output.save("correlated.png");

	///Plot Images
	plt::figure_size(960, 480);
	plt::title("Input and Correlated");
	plt::subplot2grid(1, 3, 0, 0, 1, 1);
	plt::imshow(input.data.data(), input.height, input.width, input.bpp);
	plt::subplot2grid(1, 3, 0, 1, 1, 1);
	plt::imshow(output.data.data(), output.height, output.width, output.bpp);
	plt::subplot2grid(1, 3, 0, 2, 1, 1);
	plt::imshow(filt.w.data(), filt.dim, filt.dim, 1);

	plt::show();

	return output;
}

Image dip::grayscale_negative(Image &input) {
	Image output(input.width,input.height,input.bpp);
	for (int i = 0; i < input.height; i++) {
		for (int j = 0; j < input.width; j++) {
			output.data[i * input.width + j] = 255 - input(i,j,0);
		}
	}

	///Save Images
	output.save("negative.png");

	///Plot Images
	plt::figure_size(960, 480);
	plt::title("Grayscale and Negative");
	plt::subplot2grid(1, 2, 0, 0, 1, 1);
	plt::imshow(input.data.data(), input.height, input.width, 1);
	plt::subplot2grid(1, 2, 0, 1, 1, 1);
	plt::imshow(output.data.data(), output.height, output.width, 1);

	plt::show();
	return output;
}

Image dip::negative(Image &input) {
	Image output(input.width,input.height,input.bpp);
	for (int i = 0; i < input.height; i++) {
		for (int j = 0; j < input.width; j++) {
			output.data[(i * input.width + j) * 3 + 0] = 255 - input(i,j,0);
			output.data[(i * input.width + j) * 3 + 1] = 255 - input(i,j,1);
			output.data[(i * input.width + j) * 3 + 2] = 255 - input(i,j,2);
		}
	}

	///Save Images
	output.save("negative.png");

	///Plot Images
	plt::figure_size(960, 480);
	plt::title("Image and Negative");
	plt::subplot2grid(1, 2, 0, 0, 1, 1);
	plt::imshow(input.data.data(), input.height, input.width, 3);
	plt::subplot2grid(1, 2, 0, 1, 1, 1);
	plt::imshow(output.data.data(), output.height, output.width, 3);

	plt::show();
	return output;
}

Image dip::grayscale_histogram_equalization(Image &input){
	Image output(input.width,input.height,input.bpp);

	vector<int> freq(256, 0); /// frequency table for grayscale image
	vector<int> eq(256, 0);	  /// frequency table for equalized image
	double cumulative = 0;    /// stores cumuluative sum of pdf

	///Stores mapping from intensity values to a pair of their PDF and CDF
	map<int, pair<double, uint8_t>> df; // df[i] = ( pdf[i] , 255*cdf[i] );

	///Build Frequency Table
	for (int i = 0; i < input.height; i++) {
		for (int j = 0; j < input.width; j++) {
			freq[input(i,j,0)]++;
		}
	}

	///Compute CDF and PDF of the grayscale image
	for (int i = 0; i < 256; i++) {
		auto y = double(freq[i]) / (double(input.width * input.height));
		cumulative += y;
		df[i] = { y, floor(double(255) * cumulative) };
	}

	///Perform Hist Equalization
	for (int i = 0; i < input.height; i++) {
		for (int j = 0; j < input.width; j++) {
			output.data[(i * input.width + j)] = df[input(i,j,0)].second;
			eq[output(i,j,0)]++;
		}
	}

	///Save Images
	output.save("hist_eq.png");

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
	plt::imshow(input.data.data(), input.height, input.width, 1);
	plt::subplot2grid(2, 2, 1, 1, 1, 1);
	plt::imshow(output.data.data(), output.height, output.width, 1);
	///Display Results
	plt::show();

	return output;
}

#endif