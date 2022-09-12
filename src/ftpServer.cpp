
#include "ftpServer.h"

const char *list_ok[33] = {"100 ", "110 ", "120 ", "125 ", "150 ",
						   "200 ", "202 ", "211 ", "212 ", "213 ", "214 ", "215 ", "220 ", "221 ", "225 ", "226 ", "227 ", "228 ", "229 ", "230 ", "231 ", "232 ", "234 ", "250 ", "257 ",
						   "300 ", "331 ", "332 ", "350 ",
						   "600 ", "631 ", "632 ", "633 "};
const char *list_error[21] = {"400 ", "421 ", "425 ", "426 ", "430 ", "434 ", "450 ", "451 ", "452 ",
							  "500 ", "501 ", "502 ", "503 ", "504 ", "530 ", "532 ", "534 ", "550 ", "551 ", "552 ", "553 "};

const char *systern_windown = "Windows_NT";

ftpServer::ftpServer(const char *host, uint16_t port, const char *user, const char *pass) : _host(host), _port(port), _user(user), _pass(pass)
{
	this->client = new WiFiClient;
	this->dclient = new WiFiClient;
	this->timeout = 1000;

	this->connect(host, port, user, pass);
}
ftpServer::ftpServer(String host, uint16_t port, String user, String pass) : _host(host), _port(port), _user(user), _pass(pass)
{
	this->client = new WiFiClient;
	this->dclient = new WiFiClient;
	this->timeout = 1000;

	this->connect(host.c_str(), port, user.c_str(), pass.c_str());
}

ftpServer::~ftpServer()
{
	dbg_ftp("=====> ~ftpServer");
	this->disconnect();

	delete this->client;
	delete this->dclient;
}
bool ftpServer::wait_answer(void)
{ // uint8_t outCount = 0;
	this->outBuf = "";

	// unsigned long _m = millis();
	// while (!this->client->available() && millis() < _m + this->timeout)
	// 	delay(1);

	unsigned long _m = millis();
	while (millis() < _m + this->timeout)
	{
		if (this->client->available())
			this->outBuf += (char)this->client->read();
		else
			delay(1);
	}

	dbg_ftp("answer: %s", this->outBuf.c_str());

	// check ok
	for (uint8_t i = 0; i < 33; i++)
	{
		if (this->outBuf.indexOf(list_ok[i]) != -1)
		{
			dbg_ftp("answer OK %s", list_ok[i]);
			this->isConnect = true;
			return true;
		}
	}

	// check err
	for (uint8_t i = 0; i < 21; i++)
	{
		if (this->outBuf.indexOf(list_error[i]) != -1)
		{
			dbg_ftp("answer ERR %s", list_error[i]);
			this->isConnect = false;
			return false;
		}
	}

	this->isConnect = true;
	return true;
}

bool ftpServer::connect(const char *host, uint16_t port, const char *user, const char *pass)
{
	dbg_ftp("connectting");
	if (!this->client->connect(host, port, this->timeout))
	{
		_error = ftpServerErrConnect;
		return false;
	}
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrConnect;
		return false;
	}

	this->client->setTimeout(this->timeout);

	dbg_ftp("Send USER");
	this->client->print(F("USER "));
	this->client->println(user);
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrUser;
		return false;
	}

	dbg_ftp("Send PASS");
	this->client->print(F("PASS "));
	this->client->println(pass);
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrPass;
		return false;
	}

	dbg_ftp("Send SYST");
	this->client->println(F("SYST"));
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrSys;
		return false;
	}

	// nếu không phải hệ điều hành windown
	if (this->outBuf.indexOf(systern_windown) == -1)
	{
		dbg_ftp("Send PBSZ 0");
		this->client->println(F("PBSZ 0"));
		if (this->wait_answer() == false)
		{
			_error = ftpServerErrOS;
			return false;
		}

		dbg_ftp("Send PROT P");
		this->client->println(F("PROT P"));
		if (this->wait_answer() == false)
		{
			_error = ftpServerErrOS;
			return false;
		}
	}

	this->isConnect = true;
	return true;
}
void ftpServer::disconnect(void)
{
	this->isConnect = false;
	this->client->println(F("QUIT"));
	this->client->stop();
	this->dclient->stop();
	dbg_ftp("Connection closed");
}
void ftpServer::reconnect(void)
{
	this->disconnect();
	this->connect(_host.c_str(), _port, _user.c_str(), _pass.c_str());
}
void ftpServer::set_timeout(int32_t timeout)
{
	this->timeout = timeout;
}
bool ftpServer::init_file(const char *type)
{
	if (this->isConnect == false)
		return false;

	dbg_ftp("Send TYPE");
	this->client->println(F(type));
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrType;
		return false;
	}

	dbg_ftp("Send PASV");
	this->client->println(F("PASV"));
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrPasv;
		return false;
	}

	char *tStr = strtok((char *)this->outBuf.c_str(), "(,");
	int array_pasv[6];
	for (int i = 0; i < 6; i++)
	{
		tStr = strtok(NULL, "(,");
		if (tStr == NULL)
		{
			dbg_ftp("Bad PASV Answer");
			this->disconnect();
			return false;
		}
		array_pasv[i] = atoi(tStr);
	}

	IPAddress pasvServer(array_pasv[0], array_pasv[1], array_pasv[2], array_pasv[3]);
	uint16_t port = ((array_pasv[4] << 8) & 0xFF00) + (array_pasv[5] & 0x00FF);

	if (!this->dclient->connect(pasvServer, port, this->timeout))
	{
		this->disconnect();
		return false;
	}
	return true;
}
bool ftpServer::check_file(String name) // LIST, NLST
{
	if (this->isConnect == false)
		return false;

	if (this->init_file("TYPE I") == false)
		return false;

	dbg_ftp("Send NLST");
	this->client->print(F("NLST"));
	this->client->println(F(""));
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrListFile;
		return false;
	}

	unsigned long _m = millis();
	while (!this->dclient->available() && millis() < _m + timeout)
		delay(1);

	String list;
	bool result = false;
	while (this->dclient->available())
	{
		list = this->dclient->readStringUntil('\n');

		if (list.length() <= 0)
			break;

		// list.toLowerCase();
		dbg_ftp("File is: %s", list.c_str());

		if (list.indexOf(name) > -1)
			result = true;
	}

	this->close_file();
	return result;
}
void ftpServer::close_file(void)
{
	dbg_ftp("Close File");
	this->dclient->stop();
	// this->wait_answer();
	// dbg_ftp("answer %s", this->outBuf);
}
bool ftpServer::folder(String dir)
{
	if (this->isConnect == false)
		return false;
	dbg_ftp("Send CWD %s", dir.c_str());
	this->client->print(F("CWD "));
	this->client->println(dir);
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrCWD;
		return false;
	}
	return true;
}
bool ftpServer::listDir(String dir, void (*cbFileName)(const char *name, size_t size))
{
	if (this->isConnect == false)
		return false;
	dbg_ftp("Send MLSD %s", dir.c_str());
	this->client->print(F("MLSD"));
	this->client->println(dir);
	dbg_ftp("1");
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrListDir;
		return false;
	}

	unsigned long _m = millis();
	dbg_ftp("2");
	while (!this->client->available() && millis() < _m + timeout)
		delay(1);
	dbg_ftp("3");

	String result;
	while (this->client->available())
	{
		result = this->client->readStringUntil('\n');
		dbg_ftp("Result is: %s, size %d", result.c_str(), result.length());
		if (cbFileName != NULL)
			cbFileName(result.c_str(), result.length());
	}
	dbg_ftp("4");
	return true;
}

