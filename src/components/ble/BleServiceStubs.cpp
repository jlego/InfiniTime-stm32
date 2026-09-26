#include "components/ble/MotionService.h"
#include "components/ble/HeartRateService.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/ble/MusicService.h"
#include "components/ble/NavigationService.h"
#include "components/ble/AlertNotificationService.h"

using namespace Pinetime::Controllers;

MotionService::MotionService(NimbleController& n, Controllers::MotionController& m) : nimble(n), motionController(m) {}
void MotionService::Init() {}
int MotionService::OnStepCountRequested(uint16_t, ble_gatt_access_ctxt*) { return 0; }
void MotionService::OnNewStepCountValue(uint32_t) {}
void MotionService::OnNewMotionValues(int16_t, int16_t, int16_t) {}
void MotionService::SubscribeNotification(uint16_t) {}
void MotionService::UnsubscribeNotification(uint16_t) {}

HeartRateService::HeartRateService(NimbleController& n, Controllers::HeartRateController& h) : nimble(n), heartRateController(h) {}
void HeartRateService::Init() {}
int HeartRateService::OnHeartRateRequested(uint16_t, ble_gatt_access_ctxt*) { return 0; }
void HeartRateService::OnNewHeartRateValue(uint8_t) {}
void HeartRateService::SubscribeNotification(uint16_t) {}
void HeartRateService::UnsubscribeNotification(uint16_t) {}

SimpleWeatherService::SimpleWeatherService(DateTime& d) : dateTimeController(d) {}
void SimpleWeatherService::Init() {}
int SimpleWeatherService::OnCommand(ble_gatt_access_ctxt*) { return 0; }
std::optional<SimpleWeatherService::CurrentWeather> SimpleWeatherService::Current() const { return std::nullopt; }
std::optional<SimpleWeatherService::Forecast> SimpleWeatherService::GetForecast() const { return std::nullopt; }
bool SimpleWeatherService::IsNight() const { return false; }

MusicService::MusicService(NimbleController& n) : nimble(n) {}
void MusicService::Init() {}
int MusicService::OnCommand(ble_gatt_access_ctxt*) { return 0; }
void MusicService::event(char) {}
std::string MusicService::getArtist() const { return ""; }
std::string MusicService::getTrack() const { return ""; }
std::string MusicService::getAlbum() const { return ""; }
int MusicService::getProgress() const { return 0; }
int MusicService::getTrackLength() const { return 0; }
float MusicService::getPlaybackSpeed() const { return 1.0f; }
bool MusicService::isPlaying() const { return false; }

NavigationService::NavigationService() : m_progress(0) {}
void NavigationService::Init() {}
int NavigationService::OnCommand(ble_gatt_access_ctxt*) { return 0; }
std::string NavigationService::getFlag() { return ""; }
std::string NavigationService::getNarrative() { return ""; }
std::string NavigationService::getManDist() { return ""; }
int NavigationService::getProgress() { return m_progress; }

AlertNotificationService::AlertNotificationService(Pinetime::System::SystemTask& s, Pinetime::Controllers::NotificationManager& n) : systemTask(s), notificationManager(n) {}
void AlertNotificationService::Init() {}
int AlertNotificationService::OnAlert(ble_gatt_access_ctxt*) { return 0; }
void AlertNotificationService::AcceptIncomingCall() {}
void AlertNotificationService::RejectIncomingCall() {}
void AlertNotificationService::MuteIncomingCall() {}