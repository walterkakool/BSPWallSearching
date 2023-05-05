#include <iostream>

using namespace std;

void main(int argc, char** args) {
	for (int i = 0; i < argc; i++)
		std::cout << argv[i] << endl;

}
