#ifndef DISPLAYLOOKUPTABLE_H
#define DISPLAYLOOKUPTABLE_H

/********************************************************************
 *@brief        Function for Display correct characters.
 *
 *@param[in]   digit  digit for display (A,b,C,d,E,F,L,P,-,_,G,n,V,r,t,o,u)
           A
         -----
       F | G | B
         -----
       E |   | C
         -----   .H
           D
   _______________________________________________
  |  H  |  G  |  F  |  E  |  D  |  C  |  B  |  A  |
  |_____|_____|_____|_____|_____|_____|_____|_____|
  |  h  |  c  |  d  |  e  |  f  |  a  |  b  |  g  |
  |_____|_____|_____|_____|_____|_____|_____|_____|

  Top is Actual mapping of segments , Below as per our board 
  to off segment make it 1, for on 0
  Ex for "-" =   1111 1110 = 0xFE
         "H" =   0010 0100

 ********************************************************************/

#include"stdint.h"

const uint8_t m_ig_digits [11]={0x81,0xbd,0xc8,0x98,0xb4,0x92,0x82,0xb9,0x80,0x90,0xff}; 
const uint8_t m_ig_char [20]={0xa0,0x86,0xc3,0x8c,0xc2,0xe2,0xc7,0xe0,0xfe,0xdf,0x83,0xae,0x85,0xee,0xc6,0x8e,0x8f,0x24}; /*{A,b,C,d,E,F,L,P,-,_,G,n,V,r,t,o,u}*/
const uint8_t m_iposition_buff[5] = {0,1,2,3,4}; // segment potion eg: seg0 is actually 3rd position on Board

#endif