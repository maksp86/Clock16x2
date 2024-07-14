#ifndef __UPLOAD_ROUTE_H__
#define __UPLOAD_ROUTE_H__

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Updater.h>

#include "helpers.h"
#include "defines.h"
#include "ota/ota.h"

#define UPLOADFILETYPE_CONFIG 1
#define UPLOADFILETYPE_OTA 2
#define UPLOADFILETYPE_OTA_FS 3
#define UPLOADFILETYPE_OTHER -1

int8_t upload_file_type = UPLOADFILETYPE_OTHER;

void handleUploadConfig(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final)
{
    if (!index) {
        config_unload();
        request->_tempFile = LittleFS.open("/" + filename, "w");
    }

    if (len) {
        request->_tempFile.write(data, len);
    }

    if (final) {
        request->_tempFile.close();
        request->send(200, "text/plain", F("<meta http-equiv=\"refresh\" content=\"5;url=/\"/>Uploaded config ") + filename);
        set_need_restart();
    }
}

void handleUploadFirmwareSendError(AsyncWebServerRequest* request)
{
    request->send(200, "text/plain", "OTA error " + Update.getErrorString());
    upload_file_type = UPLOADFILETYPE_OTHER;
}

void handleUploadFirmware(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final)
{
    if (!index) {
        int cmd = (upload_file_type == UPLOADFILETYPE_OTA ? U_FLASH : U_FS);
        Update.runAsync(true);
        if (!Update.begin(request->contentLength(), cmd)) {
            handleUploadFirmwareSendError(request);
            return;
        }
        else
            ota_on_start();
    }

    if (len) {
        if (Update.write(data, len) != len) {
            handleUploadFirmwareSendError(request);
            return;
        }
        else {
            ota_on_progress(Update.progress(), Update.size());
        }
    }

    if (final) {
        if (Update.end(true))
        {
            request->send_P(200, "text/plain", PSTR("<meta http-equiv=\"refresh\" content=\"5;url=/\"/>OTA completed"));
            ota_on_end();
        }
        else
        {
            handleUploadFirmwareSendError(request);
            return;
        }
        set_need_restart();
    }
}

void handleUpload(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
    if (!index) {
#if DEBUG >= 1
        Serial.print("handleUpload ");
        Serial.println(filename);
        Serial.println(request->url());
#endif
        if (request->url().compareTo("/upload") == 0)
        {
            if (filename.compareTo(CONFIG_FILENAME) == 0)
                upload_file_type = UPLOADFILETYPE_CONFIG;
            else if (filename.endsWith("fs.bin"))
                upload_file_type = UPLOADFILETYPE_OTA_FS;
            else if (filename.endsWith("firmware.bin"))
                upload_file_type = UPLOADFILETYPE_OTA;
            else
            {
                upload_file_type = UPLOADFILETYPE_OTHER;
                request->send(200, "text/plain", "Unsupported file");
                return;
            }
        }
        else
        {
            upload_file_type = UPLOADFILETYPE_OTHER;
            request->send(400);
            return;
        }
    }

    switch (upload_file_type)
    {
    case UPLOADFILETYPE_CONFIG:
        handleUploadConfig(request, filename, index, data, len, final);
        break;
    case UPLOADFILETYPE_OTA:
    case UPLOADFILETYPE_OTA_FS:
        handleUploadFirmware(request, filename, index, data, len, final);
        break;
    default:
        return;
    }
}
#endif // __UPLOAD_ROUTE_H__