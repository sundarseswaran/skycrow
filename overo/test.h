/*
 * SkyCrow
 * Overo Module
 * test.h
 * Contains the test preprocessor directives used in this project.
 */

#define ALERT(msg) cout << #__FILE__ ": " #__LINE__ ": " #msg << endl
#define ERROR(e) ALERT("ERROR " #e)
#define ASSERT(x) if (!##x##) ERROR(#x " is not true")

#if TEST
 #define CALL(x) ALERT("Called " #x)
#else
 #define CALL(x) x
#endif