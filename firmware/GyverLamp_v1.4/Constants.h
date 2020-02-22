#pragma once

#include <ESP8266WebServer.h>
#include <pgmspace.h>


// ============= НАСТРОЙКИ =============
// --- ESP -----------------------------
#define BUTTON_STEP_TIMEOUT   (100U)                        // каждые BUTTON_STEP_TIMEOUT мс будет генерироваться событие удержания кнопки (для регулировки яркости)
#define BUTTON_CLICK_TIMEOUT  (500U)                        // максимальное время между нажатиями кнопки в мс, до достижения которого считается серия последовательных нажатий

#define ESP_RESET_ON_START    (false)                       // true - если при старте нажата кнопка (или кнопки нет!), сохранённые настройки будут сброшены; false - не будут
//#define ESP_HTTP_PORT         (80U)                         // номер порта, который будет использоваться во время первой утановки имени WiFi сети (и пароля), к которой потом будет подключаться лампа в режиме WiFi клиента (лучше не менять)
//#define ESP_UDP_PORT          (8888U)                       // номер порта, который будет "слушать" UDP сервер во время работы лампы как в режиме WiFi точки доступа, так и в режиме WiFi клиента (лучше не менять)
#define ESP_CONN_TIMEOUT      (7U)                          // время в секундах (ДОЛЖНО БЫТЬ МЕНЬШЕ 8, иначе сработает WDT), которое ESP будет пытаться подключиться к WiFi сети, после его истечения автоматически развернёт WiFi точку доступа
#define ESP_CONF_TIMEOUT      (300U)                        // время в секундах, которое ESP будет ждать ввода SSID и пароля WiFi сети роутера в конфигурационном режиме, после его истечения ESP перезагружается
#define GENERAL_DEBUG                                       // если строка не закомментирована, будут выводиться отладочные сообщения
#define OTA                                                 // если строка не закомментирована, модуль будет ждать два последдовательных запроса пользователя на прошивку по воздуху (см. документацию в "шапке")
#ifdef OTA
#define ESP_OTA_PORT          (8266U)                       // номер порта, который будет "прослушиваться" в ожидании команды прошивки по воздуху
#define OTA_PASSWORD          ("Goni_Linx1")
#endif
#define LED_PIN               (2U)                          // пин ленты                (D4)
#define BTN_PIN               (4U)                          // пин кнопки               (D2)
#define MOSFET_PIN            (5U)                          // пин MOSFET транзистора   (D1) - может быть использован для управления питанием матрицы/ленты
#define ALARM_PIN             (16U)                         // пин состояния будильника (D0) - может быть использован для управления каким-либо внешним устройством на время работы будильника
#define MOSFET_LEVEL          (HIGH)                        // логический уровень, в который будет установлен пин MOSFET_PIN, когда матрица включена - HIGH или LOW
#define ALARM_LEVEL           (HIGH)                        // логический уровень, в который будет установлен пин ALARM_PIN, когда "рассвет"/будильник включен


// --- Wifi params ---
#define WIFI_SSID "yusin.me"
#define WIFI_PASSWORD "qwertyuiopasdfghjkl"
#define HOST_NAME "led_lamp"

