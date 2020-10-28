#include <iostream>
#include <cstdlib>

#include "vec2d.hpp"

namespace brandy0
{

using std::cout;
using std::endl;

void run()
{
	cout << "hello, world!" << endl;
	const vec2d a(2.3, 2);
	const vec2d b(2, 4);
	cout << a.dot(b) << endl;
}

}

int main()
{
	brandy0::run();
	return EXIT_SUCCESS;
}
