#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include "kdtree.h"

template <typename T>
struct point_3d{
	T x;
	T y;
	T z;
};

int main() {
	std::string filename = "scan00045.3d";
	std::ifstream ifs(filename, std::ofstream::in);
	if (!ifs.is_open()) {
		std::cerr << "not find file " << filename << std::endl;
		return 1;
	}
	point_3d<float> p;
	int r;
	std::vector<point_3d<float>> points;
	std::string line;
	// while (ifs.getline(line) ){
	while (getline(ifs, line) ){
		std::stringstream ss;
		ss << line;
		ss >> p.x >> p.z >> p.y >> r;
		points.push_back(p);
		// std::cout <<  p.x << " " << p.z << " " << p.y << std::endl;
	}
	ifs.close();
	spatial_index::kdtree<point_3d<float>> tree;

	for (auto &p : point_3) {
		tree.add(p,p);
	}
	size_t k = 3;
	const point_3d<float> p;



	return 0;
}