// --- ВРЕМЯ ---------------------------
#define USE_NTP               (true)                        // закомментировать или удалить эту строку, если нужно, чтобы устройство не лезло в интернет
#define NTP_ADDRESS           ("ntp2.colocall.net")         // сервер времени
#define NTP_INTERVAL          (30UL * 60UL * 1000UL)        // интервал синхронизации времени (30 минут)
//#define SUMMER_WINTER_TIME                                  // для тех, кому нужен переход на зимнее/летнее время - оставить строку, остальным - закомментировать или удалить
#if !defined(SUMMER_WINTER_TIME)
#define LOCAL_WEEK_NUM        (week_t::Last)                // для стран, где нет перехода на зимнее/летнее время это технический параметр, не нужно его изменять
#define LOCAL_WEEKDAY         (dow_t::Sun)                  // для стран, где нет перехода на зимнее/летнее время это технический параметр, не нужно его изменять
#define LOCAL_MONTH           (month_t::Mar)                // для стран, где нет перехода на зимнее/летнее время это технический параметр, не нужно его изменять
#define LOCAL_HOUR            (1U)                          // для стран, где нет перехода на зимнее/летнее время это технический параметр, не нужно его изменять
#define LOCAL_OFFSET          (3 * 60)                      // смещение локального времени относительно универсального координированного времени UTC в минутах
#define LOCAL_TIMEZONE_NAME   ("MSK")                       // обозначение локального часового пояса; до 5 символов; может быть использовано, если понадобится его вывести после вывода времени
#else
#define SUMMER_WEEK_NUM       (week_t::Last)                // номер недели в месяце, когда происходит переход на летнее время (возможные варианты: First - первая, Second - вторая, Third - третья, Fourth - четвёртая, Last - последняя)
#define SUMMER_WEEKDAY        (dow_t::Sun)                  // день недели, когда происходит переход на летнее время (возможные варианты: Mon - пн, Tue - вт, Wed - ср, Thu - чт, Sat - сб, Sun - вс)
#define SUMMER_MONTH          (month_t::Mar)                // месяц, в котором происходит переход на летнее время (возможные варианты: Jan - январь, Feb - февраль, Mar - март, Apr - апрель, May - май, Jun - июнь, Jul - июль, Aug - август, Sep - сентябрь, Oct - октябрь, Nov - ноябрь, Dec - декабрь)
#define SUMMER_HOUR           (3U)                          // час (по зимнему времени!), когда заканчивается зимнее время и начинается летнее; [0..23]
#define SUMMER_OFFSET         (3 * 60)                      // смещение летнего времени относительно универсального координированного времени UTC в минутах
#define SUMMER_TIMEZONE_NAME  ("EEST")                      // обозначение летнего времени; до 5 символов; может быть использовано, если понадобится его вывести после вывода времени; может быть "ЛЕТ"
#define WINTER_WEEK_NUM       (week_t::Last)                // номер недели в месяце, когда происходит переход на зимнее время (возможные варианты: First - первая, Second - вторая, Third - третья, Fourth - четвёртая, Last - последняя)
#define WINTER_WEEKDAY        (dow_t::Sun)                  // день недели, когда происходит переход на зимнее время (возможные варианты: Mon - пн, Tue - вт, Wed - ср, Thu - чт, Sat - сб, Sun - вс)
#define WINTER_MONTH          (month_t::Oct)                // месяц, в котором происходит переход на зимнее время (возможные варианты: Jan - январь, Feb - февраль, Mar - март, Apr - апрель, May - май, Jun - июнь, Jul - июль, Aug - август, Sep - сентябрь, Oct - октябрь, Nov - ноябрь, Dec - декабрь)
#define WINTER_HOUR           (4U)                          // час (по летнему времени!), когда заканчивается летнее время и начинается зимнее; [0..23]
#define WINTER_OFFSET         (2 * 60)                      // смещение зимнего времени относительно универсального координированного времени UTC в минутах
#define WINTER_TIMEZONE_NAME  ("EET")                       // обозначение зимнего времени; до 5 символов; может быть использовано, если понадобится его вывести после вывода времени; может быть "ЗИМ"
#endif
#define PRINT_TIME            (0U)                          // 0U - не выводить время бегущей строкой; 1U - вывод времени каждый час; 2U - каждый час + каждые 30 минут; 3U - каждый час + каждые 15 минут
// 4U - каждый час + каждые 10 минут; 5U - каждый час + каждые 5 минут; 6U - каждый час + каждую минуту
#define NIGHT_HOURS_START     (1380U)                       // начало действия "ночного времени" (в минутах от начала суток, 23:00), текущее время бегущей строкой будет выводиться с яркостью NIGHT_HOURS_BRIGHTNESS
#define NIGHT_HOURS_STOP      (479U)                        // конец действия "ночного времени" (в минутах от начала суток, 7:59)
#define DAY_HOURS_BRIGHTNESS  (255)                         // яркость для вывода текущего времени бегущей строкой днём; если -1, будет использована яркость текущего эффекта (она известна, даже когда матрица выключена)
#define NIGHT_HOURS_BRIGHTNESS (5)                          // яркость для вывода текущего времени бегущей строкой ночью; если -1, будет использована яркость текущего эффекта (она известна, даже когда матрица выключена)
// константы DAY_HOURS_BRIGHTNESS и NIGHT_HOURS_BRIGHTNESS используются только, когда матрица выключена, иначе будет использована яркость текущего эффекта

