/*
  sketch is based on https://alexgyver.ru/GyverLamp/
  GitHub: https://github.com/AlexGyver/GyverLamp/
  autor: AlexGyver, AlexGyver Technologies, 2019
  https://AlexGyver.ru/
*/

#include "pgmspace.h"
#include "Constants.h"
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include "Types.h"
#include "timerMinim.h"
#include <math.h>

#include <GyverButton.h>

#include "fonts.h"

#ifdef USE_NTP

#include <NTPClient.h>
#include <Timezone.h>

#endif

#include <TimeLib.h>

#ifdef OTA

#include "OtaManager.h"

#endif
#if USE_MQTT

#include "MqttManager.h"

#endif

#include "TimerManager.h"
#include "FavoritesManager.h"
#include "EepromManager.h"


// --- ИНИЦИАЛИЗАЦИЯ ОБЪЕКТОВ ----------
CRGB leds[NUM_LEDS];
WiFiUDP Udp;

#ifdef USE_NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, 0,
                     NTP_INTERVAL); // объект, запрашивающий время с ntp сервера; в нём смещение часового пояса не используется (перенесено в объект localTimeZone); здесь всегда должно быть время UTC
#ifdef SUMMER_WINTER_TIME
TimeChangeRule summerTime = { SUMMER_TIMEZONE_NAME, SUMMER_WEEK_NUM, SUMMER_WEEKDAY, SUMMER_MONTH, SUMMER_HOUR, SUMMER_OFFSET };
TimeChangeRule winterTime = { WINTER_TIMEZONE_NAME, WINTER_WEEK_NUM, WINTER_WEEKDAY, WINTER_MONTH, WINTER_HOUR, WINTER_OFFSET };
Timezone localTimeZone(summerTime, winterTime);
#else
TimeChangeRule localTime = {LOCAL_TIMEZONE_NAME, LOCAL_WEEK_NUM, LOCAL_WEEKDAY, LOCAL_MONTH, LOCAL_HOUR, LOCAL_OFFSET};
Timezone localTimeZone(localTime);
#endif
#endif

timerMinim timeTimer(3000);
bool ntpServerAddressResolved = false;
bool timeSynched = false;
uint32_t lastTimePrinted = 0U;


GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

#ifdef OTA
OtaManager otaManager(&showWarning);
OtaPhase OtaManager::OtaFlag = OtaPhase::None;
#endif

#if USE_MQTT
AsyncMqttClient *mqttClient = NULL;
AsyncMqttClient *MqttManager::mqttClient = NULL;
char *MqttManager::lampInputBuffer = NULL;
bool MqttManager::needToPublish = false;
char MqttManager::mqttBuffer[] = {};
uint32_t MqttManager::mqttLastConnectingAttempt = 0;
#endif

// --- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ -------
//uint16_t localPort = ESP_UDP_PORT;
//char packetBuffer[MAX_BUFFER_SIZE];                     // buffer to hold incoming packet
char inputBuffer[MAX_BUFFER_SIZE];
static const uint8_t maxDim = max(WIDTH, HEIGHT);

ModeType modes[MODE_AMOUNT];
AlarmType alarms[7];

static const uint8_t dawnOffsets[]
PROGMEM = {5, 10, 15, 20, 25, 30, 40, 50,
           60};   // опции для выпадающего списка параметра "время перед 'рассветом'" (будильник); синхронизировано с android приложением
uint8_t dawnMode;
bool dawnFlag = false;
uint32_t thisTime;
bool manualOff = false;

int8_t currentMode = 0;
bool loadingFlag = true;
bool ONflag = false;
uint32_t eepromTimeout;
bool settChanged = false;
//bool buttonEnabled = true;

unsigned char matrixValue[8][16];

bool TimerManager::TimerRunning = false;
bool TimerManager::TimerHasFired = false;
uint8_t TimerManager::TimerOption = 1U;
uint64_t TimerManager::TimeToFire = 0ULL;

