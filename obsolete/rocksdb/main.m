//
//  main.m
//  dckvparserrocksdb
//
//  Created by jacquesfauquex on 2024-02-12.
//

#import <Foundation/Foundation.h>
#import "ObjectiveRocks.h"


//short length value vr
const uint16 AE=0x4541;
const uint16 AS=0x5341;
const uint16 AT=0x5441;
const uint16 CS=0x5343;
const uint16 DA=0x4144;
const uint16 DS=0x5344;
const uint16 DT=0x5444;
const uint16 FD=0x4446;
const uint16 FL=0x4C46;
const uint16 IS=0x5349;
const uint16 LO=0x4f4c;
const uint16 LT=0x544c;
const uint16 PN=0x4e50;
const uint16 SH=0x4853;
const uint16 SL=0x4C53;
const uint16 SS=0x5353;
const uint16 ST=0x5453;
const uint16 TM=0x4d54;
const uint16 UI=0x4955;
const uint16 UL=0x4C55;
const uint16 US=0x5355;

//long length value vr
const uint16 OB=0x424F;
const uint16 OD=0x444F;
const uint16 OF=0x464F;
const uint16 OL=0x4C4F;
const uint16 OV=0x564F;
const uint16 OW=0x574F;
const uint16 SV=0x5653;
const uint16 UC=0x4355;
const uint16 UR=0x5255;
const uint16 UT=0x5455;
const uint16 UV=0x5655;

const uint16 UN=0x4E55;

const uint16 SQ=0x5153;

//propietary delimitation vr

const uint16 SA=0x0000;
const uint16 IA=0x2B2B;//++
const uint16 IZ=0x5F5F;//__
const uint16 SZ=0xFFFF;

enum repertoireenum{
   
   REPERTOIRE_GL,
   ISO_IR100,
   ISO_IR101,
   ISO_IR109,
   ISO_IR110,
   ISO_IR148,
   ISO_IR126,
   ISO_IR127,
   
   ISO_IR192,
   RFC3986,
   ISO_IR13,
   ISO_IR144,
   ISO_IR138,
   ISO_IR166,
   GB18030,
   GBK,
   
   ISO2022IR6,
   ISO2022IR100,
   ISO2022IR101,
   ISO2022IR109,
   ISO2022IR110,
   ISO2022IR148,
   ISO2022IR126,
   ISO2022IR127,
   
   ISO2022IR87,
   ISO2022IR159,
   ISO2022IR13,
   ISO2022IR144,
   ISO2022IR138,
   ISO2022IR166,
   ISO2022IR149,
   ISO2022IR58
};


NSString *repertoirestring[]={
   //one code
   @"",                //empty
   @"ISO_IR 100",      //latin1
   @"ISO_IR 101",      //latin2
   @"ISO_IR 109",      //latin3
   @"ISO_IR 110",      //latin4
   @"ISO_IR 148",      //latin5
   @"ISO_IR 126",      //greek
   @"ISO_IR 127",      //arabic

   @"ISO_IR 192",      //utf-8 (multi byte)
   @"RFC3986 ",         //for UR (not a DICOM defined code)
   @"ISO_IR 13 ",      //japanese
   @"ISO_IR 144",      //cyrilic
   @"ISO_IR 138",      //hebrew
   @"ISO_IR 166",      //thai
   @"GB18030 ",        //chinese  (multi byte)
   @"GBK ",            //chinese simplified  (multi byte)
   
   //code extension
   @"ISO 2022 IR 6 ",  //default
   @"ISO 2022 IR 100 ",//latin1
   @"ISO 2022 IR 101 ",//latin2
   @"ISO 2022 IR 109 ",//latin3
   @"ISO 2022 IR 110 ",//latin4
   @"ISO 2022 IR 148 ",//latin5
   @"ISO 2022 IR 126 ",//greek
   @"ISO 2022 IR 127 ",//arabic
   
   @"ISO 2022 IR 87",  //japanese (multi byte)
   @"ISO 2022 IR 159 ",//japanese (multi byte)
   @"ISO 2022 IR 13",  //japanese
   @"ISO 2022 IR 144 ",//cyrilic
   @"ISO 2022 IR 138 ",//hebrew
   @"ISO 2022 IR 166 ",//thai
   @"ISO 2022 IR 149 ",//korean (multi byte)
   @"ISO 2022 IR 58"   //chinese simplified (multi byte)
};

