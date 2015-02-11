/*
 * SkyCrow
 * Overo Module
 * core.cpp
 * Contains all the required functions besides main().
 */

#include <overo.h>
#include <test.h>

// Sets the proper serial options and initializes the serial handle.
void setupSerialPort()
{
	// Reset the serial config object.
	memset(&serialconfig, 0, sizeof(serialconfig));

	// Turn off all special input modes.
    serialconfig.c_iflag=0;

    // Turn off all special output modes.
    serialconfig.c_oflag=0;

    // CS8 => 8-bit packes, CREAD => Enable receiving, CLOCAL => Ignore modem control lines.
    serialconfig.c_cflag=CS8|CREAD|CLOCAL;

    // Turn off all special local modes.
    serialconfig.c_lflag=0;

    // The port's read() function will always return immediately.
    serialconfig.c_cc[VMIN]=0;
    serialconfig.c_cc[VTIME]=0;

    // Set the baud I/O rates to 115200
    cfsetospeed(&serialconfig, B115200);
    cfsetispeed(&serialconfig, B115200);

    // Open the serial port with read and write
    serialFd = open(serialName, O_RDWR);
    if (serialFd < 0)
    {
    	ERROR("Serial port failed to open.");
    }

    // Apply the configuration
	tcsetattr(serialFd, TCSANOW, &serialconfig);
}

// Initializes the camera controller.
void setupCamera()
{
	cam = new VideoCapture(0);
	if (cam->isOpened())
	{
		ERROR("Camera failed to open.");
	}
}

// Creates a new database for this mission.
void setupDatabase()
{
	time_t now_in_seconds;
	struct tm now_date;
	char db_filename[80];
	int db_error;

	// Create the format string used by strftime()
	char db_filename_format[30] = "%y-%m-%d-%H-%M-%S"
	strcat(&db_filename_format, &databaseExtension);

	now_in_seconds = time(0);	// Get the current time in seconds
	now_date = *localtime(now_in_seconds);	// Convert the current time in seconds to a date format
	strftime(db_filename, sizeof(now_string), db_filename_format, &now_date); // Use the current date/time as the database filename

	// Create the mission database
	db_error = sqlite3_open(&db_filename, database);
	if (db_error)
	{
		ERROR("Failed to open database.")
	}

	// Create the "pictures" table in the database.
	db_error = sqlite3_exec(database, &sql_createTable);
	if (db_error)
	{
		ERROR("Failed to create table.")
	}

	// Compile insert statement into bytecode
	db_error = sqlite3_prepare(database, sql_storeMetadata, -1, insertStatement);
	if (db_error)
	{
		ERROR("Failed to prepare insert statement.")
	}
}

// Checks serial buffer for data. If there is data, it parses and handles the incoming Mavlink messages.
void checkMavlink()
{
	mavlink_message_t mavMessage;
	mavlink_status_t mavStatus;
	char chunk;

	// Poll the serial port until we get back a chunk of data.
	while(read(serialFd, &chunk, 1) > 0)
	{
		// Add together the chunks until we have a MAVLINK message.
		if(mavlink_parse_char(MAVLINK_COMM_0, chunk, &mavMessage, &mavStatus)) {
			// TODO - Check mavlink status

			switch(mavMessage.msgid)
			{
				// Updates the stored latitude, longitude, and altitude.
				case MAVLINK_MSG_ID_GPS_RAW_INT:
					position.lat = mavlink_msg_gps_raw_int_get_lat(&mavMessage);
					position.lon = mavlink_msg_gps_raw_int_get_lon(&mavMessage);
					position.alt = mavlink_msg_gps_raw_int_get_alt(&mavMessage);
					break;

				// Updates the stored roll, pitch, and yaw angles.
				case MAVLINK_MSG_ID_ATTITUDE:
					attitude.roll = mavlink_msg_attitude_get_roll(&mavMessage);
					attitude.pitch = mavlink_msg_attitude_get_pitch(&mavMessage);
					attitude.yaw = mavlink_msg_attitude_get_yaw(&mavMessage);
					break;

				// Turns on/off picture capturing. If it's turning it on, it instantly captures a picture.
				case MAVLINK_MSG_ID_DIGICAM_CONTROL:
					captureOn = !captureOn;
					if (captureOn)
					{
						CALL(capturePhoto());
					}
					break;
			}
		}
	}
}

