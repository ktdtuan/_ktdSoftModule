#ifndef __HANDLE_SYNC_H
#define __HANDLE_SYNC_H

#include "ftpServer.h"
#include "utilityFs.h"

#define dbg_download(fmt, ...) Serial.printf(PSTR("[Download] " fmt "\r\n"), ##__VA_ARGS__)

typedef enum
{
	ftp2fsErrNull = 0,
	ftp2fsErrConnect,
	ftp2fsErrFolder,
	ftp2fsErrFile,
	ftp2fsErrSize,
	ftp2fsErrStart,
	ftp2fsErrRam,
	ftp2fsErrDown,
	ftp2fsErrWrite,
} ftp2fsErrEvent_t;

class ftp2fs
{
private:
	ftpServer *_from;
	utilityFs *_to;
	ftp2fsErrEvent_t _error = ftp2fsErrNull;

public:
	ftp2fs(ftpServer &from, utilityFs &to) : _from(&from), _to(&to) {}

	bool download(String from_path, String from_file, String md5)
	{
		// check connected
		if (_from->isConnected() == false)
		{
			_error = ftp2fsErrConnect;
			return false;
		}

		// select forlder in ftp
		if (_from->folder(from_path) == false)
		{
			_error = ftp2fsErrFolder;
			return false;
		}

		// check file in ftp
		if (_from->check_file(from_file) == false)
		{
			_error = ftp2fsErrFile;
			return false;
		}

		// read size of file. prepare size to download
		size_t size = _from->size(from_file);
		dbg_download("size %d", size);
		if (size == 0)
		{
			_error = ftp2fsErrSize;
			return false;
		}

		// start download
		if (_from->start_download(from_file) == false)
		{
			_error = ftp2fsErrStart;
			return false;
		}

		// importune ram free
		size_t size_appent = 4096;
		uint8_t *buff = (uint8_t *)malloc(size_appent + 10);
		if (buff == NULL)
		{
			_error = ftp2fsErrRam;
			return false;
		}

		// check and delete file in memory local
		if (_to->check() == true)
			_to->clean();
		_to->create();

		while (size)
		{
			if (size_appent > size)
				size_appent = size;

			// read data to ftp
			if (_from->downloading(buff, size_appent) != size_appent)
			{
				dbg_download("download error");
				_error = ftp2fsErrDown;
				break;
			}

			// write data to memory local
			if (_to->write(buff, size_appent) != size_appent)
			{
				dbg_download("write error");
				_error = ftp2fsErrWrite;
				break;
			}

			size -= size_appent;
			dbg_download("load OK. size append: %d", size);

			delay(1);
			yield();
		}

		// stop download
		free(buff);
		_from->stop_download();

		if (size != 0)
			return false;
		return true;
	}
	bool download(const char *from_path, const char *from_file, const char *md5)
	{
		return download(String(from_path), String(from_file), String(md5));
	}
	bool download(String from_path, String from_file)
	{
		return download(from_path, from_file, "");
	}
	bool download(String from_file)
	{
		return download("", from_file, "");
	}

	ftp2fsErrEvent_t getError(void)
	{
		return _error;
	}
};
#endif