uint8_t FavoritesManager::FavoritesRunning = 0;
uint16_t FavoritesManager::Interval = DEFAULT_FAVORITES_INTERVAL;
uint16_t FavoritesManager::Dispersion = DEFAULT_FAVORITES_DISPERSION;
uint8_t FavoritesManager::UseSavedFavoritesRunning = 0;
uint8_t FavoritesManager::FavoriteModes[MODE_AMOUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                        0, 0, 0, 0};
uint32_t FavoritesManager::nextModeAt = 0UL;


void setup() {
    Serial.begin(115200);
    Serial.println();
    ESP.wdtEnable(WDTO_8S);


    // ПИНЫ
#ifdef MOSFET_PIN                                         // инициализация пина, управляющего MOSFET транзистором в состояние "выключен"
    pinMode(MOSFET_PIN, OUTPUT);
#ifdef MOSFET_LEVEL
    digitalWrite(MOSFET_PIN, !MOSFET_LEVEL);
#endif
#endif

#ifdef ALARM_PIN                                          // инициализация пина, управляющего будильником в состояние "выключен"
    pinMode(ALARM_PIN, OUTPUT);
#ifdef ALARM_LEVEL
    digitalWrite(ALARM_PIN, !ALARM_LEVEL);
#endif
#endif


    // TELNET
#if defined(GENERAL_DEBUG) && GENERAL_DEBUG_TELNET
    telnetServer.begin();
    for (uint8_t i = 0; i < 100; i++)                         // пауза 10 секунд в отладочном режиме, чтобы успеть подключиться по протоколу telnet до вывода первых сообщений
    {
      handleTelnetClient();
      delay(100);
      ESP.wdtFeed();
    }
#endif


    // КНОПКА
    touch.setStepTimeout(BUTTON_STEP_TIMEOUT);
    touch.setClickTimeout(BUTTON_CLICK_TIMEOUT);
#if ESP_RESET_ON_START
    delay(1000);                                            // ожидание инициализации модуля кнопки ttp223 (по спецификации 250мс)
    if (digitalRead(BTN_PIN))
    {
      wifiManager.resetSettings();                          // сброс сохранённых SSID и пароля при старте с зажатой кнопкой, если разрешено
      LOG.println(F("Настройки WiFiManager сброшены"));
    }
    //buttonEnabled = true;                                   // при сбросе параметров WiFi сразу после старта с зажатой кнопкой, также разблокируется кнопка, если была заблокирована раньше
    //EepromManager::SaveButtonEnabled(&buttonEnabled);
    ESP.wdtFeed();
#endif


    // ЛЕНТА/МАТРИЦА
    FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)/*.setCorrection(TypicalLEDStrip)*/;
    FastLED.setBrightness(BRIGHTNESS);
    if (CURRENT_LIMIT > 0) {
        FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
    }
    FastLED.clear();
    FastLED.show();



    // EEPROM
    EepromManager::InitEepromSettings(
            modes, alarms, &ONflag, &dawnMode, &currentMode,
            &(FavoritesManager::ReadFavoritesFromEeprom),
            &(FavoritesManager::SaveFavoritesToEeprom));


    // WI-FI
    LOG.printf_P(PSTR("Connecting to SSID: %s\n"), WIFI_SSID);

    WiFi.hostname(HOST_NAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(480);
        LOG.print(".");
    }

    LOG.println("");
    LOG.println("WiFi connected");
    LOG.println("IP address: ");
    LOG.println(WiFi.localIP());


    ESP.wdtFeed();

    //LOG.printf_P(PSTR("Порт UDP сервера: %u\n"), localPort);
    //Udp.begin(localPort);


    // NTP
#ifdef USE_NTP
    timeClient.begin();
    ESP.wdtFeed();
#endif


    // MQTT
#if (USE_MQTT)
    mqttClient = new AsyncMqttClient();
    MqttManager::setupMqtt(mqttClient, inputBuffer);
    MqttManager::mqttConnect();
    ESP.wdtFeed();
