/*
 * SkyCrow
 * Overo Module
 * overo.h
 * Contains all the global variables used by the Overo Module.
 */

#ifndef SERIAL_NAME
 #define SERIAL_NAME "/dev/ttyUSB0"
#endif

#include <math.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <deps/mavlink/common/mavlink.h>
#include <uuid/uuid.h>
#include <time.h>
#include <core.h>

using namespace std;
using namespace cv2;

// General Globals
bool captureOn = false;
float distThreshold;		// The distance between the last photo capture and the next one.
struct position_t
{
	int lat;
	int lon;
	int alt;
} position, oldPosition;	// The current aircraft position and the position of the last photo.
struct attitude_t
{
	float roll;
	float pitch;
	float yaw;
} attitude;					// The current aircraft attitude.
const float fov = 70.0;									// Field of view in degrees diagonal.
const float gpsMultiplier = M_PI / (180f * 10000000f);	// Converts the raw gps latitude and longitude to latitude and longitude in radians.
const float altMultiplier = 1f / 1000f; 				// Converts the raw gps altitude to altitude in meters.
const int radiusOfEarth = 6371000; 						// In meters.

// Serial Globals
struct termios serialConfig;
int serialFd;					// The serial file descriptor.
const char[] serialName = SERIAL_NAME;	// Path to the serial port to use.

// OpenCV Globals
VideoCapture* cam;
Mat image;
const char[] pictureExtension = ".jpg";

// SQLite Globals
sqlite3* database;
sqlite3_stmt* insertStatement;
const char[] databaseExtension = ".mis";
const char[] sql_createTable = 
		"CREATE TABLE pictures (filename, timestamp, latitude, longitude, altitude, roll, pitch, yaw);";
const char[] sql_storeMetadata =
		"INSERT INTO pictures VALUES ($filename, $timestamp, $latitude, $longitude, $altitude, $roll, $pitch, $yaw);"