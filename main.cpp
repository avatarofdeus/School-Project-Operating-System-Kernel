#include "system.h"
#include <iostream.h>
#include <dos.h>
#include "defs.h"

extern int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]){

	System::initSystem();

	int result = userMain(argc, argv);

	System::restoreSystem();

	return result;
}