// pull
size_t ftpServer::size(String name)
{
	dbg_ftp("Send SIZE %s", name.c_str());
	this->client->print(F("SIZE "));
	this->client->println(name);

	unsigned long _m = millis();
	while (!this->client->available() && millis() < _m + timeout)
		delay(1);

	String result;
	while (this->client->available())
	{
		result = this->client->readStringUntil('\n');
		if (result.length() <= 0)
			return 0;

		dbg_ftp("Result is: %s, size %d", result.c_str(), result.length());

		uint8_t idResult = result.indexOf("213 ") + 4;
		return result.substring(idResult, result.length()).toInt();
	}
	return 0;
}
bool ftpServer::start_download(String name)
{
	if (this->isConnect == false)
		return false;

	if (this->init_file("TYPE I") == false)
		return false;

	dbg_ftp("Send RETR");
	this->client->print(F("RETR "));
	this->client->println(name);
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrRetr;
		return false;
	}

	return true;
}
size_t ftpServer::downloading(uint8_t *data, size_t length)
{
	if (this->isConnect == false)
		return false;

	unsigned long _m = millis();
	while (!this->dclient->available() && millis() < _m + this->timeout)
		delay(1);

	if (millis() >= _m + timeout)
	{
		dbg_ftp("Wait data timeout");
		return 0;
	}

	return this->dclient->readBytes(data, length);
}
void ftpServer::stop_download(void)
{
	this->close_file();
}

// push
void ftpServer::new_file(String name)
{
	if (this->isConnect == false)
		return;
	dbg_ftp("Send STOR %s", name);
	this->client->print(F("STOR "));
	this->client->println(name);
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrCreatFile;
		return;
	}
}
void ftpServer::append_file(String name)
{
	if (this->isConnect == false)
		return;
	dbg_ftp("Send APPE %s", name);
	this->client->print(F("APPE "));
	this->client->println(name);
	if (this->wait_answer() == false)
	{
		_error = ftpServerErrWriteAppend;
		return;
	}
}

bool ftpServer::uploading(String name, uint8_t *data, size_t length)
{
#define bufferSize 1500
	if (this->isConnect == false)
		return false;

	// phân tích directory và file name
	const char *file_name = name.c_str();
	while (strchr(file_name, '/'))
		file_name += 1;

	char directory[name.length()];
	memset(directory, 0, sizeof(directory));
	strncpy(directory, name.c_str(), name.length() - strlen(file_name));

	if (strlen(directory) > 1)
		directory[strlen(directory) - 1] = 0;

	// dbg_ftp(" %d - %d", name.length(), strlen(file_name));
	// dbg_ftp("directory %s", directory);
	// dbg_ftp("file_name %s", file_name);

	// thay đổi thư mực sử lý
	this->folder(String(directory));
	bool create_file = this->check_file(file_name);

	// cấu hình type
	if (this->init_file("TYPE I") == false)
		return false;

	// tạo file hoặc ghi thêm file
	if (create_file == false)
		this->new_file(file_name);
	else
		this->append_file(file_name);

	// ghi dữ liệu vào file
	size_t size_appent = bufferSize;
	while (length)
	{
		if (size_appent > length)
			size_appent = length;

		if (this->dclient->write(data, size_appent) != size_appent)
		{
			dbg_ftp("upload data ftp error");
			this->close_file();
			return false;
		}

		dbg_ftp("upload ok %d", length);
		data += size_appent;
		length -= size_appent;
	}
	this->close_file();
	return true;
}