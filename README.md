# Clock16x2

### Features / особенности:
- Created with [PlatformIO IDE](https://github.com/platformio/platformio-vscode-ide) for VS Code / создано с использованием [PlatformIO IDE](https://github.com/platformio/platformio-vscode-ide) для VS Code

### Also / также:
- All board-depend defines must be in defines.h / все железозависимые переменные должны находиться в файле defines.h

### Пояснения
- На устройство можно отправлять сообщения в формате JSON ([пример для pyscript](homeassistant_addons/pyscript/track_name_to_16x2Clock.py)):
    > каждый из параметров здесь является необязателеным
    ```
    { 
        "title": "title", //текст сверху (13 или 15 символов в зависимости от наличия icon) 
        "message": "message", //текст снизу (неограниченное количество символов)
        "time": 11337, //время показа сообщения (в миллисекундах, по умолчанию 2000)
        "icon": "62696770656e6973...", //строго 64 символа (hex) для картинки
        "enqueue": false //показать сразу (false) или включить в очередь (true, по умолчанию)
    }
    ```
- На устройство можно отправлять прогноз погоды в формате JSON ([например из автоматизации homeassistant](/homeassistant_addons/automations.yaml)):
  ```
        {
          "cond": "cloudy", // https://home-assistant.io/integrations/weather/#condition-mapping
          "temp": 1,
          "hum": 90,
          "wndspd": 10, // не используется 
          "forecast": [ {
                          "cond": "cloudy",
                          "temp": 1,
                          "templow": -1
                        },
                        {
                          "cond": "cloudy",
                          "temp": 1,
                          "templow": -1 
                        }
                      ] 
        }
  ```

### TODO:  
- [ ] Add wifimanager or something?
- [ ] Reduce ram usage
- [ ] Optimize code
- [ ] Add some animations
- [ ] ~~MAKE AMERICA GREAT AGAIN~~