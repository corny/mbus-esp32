M-Bus reader with ESP32
=======================

This Arduino project uses the UART2 interface of a ESP32 to
retreive data via [M-Bus](https://www.m-bus.de/mbus.html) from a smart meter and transmits it via
[MQTT](https://en.wikipedia.org/wiki/MQTT).
It is tested with a [CF UltraMaxx MK](https://www.itron.com/de/solutions/product-catalog/cf-ultramaxx-mk).

## Hardware

* [ESP32 development board](https://www.az-delivery.de/products/esp32-developmentboard)
* [IR Lesekopf](https://www.photovoltaikforum.com/thread/141332-neue-lesekopf-baus%C3%A4tze-ohne-smd-l%C3%B6ten/) for the optical interface

## M-Bus

Not scope of this project is decoding the mbus data.
You can learn about the data structure at:
* [M-Bus Telegramme](https://www.m-bus.de/telegramme.html)
* [Wärmezähler über optische M-Bus-Schnittstelle auslesen](https://www.mikrocontroller.net/topic/438972#6103099)
* [M-Bus Protokoll Ultramess C3](https://www.molline.de/fileadmin/content/content/Downloads/Produkte/02_W%C3%A4rmez%C3%A4hler/01_Kompaktz%C3%A4hler/06_WingStar_C3_T/M-Bus_Protokoll_Ultramess_C3_WingStar_C3.pdf)
