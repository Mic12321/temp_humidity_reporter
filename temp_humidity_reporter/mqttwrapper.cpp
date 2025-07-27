#include "MqttWrapper.h"
#include <cstdio>
#include <cstring>

extern "C" {
#include "lwip/err.h"
#include "lwip/ip_addr.h"
}

MqttWrapper::MqttWrapper() : client(nullptr), connected(false) {}

MqttWrapper::~MqttWrapper() {
    disconnect();
    if (client) {
        mqtt_client_free(client);
        client = nullptr;
    }
}

bool MqttWrapper::init() {
    client = mqtt_client_new();
    if (!client) {
        printf("MQTT: Failed to allocate client\n");
        return false;
    }
    return true;
}

bool MqttWrapper::connect(const std::string& broker_ip_str, uint16_t port) {
    if (!client) return false;

    ip_addr_t broker_ip;
    if (!ipaddr_aton(broker_ip_str.c_str(), &broker_ip)) {
        printf("MQTT: Invalid broker IP address\n");
        return false;
    }

    static const mqtt_connect_client_info_t client_info = {
        "pico_client",  // client id
        nullptr,        // will_topic
        nullptr,        // will_msg
        0,              // will_qos
        0,              // will_retain
        nullptr,        // user_name
        0,              // password
        0               // keep_alive (0 = default)
    };

    err_t err = mqtt_client_connect(client, &broker_ip, port, mqttConnectionCb, this, &client_info);
    if (err != ERR_OK) {
        printf("MQTT: mqtt_connect error: %d\n", err);
        return false;
    }

    return true;
}

void MqttWrapper::mqttConnectionCb(mqtt_client_t* client, void* arg, mqtt_connection_status_t status) {
    MqttWrapper* self = static_cast<MqttWrapper*>(arg);

    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("MQTT: Connected successfully\n");
        self->connected = true;
    } else {
        printf("MQTT: Connection failed or lost, status: %d\n", status);
        self->connected = false;
    }
}

bool MqttWrapper::publish(const std::string& topic, const std::string& payload) {
    if (!connected) {
        printf("MQTT: Not connected, cannot publish\n");
        return false;
    }

    err_t err = mqtt_publish(client, topic.c_str(), payload.c_str(),
                             payload.size(), 1, 0, nullptr, nullptr);
    if (err != ERR_OK) {
        printf("MQTT: Publish failed, error %d\n", err);
        return false;
    }
    return true;
}

void MqttWrapper::disconnect() {
    if (client && connected) {
        mqtt_disconnect(client);
        connected = false;
    }
}

bool MqttWrapper::isConnected() const {
    return connected;
}
