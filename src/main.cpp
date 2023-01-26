#include <Arduino.h>
#include <MD_YX5300.h>

#include <ThreeWire.h>  
#include <RtcDS1302.h>

// ------------------ RTC VARIABLES ------------------

#define MP3Stream Serial2  // Native serial port - change to suit the application
#define Console   Serial   // command processor input/output stream
MD_YX5300 mp3(MP3Stream);

// ------------------ RTC VARIABLES ------------------

ThreeWire myWire(19,18,5); // IO, SCLK, CE.... 19: MISO, 23: MOSI
RtcDS1302<ThreeWire> Rtc(myWire);

// ------------------ MY VARIABLES ------------------
// Folders on SD card
#define F_ORA_DALLE 1 //amount of files = ora_dalle
#define F_ORA_ALLE 2  //amount of files = ora_alle

// constants for sentence
#define M_INTRO 1
#define M_SONO_LE 2
#define M_E_ITS 3
#define M_E_AND 4
#define M_MEZZANOTTE 161
#define M_MEZZOGIORNO 162
#define M_LUNA 163

unsigned long t_checkclock = 0;
unsigned long t_printdate = 0;

#define HOURPLUS 12
#define HOURMINUS 14
#define MINUTEPLUS 27
#define MINUTEMINUS 26

int HourPlusState = 1;
int HourMinusState = 1;
int MinutePlusState = 1;
int MinuteMinusState = 1;

int ora;
int minuto;

int ora_dalle = 0;
int ora_alle = 23;

void(* resetFunc) (void) = 0;
  
void debugging(const MD_YX5300::cbData *status)
// Used to process device responses either as a library callback function
// or called locally when not in callback mode.
{
  
  
  switch (status->code)
  {
  case MD_YX5300::STS_OK:         Console.print(F("STS_OK"));         break;
  //case MD_YX5300::STS_OK:         Console.print(F("STS_OK")); start();         break;
  case MD_YX5300::STS_TIMEOUT:    Console.print(F("STS_TIMEOUT"));    break;
  case MD_YX5300::STS_VERSION:    Console.print(F("STS_VERSION"));    break;
  case MD_YX5300::STS_CHECKSUM:   Console.print(F("STS_CHECKSUM"));    break;
  case MD_YX5300::STS_TF_INSERT:  Console.print(F("STS_TF_INSERT"));  break;
  case MD_YX5300::STS_TF_REMOVE:  Console.print(F("STS_TF_REMOVE"));  break;
  case MD_YX5300::STS_ERR_FILE:   Console.print(F("STS_ERR_FILE"));   break;
  case MD_YX5300::STS_ACK_OK:     Console.print(F("STS_ACK_OK"));     break;
  case MD_YX5300::STS_FILE_END:   Console.print(F("STS_FILE_END"));   break;
  case MD_YX5300::STS_INIT:       Console.print(F("STS_INIT"));       break;
  case MD_YX5300::STS_STATUS:     Console.print(F("STS_STATUS"));     break;
  case MD_YX5300::STS_EQUALIZER:  Console.print(F("STS_EQUALIZER"));  break;
  case MD_YX5300::STS_VOLUME:     Console.print(F("STS_VOLUME"));     break;
  case MD_YX5300::STS_TOT_FILES:  Console.print(F("STS_TOT_FILES"));  break;
  case MD_YX5300::STS_PLAYING:    Console.print(F("STS_PLAYING"));    break;
  case MD_YX5300::STS_FLDR_FILES: Console.print(F("STS_FLDR_FILES")); break;
  case MD_YX5300::STS_TOT_FLDR:   Console.print(F("STS_TOT_FLDR"));   break;
  default: Console.print(F("STS_??? 0x")); Console.print(status->code, HEX); break;
  }
  Console.print(F(", 0x"));
  Console.println(status->data, HEX);

  
}

const MD_YX5300::cbData* waitingFor(enum MD_YX5300::status_t status_code){
  const MD_YX5300::cbData *status;
  bool checked = false;
  while(true){
    delay(50);
    checked = mp3.check();
    status = mp3.getStatus();
    if(checked){
      //debugging(status);
    }
    if(checked && (status->code == status_code || status->code == MD_YX5300::STS_ERR_FILE)){
      break; // leesbaarder
    }
  }
  debugging(status);

  return status;
}

void talk(int person_to_talk, int sentence[], int arrlen){
  mp3.wakeUp();
  delay(50);
    
  for(int i = 0; i < arrlen; i++){
    if(sentence[i] == 0) {
      break;
    }
    Console.print("value: "); Console.println(sentence[i]);
    mp3.playSpecific(person_to_talk, sentence[i]);
    waitingFor(MD_YX5300::STS_FILE_END);
  }
  
  Console.println("end");
  mp3.sleep();
}


void setRtcTime(int BtnPressed){
  RtcDateTime adesso = Rtc.GetDateTime();
  uint8_t newHour = adesso.Hour();
  uint8_t newMinute = adesso.Minute();
  int digitToSay[2];
  
  if(BtnPressed == HOURPLUS){
    newHour = (newHour >= 23) ? 0 : newHour + 1;
    digitToSay[0] = newHour + 100;
  }
   else if(BtnPressed == HOURMINUS){
    newHour = (newHour == 0) ? 23 : newHour - 1;
    digitToSay[0] = newHour + 100;
  }
  else if(BtnPressed == MINUTEPLUS){
    newMinute = (newMinute >= 59) ? 0 : newMinute + 1;
    digitToSay[0] = newMinute + 100;
  }
  else if(BtnPressed == MINUTEMINUS){
    newMinute = (newMinute == 0) ? 59 : newMinute - 1;
    digitToSay[0] = newMinute + 100;
  }
  
  RtcDateTime newadesso(adesso.Year(), adesso.Month(), adesso.Day(), newHour, newMinute, adesso.Second());
  Rtc.SetDateTime(newadesso);

  talk(3, digitToSay, 1);
 
}

