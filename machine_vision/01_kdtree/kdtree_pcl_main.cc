#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>

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

	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud( new pcl::PointCloud<pcl::PointXYZ>);
	cloud->width = points.size();
	cloud->height = 1;
	cloud->points.resize(cloud->width * cloud->height);
	for (int i = 0; i < points.size(); ++i) {
		cloud->points[i].x = points[i].x;
		cloud->points[i].y = points[i].y;
		cloud->points[i].z = points[i].z;
	}

	pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;

	kdtree.setInputCloud( cloud );
	pcl::PointXYZ searchPoint;

	/*
	searchPoint.x = points[0].x;
	searchPoint.y = points[0].y;
	searchPoint.z = points[0].z;
	*/
	// while (true) {
	std::cout << "please input search point " << std::endl;
	point_3d<float> point;
	std::cin >> point.x >> point.z >> point.y;
	searchPoint.x = point.x;
	searchPoint.y = point.y;
	searchPoint.z = point.z;


	int K = 3;
	std::vector<int> pointIdxNKNSearch(K);
	std::vector<float> pointNKNSquaredDistance(K);
	if ( kdtree.nearestKSearch (searchPoint, K, pointIdxNKNSearch, pointNKNSquaredDistance) > 0 ) { 
		for (size_t i = 0; i < pointIdxNKNSearch.size (); ++i) {
		       	std::cout << "    " << cloud->points[ pointIdxNKNSearch[i] ].x 
				<< " " << cloud->points[ pointIdxNKNSearch[i] ].y 
				<< " " << cloud->points[ pointIdxNKNSearch[i] ].z 
				<< " (squared distance: " << pointNKNSquaredDistance[i] << ")" << std::endl; 
		}
	}


	return 0;
}
