import json
from transliterate import translit


@state_trigger("media_player.changeme.media_title")
def on_yandex_station_track_change(**kwargs):
    track_name = media_player.changeme.media_artist + \
        " - " + media_player.changeme.media_title
    track_name_en = translit(track_name, language_code='ru', reversed=True)
    icon = [0x00, 0x00, 0x00, 0x07, 0x08, 0x11, 0x11, 0x18,
            0x00, 0x00, 0x00, 0x1C, 0x02, 0x11, 0x11, 0x03,
            0x17, 0x10, 0x10, 0x08, 0x07, 0x00, 0x00, 0x00,
            0x1D, 0x01, 0x01, 0x02, 0x1C, 0x00, 0x00, 0x00]
    icon_hex = bytes(icon).hex()
    json_payload = json.dumps({"title": "Now playing", "message": track_name_en,
                              "time": 10000, "icon": icon_hex, "enqueue": False})
    service.call("mqtt", "publish",
                 topic="changeme/please/message", payload=json_payload)
