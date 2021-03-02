#include <cstdlib>

#include "application.hpp"

// #define TESTING

#ifdef TESTING
#include "tests.hpp"
#endif // TESTING

namespace brandy0
{

void run()
{
	#ifdef TESTING
	
	Tests tests;
	tests.run();
	
	#else
	
	Application app;
	app.run();
	
	#endif // TESTING

	return;
}

}

int main()
{
	brandy0::run();
	return EXIT_SUCCESS;
}