void setup()
{
  // YX5300 Serial interface
  MP3Stream.begin(9600, SERIAL_8N1, 16, 17);
  delay(500);
  mp3.begin();
  mp3.setCallback(nullptr);
  mp3.setSynchronous(true);
  mp3.volume(30);

  Rtc.Begin();
  
  // command line interface
  Console.begin(57600);

  RtcDateTime adesso = Rtc.GetDateTime();
  ora = adesso.Hour();
  minuto = adesso.Minute();
//  ora = 12;
//  minuto = 0;

  
  Console.print("ora: "); Console.println(ora);
  Console.print("minuto: "); Console.println(minuto);
  pinMode(HOURPLUS, INPUT_PULLUP);
  pinMode(HOURMINUS, INPUT_PULLUP);
  pinMode(MINUTEPLUS, INPUT_PULLUP);
  pinMode(MINUTEMINUS, INPUT_PULLUP);

  int sentence[10];
  /* Constructing the sentence */
  sentence[0] = M_INTRO;

  if(ora == 0){
    sentence[1] = M_E_ITS;
    sentence[2] = M_MEZZANOTTE;
  }
  else if(ora == 1){
    sentence[1] = M_E_ITS;
    sentence[2] = M_LUNA;
  }
  else if(ora == 12){
    sentence[1] = M_E_ITS;
    sentence[2] = M_MEZZOGIORNO;
  }
  else {
    sentence[1] = M_SONO_LE;
    sentence[2] = ora + 100; // 101, 102, ...
  }

 
  if(minuto != 0){
    sentence[3] = M_E_AND;
    sentence[4] = minuto + 100; // 101, 102, ...  
  }

  // This is to get how many "people folders" there are. Then choose one at random for the sentence
  mp3.queryFolderCount();
  const MD_YX5300::cbData* result = waitingFor(MD_YX5300::STS_TOT_FLDR);
  int total_folders = result->data;
  //queryfoldercount gives total folders + 1;

  int person_to_talk = 3;
  if(total_folders > 4){
    person_to_talk = random(3, total_folders); //in random, min is included and max is excluded
  }
  


  //The from/to is based on the folders 01 and 02.
  //The amount of files in those folders will determine the from/to hours
  mp3.queryFolderFiles(F_ORA_DALLE);
  const MD_YX5300::cbData* files_one = waitingFor(MD_YX5300::STS_FLDR_FILES);
  if(files_one->code == MD_YX5300::STS_FLDR_FILES){
    ora_dalle = files_one->data;
  }

  Console.print("ora dalle: "); Console.println(ora_dalle);
  
  mp3.queryFolderFiles(F_ORA_ALLE);
  const MD_YX5300::cbData* files_two = waitingFor(MD_YX5300::STS_FLDR_FILES);
  if(files_two->code == MD_YX5300::STS_FLDR_FILES){
    ora_alle = files_two->data;
  }

  Console.print("ora alle: "); Console.println(ora_alle);
  
  talk(person_to_talk, sentence, 5);
}

void loop()
{
  delay(10);
  unsigned long currentMillis = millis();

  // checking if it's a new hour and has to reboot
  if(currentMillis - t_checkclock >= 1000*60){
    t_checkclock = currentMillis;
    Console.println("checking time.");
    RtcDateTime adesso = Rtc.GetDateTime();
    if(adesso.Hour() != ora && adesso.Hour() >= ora_dalle && adesso.Hour() <= ora_alle){
      resetFunc();
    }
  }

  int HourPlusStateNow = digitalRead(HOURPLUS);
  int HourMinusStateNow = digitalRead(HOURMINUS);
  int MinutePlusStateNow = digitalRead(MINUTEPLUS);
  int MinuteMinusStateNow = digitalRead(MINUTEMINUS);
  
  if(HourPlusState == 1 && HourPlusStateNow == 0){
    Console.println("HOURPLUS");
    setRtcTime(HOURPLUS);
  }
  if(HourMinusState == 1 && HourMinusStateNow == 0){
    Console.println("HOURMINUS");
    setRtcTime(HOURMINUS);
  }
  if(MinutePlusState == 1 && MinutePlusStateNow == 0){
    Console.println("MINUTEPLUS");
    setRtcTime(MINUTEPLUS);
  }
  if(MinuteMinusState == 1 && MinuteMinusStateNow == 0){
    Console.println("MINUTEMINUS");
    setRtcTime(MINUTEMINUS);
  }
  HourPlusState = HourPlusStateNow;
  HourMinusState = HourMinusStateNow;
  MinutePlusState = MinutePlusStateNow;
  MinuteMinusState = MinuteMinusStateNow;

//  if(currentMillis - t_printdate >= 3000){
//    t_printdate = currentMillis;
//    RtcDateTime dt = Rtc.GetDateTime();
//    Console.print(dt.Day()); Console.print("/"); Console.print(dt.Month()); Console.print("/"); Console.print(dt.Year());Console.print(" "); Console.print(dt.Hour()); Console.print(":"); Console.print(dt.Minute()); Console.print(":"); Console.print(dt.Second());
//    Console.println();
//  }
}
