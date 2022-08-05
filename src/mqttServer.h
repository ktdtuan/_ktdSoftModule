#ifndef __MQTT_SERVER_H_
#define __MQTT_SERVER_H_
#include <WiFi.h>
#include <functional>
#include <PubSubClient.h>

#define MQTT_TIME_RECHECK_CONNECT 100
typedef std::function<void(const char *topic, const byte *data, size_t length)> rxCallback;

class mqttServer
{
private:
	bool _isConnect = false;
	String _user;
	String _pass;
	String _supTopic;

	WiFiClient *_mqttClient;
	PubSubClient *_mqttPubSub;

public:
	mqttServer(const char *server, uint16_t port, const char *user, const char *pass) : _user(_user), _pass(pass)
	{
		_mqttClient = new WiFiClient;
		_mqttPubSub = new PubSubClient(server, port, *_mqttClient);
	}
	~mqttServer()
	{
		delete _mqttClient;
		delete _mqttPubSub;
	}

	void subscribe(const char *topic, rxCallback rxCb)
	{
		if (rxCb != NULL)
			_mqttPubSub->setCallback(rxCb);

		// set topic subrice and public
		if (topic != NULL)
			_supTopic = String(topic);
	}

	void unsubscribe(const char *topic)
	{
		_mqttPubSub->unsubscribe(topic);
	}

	void loop(void)
	{
		if (_isConnect == true)
			_isConnect = _mqttPubSub->loop(); // should be called
		else if (WiFi.status() == WL_CONNECTED)
		{
			_mqttPubSub->disconnect();
			_isConnect = _mqttPubSub->connect(WiFi.macAddress().c_str(), _user.c_str(), _pass.c_str());

			if (_isConnect == true && _supTopic!= NULL)
			{
				_mqttPubSub->unsubscribe(_supTopic.c_str());
				_mqttPubSub->subscribe(_supTopic.c_str());
			}
		}
	}

	bool printf(const char *topic, const char *fmt, ...)
	{
		if (topic == NULL)
			return false;
		if (_isConnect == false)
			return false;

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
			return false;
		};
		if (len >= sizeof(loc_buf))
		{
			temp = (char *)malloc(len + 1);
			if (temp == NULL)
			{
				va_end(arg);
				return false;
			}
			len = vsnprintf(temp, len + 1, fmt, arg);
		}
		va_end(arg);

		bool result = _mqttPubSub->publish(topic, temp, len);

		if (temp != loc_buf)
			free(temp);

		return result;
	}
};

#endif