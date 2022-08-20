#ifndef IMAGE_H
#define IMAGE_H
#include "util.h"
#include "filter.h"

namespace plt = matplotlibcpp;
using namespace std;

struct Image {
	Image() = default;
	Image(int w, int h, int _bpp) : width(w), height(h), bpp(_bpp) {
		data.resize(width*height*bpp);
	}

	Image(uint8_t* _data, int w, int h, int _bpp) : width(w), height(h), bpp(_bpp) {
		assert(_data != NULL);
		data.resize(width * height * bpp);
		for (int i = 0; i < width * height * bpp; i++) {
			data[i] = _data[i];
		}
	}

	Image(vector<uint8_t> &_data, int w, int h, int _bpp) : width(w), height(h), bpp(_bpp) {
		data.resize(width * height * bpp);
		for (int i = 0; i < width * height * bpp; i++) {
			data[i] = _data[i];
		}
	}

	Image(const char* path, int mode) {
		assert(mode == 1 || mode == 3);
		auto temp = stbi_load(path, &width, &height, &bpp, mode);
		assert(temp != NULL);
		for(int i = 0; i < width*height*bpp; i++){
			data.push_back(temp[i]);
		}
	}

	~Image() {

	}

	void save(const char* path){
		stbi_write_png(path, width, height, bpp, data.data(), width * bpp);
	}

	inline uint8_t operator()(int i, int j, int channel) {
		assert(channel < bpp && channel >= 0);
		if(i < 0 || j < 0) {
			return 0;
			// if(i < 0) i*= -1;
			// if(j < 0) j*= -1;
			// return data[(i * width + j)*bpp + channel];
		}
		if(bpp > 1) return data[(i * width + j)*bpp + channel];
		else return data[i*width + j];
	}

	inline Image operator+(Image &rhs){
		assert(rhs.width == width && rhs.height == height && rhs.bpp == bpp);
		
		Image output(width, height, bpp);

		for (int chn = 0; chn < bpp; chn++) {
			for (int r = 0; r < height; r++) {
				for (int c = 0; c < width; c++) {
					output.data[(r * width + c) * bpp + chn] = this->operator()(r,c,chn) + rhs(r,c,chn);
				}
			}
		}
		return output;
	}

	Image correlate(filter& w){
		Image output(width,height,bpp);

		for (int chn = 0; chn < bpp; chn++) {
			for (int r = 0; r < height; r++) {
				for (int c = 0; c < width; c++) {
					uint64_t pixel = 0;
					int a = (w.dim-1)/2;
					int b = (w.dim-1)/2;
					// printf("r|c : %d|%d\n", r, c);
					for(int s = -a ; s <= a; s++){
						for(int t = -b; t <= b; t++){
							pixel += w(s+a,t+b)*(this->operator()(r+s,c+t,chn));
							// printf("s+a : %d, t+b : %d, r+s : %d, c+t : %d\n", s+a, t+b, r+s, c+t);
						}
					}
					output.data[(r * width + c) * bpp + chn] = static_cast<uint8_t>(round(double(pixel) / w.mul));
				}
			}
		}
		return output;
	}

	Image convert_to_gray(void){
		Image gray(width, height, 1);
		///Convert To Grayscale
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				double r = static_cast<double>(this->operator()(i, j, 0));
				double g = static_cast<double>(this->operator()(i, j, 1));
				double b = static_cast<double>(this->operator()(i, j, 2));

				gray.data[i * gray.width + j] = static_cast<uint8_t>((r + g + b) / 3);
			}
		}
		return gray;
	}

	vector<uint8_t> data;
	int height;
	int width;
	int bpp; //bytes per pixel
};

#endif