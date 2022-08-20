#ifndef FILTER_H
#define FILTER_H

#include "util.h"

namespace plt = matplotlibcpp;
using namespace std;

struct filter {
	filter() = default;
	filter(int _dim, vector<uint8_t> _data) : dim(_dim) {
		w.resize(dim*dim);
		for (int i = 0; i < dim * dim; i++) {
			w[i] = _data[i];
			mul += w[i];
		}
	}
	filter(int _dim, uint8_t _data[]) : dim(_dim) {
		assert(_data != NULL);
		w.resize(dim * dim);
		for (int i = 0; i < dim * dim; i++) {
			w[i] = _data[i];
			mul += w[i];
		}
	}
	~filter() {

	}
	inline uint8_t operator()(int i, int j) {
		return w[i * dim + j];
	}

	vector<uint8_t> w;
	int dim;
	double mul; // multiply by 1/mul
};
#endif