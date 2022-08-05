

#ifndef __HANDLE_OTA_H
#define __HANDLE_OTA_H

// #include <FtpClientUpdate.h>
// #include "netif/ppp/polarssl/md5.h"
#include <Update.h>

#include "ftpServer.h"

#define dbg_ota(fmt, ...) Serial.printf(PSTR("[OTA] " fmt "\r\n"), ##__VA_ARGS__)

typedef enum
{
	OTA_ERROR_NULL = 0x4500,	   // không có lỗi nào
	OTA_ERROR_SERVER = 0x4501,	   // không kết nối được ftp server
	OTA_ERROR_SIZEFILE = 0x4502,   // lấy kích thước file .bin lỗi
	OTA_ERROR_FLASH = 0x4503,	   // kiểm tra phân vùng flash ota lỗi
	OTA_ERROR_RAM = 0x4504,		   // xin ram để load file lỗi
	OTA_ERROR_DOWNLOAD = 0x4505,   // tải block data khộng đạt được size mong muốn
	OTA_ERROR_WRITE = 0x4506,	   // ghi vào phân vùng flash ota lỗi
	OTA_ERROR_MD5 = 0X4507,		   // check md5 lỗi
	OTA_UPDATE_SUCCESSFUL = 0x4508 // update thành công
} ota_error_t;

class ftp2ota
{
private:
	ftpServer *_ftp;
	bool isUpdate = false;

	String file = "";
	size_t size = 0;
	ota_error_t error;

public:
	ftp2ota(ftpServer &ftp) : _ftp(&ftp) {}

	bool update(String path, String file, String md5);
	bool update(const char *path, const char *file, const char *md5)
	{
		return this->update(String(path), String(file), String(md5));
	}
	void handle(void);
	ota_error_t get_error(void)
	{
		return error;
	}
};

#endif // __HANDLE_OTA_H
