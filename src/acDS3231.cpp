/*
 * Copyright (c) 2017 by Acácio Neimar de Oliveira <neimar2009@gmail.com>
 * acDS3231.cpp
 */

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <acWire.h>
#include "acDS3231.h"

// #define bcd2bin(val) ((uint8_t)((val) - 6 * ((val) >> 4)))
// #define bin2bcd(val) ((uint8_t)((val) + 6 * ((val) / 10)))

uint8_t bcd2bin (uint8_t val) {

  return val - 6 * (val >> 4);
}

uint8_t bin2bcd (uint8_t val) {
  
  return val + 6 * (val / 10);
}

// // Convert normal decimal numbers to binary coded decimal
// byte decToBcd(byte val) {
//   return( (val/10*16) + (val%10) );
// }

// // Convert binary coded decimal to normal decimal numbers
// byte bcdToDec(byte val) {
//   return( (val/16*10) + (val%16) );
// }


/* PROGMEN  */
  const char SUN01[] PROGMEM = {"Sun"};
  const char SUN55[] PROGMEM = {"Dom"};

  const char MON01[] PROGMEM = {"Mon"};
  const char MON55[] PROGMEM = {"Seg"};

  const char TUE01[] PROGMEM = {"Tue"};
  const char TUE55[] PROGMEM = {"Ter"};

  const char WED01[] PROGMEM = {"Wed"};
  const char WED55[] PROGMEM = {"Qua"};

  const char THU01[] PROGMEM = {"Thu"};
  const char THU55[] PROGMEM = {"Qui"};

  const char FRI01[] PROGMEM = {"Fri"};
  const char FRI55[] PROGMEM = {"Sex"};

  const char SAT01[] PROGMEM = {"Sat"};
  const char SAT55[] PROGMEM = {"Sab"};

  const char* const SUN[] PROGMEM = {SUN01, SUN55};
  const char* const MON[] PROGMEM = {MON01, MON55};
  const char* const TUE[] PROGMEM = {TUE01, TUE55};
  const char* const WED[] PROGMEM = {WED01, WED55};
  const char* const THU[] PROGMEM = {THU01, THU55};
  const char* const FRI[] PROGMEM = {FRI01, FRI55};
  const char* const SAT[] PROGMEM = {SAT01, SAT55};

  const char ERR[] PROGMEM = {"Err"};

void acDS3231Class::begin() {

  internalWire.begin(0x68);
}

boolean acDS3231Class::now() {

  if (nowTime() && nowDate()) return true;
  return false;
}

boolean acDS3231Class::nowTime() {

  internalWire.writeBegin(0);
  if (internalWire.readEnd(&dateTime.second, 3)) {
    /* uint8_t second = 0, minute = 1, hour = 2, weekDay = 3, day = 4, month = 5, year = 6; */
    dateTime.second = bcd2bin(dateTime.second);
    dateTime.minute = bcd2bin(dateTime.minute);
    uint8_t hour = dateTime.hour;
    // dateTime.hour = (((hour & 0x10) >> 4) * 10) + (hour & 0x0F);
    dateTime.hour = bcd2bin(dateTime.hour);
    if (hour & 0x40 && hour & 0x20) {
        dateTime.hour += 12; // 12 hour PM to 24 hour
    }
    return true;
  } else {
    uint8_t* dt = &dateTime.second;
    for (uint8_t i = 0; i < 3; ++i) dt[i] = 0;
  }
  return false;  
}

boolean acDS3231Class::nowWeekDay() {

  internalWire.writeBegin(3);
  dateTime.weekDay = internalWire.readEnd();
  return dateTime.weekDay != 0;
}

