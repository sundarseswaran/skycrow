/*
 * SkyCrow
 * Overo Module
 * test.cpp
 * This file contains the unit testing code used in the Overo Module.
 */

#define TEST true

#include <overo.h>
#include <test.h>

void printMenu()
{
	cout << "1) Mavlink" << endl << "2) Camera" << endl << "3) Database" << endl << "Position" << endl;
}

void doTest(int choice)
{
	try
	{
		switch (choice)
		{
			case 1: mavlinkTest();
			case 2: cameraTest();
			case 3: databaseTest();
			case 4: positionTest();
		}
	}
	catch (...)
	{
		cout << "Exception was thrown by " << choice << endl;
	}
}

void mavlinkTest()
{
	setupSerialPort();
	// TODO - complete this test with fake data to the mavlink function.
}

void cameraTest()
{

}

void databaseTest()
{

}

void positionTest()
{

}

int main(int argc, char** argv)
{
	int choice;
	if (argc == 0)
	{
		while (true)
		{
			printMenu();
			choice << cin;
			doTest(choice);
		}
	}
	else
	{
		// Convert the ascii number to an int.
		choice = argv[0] - '0';
		doTest(choice);
	}
}