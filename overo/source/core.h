/*
 * SkyCrow
 * Overo Module
 * core.h
 * Contains the prototypes for the core.cpp functions.
 */

void setupSerialPort();
void setupCamera();
void setupDatabase();
void checkMavlink();
void checkPosition();
void capturePhoto();
void storeMetadata(char pictureFilename[]);