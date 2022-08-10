#ifndef __HANDLE_FS_H
#define __HANDLE_FS_H

#include <Arduino.h>
#include <FS.h>
#include <stdint.h>
#include <stdbool.h>

class utilityFs : public Stream
{
private:
	fs::FS *_fs;
	String _fileName;

public:
	utilityFs(fs::FS &fs, const char *fileName) : _fs(&fs), _fileName(fileName) {}
	utilityFs(fs::FS &fs, String fileName) : _fs(&fs), _fileName(fileName) {}

	int available() override { return 0; }
	int read() override { return -1; }

	void listDir(void (*cbFileName)(const char *name, size_t size))
	{
		File root = _fs->open(_fileName.c_str());
		if (!root || !root.isDirectory())
			return;

		File file = root.openNextFile();
		while (file)
		{
			if (cbFileName != NULL)
				cbFileName(file.name(), file.size());

			file = root.openNextFile();
		}
	}

	int read(uint8_t *buff, size_t size)
	{
		// Nếu không có file
		if (_fs->exists(_fileName.c_str()) == false)
			return -1;

		File file = _fs->open(_fileName.c_str(), FILE_READ);
		if (!file)
			return -1;

		size_t _f_size = file.size();
		if (size == -1 || size > _f_size)
			size = _f_size;

		_f_size = file.read(buff, size);
		file.close();

		return _f_size;
	}
	int read(uint8_t *buff)
	{
		return read(buff, -1);
	}
	int peek() override { return -1; }
	void flush(void) override {}
	size_t write(uint8_t c) override
	{
		write(&c, 1);
		return 1;
	}
	size_t write(const uint8_t *buffer, size_t size) override
	{
		File fs_handle = _fs->open(_fileName, FILE_APPEND);
		fs_handle.write(buffer, size);
		fs_handle.close();
		return size;
	}
	void create()
	{
		_fs->open(_fileName, FILE_WRITE);
	}
	void clean()
	{
		_fs->remove(_fileName);
	}
	bool check()
	{
		return _fs->exists(_fileName);
	}
	size_t size()
	{
		File file = _fs->open(_fileName.c_str(), FILE_READ);
		if (!file)
			return -1;
		size_t _f_size = file.size();
		file.close();
		return _f_size;
	}
};

#endif