// --- ВНЕШНЕЕ УПРАВЛЕНИЕ --------------
#define USE_MQTT              (true)                       // true - используется mqtt клиент, false - нет
#if USE_MQTT
#define MQTT_RECONNECT_TIME   (10U)                         // время в секундах перед подключением к MQTT брокеру в случае потери подключения
#endif

// --- РАССВЕТ -------------------------
#define DAWN_BRIGHT           (200U)                        // максимальная яркость рассвета (0-255)
#define DAWN_TIMEOUT          (1U)                          // сколько рассвет светит после времени будильника, минут

// --- МАТРИЦА -------------------------
#define BRIGHTNESS            (40U)                         // стандартная маскимальная яркость (0-255)
#define CURRENT_LIMIT         (2000U)                       // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит

#define WIDTH                 (16U)                         // ширина матрицы
#define HEIGHT                (16U)                         // высота матрицы

#define COLOR_ORDER           (GRB)                         // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB

#define MATRIX_TYPE           (0U)                          // тип матрицы: 0 - зигзаг, 1 - параллельная
#define CONNECTION_ANGLE      (0U)                          // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define STRIP_DIRECTION       (0U)                          // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
// при неправильной настройке матрицы вы получите предупреждение "Wrong matrix parameters! Set to default"
// шпаргалка по настройке матрицы здесь! https://alexgyver.ru/matrix_guide/


// ============= ДЛЯ РАЗРАБОТЧИКОВ =====
// список и номера эффектов ниже в списке согласованы с android приложением!
#define EFF_SPARKLES          (0U)                          // Конфетти
#define EFF_FIRE              (1U)                          // Огонь
#define EFF_WHITTE_FIRE       (2U)                          // Белый огонь
#define EFF_RAINBOW_VER       (3U)                          // Радуга вертикальная
#define EFF_RAINBOW_HOR       (4U)                          // Радуга горизонтальная
#define EFF_RAINBOW_DIAG      (5U)                          // Радуга диагональная
#define EFF_COLORS            (6U)                          // Смена цвета
#define EFF_MADNESS           (7U)                          // Безумие 3D
#define EFF_CLOUDS            (8U)                          // Облака 3D
#define EFF_LAVA              (9U)                          // Лава 3D
#define EFF_PLASMA            (10U)                         // Плазма 3D
#define EFF_RAINBOW           (11U)                         // Радуга 3D
#define EFF_RAINBOW_STRIPE    (12U)                         // Павлин 3D
#define EFF_ZEBRA             (13U)                         // Зебра 3D
#define EFF_FOREST            (14U)                         // Лес 3D
#define EFF_OCEAN             (15U)                         // Океан 3D
#define EFF_COLOR             (16U)                         // Цвет
#define EFF_SNOW              (17U)                         // Снегопад
#define EFF_SNOWSTORM         (18U)                         // Метель
#define EFF_STARFALL          (19U)                         // Звездопад
#define EFF_MATRIX            (20U)                         // Матрица
#define EFF_LIGHTERS          (21U)                         // Светлячки
#define EFF_LIGHTER_TRACES    (22U)                         // Светлячки со шлейфом
#define EFF_PAINTBALL         (23U)                         // Пейнтбол
#define EFF_CUBE              (24U)                         // Блуждающий кубик
#define EFF_WHITE_COLOR       (25U)                         // Белый свет
#define MODE_AMOUNT           (26U)                         // количество режимов

