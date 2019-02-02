/**********************************************************************/
/*This code is not clean and OFFCOURSE will be possible do it better! */ 
/*this is only a group of functions to be used as independent code,  */ 
/*and by this way will be possible to any person can do the changes   */ 
/*of code and see what happen.                                         */
/*The code don't depend of any external library or extenal functions  */
/*complicated.                                                         */
/*I'm let the values to sent as binnary, this allow swap bit by bit */
/* to is possible test segment by segment without convert to HEX    */
/**********************************************************************/
/* This code show how is possible work with the driver LC75824 Sanyo */
/* This IC same times is marked as only the "366-34" from SANYO */
/* Also pinout is equivalent to the PT6524, check the datasheet */
/*Begining of Auto generated code by Atmel studio */
#define VFD_in 7  // This is the pin number 7 on Arduino UNO
#define VFD_clk 8 // This is the pin number 8 on Arduino UNO
#define VFD_ce 9 // This is the pin number 9 on Arduino UNO
//ATT: On the Uno and other ATMEGA based boards, unsigned ints (unsigned integers) are the same as ints in that they store a 2 byte value.
//Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647.
//*************************************************//
void setup() {
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_ce, OUTPUT);
 
  pinMode(13, OUTPUT);
  Serial.begin(9600); // only to debug
}
void send_char(unsigned char a)
{
 //
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  // the validation of data happen when clk go from LOW to HIGH.
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
  digitalWrite(VFD_ce, LOW); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  digitalWrite(VFD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
  for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(VFD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
    //
    digitalWrite(VFD_ce, HIGH); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  }
}
/*********************************************************************/
// I h've created 3 functions to send bit's, one with strobe, other without strobe and one with first byte with strobe followed by remaing bits.
void send_char_without(unsigned char a)
{
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
  }
}
/*********************************************************************/
void send_char_8bit_stb(unsigned char a)
{
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
 int i = -1;
  data=a;
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
  for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
   i++;
   digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// 
    delayMicroseconds(1);
    if (i==7){
    Serial.println(i);
    digitalWrite(VFD_ce, HIGH);
    delayMicroseconds(1);
    }  
  }
}
/*******************************************************************/
void patternVFD(){
//send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
//The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B01010011);  send_char_without(0B10110100); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000111);  send_char_without(0B11111001); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B00000010);  send_char_without(0B10101110); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000100);  send_char_without(0B00000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
/*******************************************************************/
void allON(){
//send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
//The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000001); //(0x41) firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B11111111);  send_char_without(0B11111111); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B11111111);  send_char_without(0B11111111); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B11111111);  send_char_without(0B11111111); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00001111);  send_char_without(0B00000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B11111111);  send_char_without(0B11111111); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B11111111);  send_char_without(0B11111111); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B11111111);  send_char_without(0B11111111); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00001111);  send_char_without(0B01000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B11111111);  send_char_without(0B11111111); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B11111111);  send_char_without(0B11111111); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B11111111);  send_char_without(0B11111111); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00001111);  send_char_without(0B10000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B11111111);  send_char_without(0B11111111); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B11111111);  send_char_without(0B11111111); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B11111111);  send_char_without(0B11111111); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00001111);  send_char_without(0B11000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
/**************************************************************************/
void allOFF(){
//send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
//The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000000);  send_char_without(0B00000000); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000000);  send_char_without(0B00000000); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B00000000);  send_char_without(0B00000000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B00000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000000);  send_char_without(0B00000000); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000000);  send_char_without(0B00000000); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B00000000);  send_char_without(0B00000000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B01000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000000);  send_char_without(0B00000000); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000000);  send_char_without(0B00000000); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B00000000);  send_char_without(0B00000000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B10000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000000);  send_char_without(0B00000000); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000000);  send_char_without(0B00000000); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B00000000);  send_char_without(0B00000000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B11000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
/****************************************************************************/
void HiFolks(){
//send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
//The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000000);  send_char_without(0B00000000); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000010);  send_char_without(0B00100000); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B00000110);  send_char_without(0B00000111); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B00000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00100000);  send_char_without(0B00000000); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000000);  send_char_without(0B00000111); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B01110010);  send_char_without(0B01011000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000110);  send_char_without(0B01000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000000);  send_char_without(0B00000000); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000000);  send_char_without(0B00000000); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B00000000);  send_char_without(0B00000000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B10000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000000);  send_char_without(0B01010110); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00110000);  send_char_without(0B01101000); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B10000010);  send_char_without(0B00000000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B11000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
/*********************************************************************/
void ImAlive(){
//send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
//The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000000);  send_char_without(0B00000000); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000010);  send_char_without(0B00010001); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B00001000);  send_char_without(0B00000000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B00000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00010110);  send_char_without(0B00000101); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000000);  send_char_without(0B00000111); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B01100111);  send_char_without(0B00001000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B01000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000000);  send_char_without(0B00000000); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B00000000);  send_char_without(0B00000000); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B00000000);  send_char_without(0B00000000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B10000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
//
send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
//
// On the 75824 of Sanyo the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
send_char_without(0B00000010);  send_char_without(0B00010110); //  1:8    -9:16// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B01110000);  send_char_without(0B00101000); // 17:24  -25:32// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B01000010);  send_char_without(0B00000000); // 33:40  -41:48// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B00000000);  send_char_without(0B11000000); // 49:56  -57:64 // 0,0,CU,p0,p1,p2,p3,DR,SC,BU, 0, 0; Last 2 bits is "DD" data direction, and is used
// to mark the 4 groups of 64 bits, 00, 01, 10, 11.
delayMicroseconds(1);
digitalWrite(VFD_ce, LOW); // 
delayMicroseconds(1);
}
/*********************************************************************/
void loop() {
 //patternVFD(); // Only to debug, send 8 bits command and 64 remaning bites. 52 to segments and last 12 of controle
 delay(2000);
 allON(); // All on
 delay(2000);
 allOFF(); // All off
 delay(1000);
 HiFolks();
 delay(1500);
     for (int n=0; n< 3; n++){
       ImAlive();
     delay(500);
     allOFF(); // All off
     delay(300);
     }
