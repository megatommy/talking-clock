# Talking clock

An ESP32 that tells the time with your own voice.

It's achieved using an RTC module to keep track of the time, and an mp3 module to store the files needed and to connect to the speakers

The parts:
- ESP32 ([ESP32-DevKitC V4](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html))
- RTC module ([Waveshare DS1302](https://www.waveshare.com/ds1302-rtc-board.htm))
- MP3 module ([Catalex Serial MP3 Player V1.0 / YX5300](https://github.com/MajicDesigns/MD_YX5300/))

First you need to record all the numbers with your voice, and store them on an SD card, which you then insert in the MP3 player.

With the help of the code, you can "construct a sentence" which will be played by the MP3 player.

## Some notes on the SD card

The SD card needs to be formatted and needs to have the file name formats as described in [this article](https://arduinoplusplus.wordpress.com/2018/07/23/yx5300-serial-mp3-player-catalex-module/).


A couple of things to consider:
- `01` folder: The amount of files in here determines the "from" hour
- `02` folder: The amount of files in here determines the "until" hour
- `03`, `04`, ... folder: the words to say. The "words" folder(s)


### From and until hours

The files you put in these folders have to be .mp3 files, but can also be empty files. they won't be played. It's about the amount on files in those respective folders.

Example: 8 mp3 files in `01` and 20 mp3 files in `02`: The clock will say the hours from 8:00 until 20:00 (including those hours)

### The words folders

In the words folders you put the recorded words.

**The YX5300 supports 8kHz to 48kHz sampling frequency MP3 and WAV file formats.**

If you have more words folders, it will pick a words folder random at startup.

The format to follow is as follows:

| Filename | Type of file | Example | Notes |
| --- | --- | --- | --- |
| 001.mp3 | Intro | "Hi grandma!" | Will always be played |
| 002.mp3 | It is (plural) | "it's" | Will be played at all hours except 00:00, 01:00 and 12:00 |
| 003.mp3 | It is (singular) | "it's" | Will be played at 00:00, 01:00 and 12:00 |
| 100.mp3 - 160.mp3 | Numbers | "one", "two", etc | The numbers will be used for both the hours and the minutes |
| 161.mp3 | Midnight | "Midnight" | Will be played instead of 00:00 |
| 162.mp3 | Mid day | "Midday" | Will be played instead of 12:00 |
| 163.mp3 | "Thirteen" | "Thirteen" | Will be played instead of 13:00 |

**Note:** Some things in the table above might not make sense in the English language, because it's been programmed for the Italian language. For example, in Italian when it's 13:00, you say "é l'una" instead of "sono le 13". In the `setup()`, there is a "sentence construction" section, where you can modify the code according to your needs/language, based on the current hour and minute.


## Wiring

A schematic will follow...

For now, the table down here uses the PURPLE pin numbers of this image:
![ESP32 Pinout](https://www.mischianti.org/wp-content/uploads/2021/07/ESP32-DEV-KIT-DevKitC-v4-pinout-mischianti.png)


| Module pin | ESP32 pin |
| --- | --- |
| **RTC module** | |
| RTC VCC | 3.3V |
| RTC GND | GND |
| RTC MISO/IO | GPIO 19 |
| RTC SCK/SCLK | GPIO 18 |
| RTC CS/CE | GPIO 5 |
| **MP3 module** | |
| MP3 VCC | 5V |
| MP3 GND | GND |
| MP3 RX | GPIO 17 |
| MP3 TX | GPIO 16 |