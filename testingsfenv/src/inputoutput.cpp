#include <iostream>
#include <Eigen/Geometry>
#include <vector>

using namespace std;
using namespace Eigen;


void foo(std::vector<int> v) {
	v.pop_back();
	v.push_back(20);
}

int main(int argc, char** args) {

	Eigen::MatrixXi facets;
	for (int i = 0; i < argc; i++)
		std::cout << args[i] << endl;

	std::vector<int> v;
	v.push_back(1);
	foo(v);
	std::cout << v.at(0) << endl;
	return 0;
}
