# ESP32 for Windex

Here a brief guide on how to set up ESPs boards for windex communication is provided.

## ESP32 Setup and Configuration

The easiest way to set up the ESP32 boards is to use the Arduino IDE.

There are some steps to follow to set up the Arduino IDE to work with the ESP32 boards that are well explained in [this guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-arduino-ide).
Notice that you might also need to install additional drivers for the ESP32 boards to work properly.

> [!IMPORTANT]
> When uploadind the code to the ESP32 boards, you need to press the `BOOT` button on the board and then press the `EN` button when finished flashing the code.
> In case you want to upload the `SensorNode` code, you also need to deconnect the Windex sensor from the ESP board (remove the cable from the `RX` pin of the ESP board).

## Load the library into the sketch

To use the `NMEA0183` library, you need to include it in your sketch.
You can do this by copying all the `*.c` and `*.h` files from the `NMEA0183/src/` folder into the `src` subfolders of each sketch.

Then, you can include the library by adding the following line at the beginning of your sketch:

```cpp
extern "C" {
  #include "src/NMEA0183/NMEA0183.h"
  #include "src/NMEA0183/sensors/mwv.h"
  #include "src/NMEA0183/sensors/xdr.h"
}
```

Notice that the `extern "C" {}` is needed to avoid name mangling when including C code in a C++ project.

## Load the CAN library into the sketch

To use the `CAN` library, you need to include it in your sketch.
The library currently used comes from this [repository](https://github.com/sandeepmistry/arduino-CAN/tree/master).

To include the library in your sketch, you can either download the repository and include the library under the `src/arduino-CAN` folder in your project or you can use the `git submodule` command to include the library as a submodule in your project.
For the second option, you can run the following command in the root of your project:

```bash
git submodule init
git submodule update
```

Then, you can include the library in your sketch by adding the following line at the beginning of your sketch:

```cpp
#include "src/arduino-CAN/src/CAN.h"
```

## Board connection

<div align="center">

| CV7-E  | Function  |
| ------ | --------- |
| Blue   | (GND)     |
| Red    | (VCC) 12V |
| Yellow | + NMEA    |
| Green  | - NMEA    |

</div>

Resistance values must be checked at PST lab.

<div style="width:80%; margin:auto">

![Connection circuit](../assets/img/Circuit.png)

</div>


### MAC Addresses

So far I've marked with a black dot over the connection port the ESP board that is currently used as the base node.
Given that the MAC address of the ESP board is needed to set up the communication, here is its MAC address as a reference:

<div align="center">

| ESP board  | MAC Address       |
| ---------- | ----------------- |
| BaseNode   | 08:D1:F9:27:F3:A0 |
| SensorNode | To be retrived    |

</div>


In case you need to know the MAC address of the ESP board, you can upload this code to the board and check the serial monitor.

```cpp
#include "WiFi.h"

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_MODE_STA);

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {}
```

## Usefull links

- [ESP32 Arduino Core’s documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/index.html): usefull as a qualified documentation for the ESP32 board.
- [Peer to Peer ESP32 Communications](https://dronebotworkshop.com/esp-now/): one of the few valid resources I found on the web to understand and start working with the `ESP-NOW` protocol.