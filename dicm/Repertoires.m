//
//  Repertoires.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-02-28.
//

#include <Foundation/Foundation.h>
#include "Repertoires.h"


const char *repertoirestr[]={
   //one code
   "",                //empty
   "ISO_IR 100",      //latin1
   "ISO_IR 101",      //latin2
   "ISO_IR 109",      //latin3
   "ISO_IR 110",      //latin4
   "ISO_IR 148",      //latin5
   "ISO_IR 126",      //greek
   "ISO_IR 127",      //arabic

   "ISO_IR 192",      //utf-8 (multi byte)
   "RFC3986 ",         //for UR (not a DICOM defined code)
   "ISO_IR 13 ",      //japanese
   "ISO_IR 144",      //cyrilic
   "ISO_IR 138",      //hebrew
   "ISO_IR 166",      //thai
   "GB18030 ",        //chinese  (multi byte)
   "GBK ",            //chinese simplified  (multi byte)
   //code extension
   "ISO 2022 IR 6",  //default
   "ISO 2022 IR 100",//latin1
   "ISO 2022 IR 101",//latin2
   "ISO 2022 IR 109",//latin3
   "ISO 2022 IR 110",//latin4
   "ISO 2022 IR 148",//latin5
   "ISO 2022 IR 126",//greek
   "ISO 2022 IR 127",//arabic
   "ISO 2022 IR 87",  //japanese (multi byte)
   "ISO 2022 IR 159",//japanese (multi byte)
   "ISO 2022 IR 13",  //japanese
   "ISO 2022 IR 144",//cyrilic
   "ISO 2022 IR 138",//hebrew
   "ISO 2022 IR 166",//thai
   "ISO 2022 IR 149",//korean (multi byte)
   "ISO 2022 IR 58"   //chinese simplified (multi byte)
};

uint32 repertoireidx( uint8_t *vbuf, uint16 vallength )
{
   uint32 idx=0x9;//error
   switch (vallength){
      case 0: idx=REPERTOIRE_GL;break;
      case 4: idx=GBK;break;
      case 8: idx=GB18030;break;
      case 10:{
         switch ((vbuf[9]<<8) | vbuf[8]) {
            case 0x3030: idx=ISO_IR100;break;//latin1
            case 0x3130: idx=ISO_IR101;break;//latin2
            case 0x3930: idx=ISO_IR109;break;//latin3
            case 0x3031: idx=ISO_IR110;break;//latin4
            case 0x3834: idx=ISO_IR148;break;//latin5
            case 0x3632: idx=ISO_IR126;break;//greek
            case 0x3732: idx=ISO_IR127;break;//arabic
            case 0x3239: idx=ISO_IR192;break;//utf-8
            case 0x2033: idx=ISO_IR13;break;//japones
               //return RFC3986
            case 0x3434: idx=ISO_IR144;break;//cyrilic
            case 0x3833: idx=ISO_IR138;break;//hebrew
            case 0x3636: idx=ISO_IR166;break;//thai
         }
      }
      case 14:{
#pragma mark TODO
      }
      case 16:{
#pragma mark TODO
      }
      default:{
#pragma mark TODO encoding extension
      }
   }
   if (strncmp(repertoirestr[idx],(char*)vbuf,vallength)==0) return idx;
   else return 0x9;//error
}

NSString *repertoirename( uint16 repertoireidx)
{
   return [NSString stringWithUTF8String:repertoirestr[repertoireidx]];
}