uint32 repertoireindexes( uint8_t *valbytes, uint16 vallength )
{
   switch (vallength){
      case 0:
         return REPERTOIRE_GL;
      case 4://@"GBK "
         return GBK;
      case 8://@"GB18030 "
         return GB18030;
      case 10:{
         uint16 *duos = (uint16*) valbytes+4 ;
         switch (*duos) {
            case 0x3030://latin1
               return ISO_IR100;
            case 0x3130://latin2
               return ISO_IR101;
            case 0x3930://latin3
               return ISO_IR109;
            case 0x3031://latin4
               return ISO_IR110;
            case 0x3834://latin5
               return ISO_IR148;
            case 0x3632://greek
               return ISO_IR126;
            case 0x3732://arabic
               return ISO_IR127;
            case 0x3239://utf-8
               return ISO_IR192;
            case 0x2033://japones
               return ISO_IR13;
               
               //return RFC3986
            case 0x3434://cyrilic
               return ISO_IR144;
            case 0x3833://hebrew
               return ISO_IR138;
            case 0x3636://thai
               return ISO_IR166;
            default:
               return 0x9;//error
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
   return 0x9;//error
}


struct trl {
   uint32 t;
   uint16 r;
   uint16 l;
};

struct ele {
   uint8 g;
   uint8 G;
   uint8 u;
   uint8 U;
   uint16 r;
   uint16 l;
};

void swaptag(struct ele *a)
{
   unsigned char swap;
   swap=a->g;
   a->g=a->G;
   a->G=swap;
   swap=a->u;
   a->u=a->U;
   a->U=swap;
}

uint32 letag(uint32 t)
{
   return
     ((t & 0xff) << 24)
   + ((t & 0xff00) << 16)
   + ((t & 0xff0000) >> 16)
   + ((t & 0xff000000) >> 24)
   ;
}

uint32 attrletag(struct ele *a)
{
   return (a->g << 24) + (a->G << 16) + (a->u << 8) + a->U;
}

NSData  *emptyData=nil;
const uint64 SZbytes=0xDDE0FEFF;//FFFEE0DD00000000
NSData  *SZdata=nil;
const uint64 IAbytes=0xFFFFFFFF00E0FEFF;//FFFEE000FFFFFFFF
NSData  *IAdata=nil;
const uint64 IZbytes=0x0DE0FEFF;//FFFEE00D00000000
NSData  *IZdata=nil;

const uint32 ffffffff=0xFFFFFFFF;
const uint32 fffee000=0xFFFEE000;
const uint32 fffee00d=0xFFFEE00D;
const uint32 fffee0dd=0xFFFEE0DD;

BOOL ignorebeforebasetag(
           uint8_t *keybytes,
           BOOL readfirstattr,
           uint8_t *valbytes,
           NSInputStream *stream,
           uint64 *idx,
           BOOL basetag,
           uint32 beforebyte,
           uint32 beforetag,
           NSError *error
           )
{
   uint16 vl=0;//keeps vl while overwritting it in keybytes
   uint8 *attrbytes=keybytes;//subbuffer for attr reading
   struct trl *attrstruct=(struct trl*) attrbytes;//corresponding struct
   NSInteger attrbytesread=0;
   uint8 *llbytes=keybytes+8;//subbuffer for ll reading
   uint32 *ll=(uint32*)llbytes;//corresponding uin32
   if (readfirstattr) {
      attrbytesread=[stream read:attrbytes maxLength:8];
      switch (attrbytesread) {
         case 8:
         {
            if (basetag && (attrstruct->t==beforetag)) return true;
         };break;
         case 0: return false;//no more attrs
         case -1://stream error
         {
            NSLog(@"stream error: %@", [stream streamError]);
            return false;
         }
         default://less than 8 bytes
         {
            NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
            NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
            return false;
         }
      }
   }
   while ((*idx < beforebyte) &&  ( attrstruct->t != beforetag))
   {
      NSLog(@"%04x%04x",attrstruct->t & 0xFFFF,attrstruct->t >> 16);
      switch (attrstruct->r) {
#pragma mark vl
         case FD://floating point double
         case FL://floating point single
         case SL://signed long
         case SS://signed short
         case UL://unsigned long
         case US://unsigned short
         case AT://attribute tag
         case CS://coded string
         case UI://unique ID
         case AE://application entity
         case AS://age string
         case DA://date
         case DS://decimal string
         case DT://date time
         case IS://integer string
         case TM://time
         case LO://long string
         case LT://long text
         case SH://short string
         case ST://short text
         case PN://person name
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return false;
               }
               
            }
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8:
               {
                  if (basetag && (attrstruct->t==beforetag)) return true;
               };break;
               case 0: return false;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return false;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return false;
               }
            }
         } break;
            
#pragma mark vll
         case OB://other byte
         case OD://other double
         case OF://other float
         case OL://other long
         case OV://other 64-bit very long
         case OW://other word
         case SV://signed 64-bit very long
         case UV://unsigned 64-bit very long
         case UC://unlimited characters
         case UT://unlimited text
         case UR://universal resource identifier/locator
         case UN://unknown
         {
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return false;
            }
            *idx += 12 + *ll;

            if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
               }
            } else {
               //loop read into NSMutableData
               while (*ll>0xFFFF)
               {
                  if ([stream read:valbytes maxLength:0xFFFF]!=0xFFFF) {
                     //NSLog(@"error");
                     return false;
                  }
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
               }
            }
               
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8:
               {
                  if (basetag && (attrstruct->t==beforetag)) return true;
               };break;
               case 0: return false;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return false;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return false;
               }
            }
         } break;

