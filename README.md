# CO2meter
Температура, влажность, давление, CO2
- [Возможности](https://github.com/killadog/CO2meter#%D0%B2%D0%BE%D0%B7%D0%BC%D0%BE%D0%B6%D0%BD%D0%BE%D1%81%D1%82%D0%B8)
- [Компоненты](https://github.com/killadog/CO2meter#%D0%BA%D0%BE%D0%BC%D0%BF%D0%BE%D0%BD%D0%B5%D0%BD%D1%82%D1%8B)
- [Схемы](https://github.com/killadog/CO2meter#%D1%81%D1%85%D0%B5%D0%BC%D1%8B)
- [Библиотеки](https://github.com/killadog/CO2meter#%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA%D0%B8)
- [Шрифты]()
## Возможности
- Текущая температура, влажность, давление и значение CO2.
- Отображение минимального и максимального значений для каждого датчика.
- Управление одной кнопкой:
  - одно нажатие - следущий датчик;
  - два нажатия - смена режима (обычный / min-max режим);
  - удерживание - переворот экрана.
- Цветовая индикация концентрации CO2:
  - зелёный - менее 800 ppm;
  - жёлтый - от 800 до 1000 ppm;
  - красный - более 1000 ppm.
- Вывод значений в Serial.
## Компоненты
- Arduino Nano
- OLED SSD1306 128x32
- BME280
- MH-Z19B
## Схемы
<img src="CO2meter_01.png">
Для изготовления платы в формате Sprint Layout

[CO2.lay](CO2.lay)

<img src="CO2_02.png">

## Библиотеки

- [GyverButton](https://github.com/AlexGyver/GyverLibs/tree/master/GyverButton) - работа с кнопками.
- [GyverUART](https://github.com/AlexGyver/GyverLibs/tree/master/GyverUART) - быстрая и лёгкая библиотека для работы с Serial.
- [SparkFun BME280](https://github.com/sparkfun/SparkFun_BME280_Arduino_Library) - библиотека для датчика BME280 (температура, влажность, давление).
- [MH-Z19](https://github.com/WifWaf/MH-Z19) - библиотека для MH-Z19 и MH-Z19B CO2 датчиков.
- [U8g2](https://github.com/olikraus/u8g2) - графическая библиотека для монохромных OLED дисплеев.
## Шрифты
Созданы с помощью [Fony 1.4.7-WIP](http://hukka.ncn.fi/?fony) и конвертированы для вставки в скетч с помощью [bdfconv](https://github.com/olikraus/u8g2/tree/master/tools/font/bdfconv).

<img src="lcdnums_10x16.png"> [lcdnums_10x16.fon](lcdnums_10x16.fon)

<img src="lcdnums_12x32.png"> [lcdnums_12x32.fon](lcdnums_12x32.fon)
