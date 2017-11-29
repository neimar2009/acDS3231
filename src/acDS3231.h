/*
 * Copyright (c) 2017 by Acácio Neimar de Oliveira <neimar2009@gmail.com>
 * acDS3231.h
 */

const char us    PROGMEM = 1;
const char pt_br PROGMEM = 55;

class acDS3231Class {
public:
  // acDS3231Class();
  acDS3231Class(uint8_t pinSDA, uint8_t pinSCK) : internalWire(pinSDA, pinSCK) {}
  ~acDS3231Class() {}
  void begin();
  boolean now();
  boolean nowTime();
  boolean nowWeekDay();
  boolean nowDate();
  char* nowTimeFmt();
  char* nowWeekDayFmt(uint8_t cod = 1);
  char* nowDateFmt();
  boolean setNow(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
  boolean setTime(uint8_t hour, uint8_t minute, uint8_t second);
  boolean setWeekDay(uint8_t weekDay);
  boolean setDate(uint16_t year, uint8_t month, uint8_t day);
  uint16_t year()  { return dateTime.year + 2000; }
  uint8_t month()  { return dateTime.month; }
  uint8_t day()    { return dateTime.day; }
  uint8_t weekDay(){ return dateTime.weekDay; }
  uint8_t hour()   { return dateTime.hour; }
  uint8_t minute() { return dateTime.minute; }
  uint8_t second() { return dateTime.second; }
private:

  acWireClass internalWire;
  struct {
    uint8_t second = 0, minute = 0, hour = 0, weekDay = 0, day = 0, month = 0, year = 0;
  } dateTime;
};
