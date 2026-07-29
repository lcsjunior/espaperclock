#include "wifi_setup.h"

#include <ArduinoOTA.h>
#include <Config.h>
#include <device.h>

#define WIFI_PORTAL_TIMEOUT_S 300

#ifndef WIFI_CONNECT_TIMEOUT_MS
#define WIFI_CONNECT_TIMEOUT_MS 30000UL
#endif

#ifndef NTP_SYNC_TIMEOUT_MS
#define NTP_SYNC_TIMEOUT_MS 5000UL
#endif

constexpr const char* NO_DEFAULT = "";

constexpr int OTA_PASS_LEN = 15;
constexpr int OWM_API_KEY_LEN = 47;
constexpr int OWM_LOCATION_LEN = 31;
constexpr int TIMEZONE_LEN = 15;
constexpr int NTP_SERVER_LEN = 31;

WiFiManager wifiManager;

// clang-format off
WiFiManagerParameter wmOtaPass("ota_pass",         "OTA Password",        NO_DEFAULT, OTA_PASS_LEN,    "type=\"password\"");
WiFiManagerParameter wmOwmApiKey("owm_api_key",    "OpenWeather API Key", NO_DEFAULT, OWM_API_KEY_LEN, "type=\"password\"");
WiFiManagerParameter wmOwmLocation("owm_location", "Weather City,Country",NO_DEFAULT, OWM_LOCATION_LEN);
WiFiManagerParameter wmTimezone("timezone",        "Timezone (POSIX TZ, ex.: BRT3)", NO_DEFAULT, TIMEZONE_LEN);
WiFiManagerParameter wmNtpServer("ntp_server",     "NTP Server",          NO_DEFAULT, NTP_SERVER_LEN);
// clang-format on

static void onWifiManagerSaveParams() {
  AppConfig.setOtaPass(wmOtaPass.getValue());
  AppConfig.setOwmApiKey(wmOwmApiKey.getValue());
  AppConfig.setOwmLocation(wmOwmLocation.getValue());
  AppConfig.setTimezone(wmTimezone.getValue());
  AppConfig.setNtpServer(wmNtpServer.getValue());
  AppConfig.save();
}

static void initPortal() {
  wmOtaPass.setValue(AppConfig.otaPass(), OTA_PASS_LEN);
  wmOwmApiKey.setValue(AppConfig.owmApiKey(), OWM_API_KEY_LEN);
  wmOwmLocation.setValue(AppConfig.owmLocation(), OWM_LOCATION_LEN);
  wmTimezone.setValue(AppConfig.timezone(), TIMEZONE_LEN);
  wmNtpServer.setValue(AppConfig.ntpServer(), NTP_SERVER_LEN);

  wifiManager.addParameter(&wmOtaPass);
  wifiManager.addParameter(&wmOwmApiKey);
  wifiManager.addParameter(&wmOwmLocation);
  wifiManager.addParameter(&wmTimezone);
  wifiManager.addParameter(&wmNtpServer);

  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT_S);
  wifiManager.setHostname(DEVICE_HOSTNAME);
  wifiManager.setSaveParamsCallback(onWifiManagerSaveParams);
  wifiManager.setWiFiAutoReconnect(true);
  wifiManager.autoConnect(getApName());
  wifiManager.startWebPortal();
  waitWifi(WIFI_CONNECT_TIMEOUT_MS);
}

static void initNtp() {
  configTzTime(AppConfig.timezone(), AppConfig.ntpServer(), "pool.ntp.org");
  waitNtp(NTP_SYNC_TIMEOUT_MS);
}

static void initOta() {
  ArduinoOTA.setHostname(DEVICE_HOSTNAME);
  ArduinoOTA.setPassword(AppConfig.otaPass());

  ArduinoOTA.onStart([]() { log_i("OTA start"); });
  ArduinoOTA.onEnd([]() { log_i("OTA end"); });
  ArduinoOTA.onError([](ota_error_t error) { log_e("OTA error %u", error); });

  ArduinoOTA.begin();
}

void initWifi() {
  initPortal();
  initNtp();
  initOta();
}

void handleWifi() {
  wifiManager.process();
  ArduinoOTA.handle();
}
