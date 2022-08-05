#include "writeLog.h"

writeLog::writeLog(/* args */) {}

writeLog::~writeLog() {}

void writeLog::begin(fs::FS *fs, String path)
{
	this->FSys = fs;
	this->path = path;

	if (this->FSys->exists(this->path) == false)
		this->FSys->mkdir(this->path);
}

void writeLog::printf(const char *fmt, ...)
{
	char loc_buf[64];
	char *temp = loc_buf;
	va_list arg;
	va_list copy;
	va_start(arg, fmt);
	va_copy(copy, arg);
	int len = vsnprintf(temp, sizeof(loc_buf), fmt, copy);
	va_end(copy);
	if (len < 0)
	{
		va_end(arg);
		return;
	}
	if (len >= sizeof(loc_buf))
	{
		temp = (char *)malloc(len + 1);
		if (temp == NULL)
		{
			va_end(arg);
			return;
		}
		len = vsnprintf(temp, len + 1, fmt, arg);
	}
	va_end(arg);

	this->give(temp, len);

	if (temp != loc_buf)
		free(temp);
}

void writeLog::give(char *data, size_t size)
{
	if (size == 0)
		return;
	dbg_log("start");

	String log_data = this->get_timestamp() + " " + String(data);
	String file_name = this->get_file_name();

	// tạo tên file theo ngày
	// sd_pcs.write_append(buffer, (uint8_t *)log_data.c_str(), log_data.length());
	this->write(file_name, log_data, log_data.length());
}

String writeLog::get_timestamp(void)
{
	struct tm time;
	if (!getLocalTime(&time))
	{
		static uint32_t cnt = 0;
		return String(cnt++);
	}

	char buffer[80];
	// tạo timestamp cho từng gói tin
	strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &time);
	return String(buffer);
}

String writeLog::get_file_name(void)
{
	struct tm time;
	if (!getLocalTime(&time))
		return String("/log.txt");

	char buffer[80];

	strftime(buffer, sizeof(buffer), "/%Y_%m_%d_log.txt", &time);
	return String(buffer);
}

void writeLog::write(String name, String data, size_t size)
{
	String link = this->path + name;
	dbg_log("%s write %d: %s", link.c_str(), size, data.c_str());

	File file;
	if (this->FSys->exists(link))
		file = this->FSys->open(link, FILE_APPEND);
	else
		file = this->FSys->open(link, FILE_WRITE);

	// dbg_log("create file %s\r\n", link);
	// check nếu không mở được file
	if (!file)
	{
		file.close();
		return;
	}

	dbg_log("finish");
	file.write((uint8_t *)data.c_str(), size);
	file.close();
}

writeLog log_pcs;