boolean acDS3231Class::nowDate() {

  internalWire.writeBegin(4);
  if (internalWire.readEnd(&dateTime.day, 3)) {
    /* uint8_t second = 0, minute = 1, hour = 2, weekDay = 3, day = 4, month = 5, year = 6; */
    dateTime.day   = bcd2bin(dateTime.day);
    dateTime.month = bcd2bin(dateTime.month);
    dateTime.year  = bcd2bin(dateTime.year);
    return true;
  } else {
    uint8_t* dt = &dateTime.second;
    for (uint8_t i = 4; i < 3; ++i) dt[i] = 0;
  }
  return false;
}

uint8_t acDS3231Class::dateTimeFmt(uint16_t v, uint8_t& p, char& sep) {

  if (v < 10) {
    Fmt[p] = '0';
    p++;
  }
  p += strlen(ltoa(v, &Fmt[p], 10));
  if (sep != 0){
    Fmt[p] = sep;
    p++;
  } else {
    Fmt[p] = 0;
  }
  return p;
}

void acDS3231Class::dtFmt(uint8_t v) {

  char sep = 0;
  uint8_t p = 0;
  dateTimeFmt(v, p, sep);
}

char* acDS3231Class::strHour() {

  dtFmt(dateTime.hour);
  return Fmt;
}

char* acDS3231Class::strMinute() {

  dtFmt(dateTime.minute);
  return Fmt;
}

char* acDS3231Class::strSecond() {

  dtFmt(dateTime.second);
  return Fmt;
}

char* acDS3231Class::nowTimeFmt(char sep) {

  uint8_t p = 0;
  
  nowTime();

  p = dateTimeFmt(dateTime.hour,   p, sep);
  p = dateTimeFmt(dateTime.minute, p, sep);
  sep = 0;
  p = dateTimeFmt(dateTime.second, p, sep);
  return Fmt;
}

char* acDS3231Class::nowDateFmt(char sep) {

  uint8_t p = 0;

  nowDate();

  p = dateTimeFmt(dateTime.year + 2000, p, sep);
  p = dateTimeFmt(dateTime.month, p, sep);
  sep = 0;
  p = dateTimeFmt(dateTime.day,   p, sep);
  return Fmt;
}

char* acDS3231Class::nowWeekDayFmt(uint8_t cod = 1) {

  switch (cod) {
    case 55 : cod = 1;
    break;
    default:
      cod = 0;
    break;
  }

  nowWeekDay();

  switch (dateTime.weekDay) {
    case 1:
      strcpy_P(Fmt, SUN[cod]);
      break;
    case 2:
      strcpy_P(Fmt, MON[cod]);
      break;
    case 3:
      strcpy_P(Fmt, TUE[cod]);
      break;
    case 4:
      strcpy_P(Fmt, WED[cod]);
      break;
    case 5:
      strcpy_P(Fmt, THU[cod]);
      break;
    case 6:
      strcpy_P(Fmt, FRI[cod]);
      break;
    case 7:
      strcpy_P(Fmt, SAT[cod]);
      break;
    default: 
      strcpy(Fmt, ERR);
    break;
  }
  return Fmt;
}

boolean acDS3231Class::setNow(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {

  return(setTime(hour, minute, second) && setDate(year, month, day));
}

boolean acDS3231Class::setTime(uint8_t hour, uint8_t minute, uint8_t second) {

  second = bin2bcd(second);
  minute = bin2bcd(minute);
  hour   = bin2bcd(hour);

  internalWire.writeBegin(0);
  internalWire.write(second);
  internalWire.write(minute);
  internalWire.writeEnd(hour);

  return true;
}

boolean acDS3231Class::setWeekDay(uint8_t weekDay) {

  internalWire.writeBegin(3);
  internalWire.writeEnd(weekDay);

  return true;
}

boolean acDS3231Class::setDate(uint16_t year, uint8_t month, uint8_t day) {

  day = bin2bcd(day);
  month = bin2bcd(month);
  if(year > 99) year -= 2000;
  uint8_t shortYear = bin2bcd((uint8_t)year);

  internalWire.writeBegin(4);
  internalWire.write(day);
  internalWire.write(month);
  internalWire.writeEnd(shortYear);

  return true;
}