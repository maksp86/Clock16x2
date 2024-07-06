#include "ntp.h"

char* ntp_server;
int8_t ntp_hour_offset;
uint32_t ntp_update_timer;
uint32_t ntp_last_update = 0;

uint32_t ntp_timeout_timer;
bool ntp_timeout_wait = false;

uint32_t ntp_update_time;
bool ntp_need_update = false;

uint32_t ntp_curr_update_time;

AsyncUDP* ntp_udpClient;
int8_t ntp_request_state;
time_t ntp_timestamp;
time_t ntp_timestamp_tmp;
bool ntp_is_time_set = false;
void ntp_dns_got_addr(const ip_addr_t* addr);
void ntp_on_dns_found(const char* name, const ip_addr_t* ipaddr, void* arg);
void ntp_on_udp_packet(AsyncUDPPacket& packet);
void ntp_send_time_packet();

bool ntp_time_set()
{
    return ntp_is_time_set;
}

void ntp_setup(const char* _ntp_server, int8_t _hour_offset, uint32_t _update_time)
{
#if DEBUG >= 3
    Serial.println("NTP_SETUP");
#endif
    uint8_t ntp_server_len = strlen(_ntp_server);
    ntp_server = new char[ntp_server_len + 1];
    strcpy(ntp_server, _ntp_server);
    ntp_server[ntp_server_len] = '\0';

    ntp_hour_offset = _hour_offset;
    ntp_curr_update_time = ntp_update_time = _update_time;
    ntp_request_state = NTP_REQUEST_STATE_NONE;

    ntp_udpClient = new AsyncUDP();
    ntp_udpClient->onPacket([](AsyncUDPPacket packet) { ntp_on_udp_packet(packet); });
}

void ntp_update()
{
    if (ntp_need_update && millis() - ntp_update_timer > ntp_curr_update_time)
    {
        ntp_send_time_packet();
        statusbar_set_wifi_busy();
        ntp_update_timer = millis();
    }

    if (ntp_timeout_wait && millis() - ntp_timeout_timer > 1000)
    {
        ntp_timeout_wait = false;
        ntp_request_state = NTP_REQUEST_STATE_NONE;
        ntp_curr_update_time = 10000;
        ntp_timeout_wait = millis();
    }
}

void ntp_on_wifi_event(wifi_event event)
{
    switch (event)
    {
    case wifi_event::WIFI_ON_CONNECTED:
        if (ntp_udpClient->listen(1337))
        {
#if DEBUG >= 3
            Serial.println("NTP_STARTED");
#endif
            ntp_need_update = true;
            ntp_timeout_timer = ntp_update_timer = millis();
            ntp_send_time_packet();
        }
        break;

    case wifi_event::WIFI_ON_DISCONNECT:
        ntp_udpClient->close();
        ntp_need_update = false;
        ntp_timeout_wait = false;
        break;
    }
}

void ntp_send_time_packet()
{
    if (ntp_request_state != NTP_REQUEST_STATE_NONE) return;
#if DEBUG >= 3
    Serial.println("ntp_send_time_packet");
#endif
    ip_addr_t addr;
    err_t err = dns_gethostbyname(ntp_server, &addr, ntp_on_dns_found, NULL);
    switch (err)
    {
    case ERR_OK:
        ntp_request_state = NTP_REQUEST_STATE_DNS_REQ;
        ntp_dns_got_addr(&addr);
        break;
    case ERR_INPROGRESS:
        ntp_request_state = NTP_REQUEST_STATE_DNS_REQ;
        break;
    default:
        break;
    }
}

void ntp_dns_got_addr(const ip_addr_t* addr)
{
    if (ntp_request_state != NTP_REQUEST_STATE_DNS_REQ) return;
    ntp_request_state = NTP_REQUEST_STATE_UDP_SEND;

    if (!addr)
        return;

    ip_addr_t* ntps_addr = new ip_addr_t(*addr);

    byte* packetBuffer = (byte*)malloc(sizeof(byte) * NTP_PACKET_SIZE);
    memset(packetBuffer, 0, NTP_PACKET_SIZE);

    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    AsyncUDPMessage message = AsyncUDPMessage(NTP_PACKET_SIZE);
    message.write(packetBuffer, NTP_PACKET_SIZE);

    ntp_udpClient->sendTo(message, ntps_addr, 123);

    ntp_timeout_wait = true;
    ntp_timeout_timer = millis();

    free(ntps_addr);
    free(packetBuffer);
}

void ntp_on_dns_found(const char* name, const ip_addr_t* ipaddr, void* arg)
{
    (void)name;
    if (ipaddr)
        ntp_dns_got_addr(ipaddr);
}

void ntp_on_udp_packet(AsyncUDPPacket& packet)
{
    if (ntp_request_state != NTP_REQUEST_STATE_UDP_SEND) return;

    ntp_request_state = NTP_REQUEST_STATE_UDP_RECV;

    if (packet.length() == NTP_PACKET_SIZE)
    {
        uint8_t* data = packet.data();
        unsigned long highWord = word(data[40], data[41]);
        unsigned long lowWord = word(data[42], data[43]);

        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;

        ntp_timestamp = (secsSince1900 - 2208988800UL) + (ntp_hour_offset * 3600);
        ntp_is_time_set = true;

#if DEBUG >= 3
        Serial.print("NTP GOT TIME: ");
        Serial.println(ntp_timestamp);
#endif
        ntp_last_update = millis();

        ntp_timeout_wait = false;
        ntp_curr_update_time = ntp_update_time;
    }
    else
    {
        ntp_curr_update_time = 10000;
    }
    ntp_request_state = NTP_REQUEST_STATE_NONE;

}

time_t* ntp_get_epoch_time()
{
    if (ntp_is_time_set)
    {
        uint32_t change = ((millis() - ntp_last_update) / 1000);
        ntp_timestamp_tmp = change + ntp_timestamp;

        return &ntp_timestamp_tmp;
    }
    else
        return nullptr;
}
