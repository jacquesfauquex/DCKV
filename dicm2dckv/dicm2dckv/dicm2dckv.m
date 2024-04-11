//
//  dicm2mdbx.m
//  dicm2mdbx
//
//  Created by jacquesfauquex on 2024-02-28.
//

#include "dicm2dckv.h"
#include "os_log.h"

#include "dckvapi.h"

const uint32 tag00020002=0x02000200;
const uint32 tag00020003=0x03000200;
const uint32 tag00020010=0x10000200;

const uint32 tag00081150=0x50110800;

const uint64 SZbytes=0xDDE0FEFF;//FFFEE0DD00000000
const uint64 IAbytes=0xFFFFFFFF00E0FEFF;//FFFEE000FFFFFFFF
const uint64 IZbytes=0x0DE0FEFF;//FFFEE00D00000000
const uint32 ffffffff=0xFFFFFFFF;
const uint32 fffee000=0xfffee000;
const uint32 fffee00d=0xfffee00d;
const uint32 fffee0dd=0xfffee0dd;

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

uint8 swapchar;

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
      case 8:
      {
         swapchar=*buffer;
         *buffer=*(buffer+1);
         *(buffer+1)=swapchar;
         swapchar=*(buffer+2);
         *(buffer+2)=*(buffer+3);
         *(buffer+3)=swapchar;
         return true;
      }
      case -1://stream error
      {
         os_log_error(dicm2dckvLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
         return false;
      }
   }
   os_log_error(dicm2dckvLogger,"stream premature end (less than 8 bytes remaining)");
   return false;
}

