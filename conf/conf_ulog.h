/**
 * Configuration file for ULOG logger
 */

//
// Define the project preferred log level
//

#ifdef NDEBUG
// Release build keep info level
#  define ULOG_LEVEL ULOG_LEVEL_INFO
#else
// Debug build keep all debug levels
#  define ULOG_LEVEL ULOG_LEVEL_DEBUG3
#endif
