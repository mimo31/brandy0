#include <cstdlib>

#include "application.hpp"

namespace brandy0
{

void run()
{
	Application app;
	app.run();
	return;
}

}

int main()
{
	brandy0::run();
	return EXIT_SUCCESS;
}
