#ifndef __HANDLE_LOG_H
#define __HANDLE_LOG_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <String.h>

#include <FS.h>
#include <SPIFFS.h>

#define dbg_log(...) //Serial.printf(__VA_ARGS__)

// #define LOGV_FORMAT(letter, format) "[" #letter "]<%s> %s:%u " format "\r\n", __FUNCTION__, Dehisce_Name(__FILE__), __LINE__
#define LOGV_FORMAT(letter, format) "[" #letter "]<%s> :%u " format "\r\n", __FUNCTION__, __LINE__
#define log_info(format, ...) log_pcs.printf(LOGV_FORMAT(INFO, format), ##__VA_ARGS__)
#define log_warn(format, ...) log_pcs.printf(LOGV_FORMAT(WARN, format), ##__VA_ARGS__)
#define log_nomal(format, ...) log_pcs.printf(LOGV_FORMAT(DEBUG, format), ##__VA_ARGS__)
#define log_error(format, ...) log_pcs.printf(LOGV_FORMAT(ERROR, format), ##__VA_ARGS__)

class writeLog
{
private:
	FS *FSys;
	String path = "";

	void give(char *fmt, size_t size);
	String get_timestamp(void);
	String get_file_name(void);
	void write(String name, String data, size_t size);

public:
	writeLog(/* args */);
	~writeLog();

	void begin(fs::FS *fs, String path);
	void printf(const char *fmt, ...);
};

extern writeLog log_pcs;

#endif
