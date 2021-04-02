#include <cstdlib>

#include "application.hpp"

#ifdef TESTING
#include "tests.hpp"
#endif // TESTING

namespace brandy0
{

/**
 * Run's the application or just tests (iff the TESTING macro is defined)
 * @param argc the number of command-line arguments (the argc as in the main function)
 * @param argv the array of command-line arguments (the argv as in the main function)
 */
void run(const int argc, const char *const *const argv)
{
	#ifdef TESTING
	
	Tests::run();
	
	#else
	
	Application app;
	app.run(argc, argv);
	
	#endif // TESTING

	return;
}

}

/**
 * Our application's main
 */
int main(const int argc, const char *const *const argv)
{
	brandy0::run(argc, argv);
	return EXIT_SUCCESS;
}
