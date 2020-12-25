
#include <iostream>

int main(int argc, const char **argv) {
	std::cout << "Hello";
	if (argc > 1) {
		std::cout << " " << argv[1];
	}
	std::cout << std::endl;

	return 0;
}
