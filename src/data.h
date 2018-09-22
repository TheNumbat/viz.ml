
#pragma once

#include "vmath.h"

#include <string>
#include <vector>
#include <fstream>

struct data_point {
	i32 label = -1;
	f32 data[784] = {};
};

std::vector<data_point> load_mnist(std::string images, std::string labels) {

	std::vector<data_point> ret;

	std::ifstream iin(images);
	std::ifstream lin(labels);

	return ret;	
}
