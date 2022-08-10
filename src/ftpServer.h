#ifndef __HANDLE_FTP_H__
#define __HANDLE_FTP_H__

#include <WiFiClient.h>

#define dbg_ftp(fmt, ...) Serial.printf(PSTR("[FTP] " fmt "\r\n"), ##__VA_ARGS__)

class ftpServer
{
private:
	WiFiClient *client;
	WiFiClient *dclient;

	// Login
	String _host;
	uint16_t _port;
	String _user;
	String _pass;

	// status
	int32_t timeout;
	bool isConnect = false;

	// CMD reply
	String outBuf = "";

	// handle
	bool init_file(const char *type);
	void close_file(void);

	// answer
	bool wait_answer(void);

	// push
	void new_file(String name);
	void append_file(String name);

public:
	ftpServer(const char *host, uint16_t port, const char *user, const char *pass);
	ftpServer(String host, uint16_t port, String user, String pass);
	~ftpServer();

	bool connect(const char *host, uint16_t port, const char *user, const char *pass);
	void disconnect(void);
	void reconnect(void);
	void set_timeout(int32_t timeout);
	bool check_file(String name);
	size_t size(String name);
	bool folder(String dir);
	bool listDir(String dir, void (*cbFileName)(const char *name, size_t size));

	// check
	bool isConnected(void)
	{
		return isConnect;
	}

	// pull
	bool start_download(String name);
	size_t downloading(uint8_t *data, size_t length);
	void stop_download(void);

	// push
	bool uploading(String name, uint8_t *data, size_t length);
};

#endif