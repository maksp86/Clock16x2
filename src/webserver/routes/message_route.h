#ifndef __MESSAGE_ROUTE_H__
#define __MESSAGE_ROUTE_H__

#include <ESPAsyncWebServer.h>
#include "message_from_mqtt.h"

#define BODY_TYPE_OTHER -1
#define BODY_TYPE_MESSAGE_JSON 0

int8_t body_type = BODY_TYPE_OTHER;

uint8_t* body_data = nullptr;
uint8_t* body_data_end_ptr = body_data;

void handleBody(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total)
{
    if (!index)
    {
        if (request->url().compareTo(F("/message")) == 0)
        {
            body_type = BODY_TYPE_MESSAGE_JSON;
        }
        else
        {
            request->send_P(500, "text/plain", PSTR("Incorrect data"));
            body_type = BODY_TYPE_OTHER;
            return;
        }
        body_data = new uint8_t[total];
        body_data_end_ptr = body_data;
    }

    if (len && body_type != BODY_TYPE_OTHER)
    {
        memcpy(body_data_end_ptr, data, len);
        body_data_end_ptr += len;
    }

    if (index + len == total)
    {
        switch (body_type)
        {
        case BODY_TYPE_MESSAGE_JSON:
            check_message((const char*)body_data, total);
            request->send_P(200, "text/plain", PSTR("Message sent"));
            break;
        default:
            break;
        }

        if (body_data)
        {
            delete[] body_data;
            body_data_end_ptr = body_data = nullptr;
        }
        body_type = BODY_TYPE_OTHER;
    }

}

#endif // __MESSAGE_ROUTE_H__