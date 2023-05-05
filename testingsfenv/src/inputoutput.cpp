#include <iostream>

using namespace std;

void main(int argc, char** args) {
	for (int i = 0; i < argc; i++)
		std::cout << args[i] << endl;

}
