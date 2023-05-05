#include <iostream>

using namespace std;

int main(int argc, char** args) {
	for (int i = 0; i < argc; i++)
		std::cout << args[i] << endl;
	return 0;
}
