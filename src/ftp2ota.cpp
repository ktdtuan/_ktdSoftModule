#include "ftp2ota.h"

bool ftp2ota::update(String path, String file, String md5)
{
	// check FTP connect
	this->isUpdate = false;
	this->error = OTA_ERROR_SERVER;
	if (_ftp->isConnected() == false)
		return false;

	dbg_ota("update file %s%s", path.c_str(), file.c_str());
	this->file = file;

	if (_ftp->folder(path) == false)
		return false;
	if (_ftp->check_file(file) == false)
		return false;

	// check file size
	this->error = OTA_ERROR_SIZEFILE;
	this->size = _ftp->size(this->file);
	dbg_ota("size %d", this->size);
	if (this->size == 0)
		return false;

	// update
	this->error = OTA_ERROR_FLASH;
	dbg_ota("Begin update");
	if (!Update.begin(this->size, U_FLASH))
		return false;

	// Set MD5
	md5.toLowerCase();
	Update.setMD5(md5.c_str());

	this->isUpdate = true;
	return true;
}

void ftp2ota::handle(void)
{
	if (this->isUpdate == false)
		return;
	this->isUpdate = false;

	this->error = OTA_ERROR_SERVER;
	if (_ftp->start_download(this->file) == false)
		return;

	this->error = OTA_ERROR_RAM;
	size_t size_appent = 4096;
	uint8_t *buff = (uint8_t *)malloc(size_appent);
	if (buff == NULL)
		return;

	while (this->size)
	{
		if (size_appent > this->size)
			size_appent = this->size;

		if (_ftp->downloading(buff, size_appent) != size_appent)
		{
			this->error = OTA_ERROR_DOWNLOAD;
			dbg_ota("download error");
			break;
		}

		if (Update.write(buff, size_appent) != size_appent)
		{
			this->error = OTA_ERROR_WRITE;
			dbg_ota("update error at byte %d", this->size);
			break;
		}

		this->size -= size_appent;
		dbg_ota("update: %u OK", this->size);

		delay(1);
		yield();
	}
	free(buff);

	_ftp->stop_download();
	_ftp->disconnect();
	if (Update.end(true) == false)
	{
		this->error = (ota_error_t)Update.getError();
		return;
	}

	this->error = OTA_UPDATE_SUCCESSFUL;
	dbg_ota("Update Success: \nReboot to finish\n");
}
