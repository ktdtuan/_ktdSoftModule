// #include "syncFtp2fs.h"

// syncFtp2fs::syncFtp2fs(/* args */) {}
// syncFtp2fs::~syncFtp2fs() {}

// bool syncFtp2fs::connect(String host, uint16_t port, String user, String pass)
// {
// 	this->ftp = new ftpServer();

// 	this->connected = true;
// 	if (this->ftp->connect(host, port, user, pass) == false)
// 	{
// 		delete this->ftp;
// 		this->connected = false;
// 	}
// 	return this->connected;
// }

// void syncFtp2fs::set_folder(String ftp, fs::FS *fs, String path)
// {
// 	if (this->connected == false)
// 		return;

// 	this->ftp->folder(ftp);

// 	this->FSys = fs;
// 	this->path = (path.length() > 1) ? path : "";

// 	if (this->FSys->exists(this->path) == false)
// 		this->FSys->mkdir(this->path);
// }

// void syncFtp2fs::download_file(String file_ftp, String file_disk, size_t file_size)
// {
// 	size_t size_appent = 4096;
// 	uint8_t *buff = (uint8_t *)malloc(size_appent);

// 	if (buff == NULL)
// 	{
// 		this->isSyncing = false;
// 		delete this->ftp;
// 		return;
// 	}

// 	if (this->ftp->start_download(file_ftp) == false)
// 	{
// 		this->isSyncing = false;
// 		delete this->ftp;
// 		return;
// 	}

// 	dbg_sync("start sync file %s. size %d", file_disk.c_str(), file_size);

// 	while (file_size)
// 	{
// 		if (size_appent > file_size)
// 			size_appent = file_size;

// 		if (this->ftp->downloading(buff, size_appent) != size_appent)
// 		{
// 			dbg_sync("download error");
// 			this->isSyncing = false;
// 			free(buff);
// 			this->ftp->stop_download();
// 			return;
// 		}

// 		file_size -= size_appent;
// 		dbg_sync("download OK. rest: %u byte", file_size);

// 		delay(1);
// 		yield();
// 	}
// 	free(buff);

// 	this->ftp->stop_download();
// 	return;
// }

// void syncFtp2fs::download_lastupdate(void)
// {
// 	String file_sync_config = this->path + lastupdate_file;

// 	if (this->FSys->exists(file_sync_config) == false)
// 		this->FSys->remove(file_sync_config);

// 	if (!this->FSys->open(file_sync_config, FILE_WRITE))
// 		this->isSyncing = false;

// 	if (ftp->check_file("lastupdate.json") == true)
// 	{
// 		size_t size = ftp->size(String("lastupdate.json"));
// 		download_file(String("lastupdate.json"), file_sync_config, size);
// 	}
// 	else
// 		this->isSyncing = false;
// }

// void syncFtp2fs::loop(void)
// {
// 	if (this->connected == false)
// 		return;
// 	this->connected = false;
// 	this->isSyncing = true;

// 	dbg_sync("start sync file");

// 	this->download_lastupdate();
// 	// while (this->isSyncing)
// 	// {
// 	// }

// 	this->ftp->disconnect();
// }