// Calls capturePhoto() if the aircraft has traveled far enough to require another photo.
void checkPosition()
{
	// Convert the stored data into GPS angles.
	float lat = position.lat * gpsMultiplier;
	float lon = position.lon * gpsMultiplier;
	float oldLat = oldPosition.lat * gpsMultiplier;
	float oldLon = oldPosition.lon * gpsMultiplier;

	float dLat = lat - oldLat;
	float dLon = lon - oldLon;

	// Use the haversine function to calculate the distance between the two positions.
	float a = (sin(dLat/2) * sin(dLat/2)) + (cos(lat) * cos(oldLat) * sin(dLon/2) * sin(dLon/2));
	float c = 2 * atan2(sqrt(a), sqrt(1-a));
	float distance = radiusOfEarth * c; // In meters.

	if (distance > distThreshold)
	{
		CALL(capturePhoto());
	}
}

// Captures a photo, updates the oldPosition and distThreshold values, and calls storeMetdata().
void capturePhoto()
{
	// Generate photo filename from a random UUID.
	char pictureFilename[41]; //36 uuid characters, the dot, 3 extension characters, and null
	uuid_t uuid;
	uuid_generate_random(uuid);
	uuid_unparse(uuid, pictureFilename);
	
	// Add an extension to the filename
	memcpy(&pictureExtension, &pictureFilename+36);

	// TODO - add a path to the filename

	// Capture and save a picture
	cap >> image;
	imwrite(pictureFilename, image);

	// Store the current position as the old position.
	memcpy(&oldPosition, &position, sizeof(position_t));

	// Update the distance threshold with the new position
	float f = tan(fov/2) * oldPosition.alt * altMultiplier;
	distThreshold = sqrt((f*f)/2);

	CALL(storeMetadata(pictureFilename));
}

// Stores the latitude, longitude, altitude, and attitude of the aircraft for a particular picture file in the database.
void storeMetadata(char[] pictureFilename)
{
	int db_error;

	// Add picture filename to insert statement.
	db_error = sqlite3_bind_text(insertStatement, 1, pictureFilename, -1, null);
	if (db_error)
	{
		ERROR("Failed to bind the picture filename.")
	}

	// Add current time in seconds to insert statement.
	db_error = sqlite3_bind_int(insertStatement, 2, (int)time(0));
	if (db_error)
	{
		ERROR("Failed to bind the current time.")
	}

	// Add current lattitude to insert statement.
	db_error = sqlite3_bind_int(insertStatement, 3, position.lat);
	if (db_error)
	{
		ERROR("Failed to bind the latitude.")
	}

	// Add current longitude to insert statement.
	db_error = sqlite3_bind_int(insertStatement, 4, position.lon);
	if (db_error)
	{
		ERROR("Failed to bind the longitude.")
	}

	// Add current altitude to insert statement.
	db_error = sqlite3_bind_int(insertStatement, 5, position.alt);
	if (db_error)
	{
		ERROR("Failed to bind the altitude.")
	}

	// Add current roll to insert statement.
	db_error = sqlite3_bind_double(insertStatement, 6, attitude.roll);
	if (db_error)
	{
		ERROR("Failed to bind the roll.")
	}

	// Add current pitch to insert statemetn
	db_error = sqlite3_bind_double(insertStatement, 7, attitude.pitch);
	if (db_error)
	{
		ERROR("Failed to bind the pitch.")
	}

	// Add current yaw to insert statement
	db_error = sqlite3_bind_double(insertStatement, 8, attitude.yaw);
	if (db_error)
	{
		ERROR("Failed to bind the yaw.")
	}

	// Execute the insert statement.
	db_error = sqlite3_step(insertStatement);
	if (db_error != SQLITE_DONE)
	{
		ERROR("Failed to execute the insert statement.")
	}

	db_error = sqlite3_clear_bindings(insertStatement);
	if (db_error != SQLITE_DONE)
	{
		ERROR("Failed to clear the bindings.")
	}
}