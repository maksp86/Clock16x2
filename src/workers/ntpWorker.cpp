#include "workers/ntpWorker.h"

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiUdp.h>
#include <Time.h>

#include "statics.h"
#include "helpers/debugHelper.h"

static const char* TIMEZONE_KEY = "tz";
static const char* NTP_SERVER_KEY = "ts";
static const char* NTP_SERVER_DEFAULT = "0.pool.ntp.org";
const int NTP_PACKET_SIZE = 48;

NtpWorker::NtpWorker()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "init");
#endif
    _enabled = false;
    //udp = new WiFiUDP();

#ifdef _DEBUG
    DebugHelper::Log(this, "arr init");
#endif
    packetBuffer = new byte[NTP_PACKET_SIZE];
}

NtpWorker::NtpWorker(configHelper* ch) : NtpWorker()
{
    config = ch;
}

NtpWorker::~NtpWorker()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "destruct");
#endif
    udp.stop();
    if (packetBuffer)
        delete[] packetBuffer;
}

void NtpWorker::OnConfigStart()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "config start");
#endif
    config->AddProperty("NTP Settings:");
    config->AddProperty(TIMEZONE_KEY, "Timezone (UTC)", "0");
    config->AddProperty(NTP_SERVER_KEY, "NTP Server", "0.pool.ntp.org");
#ifdef _DEBUG
    DebugHelper::Log(this, "config end");
#endif
}

void NtpWorker::OnConfigSave()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "config save");
#endif

#ifdef _DEBUG
    DebugHelper::Log(this, "config saved");
#endif
}

void NtpWorker::Setup()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "Setup");
    DebugHelper::Log(this, config->GetValue(TIMEZONE_KEY));
    DebugHelper::Log(this, config->GetValue(NTP_SERVER_KEY));
#endif
    if (config->GetValue(TIMEZONE_KEY) != NULL && config->GetValue(NTP_SERVER_KEY) != NULL)
    {
#ifdef _DEBUG
        DebugHelper::Log(this, "k");
#endif
        _enabled = true;
        _syncTime = 1000;
        udp.begin(8888);
    }
    else
        _enabled = false;
}

void NtpWorker::Update()
{
    if (_enabled)
        if (millis() - _nextSyncTimer > _syncTime)
        {
#ifdef _DEBUG
            DebugHelper::Log(this, "time get start");
#endif
            //For not to reset the time in case of any error
            time_t ntpTime = getNtpTime();
            if (ntpTime == 0)
            {
                if (timeStatus() == 2)
                    ntpTime = now();
                _syncTime = 30000;
            }
            else
                _syncTime = 300000;

            setTime(ntpTime);
            _nextSyncTimer = millis();
#ifdef _DEBUG
            DebugHelper::Log(this, "time get end");
#endif
        }
}

time_t NtpWorker::getNtpTime()
{
    IPAddress ntpServerIP; // NTP server's ip address
    while (udp.parsePacket() > 0); // discard any previously received packets
    char* host = new char[32];
    strcpy(host, config->GetValueOrDefault(NTP_SERVER_KEY, NTP_SERVER_DEFAULT));
#ifdef _DEBUG
    DebugHelper::Log(this, host);
#endif
    if (WiFi.hostByName(host, ntpServerIP, 1000) == 0)
    {
#ifdef _DEBUG
        DebugHelper::Log(this, "Get ip fail");
#endif
        return 0;
    }
#ifdef _DEBUG
    DebugHelper::Log(this, "got ntp ip");
#endif
    sendNTPpacket(ntpServerIP);
    uint32_t beginWait = millis();
    int timeZone = atoi(config->GetValue(TIMEZONE_KEY));
    while (millis() - beginWait < 1000) {
        int size = udp.parsePacket();
        if (size >= NTP_PACKET_SIZE)
        {
#ifdef _DEBUG
            DebugHelper::Log(this, "Receive NTP Response");
#endif
            udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 = (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
        }
    }
#ifdef _DEBUG
    DebugHelper::Log(this, "Reached timeout");
#endif
    return 0; // return 0 if unable to get the time
}

void NtpWorker::sendNTPpacket(IPAddress& address)
{
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);

    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

InternetNeedState NtpWorker::InternetNeed() { return InternetNeedState::NEED_INTERNET; }
char* NtpWorker::Name() { return "NtpWorker"; }