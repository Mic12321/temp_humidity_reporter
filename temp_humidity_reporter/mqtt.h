#ifndef MQTT_H
#define MQTT_H

#include <string>

class MQTTClient {
public:
    MQTTClient();
    ~MQTTClient();

    // Initialize MQTT client (network stack etc)
    bool init();

    // Connect to the MQTT broker
    bool connect();

    // Publish a message to topic
    bool publish(const std::string& topic, const std::string& payload);

    // Poll MQTT client tasks (socket, keepalive)
    void poll();

    // Disconnect cleanly
    void disconnect();

private:
    bool connected;
    // Add your internal client state here (sockets, buffers etc)
};

#endif // MQTT_H