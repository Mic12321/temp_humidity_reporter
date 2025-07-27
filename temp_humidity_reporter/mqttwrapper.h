#ifndef MQTTWRAPPER_H
#define MQTTWRAPPER_H

#include <string>
#include <cstdint>

extern "C" {
#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"
}
#define LWIP_MQTT 1

class MqttWrapper {
public:
    MqttWrapper();
    ~MqttWrapper();

    bool init();
    bool connect(const std::string& broker_ip, uint16_t port);
    bool publish(const std::string& topic, const std::string& payload);
    void disconnect();

    bool isConnected() const;

private:
    mqtt_client_t* client;
    bool connected;

    static void mqttConnectionCb(mqtt_client_t* client, void* arg, mqtt_connection_status_t status);
};

#endif // MQTTWRAPPER_H
