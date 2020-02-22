#ifdef USE_MQTT

#include <AsyncMqttClient.h>
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>
#include "Constants.h"
#include "Types.h"

#define ROOM_MAME             "bedroom"
#define STATE_TOPIC           "home/" ROOM_MAME "/light/led"
#define COMMAND_TOPIC         STATE_TOPIC "/set"
#define AVAILABILITY_TOPIC    STATE_TOPIC "/available"

#define MQTT_SERVER           "nas"
//#define MQTT_USER             ""
//#define MQTT_PASSWORD         ""
#define MQTT_PORT             1883U
#define MQTT_CLIENT_ID        ROOM_MAME "_lamp"

class MqttManager
{
  public:
    static uint32_t mqttLastConnectingAttempt;
    static void setupMqtt(AsyncMqttClient* mqttClient, char* lampInputBuffer);
    static void mqttConnect();
    static void onMqttConnect(bool sessionPresent);
    static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    static void onMqttMessage(char *topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    static bool publish(const char *topic, const char *value);
    static void publishState();
    static bool needToPublish;
    static char mqttBuffer[MAX_BUFFER_SIZE];
  private:
//    static char* mqttServer;
//    static char* mqttUser;
//    static char* mqttPassword;
//    static char* topicInput;                                                    // TopicBase + '/' + MqttClientId + '/' + TopicCmnd
//    static char* topicOutput;                                                   // TopicBase + '/' + MqttClientId + '/' + TopicState
//    static char* clientId;
    static char* lampInputBuffer;                                               // ссылка на inputBuffer для записи в него пришедшей MQTT команды и последующей её обработки лампой
    static AsyncMqttClient* mqttClient;
    static const uint8_t qos = 0U;                                              // MQTT quality of service
    static const uint32_t connectionTimeout = MQTT_RECONNECT_TIME * 1000U;      // период времени для проверки (пере)подключения к MQTT брокеру, 10 секунд
//    static char* byteToHex(char *out, uint8_t value);
//    static bool allocStr(char **str, const char *src);
//    static bool allocStr_P(char **str, PGM_P src);
    static void sendCurrentJson(char* buffer);
    static void updateCurrentJson(char* buffer, size_t len);
    
};


void MqttManager::setupMqtt(AsyncMqttClient* mqttClient, char* lampInputBuffer)
{
  MqttManager::mqttClient = mqttClient;
  MqttManager::mqttClient->setServer(MQTT_SERVER, MQTT_PORT);
  MqttManager::mqttClient->setClientId(MQTT_CLIENT_ID);
#ifdef MQTT_USER
  MqttManager::mqttClient->setCredentials(MQTT_USER, MQTT_PASSWORD);
#endif

#ifdef GENERAL_DEBUG
  LOG.printf_P(PSTR("MQTT command topic: %s\n"), COMMAND_TOPIC);
  LOG.printf_P(PSTR("MQTT state topic: %s\n"), STATE_TOPIC);
  LOG.printf_P(PSTR("MQTT availability topic: %s\n"), AVAILABILITY_TOPIC);
#endif

  mqttClient->onConnect(onMqttConnect);
  mqttClient->onDisconnect(onMqttDisconnect);
  mqttClient->onMessage(onMqttMessage);
}

void MqttManager::mqttConnect()
{
  if ((!mqttLastConnectingAttempt) || (millis() - mqttLastConnectingAttempt >= connectionTimeout))
  {
#ifdef GENERAL_DEBUG
    LOG.print(F("Connection to MQTT brocker \""));
    LOG.print(MQTT_SERVER);
    LOG.print(':');
    LOG.print(MQTT_PORT);
    LOG.println(F("\"..."));
#endif
    mqttClient->disconnect();
    mqttClient->connect();
    mqttLastConnectingAttempt = millis();
  }
}

bool MqttManager::publish(const char *topic, const char *value)
{
  if (mqttClient->connected())
  {
#ifdef GENERAL_DEBUG
    LOG.print(F("Send MQTT: topic \""));
    LOG.print(topic);
    LOG.print(F("\", value \""));
    LOG.print(value);
    LOG.println('"');
    LOG.println();
#endif

    return mqttClient->publish(topic, qos, true, value, 0) != 0;
  }

  return false;
}

void MqttManager::onMqttConnect(bool sessionPresent)
{
#ifdef GENERAL_DEBUG
  LOG.println(F("Connected to MQTT broker"));
#endif
  mqttLastConnectingAttempt = 0;

  mqttClient->setWill(AVAILABILITY_TOPIC,0,true,"offline");
  mqttClient->subscribe(COMMAND_TOPIC, 1);
  publish(AVAILABILITY_TOPIC,"online");
  publishState();
}

void MqttManager::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
#ifdef GENERAL_DEBUG
  LOG.println(F("Disconnected from MQTT broker"));
#endif
}

void MqttManager::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{

#ifdef GENERAL_DEBUG
  LOG.print(F("recived MQTT:"));
  LOG.print(F(" topic \""));
  LOG.print(topic);
  LOG.print("\"");
  /*
  LOG.print(F(" qos: "));
  LOG.println(properties.qos);
  LOG.print(F(" dup: "));
  LOG.println(properties.dup);
  LOG.print(F(" retain: "));
  LOG.println(properties.retain);
  LOG.print(F(" len: "));
  LOG.println(len);
  LOG.print(F(" index: "));
  LOG.println(index);
  LOG.print(F(" total: "));
  LOG.println(total);
  */
  LOG.print(F(", value \""));
  for(int i=0;i<len;i++){
    LOG.print(payload[i]);
  }
  //LOG.print(lampInputBuffer);
  LOG.println("\"");
  LOG.println();
#endif

  //LOG.println((const char*)doc["state"]);
  updateCurrentJson(payload,len);
  needToPublish = true;
}

void MqttManager::publishState()
{
  if (mqttBuffer == NULL || strlen(mqttBuffer) <= 0)
  {
    sendCurrentJson(mqttBuffer);
  }

  if (mqttBuffer != NULL && strlen(mqttBuffer) > 0)
  {
    publish(STATE_TOPIC, mqttBuffer);                       // публикация буфера MQTT ответа
    mqttBuffer[0] = '\0';                                   // очистка буфера
    needToPublish = false;                                  // сброс флага для предотвращения повторной публикации
  }
}

#endif