//hexa representation of read8bytes
NSString *hexa8bytes(uint64 *eightBytes)
{
   struct l4h4 *lh=(struct l4h4*) eightBytes;
   return [NSString stringWithFormat:@"%04x%04x",lh->h,lh->l];
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
   NSString *source,
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

   struct t4r2l2 *attrstruct=(struct t4r2l2*) keybytes;//corresponding struct

#pragma mark dataset without preface ?
   uint64 *attruint64=(uint64*) keybytes;//first 8 bytes are not 0x00
   if (*attruint64 !=0)
   {
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
            os_log_debug(dicm2dckvLogger,"no DICM prolog. First 8 bytes: %@",hexa8bytes(attruint64));
            //first attr is loaded
            return @"";
         }
         default:
         {
            os_log_error(dicm2dckvLogger,"no DICM prolog. Not explicit little endian or derived. First 8 bytes: %@",hexa8bytes(attruint64));
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
         os_log_error(dicm2dckvLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
         return false;
      }
      default://not enough bytes
      {
         os_log_error(dicm2dckvLogger,"stream premature end (less than 158 bytes)");
         return nil;
      }
   }

   if ( (*(valbytes+128)!='D') || (*(valbytes+129)!='I') || (*(valbytes+130)!='C') || (*(valbytes+131)!='M') )
   {
      os_log_error(dicm2dckvLogger,"no DICM signature");
      return nil;
   }
   *inloc = 158;

   
   
#pragma mark read sop object [so] (=sop class)
   if (!read8bytes(stream, keybytes, &bytescount)) return nil;
   if ((*attruint64 % 0x100000000)!=tag00020002)
   {
      os_log_error(dicm2dckvLogger,"no 00020002");
      return nil;
   }
   *inloc += 8;
   *soloc=*inloc;
   *solen=attrstruct->l;
   bytescount=[stream read:valbytes+*soloc maxLength:*solen];
   if (bytescount==-1)//stream error
   {
      os_log_error(dicm2dckvLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
      return false;
   }
   if  (bytescount!=*solen)//not enough bytes
   {
      os_log_error(dicm2dckvLogger,"stream premature end (sop class)");
      return nil;
   }
   *soidx=scidx( valbytes+*soloc, *solen - (valbytes[*soloc + *solen - 1]==0x0) );
   *inloc += *solen;

   
   
#pragma mark read sop instance uid [si]
   if (!read8bytes(stream, keybytes, &bytescount)) return nil;
   if ((*attruint64 % 0x100000000)!=tag00020003)
   {
      os_log_error(dicm2dckvLogger,"no 00020003");
      return nil;
   }
   *inloc += 8;
   *siloc=*inloc;
   *silen=attrstruct->l;
   bytescount=[stream read:valbytes+*siloc maxLength:*silen];
   if (bytescount==-1)//stream error
   {
      os_log_error(dicm2dckvLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
      return false;
   }
   if  (bytescount!=*silen)//not enough bytes
   {
      os_log_error(dicm2dckvLogger,"stream premature end (sop instance)");
      return nil;
   }
   NSString *sopiuid=[[NSString alloc]initWithData:[NSData dataWithBytes:valbytes+*siloc length:*silen - (valbytes[*siloc + *silen - 1]==0x0)] encoding:NSASCIIStringEncoding];
   *inloc += *silen;


   
#pragma mark read transfer syntax
   if (!read8bytes(stream, keybytes, &bytescount)) return nil;
   if ((*attruint64 % 0x100000000)!=tag00020010)
   {
      os_log_error(dicm2dckvLogger,"no 00020010");
      return nil;
   }
   *inloc += 8;
   *stloc=*inloc;
   *stlen=attrstruct->l;
   bytescount=[stream read:valbytes+*stloc maxLength:*stlen];
   if (bytescount==-1)//stream error
   {
      os_log_error(dicm2dckvLogger,"error code %ld: %@",(long)[[stream streamError]code],[[[stream streamError]userInfo]debugDescription]);
      return false;
   }
   if  (bytescount!=*stlen)//not enough bytes
   {
      os_log_error(dicm2dckvLogger,"stream premature end (transfert syntax)");
      return nil;
   }
   *stidx=tsidx( valbytes+*stloc, *stlen - (valbytes[*stloc + *stlen - 1]==0x0) );
   *inloc += *stlen;


   return sopiuid;
}


BOOL dicm2kvdb(
   NSString *source,
   uint8_t *keybytes,
   uint8 keydepth,
   BOOL readfirstattr,
   uint16 keycs,
   uint8_t *valbytes,
   NSInputStream *stream,
   uint64 *loc,
   uint32 beforebyte,
   uint32 beforetag
)
{
   //inits
   NSInteger bytescount=0;
   uint16 vl=0;//keeps vl while overwritting it in keybytes
   uint8 *attrbytes=keybytes+keydepth;//subbuffer for attr reading
   struct t4r2l2 *attrstruct=(struct t4r2l2*) attrbytes;//corresponding struct
   uint8 *llbytes=keybytes+keydepth+8;//subbuffer for ll reading
   uint32 *ll=(uint32*)llbytes;//corresponding uin32

   if (readfirstattr && (! read8bytes(stream, attrbytes, &bytescount))) return false;
   while ((*loc < beforebyte) &&  ( CFSwapInt32(attrstruct->t) < beforetag))
   {
      
      switch (attrstruct->r) {
#pragma mark vl num
         case FD://floating point double
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(keybytes,keydepth,false,kvFD,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;
         case FL://floating point single
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(keybytes,keydepth,false,kvFL,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;
         case SL://signed long
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(keybytes,keydepth,false,kvSL,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;
         case SS://signed short
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(keybytes,keydepth,false,kvSS,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;
         case UL://unsigned long
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(keybytes,keydepth,false,kvUL,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;
        case US://unsigned short
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(keybytes,keydepth,false,kvUS,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl tag code
         case AT://attribute tag
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(keybytes,keydepth,false,kvAT,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl ascii code
         case CS://coded string
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            //charset
            
            if ( attrstruct->t ==0x05000800 ){
               if (vl && [stream read:valbytes maxLength:vl]!=vl) return false;
               uint16 repidxs=repertoireidx(valbytes,vl);
               if (repidxs==0x09)
               {
                  os_log_error(dicm2dckvLogger,"bad repertoire %@",[[NSString alloc]initWithData:[NSData dataWithBytes:valbytes length:vl] encoding:NSASCIIStringEncoding]);
                  return false;
               }
               else
               {
                  keycs=(keycs & 0x8000) | repidxs;
                  attrstruct->l=repidxs;
               }
               if (!appendkv(keybytes,keydepth,false,kvTXT,source,*loc,vl,nil,valbytes)) return false;
            }
            else
            {
              if (!appendkv(keybytes,keydepth,false,kvTXT,source,*loc,vl,stream,valbytes)) return false;
            }
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;
            
#pragma mark vl oid code
         case UI://unique ID
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            
            if ( attrstruct->t==tag00081150 ){//0x50110800
               if (vl && [stream read:valbytes maxLength:vl]!=vl) return false;
               uint16 sopclassidx=scidx( valbytes, vl );
               if (sopclassidx==0x00)
               {
                  os_log_error(dicm2dckvLogger,"bad sop class %@",[[NSString alloc]initWithData:[NSData dataWithBytes:valbytes length:vl] encoding:NSASCIIStringEncoding]);
                  return false;
               }
               else attrstruct->l=sopclassidx;
               if (!appendkv(keybytes,keydepth,false,kvUI,source,*loc,vl,nil,valbytes)) return false;
            }
            else
            {
               if (!appendkv(keybytes,keydepth,false,kvUI,source,*loc,vl,stream,valbytes)) return false;
            }
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
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
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(keybytes,keydepth,false,kvTXT,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl charset
         case LO://long string
         case LT://long text
         case SH://short string
         case ST://short text
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            if (!appendkv(keybytes,keydepth,false,kvTXT,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;
            
#pragma mark vl person
         case PN://person name
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(keybytes,keydepth,false,kvTXT,source,*loc,vl,stream,valbytes)) return false;
            *loc += 8 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
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
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if ([stream read:llbytes maxLength:4]!=4) {
               os_log_error(dicm2dckvLogger,"stream end instead of vll");
               return false;
            }
            if (!appendkv(keybytes,keydepth,true,kvBIN,source,*loc,*ll,stream,valbytes)) return false;
            *loc += 12 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;

#pragma mark vll charset
         case UC://unlimited characters
         case UT://unlimited text
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if ([stream read:llbytes maxLength:4]!=4) {
               os_log_error(dicm2dckvLogger,"stream end instead of vll");
               return false;
            }
            if (!appendkv(keybytes,keydepth,true,kvTXT,source,*loc,*ll,stream,valbytes)) return false;
            *loc += 12 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;

#pragma mark vll RFC3986
         case UR://universal resource identifier/locator
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if ([stream read:llbytes maxLength:4]!=4) {
               os_log_error(dicm2dckvLogger,"stream end instead of vll");
               return false;
            }
            if (!appendkv(keybytes,keydepth,true,kvTXT,source,*loc,*ll,stream,valbytes)) return false;
            *loc += 12 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;

      //---------
#pragma mark UN
         case UN://unknown
            // https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_6.2.2
             //5. The Value Length Field of VR UN may contain Undefined Length (FFFFFFFFH), in which case the contents can be assumed to be encoded with Implicit VR. See Section 7.5.1 to determine how to parse Data Elements with an Undefined Length.
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if ([stream read:llbytes maxLength:4]!=4) {
               os_log_error(dicm2dckvLogger,"stream end instead of vll");
               return false;
            }
            if (!appendkv(keybytes,keydepth,true,kvBIN,source,*loc,*ll,stream,valbytes)) return false;
            *loc += 12 + vl;
            if (! read8bytes(stream, attrbytes, &bytescount)) return false;
         } break;

      //---------
#pragma mark SQ
         case SQ://sequence
         {
            //register SQ con vr 0000 y length undefined
            keybytes[keydepth+0x8]=0xff;
            keybytes[keydepth+0x9]=0xff;
            keybytes[keydepth+0xA]=0xff;
            keybytes[keydepth+0xB]=0xff;
            attrstruct->r=SA;
            attrstruct->l=REPERTOIRE_GL;


            if (!appendkv(keybytes,keydepth,true,kvSA,source,*loc,12,nil,valbytes)) return false;

            //read length
            if ([stream read:llbytes maxLength:4]!=4) {
               os_log_error(dicm2dckvLogger,"stream end instead of SQ vll");
               return false;
            }
            if (*ll==0)
            {
               attrstruct->r=SZ;
               attrstruct->l=0xFFFF;
               if (!appendkv(keybytes,keydepth,true,kvSZ,source,*loc,8,stream,(void*)&SZbytes)) return false;

               //read nextattr
               *loc += 12;//do not add *ll !
               if (! read8bytes(stream, attrbytes, &bytescount)) return false;
            }
            else //SQ *ll!=0
            {
               //SQ length
               uint64 beforebyteSQ;
               if (*ll==ffffffff) beforebyteSQ=beforebyte;
               else if (beforebyte==ffffffff) beforebyteSQ= *loc + *ll;
               else if (*loc + *ll > beforebyte) {
                  os_log_error(dicm2dckvLogger,"SQ incomplete input");
                  return false;
               }
               else beforebyteSQ=*loc + *ll;

               
               //replace vr and vl of SQ by itemnumber
               uint32 itemnumber=1;
               attrstruct->r=CFSwapInt16(itemnumber/0x10000);
               attrstruct->l=CFSwapInt16(itemnumber%0x10000);

               *loc += 12;//do not add *ll !

               
#pragma mark item level
               keydepth+=8;
               uint8 *itembytes=keybytes+keydepth;
               struct t4r2l2 *itemstruct=(struct t4r2l2*) itembytes;
               if (! read8bytes(stream, itembytes, &bytescount)) return false;

               
               //for each first attr fffee000 of any new item
              while ((*loc < beforebyteSQ) && (CFSwapInt32(itemstruct->t)==fffee000)) //itemstart compulsory
               {
                  uint32 IQll = (itemstruct->l << 16) | itemstruct->r;
                  itemstruct->t=0x00000000;
                  itemstruct->r=IA;
                  itemstruct->l=REPERTOIRE_GL;
                  if (!appendkv(keybytes,keydepth,false,kvIA,source,*loc,8,nil,(void*)&IAbytes)) return false;

                  uint64 beforebyteIT;//to be computed from after item start
                  *loc+=8;
                  if (IQll==ffffffff) beforebyteIT=beforebyteSQ;
                  else if (beforebyteSQ==ffffffff) beforebyteIT=*loc + *ll;
                  else if (*loc + *ll > beforebyteSQ) {
                     os_log_error(dicm2dckvLogger,"IT incomplete input");
                     return false;
                  }
                  else beforebyteIT=*loc + *ll;

                  dicm2kvdb(
                        source,
                        keybytes,
                        keydepth,
                        true,
                        keycs,
                        valbytes,
                        stream,
                        loc,
                        (uint32)beforebyteIT,
                        fffee00d
                        );
                  //Atención!!! attr still is the first attr of the first item
                  //the attr of the recursion is not available
                  //keybytes kept the last attr read one level deeper than current attr

                  
                  //write IZ to db
                  if (CFSwapInt32(itemstruct->t)==fffee00d)
                  {
                     itemstruct->t=ffffffff;
                     itemstruct->r=IZ;
                     itemstruct->l=0x00;
                     if (!appendkv(keybytes,keydepth,false,kvIZ,source,*loc,8,nil,(void*)&IZbytes)) return false;
                     *loc+=8;
                  }
                  else
                  {
                     struct t4r2l2 copyattr;
                     copyattr.t=itemstruct->t;
                     copyattr.r=itemstruct->r;
                     copyattr.l=itemstruct->l;
                     itemstruct->t=ffffffff;
                     itemstruct->r=IZ;
                     itemstruct->l=0x0;
                     if (!appendkv(keybytes,keydepth,false,kvIZ,source,*loc,8,nil,(void*)&IZbytes)) return false;
                     itemstruct->t=copyattr.t;
                     itemstruct->r=copyattr.r;
                     itemstruct->l=copyattr.l;
                  }
                  
                  //new item number
                  itemnumber+=1;
                  attrstruct->r=CFSwapInt16(itemnumber/0x10000);
                  attrstruct->l=CFSwapInt16(itemnumber%0x10000);

               }//end while item
               keydepth-=8;
#pragma mark item level end

               attrstruct->r=SZ;
               attrstruct->l=0xFFFF;
               if (!read8bytes(stream, itembytes, &bytescount)) return false;
               if (!appendkv(keybytes,keydepth,false,kvSZ,source,*loc,8,nil,(void*)&SZbytes)) return false;

               
               //itemstruct may be SZ or post SQ
               if (CFSwapInt32(itemstruct->t)==fffee0dd)
               {
                  *loc+=8;
                  if (! read8bytes(stream, attrbytes, &bytescount)) return false;
               }
               else
               {
                  attrstruct->t=itemstruct->t;
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

