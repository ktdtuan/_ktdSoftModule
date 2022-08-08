// #ifndef __HANDLE_SYNC_H
// #define __HANDLE_SYNC_H

#include "ftp2fs.h"

// /**
//  * @brief syncFtp2fs là chương trình đồng bộ dữ liệu từ FTP về thư mục hệ thống.
//  * Thực hiện theo các bước
//  * 1. connected FTP server
//  * 2. select thư mục trên FTP server
//  * 3. select hoặc tạo thư mục trên bộ nhớ local
//  *
//  * Quy trình đồng bộ
//  * 1. đổi tên file lastupdate.json ở thư mục bộ nhớ local
//  * 2. tải file lastupdate.json từ ftp server
//  * 3. so sánh các dữ liệu về file hệ thống
//  * 4. nếu khác thì tải file hệ thống đó với tên được đính kèm là downloading
//  * 5. tải xong thì check md5, nếu lỗi thì xóa tải lại
//  */

// #define dbg_sync(...) dbg_msg(__VA_ARGS__)

// class syncFtp2fs
// {
// private:
// 	FS *FSys;
// 	ftpServer *ftp;

// 	bool connected = false;
// 	bool isSyncing = false;

// 	String path;

// 	const char *lastupdate_file = "/lastupdate.json";

// 	void download_lastupdate(void);
// 	void download_file(String file_ftp, String file_disk, size_t file_size);

// public:
// 	syncFtp2fs(/* args */);
// 	~syncFtp2fs();

// 	bool connect(String host, uint16_t port, String user, String pass);
// 	void set_folder(String ftp, fs::FS *fs, String path);
// 	void loop(void);
// };

// #endif