#endif


    // ОСТАЛЬНОЕ
    memset(matrixValue, 0, sizeof(matrixValue));
    randomSeed(micros());
    changePower();
    loadingFlag = true;
}


void loop() {
//    parseUDP();
    effectsTick();

    EepromManager::HandleEepromTick(&settChanged, &eepromTimeout, &ONflag,
                                    &currentMode, modes, &(FavoritesManager::SaveFavoritesToEeprom));

#ifdef USE_NTP
    timeTick();
#endif

    buttonTick();

#ifdef OTA
    otaManager.HandleOtaUpdate();                             // ожидание и обработка команды на обновление прошивки по воздуху
#endif

    TimerManager::HandleTimer(&ONflag, &settChanged,          // обработка событий таймера отключения лампы
                              &eepromTimeout, &changePower);

    if (FavoritesManager::HandleFavorites(                    // обработка режима избранных эффектов
            &ONflag,
            &currentMode,
            &loadingFlag
#ifdef USE_NTP
            , &dawnFlag
#endif
    )) {
        FastLED.setBrightness(modes[currentMode].Brightness);
        FastLED.clear();
        delay(1);
    }

#if USE_MQTT
    if (mqttClient && WiFi.isConnected() && !mqttClient->connected()) {
        MqttManager::mqttConnect();                             // библиотека не умеет восстанавливать соединение в случае потери подключения к MQTT брокеру, нужно управлять этим явно
        MqttManager::needToPublish = true;
    }

    if (MqttManager::needToPublish) {
//        if (strlen(inputBuffer) > 0) {
//            processInputBuffer(inputBuffer, MqttManager::mqttBuffer, true);
//        }
        MqttManager::publishState();
    }
#endif

#if defined(GENERAL_DEBUG) && GENERAL_DEBUG_TELNET
    handleTelnetClient();
#endif

    ESP.wdtFeed();                                            // пнуть собаку
}

void MqttManager::sendCurrentJson(char *buffer) {
    StaticJsonDocument<200> doc;
    doc["effect"] = FPSTR(LIST_EFF_NAMES[currentMode]);
    doc["brightness"] = modes[currentMode].Brightness;
//    doc["speed"] = modes[currentMode].Speed;
//    doc["scale"] = modes[currentMode].Scale;
    doc["state"] = ONflag ? "ON" : "OFF";
    JsonObject color = doc.createNestedObject("color");
    color["h"] = 360.0/255.0*(float)modes[currentMode].Scale;
    color["s"] = 100.0/255.0*(float)(255U-modes[currentMode].Speed);
    //color["b"] = modes[currentMode].Blue;

    serializeJson(doc, buffer, MAX_BUFFER_SIZE);
}

void MqttManager::updateCurrentJson(char *buffer, size_t len) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, buffer, len);
    // Test if parsing succeeds.
    if (error) {
        LOG.print(F("deserializeJson() failed: "));
        LOG.println(error.c_str());
        return;
    }

    const char *state = doc["state"];
    if (state != nullptr) {
        ONflag = state[1] == 'N';
        changePower();
    }

    const char *effect = doc["effect"];
    if (effect != nullptr) {
        for (int i = 0; i < MODE_AMOUNT; i++) {
            if (strcmp_P(effect, LIST_EFF_NAMES[i]) == 0) {
                currentMode = i;
//                LOG.println("current mode");
//                LOG.println(i);
//                LOG.println(effect);
            }
        }
    }

    if (doc.containsKey("brightness")) {
        modes[currentMode].Brightness = doc["brightness"];
        FastLED.setBrightness(modes[currentMode].Brightness);
    }

    if(doc.containsKey("color")){
        JsonObject color = doc["color"];
        float s = color["s"];
        float h = color["h"];
        modes[currentMode].Scale = (uint8_t)round(255.0/360.0*h);
        modes[currentMode].Speed = 255U - (uint8_t)round(255.0/100.0*s);
        //modes[currentMode].Blue = color["b"];
    }

    settChanged = true;
    eepromTimeout = millis();
    loadingFlag = true;
}
