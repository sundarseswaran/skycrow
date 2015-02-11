/*
 * SkyCrow
 * Overo Module
 * main.cpp
 * Contains the main loop.
 */

#include <overo.h>

int main(int argc, char** argv)
{
	setupSerialPort();
	setupCamera();
	setupDatabase();

	while (1)
	{
		checkMavlink();

		if (captureOn)
		{
			checkPosition();
		}
	}
}