#pragma mark SQ
         case SQ://sequence
         {
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return false;
            }
            if (*ll==0)
            {
               *idx += 12;//do not add *ll !
               attrbytesread=[stream read:attrbytes maxLength:8];
               switch (attrbytesread) {
                  case 8:
                  {
                     if (attrstruct->t==beforetag) return true;
                  };break;
                  case 0: return false;//no more attrs
                  case -1://stream error
                  {
                     NSLog(@"stream error: %@", [stream streamError]);
                     return false;
                  }
                  default://less than 8 bytes
                  {
                     NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                     NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                     return false;
                  }
               }
            }
            else //SQ *ll!=0
            {
               //SQ length
               uint64 beforebyteSQ;
               if (*ll==ffffffff) beforebyteSQ=beforebyte;
               else if (beforebyte==ffffffff) beforebyteSQ= *idx + *ll;
               else if (*idx + *ll > beforebyte) {
                  NSLog(@"incomplete input");
                  return false;
               }
               else beforebyteSQ=*idx + *ll;

               *idx += 12;//do not add *ll !

               
#pragma mark item level
               attrbytesread=[stream read:attrbytes maxLength:8];
               switch (attrbytesread) {
                  case 8:
                  {
                     if (basetag && (attrstruct->t==beforetag)) return true;
                  };break;
                  case 0: return false;//no more attrs
                  case -1://stream error
                  {
                     NSLog(@"stream error: %@", [stream streamError]);
                     return false;
                  }
                  default://less than 8 bytes
                  {
                     NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                     NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                     return false;
                  }
               }

               //for each first attr fffee000 of any new item
               while ((*idx < beforebyteSQ) && (attrstruct->t==0xe000fffe)) //itemstart
               {
                  uint32 IQll = (attrstruct->l << 16) | attrstruct->r;
                  uint64 beforebyteIT;//to be computed from after item start
                  *idx+=8;
                  if (IQll==ffffffff) beforebyteIT=beforebyteSQ;
                  else if (beforebyteSQ==ffffffff) beforebyteIT=*idx + *ll;
                  else if (*idx + *ll > beforebyteSQ) {
                     NSLog(@"incomplete input");
                     return false;
                  }
                  else beforebyteIT=*idx + *ll;

                  ignorebeforebasetag(
                        keybytes,
                        true,
                        valbytes,
                        stream,
                        idx,
                        false,
                        (uint32)beforebyteIT,
                        0xE00DFFFE,
                        error
                        );
                  //Atención!!! attr still is the first attr of the first item
                  //the attr of the recursion is not available
                  //keybytes kept the last attr read one level deeper than current attr

                  
                  //write IZ to db feff0de0
                  if (attrstruct->t==0xe00dfffe)
                  {
                     *idx+=8;
                     attrbytesread=[stream read:attrbytes maxLength:8];
                     switch (attrbytesread) {
                        case 8:
                        {
                           if (basetag && (attrstruct->t==beforetag)) return true;
                        };break;
                        case 0: return false;//no more attrs
                        case -1://stream error
                        {
                           NSLog(@"stream error: %@", [stream streamError]);
                           return false;
                        }
                        default://less than 8 bytes
                        {
                           NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                           NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                           return false;
                        }
                     }

                  }
               }//end while item
#pragma mark item level end
               
               //itemstruct may be SZ or post SQ feffdde0
               if (attrstruct->t==0xe0ddfffe)
               {
                  *idx+=8;
                  attrbytesread=[stream read:attrbytes maxLength:8];
                  switch (attrbytesread) {
                     case 8:
                     {
                        if (basetag && (attrstruct->t==beforetag)) return true;
                     };break;
                     case 0: return false;//no more attrs
                     case -1://stream error
                     {
                        NSLog(@"stream error: %@", [stream streamError]);
                        return false;
                     }
                     default://less than 8 bytes
                     {
                        NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                        NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                        return false;
                     }
                  }
               }
            }
         } break;

