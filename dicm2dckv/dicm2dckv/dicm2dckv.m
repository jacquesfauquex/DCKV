//
//  dicm2mdbx.m
//  dicm2mdbx
//
//  Created by jacquesfauquex on 2024-02-28.
//

#include "dicm2dckv.h"
#include "os_log.h"


const uint64 SZbytes=0xDDE0FEFF;//FFFEE0DD00000000
const uint64 IAbytes=0xFFFFFFFF00E0FEFF;//FFFEE000FFFFFFFF
const uint64 IZbytes=0x0DE0FEFF;//FFFEE00D00000000
const uint32 ffffffff=0xFFFFFFFF;
const uint32 fffee000=0xFFFEE000;
const uint32 fffee00d=0xFFFEE00D;
const uint32 fffee0dd=0xFFFEE0DD;

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


//-------------------------
// subroutinas
//-------------------------

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

NSString *hexauint64(uint64 *eightBytes)
{
   struct uint64lh *lh=(struct uint64lh*) eightBytes;
   return [NSString stringWithFormat:@"%04x%04x",lh->h,lh->l];
}

//-------------------------
// blocking read actions
//-------------------------

//reescribir NSInputStream en base a ZeroMQ
//https://zeromq.org/download/

//returns true when 8 bytes were read
BOOL read8bytes(NSInputStream *stream, uint8_t *buffer, NSInteger *bytesReadRef)
{
   *bytesReadRef=[stream read:buffer maxLength:8];
   switch (*bytesReadRef) {
      case 8: return true;
      case -1://stream error
      {
         os_log_error(dicm2mdbxLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
         return false;
      }
   }
   os_log_error(dicm2mdbxLogger,"stream premature end (less than 8 bytes remaining)");
   return false;
}


//-------------------------
// funciones de parseo
//-------------------------


/*
 NON RECURSIVE
 read stream up to transfer syntax.
 finds sopclassidx
 finds soptsidx
 
 returns sopiuid:
 - nil -> not dicom
 - @"" -> not dicom part 10 (no prolog)
 - sopiuid (0002,0003)
 
 */
NSString *dicmuptosopts(
   uint8_t *keybytes,     // buffer matriz de creación de nuevos keys por diferencial
   uint8_t *valbytes,     // lectura del valor del atributo returns with sopiuid
   NSInputStream *stream, // input
   uint64 *inloc,         // current stream byte index
   uint64 *soloc,         // offset in valbyes for sop class
   uint16 *solen,         // length in valbyes for sop class
   uint16 *soidx,         // index in const char *scstr[]
   uint64 *siloc,         // offset in valbyes for sop instance uid
   uint16 *silen,         // length in valbyes for sop instance uid
   uint64 *stloc,         // offset in valbyes for transfer syntax
   uint16 *stlen,         // length in valbyes for transfer syntax
   uint8  *stidx          // index in const char *csstr[]
)
{
   //read first 8 bytes
   NSInteger bytescount;
   if (!read8bytes(stream, keybytes, &bytescount)) return nil;

   struct ele *attrstruct=(struct ele*) keybytes;//corresponding struct

#pragma mark dataset without preface ?
   uint64 *attruint64=(uint64*) keybytes;//first 8 bytes are not 0x00
   if (*attruint64 !=0)
   {
      swaptag(attrstruct);
      switch (attrstruct->r) {
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
         case SQ://sequence
         {
            os_log_debug(dicm2mdbxLogger,"no DICM prolog. First 8 bytes: %@",hexauint64(attruint64));
            //first attr is loaded
            return @"";
         }
         default:
         {
            os_log_error(dicm2mdbxLogger,"no DICM prolog. Not explicit little endian or derived. First 8 bytes: %@",hexauint64(attruint64));
            return nil;
         }
      }
   }

   
#pragma mark dataset with preface ?
   //check if 128-131 = DICM
   //read up to dicom version 0002001 (8+150 bytes)
   memcpy(valbytes, keybytes, 8);
   bytescount=[stream read:valbytes+8 maxLength:150];
   switch (bytescount) {
      case 150: break;
      case -1://stream error
      {
         os_log_error(dicm2mdbxLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
         return false;
      }
      default://not enough bytes
      {
         os_log_error(dicm2mdbxLogger,"stream premature end (less than 158 bytes)");
         return nil;
      }
   }

   if ( (*(valbytes+128)!='D') || (*(valbytes+129)!='I') || (*(valbytes+130)!='C') || (*(valbytes+131)!='M') )
   {
      os_log_error(dicm2mdbxLogger,"no DICM signature");
      return nil;
   }
   *inloc = 158;

   
   
#pragma mark read sop object [so] (=sop class)
   if (!read8bytes(stream, keybytes, &bytescount)) return nil;
   if ((*attruint64 % 0x100000000)!=0x00020002)
   {
      os_log_error(dicm2mdbxLogger,"no 00020002");
      return nil;
   }
   *inloc += 8;
   *soloc=*inloc;
   *solen=attrstruct->l;
   bytescount=[stream read:valbytes+*soloc maxLength:*solen];
   if (bytescount==-1)//stream error
   {
      os_log_error(dicm2mdbxLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
      return false;
   }
   if  (bytescount!=*solen)//not enough bytes
   {
      os_log_error(dicm2mdbxLogger,"stream premature end (sop class)");
      return nil;
   }
   *soidx=scidx( valbytes+*soloc, *solen - (valbytes[*soloc + *solen - 1]==0x0) );
   *inloc += *solen;

   
   
#pragma mark read sop instance uid [si]
   if (!read8bytes(stream, keybytes, &bytescount)) return nil;
   if ((*attruint64 % 0x100000000)!=0x00030002)
   {
      os_log_error(dicm2mdbxLogger,"no 00020003");
      return nil;
   }
   *inloc += 8;
   *siloc=*inloc;
   *silen=attrstruct->l;
   bytescount=[stream read:valbytes+*siloc maxLength:*silen];
   if (bytescount==-1)//stream error
   {
      os_log_error(dicm2mdbxLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
      return false;
   }
   if  (bytescount!=*silen)//not enough bytes
   {
      os_log_error(dicm2mdbxLogger,"stream premature end (sop instance)");
      return nil;
   }
   NSString *sopiuid=[[NSString alloc]initWithData:[NSData dataWithBytes:valbytes+*siloc length:*silen - (valbytes[*siloc + *silen - 1]==0x0)] encoding:NSASCIIStringEncoding];
   *inloc += *silen;


   
#pragma mark read transfer syntax
   if (!read8bytes(stream, keybytes, &bytescount)) return nil;
   if ((*attruint64 % 0x100000000)!=0x00100002)
   {
      os_log_error(dicm2mdbxLogger,"no 00020010");
      return nil;
   }
   *inloc += 8;
   *stloc=*inloc;
   *stlen=attrstruct->l;
   bytescount=[stream read:valbytes+*stloc maxLength:*stlen];
   if (bytescount==-1)//stream error
   {
      os_log_error(dicm2mdbxLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
      return false;
   }
   if  (bytescount!=*stlen)//not enough bytes
   {
      os_log_error(dicm2mdbxLogger,"stream premature end (transfert syntax)");
      return nil;
   }
   *stidx=tsidx( valbytes+*stloc, *stlen - (valbytes[*stloc + *stlen - 1]==0x0) );
   *inloc += *stlen;


   return sopiuid;
}


/*log attributes tags to stdout
BOOL dicm2null(
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

                  dicm2null(
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
*/

/*
BOOL dicm2kvdb(
   uint8_t *keybytes,
   uint8 keydepth,
   BOOL readfirstattr,
   uint16 keycs,
   uint8_t *valbytes,
   NSInputStream *stream,
   uint64 *idx,
   uint32 beforebyte,
   uint32 beforetag,           
   id *db,
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
         case 0: return true ;//no more attrs
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
                  return false;
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
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return true;//no more attrs
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
#pragma mark vl tag code
         case AT://attribute tag
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return false;
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
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return true;//no more attrs
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
#pragma mark vl ascii code
         case CS://coded string
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return false;
               } else {
                  attrstruct->l=REPERTOIRE_GL;

                  if ( (attrstruct->U ==0x05) && (attrstruct->g ==0x00) && (attrstruct->G ==0x08) && (attrstruct->u ==0x00) ){
                     uint16 repidxs=repertoireidx(valbytes,vl);
                     if (repidxs==0x09) NSLog(@"BAD REPERTOIRE");
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
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return true;//no more attrs
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
#pragma mark vl oid code
         case UI://unique ID
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return false;
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
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return true;//no more attrs
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
                  return false;
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
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return true;//no more attrs
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
                  return false;
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
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return true;//no more attrs
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
#pragma mark vl person
         case PN://person name
         {
            vl=attrstruct->l;
            if (vl > 0){
               if ([stream read:valbytes maxLength:vl]!=vl) {
                  //NSLog(@"error");
                  return false;
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
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + vl;
            attrbytesread=[stream read:attrbytes maxLength:8];
            switch (attrbytesread) {
               case 8: swaptag(attrstruct); break;
               case 0: return true;//no more attrs
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
               return false;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
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
                     return false;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
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
               case 0: return true;//no more attrs
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
#pragma mark vll charset
         case UC://unlimited characters
         case UT://unlimited text
         {
            attrstruct->l=keycs;

            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return false;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
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
                     return false;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
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
               case 0: return true;//no more attrs
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
#pragma mark vll RFC3986
         case UR://universal resource identifier/locator
         {
            attrstruct->l=RFC3986;

            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return false;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
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
                     return false;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
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
               case 0: return true;//no more attrs
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

      //---------
#pragma mark UN
         case UN://unknown
            // https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_6.2.2
             
             //5. The Value Length Field of VR UN may contain Undefined Length (FFFFFFFFH), in which case the contents can be assumed to be encoded with Implicit VR. See Section 7.5.1 to determine how to parse Data Elements with an Undefined Length.
             

         {
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return false;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:[NSData data]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
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
                     return false;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
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
               case 0: return true;//no more attrs
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
               return false;
            }
            if (*ll==0)
            {
               attrstruct->r=SZ;
               attrstruct->l=0x0;
               [db
                setData:[NSData dataWithBytesNoCopy:(void*)&SZbytes length:8]
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];

               //read nextattr
               *idx += 12;//do not add *ll !
               attrbytesread=[stream read:attrbytes maxLength:8];
               switch (attrbytesread) {
                  case 8: swaptag(attrstruct); break;
                  case 0: return true;//no more attrs
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
                  case 0: return true;//no more attrs
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
                   setData:[NSData dataWithBytesNoCopy:(void*)&IAbytes length:8]
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
                     return false;
                  }
                  else beforebyteIT=*idx + *ll;

                  dicm2kvdb(
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
                      setData:[NSData dataWithBytesNoCopy:(void*)&IAbytes length:8]
                      forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                      error:&error
                     ];
                     *idx+=8;
                     attrbytesread=[stream read:itembytes maxLength:8];
                     switch (attrbytesread) {
                        case 8: swaptag(itemstruct); break;
                        case 0: return true;//no more attrs
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
                      setData:[NSData dataWithBytesNoCopy:(void*)&IAbytes length:8]
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
                setData:[NSData dataWithBytesNoCopy:(void*)&SZbytes length:8]
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
                     case 0: return true;//no more attrs
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
            return false;
         }
            
      //---------
      }//end switch
   }//end while (*index < beforebyte)
   return false;
}
*/
