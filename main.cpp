#include "include/util.h"
#include "include/image.h"
#include "include/filter.h"
#include "include/dip.h"

namespace plt = matplotlibcpp;
using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cerr << "usage : ./main [path-to-file]\n";
		return 1;
	}

	///Load image
	Image input(argv[1],3); //input color images
	if(input.bpp < 3){
		input = input.convert_to_gray();
	}
	Image gray_output(input.width,input.height,1);
	Image col_output(input.width,input.height,3);
	cerr << "Image loaded!: " << argv[1] << " : " << input.width << " * " << input.height << endl;

	cout << "Choose Operation: \n(0) Nothing/Experimental\n(1) Negative\n(2) Grayscale Histogram Equalization\n";
	int choice = 0;
	cin >> choice;

	if (choice == 0) {//do nothing/experimental

		// uint8_t image_data[25] = {0};
		// for(int i = 0; i < 5; i++){
		// 	image_data[i*5+i] = 255;
		// }
		// input = Image(image_data,5,5,1);

		float filter_data[] = {
								0, 1, 0,
								1,-4, 1,
								0, 1, 0
							};
		filter W(3,filter_data);
		input = input.convert_to_gray();
		input.save("col_input.png");
		col_output = dip::laplace(input,W);
	}

	else if (choice == 1) {//Image Negative
		cout << "Grayscale(0) or Color(1) Negative : ";
		cin >> choice;
		if (choice == 0) {//grayscale negative
			
			///Convert To Grayscale
			auto grayscale = input.convert_to_gray();

			///Get Negative
			gray_output = dip::grayscale_negative(grayscale);
			grayscale.save("gray_input.png");
		}
		else if (choice == 1) {//col negative
			input.save("col_input.png");
			col_output = dip::negative(input);
		}
	}

	else if (choice == 2) {//grayscale histogram equalization
		
		///Convert To Grayscale
		auto grayscale = input.convert_to_gray();

		///Histogram Equalization
		gray_output = dip::grayscale_histogram_equalization(grayscale);
		grayscale.save("gray_input.png");
	}

	return 0;
}