#pragma mark unknown VR
         default:
         {
            NSLog(@"error unknown vr");
            return false;
         }
      }//end switch
   }//end while (*index < beforebyte)

   return false;
}

void parse(
           uint8_t *keybytes,
           uint8 keydepth,
           BOOL readfirstattr,
           uint16 keycs,
           uint8_t *valbytes,

           NSInputStream *stream,
           uint64 *idx,
           uint32 beforebyte,
           uint32 beforetag,
           
           RocksDB *db,
           NSError *error
           )
{
   uint16 vl=0;//keeps vl while overwritting it in keybytes
   uint8 *attrbytes=keybytes+keydepth;//subbuffer for attr reading
   struct ele *attrstruct=(struct ele*) attrbytes;//corresponding struct
   NSInteger attrbytesread=0;
   uint8 *llbytes=keybytes+keydepth+8;//subbuffer for ll reading
   uint32 *ll=(uint32*)llbytes;//corresponding uin32

   if (readfirstattr)
   {
      attrbytesread=[stream read:attrbytes maxLength:8];
      switch (attrbytesread) {
         case 8: swaptag(attrstruct); break;
         case 0: return;//no more attrs
         case -1://stream error
         {
            NSLog(@"stream error: %@", [stream streamError]);
            return;
         }
         default://less than 8 bytes
         {
            NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
            NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
            return;
         }
      }
   }

   while ((*idx < beforebyte) &&  ( attrletag(attrstruct) < beforetag))
   {
      switch (attrstruct->r) {
#pragma mark vl bin
         case FD://floating point double
         case FL://floating point single
         case SL://signed long
         case SS://signed short
         case UL://unsigned long
         case US://unsigned short
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return;
               } else {
                  attrstruct->l=REPERTOIRE_GL;
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:vl freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }

         } break;
#pragma mark vl tag code
         case AT://attribute tag
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return;
               } else {
                  attrstruct->l=REPERTOIRE_GL;
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:vl freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;
#pragma mark vl ascii code
         case CS://coded string
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return;
               } else {
                  attrstruct->l=REPERTOIRE_GL;

                  if ( (attrstruct->U ==0x05) && (attrstruct->g ==0x00) && (attrstruct->G ==0x08) && (attrstruct->u ==0x00) ){
                     NSData *encodingdata=[NSData dataWithBytesNoCopy:valbytes length:vl freeWhenDone:false];
                     NSString *encodingstring=[[NSString alloc]initWithData:encodingdata encoding:NSASCIIStringEncoding];
                     uint16 repidxs=repertoireindexes(valbytes,vl);

                     if ((repidxs==0x09)||(![repertoirestring[repidxs] isEqualToString:encodingstring]))
                        NSLog(@"BAD REPERTOIRE: '%@' %@",encodingstring,encodingdata.description);
                     else keycs=(keycs & 0x8000) | repidxs;
                  }
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:vl freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;
#pragma mark vl oid code
         case UI://unique ID
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return;
               } else {
                  attrstruct->l=REPERTOIRE_GL;
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:vl freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;
#pragma mark vl ascii
         case AE://application entity
         case AS://age string
         case DA://date
         case DS://decimal string
         case DT://date time
         case IS://integer string
         case TM://time
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return;
               } else {
                  attrstruct->l=REPERTOIRE_GL;
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:vl freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db 
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;
#pragma mark vl charset
         case LO://long string
         case LT://long text
         case SH://short string
         case ST://short text
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return;
               } else {
                  attrstruct->l=keycs;
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:vl freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;
#pragma mark vl person
         case PN://person name
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return;
               } else {
                  attrstruct->l=keycs;
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:vl freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;
#pragma mark vll bin
         case OB://other byte
         case OD://other double
         case OF://other float
         case OL://other long
         case OV://other 64-bit very long
         case OW://other word
         case SV://signed 64-bit very long
         case UV://unsigned 64-bit very long
         {
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:*ll freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else {
               //loop read into NSMutableData
               NSMutableData *lldata=[NSMutableData dataWithLength:*ll];
               while (*ll>0xFFFF)
               {
                  if ([stream read:valbytes maxLength:0xFFFF]!=0xFFFF) {
                     //NSLog(@"error");
                     return;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               }
               [lldata appendBytes:valbytes length: *ll];
               [db
                setData:lldata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
               
             
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;
#pragma mark vll charset
         case UC://unlimited characters
         case UT://unlimited text
         {
            attrstruct->l=keycs;

            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:*ll freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else {
               //loop read into NSMutableData
               NSMutableData *lldata=[NSMutableData dataWithLength:*ll];
               while (*ll>0xFFFF)
               {
                  if ([stream read:valbytes maxLength:0xFFFF]!=0xFFFF) {
                     //NSLog(@"error");
                     return;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               }
               [lldata appendBytes:valbytes length: *ll];
               [db
                setData:lldata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;
#pragma mark vll RFC3986
         case UR://universal resource identifier/locator
         {
            attrstruct->l=RFC3986;

            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:*ll freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else {
               //loop read into NSMutableData
               NSMutableData *lldata=[NSMutableData dataWithLength:*ll];
               while (*ll>0xFFFF)
               {
                  if ([stream read:valbytes maxLength:0xFFFF]!=0xFFFF) {
                     //NSLog(@"error");
                     return;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               }
               [lldata appendBytes:valbytes length: *ll];
               [db
                setData:lldata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;

      //---------
#pragma mark UN
         case UN://unknown
         {
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:*ll freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else {
               //loop read into NSMutableData
               NSMutableData *lldata=[NSMutableData dataWithLength:*ll];
               while (*ll>0xFFFF)
               {
                  if ([stream read:valbytes maxLength:0xFFFF]!=0xFFFF) {
                     //NSLog(@"error");
                     return;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               }
               [lldata appendBytes:valbytes length: *ll];
               [db
                setData:lldata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return;//no more attrs
               case -1://stream error
               {
                  NSLog(@"stream error: %@", [stream streamError]);
                  return;
               }
               default://less than 8 bytes
               {
                  NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                  NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                  return;
               }
            }
         } break;
            
      //---------
#pragma mark SQ
         case SQ://sequence
         {
            //register SQ con vr 0000 in rocksdb
            keybytes[keydepth+0x8]=0xff;
            keybytes[keydepth+0x9]=0xff;
            keybytes[keydepth+0xA]=0xff;
            keybytes[keydepth+0xB]=0xff;
            NSData *SQdata=[NSData dataWithBytes:keybytes+keydepth length:12];
            attrstruct->r=SA;
            attrstruct->l=REPERTOIRE_GL;
            [db
             setData:SQdata
             forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
             error:&error
            ];
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return;
            }
            if (*ll==0)
            {
               attrstruct->r=SZ;
               attrstruct->l=0x0;
               [db
                setData:SZdata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];

               //read nextattr
               *idx += 12;//do not add *ll !
               attrbytesread=[stream read:attrbytes maxLength:8];
               switch (attrbytesread) {
                  case 8: swaptag(attrstruct); break;
                  case 0: return;//no more attrs
                  case -1://stream error
                  {
                     NSLog(@"stream error: %@", [stream streamError]);
                     return;
                  }
                  default://less than 8 bytes
                  {
                     NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                     NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                     return;
                  }
               }
            }
            else //SQ *ll!=0
            {
               //SQ length
               uint64 beforebyteSQ;
               if (*ll==ffffffff) beforebyteSQ=beforebyte;
               else if (beforebyte==ffffffff) beforebyteSQ= *idx + *ll;
               else if (*idx + *ll > beforebyte) {
                  NSLog(@"incomplete input");
                  return;
               }
               else beforebyteSQ=*idx + *ll;

               
               //replace vr and vl of SQ by itemnumber
               uint32 itemnumber=1;
               uint8_t *inb=(uint8_t*)&itemnumber;
               attrstruct->r= (*(inb+2) << 8) + *(inb+3);
               attrstruct->l= (*(inb+0) << 8) + *(inb+1);

               *idx += 12;//do not add *ll !

               
#pragma mark item level
               keydepth+=8;
               uint8 *itembytes=keybytes+keydepth;
               struct ele *itemstruct=(struct ele*) itembytes;
               attrbytesread=[stream read:itembytes maxLength:8];
               switch (attrbytesread) {
                  case 8: swaptag(itemstruct); break;
                  case 0: return;//no more attrs
                  case -1://stream error
                  {
                     NSLog(@"stream error: %@", [stream streamError]);
                     return;
                  }
                  default://less than 8 bytes
                  {
                     NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                     NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                     return;
                  }
               }

               uint32 itemtag=attrletag(itemstruct);
               //for each first attr fffee000 of any new item
               while ((*idx < beforebyteSQ) && (itemtag==fffee000)) //itemstart compulsory
               {
                  uint32 IQll = (itemstruct->l << 16) | itemstruct->r;
                  itemstruct->g=0x0;
                  itemstruct->G=0x0;
                  itemstruct->u=0x0;
                  itemstruct->U=0x0;
                  itemstruct->r=IA;
                  itemstruct->l=REPERTOIRE_GL;
                  [db
                   setData:IAdata
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];

                  //
                  uint64 beforebyteIT;//to be computed from after item start
                  *idx+=8;
                  if (IQll==ffffffff) beforebyteIT=beforebyteSQ;
                  else if (beforebyteSQ==ffffffff) beforebyteIT=*idx + *ll;
                  else if (*idx + *ll > beforebyteSQ) {
                     NSLog(@"incomplete input");
                     return;
                  }
                  else beforebyteIT=*idx + *ll;

                  parse(
                        keybytes,
                        keydepth,
                        true,
                        keycs,
                        valbytes,
                        stream,
                        idx,
                        (uint32)beforebyteIT,
                        0xFFFEE00D,
                        db,
                        error
                        );
                  //Atención!!! attr still is the first attr of the first item
                  //the attr of the recursion is not available
                  //keybytes kept the last attr read one level deeper than current attr

                  
                  //write IZ to db
                  if (attrletag(itemstruct)==fffee00d)
                  {
                     itemstruct->g=0xff;
                     itemstruct->G=0xff;
                     itemstruct->u=0xff;
                     itemstruct->U=0xff;
                     itemstruct->r=IZ;
                     itemstruct->l=0x00;
                     [db
                      setData:IZdata
                      forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                      error:&error
                     ];
                     *idx+=8;
                     attrbytesread=[stream read:itembytes maxLength:8];
                     switch (attrbytesread) {
                        case 8: swaptag(itemstruct); break;
                        case 0: return;//no more attrs
                        case -1://stream error
                        {
                           NSLog(@"stream error: %@", [stream streamError]);
                           return;
                        }
                        default://less than 8 bytes
                        {
                           NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                           NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                           return;
                        }
                     }
                  }
                  else
                  {
                     struct ele copyattr;
                     copyattr.g=itemstruct->g;
                     copyattr.G=itemstruct->G;
                     copyattr.u=itemstruct->u;
                     copyattr.U=itemstruct->U;
                     copyattr.r=itemstruct->r;
                     copyattr.l=itemstruct->l;
                     itemstruct->g=0xff;
                     itemstruct->G=0xff;
                     itemstruct->u=0xff;
                     itemstruct->U=0xff;
                     itemstruct->r=IZ;
                     itemstruct->l=0x0;
                     [db
                      setData:IZdata
                      forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                      error:&error
                     ];
                     itemstruct->g=copyattr.g;
                     itemstruct->G=copyattr.G;
                     itemstruct->u=copyattr.u;
                     itemstruct->U=copyattr.U;
                     itemstruct->r=copyattr.r;
                     itemstruct->l=copyattr.l;
                  }
                  
                  //new item number
                  itemnumber+=1;
                  attrstruct->r= (*(inb+2) << 8) + *(inb+3);
                  attrstruct->l= (*(inb+0) << 8) + *(inb+1);

                  itemtag=attrletag(itemstruct);
               }//end while item
               keydepth-=8;
#pragma mark item level end

               attrstruct->r=SZ;
               attrstruct->l=0x0;
               [db
                setData:SZdata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];

               
               //itemstruct may be SZ or post SQ
               if (attrletag(itemstruct)==fffee0dd)
               {
                  *idx+=8;
                  attrbytesread=[stream read:attrbytes maxLength:8];
                  switch (attrbytesread) {
                     case 8: swaptag(attrstruct); break;
                     case 0: return;//no more attrs
                     case -1://stream error
                     {
                        NSLog(@"stream error: %@", [stream streamError]);
                        return;
                     }
                     default://less than 8 bytes
                     {
                        NSData *readdata=[NSData dataWithBytesNoCopy:attrstruct length:attrbytesread ];
                        NSLog(@"read %ld bytes. Should be 8 for tag vr vl %@", (long)attrbytesread, readdata.description);
                        return;
                     }
                  }
               }
               else
               {
                  attrstruct->g=itemstruct->g;
                  attrstruct->G=itemstruct->G;
                  attrstruct->u=itemstruct->u;
                  attrstruct->U=itemstruct->U;
                  attrstruct->r=itemstruct->r;
                  attrstruct->l=itemstruct->l;
               }
            }
         } break;
            
         default:
         {
            NSLog(@"error unknown vr");
            return;
         }
            
      //---------
      }//end switch
   }//end while (*index < beforebyte)
    
}

int main(int argc, const char * argv[]) {
   int returnstatus=0;
   NSError *error=nil;
   NSDate *start=[NSDate date];
   emptyData=[NSData data];
   SZdata=[NSData dataWithBytesNoCopy:(void*)&SZbytes length:8];
   IAdata=[NSData dataWithBytesNoCopy:(void*)&IAbytes length:8];
   IZdata=[NSData dataWithBytesNoCopy:(void*)&IZbytes length:8];

   @autoreleasepool {
      RocksDB *db = [RocksDB databaseAtPath:@"/Users/jacquesfauquex/rocksdb" andDBOptions:^(RocksDBOptions *options) {
         options.createIfMissing = YES;
      }];
      
      //https://stackoverflow.com/questions/19165134/correct-portable-way-to-interpret-buffer-as-a-struct
      uint8_t *keybytes = malloc(144);
      struct ele *baseattrstruct=(struct ele*) keybytes;
      char keydepth=0;
      uint8_t *valbytes = malloc(0xFFFF);//max vl
      //additional ll and llvaluedata read ll attrs of the stream
      uint64 streamindex=144;
      uint64 *idx=&streamindex;

//img
/*
      NSInputStream *stream1=[NSInputStream inputStreamWithFileAtPath:@"/Users/jacquesfauquex/dcvvparser/dcvvparser/img.dcm"];
      [stream1 open];
      streamindex=144;
      if (![stream1 hasBytesAvailable]) NSLog(@"no byte disponible");
      else if ([stream1 read:keybytes maxLength:*idx]!=*idx) NSLog(@"could not chop preamble");
      else
      {
         parse(
            keybytes,
            keydepth,
            true,
            ISO_IR100,
            valbytes,
            stream1,
            idx,
            0xFFFFFFFF,
            0x7FE00010,
            db,
            error
            );
      }
      [stream1 close];
*/
//sr
      NSInputStream *stream2=[NSInputStream inputStreamWithFileAtPath:@"/Users/jacquesfauquex/dcvvparser/dcvvparser/sr.dcm"];
      [stream2 open];
      streamindex=144;

      uint32 beforetag=0x0;
      if (![stream2 hasBytesAvailable]) NSLog(@"no byte dispo");
      else if ([stream2 read:keybytes maxLength:*idx]!=*idx) NSLog(@"could not chop preamble");
      {
         beforetag=0x00080018;
         NSLog(@"ignore before %08x",beforetag);
         if (ignorebeforebasetag(
               keybytes,
               true,
               valbytes,
               stream2,
               idx,
               true,
               0xFFFFFFFF,
               (beforetag>>16)+((beforetag & 0xFFFF)<<16),
               error
               )) NSLog(@"found %08x at index %llu",beforetag,*idx);
          else    NSLog(@"not found %08x before index %llu",beforetag,*idx);
          swaptag(baseattrstruct);

      
          parse(
                keybytes,
                keydepth,
                false,
                ISO_IR100,
                valbytes,
                stream2,
                idx,
                0xFFFFFFFF,
                0x00080020,
                db,
                error
                );//7FE00010

      
          beforetag=0x0020000d;
          NSLog(@"ignore before %08x",beforetag);
          if (ignorebeforebasetag(
                keybytes,
                false,
                valbytes,
                stream2,
                idx,
                true,
                0xFFFFFFFF,
                (beforetag>>16)+((beforetag & 0xFFFF)<<16),
                error
                )) NSLog(@"found %08x at index %llu",beforetag,*idx);
          else     NSLog(@"not found %08x before index %llu",beforetag,*idx);
          swaptag(baseattrstruct);

      
          parse(
                keybytes,
                keydepth,
                false,
                ISO_IR100,
                valbytes,
                stream2,
                idx,
                0xFFFFFFFF,
                0x00200010,
                db,
                error
               );//7FE00010
      }
      [stream2 close];

      
      RocksDBIterator *iterator = [db iterator];
      [iterator enumerateKeysAndValuesUsingBlock:^(NSData *key, NSData *value, BOOL *stop)
      {
         NSString *valuestring=[[NSString alloc]initWithData:value encoding:NSUTF8StringEncoding];
         NSLog(@"%s %@",key.description.UTF8String,valuestring);
         //fprintf(stderr, "%s %@\n",key.description.UTF8String,valuestring);
      }];
      [iterator close];
       

      [db close];

       if (error) NSLog(@"%@",error.description);
      
      free(keybytes);
      free(valbytes);

   }
   NSLog(@"elapsed %f", [[NSDate date]timeIntervalSinceDate:start]);
   return returnstatus;
}
