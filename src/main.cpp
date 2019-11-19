#include "thread.h"
#include "kernel.h"

extern int userMain(int argc, char *argv[]);

int main(int argc, char *argv[]) {
	Kernel::setup();
	int result = userMain(argc, argv);
	Kernel::cleanup();
	return result;
}