const char NAME_EFF_SPARKLES[]          PROGMEM = "Конфетти";
const char NAME_EFF_FIRE[]              PROGMEM = "Огонь";
const char NAME_EFF_WHITTE_FIRE[]       PROGMEM = "Белый огонь";
const char NAME_EFF_RAINBOW_VER[]       PROGMEM = "Радуга вертикальная";
const char NAME_EFF_RAINBOW_HOR[]       PROGMEM = "Радуга горизонтальная";
const char NAME_EFF_RAINBOW_DIAG[]      PROGMEM = "Радуга диагональная";
const char NAME_EFF_COLORS[]            PROGMEM = "Смена цвета";
const char NAME_EFF_MADNESS[]           PROGMEM = "Безумие 3D";
const char NAME_EFF_CLOUDS[]            PROGMEM = "Облака 3D";
const char NAME_EFF_LAVA[]              PROGMEM = "Лава 3D";
const char NAME_EFF_PLASMA[]            PROGMEM = "Плазма 3D";
const char NAME_EFF_RAINBOW[]           PROGMEM = "Радуга 3D";
const char NAME_EFF_RAINBOW_STRIPE[]    PROGMEM = "Павлин 3D";
const char NAME_EFF_ZEBRA[]             PROGMEM = "Зебра 3D";
const char NAME_EFF_FOREST[]            PROGMEM = "Лес 3D";
const char NAME_EFF_OCEAN[]             PROGMEM = "Океан 3D";
const char NAME_EFF_COLOR[]             PROGMEM = "Цвет";
const char NAME_EFF_SNOW[]              PROGMEM = "Снегопад";
const char NAME_EFF_SNOWSTORM[]         PROGMEM = "Метель";
const char NAME_EFF_STARFALL[]          PROGMEM = "Звездопад";
const char NAME_EFF_MATRIX[]            PROGMEM = "Матрица";
const char NAME_EFF_LIGHTERS[]          PROGMEM = "Светлячки";
const char NAME_EFF_LIGHTER_TRACES[]    PROGMEM = "Светлячки со шлейфом";
const char NAME_EFF_PAINTBALL[]         PROGMEM = "Пейнтбол";
const char NAME_EFF_CUBE[]              PROGMEM = "Блуждающий кубик";
const char NAME_EFF_WHITE_COLOR[]       PROGMEM = "Белый свет";

const char *const LIST_EFF_NAMES[] PROGMEM = {
        NAME_EFF_SPARKLES,
        NAME_EFF_FIRE,
        NAME_EFF_WHITTE_FIRE,
        NAME_EFF_RAINBOW_VER,
        NAME_EFF_RAINBOW_HOR,
        NAME_EFF_RAINBOW_DIAG,
        NAME_EFF_COLORS,
        NAME_EFF_MADNESS,
        NAME_EFF_CLOUDS,
        NAME_EFF_LAVA,
        NAME_EFF_PLASMA,
        NAME_EFF_RAINBOW,
        NAME_EFF_RAINBOW_STRIPE,
        NAME_EFF_ZEBRA,
        NAME_EFF_FOREST,
        NAME_EFF_OCEAN,
        NAME_EFF_COLOR,
        NAME_EFF_SNOW,
        NAME_EFF_SNOWSTORM,
        NAME_EFF_STARFALL,
        NAME_EFF_MATRIX,
        NAME_EFF_LIGHTERS,
        NAME_EFF_LIGHTER_TRACES,
        NAME_EFF_PAINTBALL,
        NAME_EFF_CUBE,
        NAME_EFF_WHITE_COLOR};

//#define MAX_UDP_BUFFER_SIZE (UDP_TX_PACKET_MAX_SIZE + 1)
#define MAX_BUFFER_SIZE   (129U)

#define GENERAL_DEBUG_TELNET  (false)                       // true - отладочные сообщения будут выводиться в telnet вместо Serial порта (для удалённой отладки без подключения usb кабелем)
#define TELNET_PORT           (23U)                         // номер telnet порта

#if defined(GENERAL_DEBUG) && GENERAL_DEBUG_TELNET
WiFiServer telnetServer(TELNET_PORT);                       // telnet сервер
WiFiClient telnet;                                          // обработчик событий telnet клиента
bool telnetGreetingShown = false;                           // признак "показано приветствие в telnet"
#define LOG                   telnet
#else
#define LOG                   Serial
#endif

// --- БИБЛИОТЕКИ ----------------------
#define FASTLED_INTERRUPT_RETRY_COUNT   (0)                 // default: 2; // Use this to determine how many times FastLED will attempt to re-transmit a frame if interrupted for too long by interrupts
//#define FASTLED_ALLOW_INTERRUPTS      (1)                 // default: 1; // Use this to force FastLED to allow interrupts in the clockless chipsets (or to force it to disallow), overriding the default on platforms that support this. Set the value to 1 to allow interrupts or 0 to disallow them.
#define FASTLED_ESP8266_RAW_PIN_ORDER                       // FASTLED_ESP8266_RAW_PIN_ORDER, FASTLED_ESP8266_D1_PIN_ORDER or FASTLED_ESP8266_NODEMCU_PIN_ORDER

#define NUM_LEDS              (uint16_t)(WIDTH * HEIGHT)
#define SEGMENTS              (1U)                          // диодов в одном "пикселе" (для создания матрицы из кусков ленты)
