# Clock16x2

### Features / особенности:
- Created with [PlatformIO IDE](https://github.com/platformio/platformio-vscode-ide) for VS Code / создано с использованием [PlatformIO IDE](https://github.com/platformio/platformio-vscode-ide) для VS Code

### Also / также:
- All board-depend defines must be in defines.h / все железозависимые переменные должны находиться в файле defines.h

### Пояснения
- На устройство можно отправлять сообщения в формате JSON:
    > каждый из параметров здесь является необязателеным
    ```
    { 
        "title": "title", //текст сверху (13 или 15 символов в зависимости от наличия icon) 
        "message": "message", //текст снизу (неограниченное количество символов)
        "time": 11337, //время показа сообщения (в миллисекундах, по умолчанию 2000)
        "icon": "62696770656e6973...", //строго 64 символа для картинки
        "enqueue": false //показать сразу (false) или включить в очередь (true, по умолчанию)
    }
    ```

### TODO:  
- [ ] Add wifimanager or something?
- [ ] Reduce ram usage
- [ ] Optimize code
- [ ] Add some animations
- [ ] ~~MAKE AMERICA GREAT AGAIN~~