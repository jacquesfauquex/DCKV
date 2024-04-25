//
//  dicm2dckv.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-02-28.
//

#include "dicm2dckv.h"
#include "log.h"

#include "dckvapi.h"


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


//returns true when 8 bytes were read
BOOL read8(uint8_t *buffer, unsigned long *bytesReadRef)
{
   *bytesReadRef=fread(buffer, 1, 8, stdin);
   if (ferror(stdin)){
      E("%s","stdin error");
      return false;
   }
   
   if (*bytesReadRef==8){
      swapchar=*buffer;
      *buffer=*(buffer+1);
      *(buffer+1)=swapchar;
      swapchar=*(buffer+2);
      *(buffer+2)=*(buffer+3);
      *(buffer+3)=swapchar;
   }
   else
   {
      *buffer=0xFF;
      *(buffer+1)=0xFF;
      *(buffer+2)=0xFF;
      *(buffer+3)=0xFF;
   }
   return true;
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
 - "" -> no DICM
 - sopiuid (0002,0003)
 */
char *dicmuptosopts(
   uint8_t *kbuf, // buffer matriz de creación de nuevos keys por diferencial
   uint8_t *vbuf, // lectura del valor del atributo returns with sopiuid
   uint64 *inloc, // current stdin byte index
   uint64 *soloc, // offset in valbyes for sop class
   uint16 *solen, // length in valbyes for sop class
   uint16 *soidx, // index in const char *scstr[]
   uint64 *siloc, // offset in valbyes for sop instance uid
   uint16 *silen, // length in valbyes for sop instance uid
   uint64 *stloc, // offset in valbyes for transfer syntax
   uint16 *stlen, // length in valbyes for transfer syntax
   uint16 *stidx  // index in const char *csstr[]
)
{
   NSLog(@"waiting");
   *inloc=fread(kbuf, 1,158, stdin);
   if (*inloc!=158) return "";
   //check if 128-131 = DICM
   if (kbuf[128]!=0x44 || kbuf[129]!=0x49 || kbuf[130]!=0x43 || kbuf[131]!=0x4D) return "";
   //read up to dicom version 0002001 (8+150 bytes)
   struct t4r2l2 *attrstruct=(struct t4r2l2*) kbuf;//corresponding struct
   uint64 *attruint64=(uint64*) kbuf;
   uint16 bytescount;
#pragma mark read sop object [so] (=sop class)
   bytescount=fread(kbuf, 1, 8, stdin);
   *inloc+=bytescount;
   if (bytescount!=8) return "";
   if ((*attruint64 % 0x100000000)!=0x00020002)
   {
      E("%08llx [no 00020002]",*attruint64);
      return "";
   }
   *soloc=*inloc;
   *solen=attrstruct->l;
   bytescount=fread(vbuf+*soloc, 1, *solen, stdin);
   *inloc += *solen;
   if  (bytescount!=*solen)//not enough bytes
   {
      E("%s","bad 00020002");
      return "";
   }
   *soidx=scidx( vbuf+*soloc, *solen - (vbuf[*soloc + *solen - 1]==0x0) );
   if (*soidx==0) return "";//no valid sopclass
   
#pragma mark read sop instance uid [si]
   bytescount=fread(vbuf+*inloc, 1, 8, stdin);
   memcpy(kbuf, vbuf+*inloc, bytescount);
   *inloc+=bytescount;
   if (bytescount!=8) return "";
   if ((*attruint64 % 0x100000000)!=0x00030002)
   {
      E("%s","no 00020003");
      return "";
   }
   *siloc=*inloc;
   *silen=attrstruct->l;
   bytescount=fread(vbuf+*siloc, 1, *silen, stdin);
   *inloc += *silen;
   if  (bytescount!=*silen)//not enough bytes
   {
      E("%s","bad 00020003");
      return "";
   }
   
#pragma mark read transfer syntax
   bytescount=fread(vbuf+*inloc, 1, 8, stdin);
   memcpy(kbuf, vbuf+*inloc, bytescount);
   *inloc+=bytescount;
   if (bytescount!=8) return "";
   if ((*attruint64 % 0x100000000)!=0x00100002)
   {
      E("%s","no 00020010");
      return "";
   }
   *stloc=*inloc;
   *stlen=attrstruct->l;
   bytescount=fread(vbuf+*stloc, 1, *stlen, stdin);
   *inloc += *stlen;
   if  (bytescount!=*stlen)//not enough bytes
   {
      E("%s","no 00020010");
      return "";
   }
   *stidx=tsidx( vbuf+*stloc, *stlen - (vbuf[*stloc + *stlen - 1]==0x0) );
   if (*stidx==0) return "";

   return (char*)vbuf+*siloc;
}

BOOL dicm2dckvInstance(
   const char * dstdir,
   uint8_t *kbuf,     // buffer matriz de creación de nuevos keys por diferencial
   uint8_t *vbuf,     // lectura del valor del atributo
   uint8_t *lbuf,
   uint32 *llul,      // buffer lectura 4-bytes ll de atributos largos
   uint64 *inloc,           // offstet en stream
   uint32 beforebyte,     // limite superior de lectura
   uint32 beforetag,       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
   uint64 *soloc,         // offset in valbyes for sop class
   uint16 *solen,         // length in valbyes for sop class
   uint16 *soidx,         // index in const char *scstr[]
   uint64 *siloc,         // offset in valbyes for sop instance uid
   uint16 *silen,         // length in valbyes for sop instance uid
   uint64 *stloc,         // offset in valbyes for transfer syntax
   uint16 *stlen,         // length in valbyes for transfer syntax
   uint16 *stidx,         // index in const char *csstr[]
   uint16 *siidx          // SOPinstance index
)
{
   if (!createtx(
                 dstdir,
                 vbuf,
                 soloc,         // offset in valbyes for sop class
                 solen,         // length in valbyes for sop class
                 soidx,         // index in const char *scstr[]
                 siloc,         // offset in valbyes for sop instance uid
                 silen,         // length in valbyes for sop instance uid
                 stloc,         // offset in valbyes for transfer syntax
                 stlen,         // length in valbyes for transfer syntax
                 stidx,         // index in const char *csstr[]
                 siidx
                 )) return false;
   if (!createdb(kvDEFAULT))
   {
      if (!canceltx(siidx)) E("%s","cannot cancel tx");
      return false;
   }
   
   if (*soidx>0) //part 10
   {
      const uint64 key00020002=0x0000554902000200;
      if(!appendkv((uint8_t*)&key00020002,0,false,kvUI, *soloc, *solen,false,vbuf+*soloc)) return false;
      const uint64 key00020003=0x0000554903000200;
      if(!appendkv((uint8_t*)&key00020003,0,false,kvUI, *siloc, *silen,false,vbuf+*siloc)) return false;
      const uint64 key00020010=0x0000554910000200;
      if(!appendkv((uint8_t*)&key00020010,0,false,kvUI, *stloc, *stlen,false,vbuf+*stloc)) return false;

      if (dicm2dckvDataset(
                  kbuf,
                  0,          //kloc
                  true,       //readfirstattr
                  0,          //keycs
                  lbuf,
                  llul,
                  vbuf,
                  true,       //fromStdin
                  inloc,
                  beforebyte, //beforebyte
                  beforetag  //beforetag
                 ) && committx(siidx)) return true;
   }
   else //pure dataset
   {
      if (dicm2dckvDataset(
                     kbuf,
                     0,          //kloc
                     false,      //readfirstattr
                     0,          //keycs
                     lbuf,
                     llul,
                     vbuf,
                     true,       //fromStdin
                     inloc,
                     beforebyte, //beforebyte
                     beforetag  //beforetag
                    ) && committx(siidx)) return true;
   }
   canceltx(siidx);
   return false;
}



BOOL dicm2dckvDataset(
   uint8_t *kbuf,
   unsigned long kloc,
   BOOL readfirstattr,
   uint16 keycs,
   uint8_t *lbuf,
   uint32 *llul,
   uint8_t *vbuf,
   BOOL fromStdin,
   uint64 *inloc,
   uint32 beforebyte,
   uint32 beforetag
)
{
   //inits
   unsigned long bytescount=0;
   uint16 vl=0;//keeps vl while overwritting it in kbuf
   uint8 *attrbytes=kbuf+kloc;//subbuffer for attr reading
   struct t4r2l2 *attrstruct=(struct t4r2l2*) attrbytes;//corresponding struct
   lbuf=kbuf+kloc+8;//subbuffer for ll reading
   llul=(uint32*)lbuf;
   //uint32 *ll=(uint32*)llbytes;//corresponding uin32

   if (readfirstattr && (! read8(attrbytes, &bytescount))) return false;
   while (
      (*inloc < beforebyte)
   && ( CFSwapInt32(attrstruct->t) < beforetag)
   )
   {
      //((attrstruct->t >> 16)==0xFFFE ||
      switch (attrstruct->r) {
#pragma mark vl num
         case FD://floating point double
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,false,kvFD,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;
         case FL://floating point single
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,false,kvFL,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;
         case SL://signed long
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,false,kvSL,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;
         case SS://signed short
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,false,kvSS,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;
         case UL://unsigned long
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,false,kvUL,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;
        case US://unsigned short
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,false,kvUS,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl tag code
         case AT://attribute tag
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,false,kvAT,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl ascii code
         case CS://coded string
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            //charset
            
            if ( attrstruct->t ==0x05000800 ){
               if (vl && (fread(vbuf, 1,vl,stdin)!=vl)) return false;
               uint16 repidxs=repertoireidx(vbuf,vl);
               if (repidxs==0x09)
               {
                  E("bad repertoire %s",[[[NSString alloc]initWithData:[NSData dataWithBytes:vbuf length:vl] encoding:NSASCIIStringEncoding]  cStringUsingEncoding:NSASCIIStringEncoding]);
                  return false;
               }
               else
               {
                  keycs=(keycs & 0x8000) | repidxs;
                  attrstruct->l=repidxs;
               }
               if (!appendkv(kbuf,kloc,false,kvTXT,*inloc,vl,false,vbuf)) return false;
            }
            else
            {
              if (!appendkv(kbuf,kloc,false,kvTXT,*inloc,vl,true,vbuf)) return false;
            }
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;
            
#pragma mark vl oid code
         case UI://unique ID
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            
            if ( attrstruct->t==tag00081150 ){//0x50110800
               if (vl && (fread(vbuf, 1,vl,stdin)!=vl)) return false;
               uint16 sopclassidx=scidx( vbuf, vl );
               if (sopclassidx==0x00)
               {
                  E("bad sop class %s",[[[NSString alloc]initWithData:[NSData dataWithBytes:vbuf length:vl] encoding:NSASCIIStringEncoding]  cStringUsingEncoding:NSASCIIStringEncoding]);
                  return false;
               }
               else attrstruct->l=sopclassidx;
               if (!appendkv(kbuf,kloc,false,kvUI,*inloc,(unsigned long)vl,false,vbuf)) return false;
            }
            else
            {
               if (!appendkv(kbuf,kloc,false,kvUI,*inloc,vl,true,vbuf)) return false;
            }
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
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
            if (!appendkv(kbuf,kloc,false,kvTXT,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl charset
         case LO://long string
         case LT://long text
         case SH://short string
         case ST://short text
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            if (!appendkv(kbuf,kloc,false,kvTXT,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;
            
#pragma mark vl person
         case PN://person name
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,false,kvPN,*inloc,vl,true,vbuf)) return false;
            *inloc += 8 + vl;
            if (! read8(attrbytes, &bytescount)) return false;
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
            if (fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            if (!appendkv(kbuf,kloc,true,kvBIN,*inloc,*llul,true,vbuf)) return false;
            *inloc += 12 + *llul;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;


#pragma mark vll charset
         case UC://unlimited characters
         case UT://unlimited text
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            if (!appendkv(kbuf,kloc,true,kvTXT,*inloc,*llul,true,vbuf)) return false;
            *inloc += 12 + *llul;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;

#pragma mark vll RFC3986
         case UR://universal resrcurl identifier/locator
         {
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            if (!appendkv(kbuf,kloc,true,kvTXT,*inloc,*llul,true,vbuf)) return false;
            *inloc += 12 + *llul;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;

      //---------
#pragma mark UN
         case UN://unknown
         {
            // https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_6.2.2
             //5. The Value Length Field of VR UN may contain Undefined Length (FFFFFFFFH), in which case the contents can be assumed to be encoded with Implicit VR. See Section 7.5.1 to determine how to parse Data Elements with an Undefined Length.
            vl=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            if (!appendkv(kbuf,kloc,true,kvBIN,*inloc,*llul,true,vbuf)) return false;
            *inloc += 12 + *llul;
            if (! read8(attrbytes, &bytescount)) return false;
         } break;

      //---------
#pragma mark SQ
         case SQ://sequence
         {
            //register SQ con vr 0000 y length undefined
            kbuf[kloc+0x8]=0xff;
            kbuf[kloc+0x9]=0xff;
            kbuf[kloc+0xA]=0xff;
            kbuf[kloc+0xB]=0xff;
            attrstruct->r=SA;
            attrstruct->l=REPERTOIRE_GL;


            if (!appendkv(kbuf,kloc,true,kvSA,*inloc,12,false,vbuf)) return false;

            //read length
            if (fread(lbuf, 1,4,stdin)!=4) {
               E("%s","eof instead of SQ vll");
               return false;
            }
            if (*llul==0)
            {
               attrstruct->r=SZ;
               attrstruct->l=0xFFFF;
               if (!appendkv(kbuf,kloc,false,kvSZ,*inloc,8,false,(void*)&SZbytes)) return false;

               //read nextattr
               *inloc += 12;//do not add *llul !
               if (! read8(attrbytes, &bytescount)) return false;
            }
            else //SQ *llul!=0
            {
               //SQ length
               uint64 beforebyteSQ;
               if (*llul==ffffffff) beforebyteSQ=beforebyte;
               else if (beforebyte==ffffffff) beforebyteSQ= *inloc + *llul;
               else if (*inloc + *llul > beforebyte) {
                  E("%s","SQ incomplete input");
                  return false;
               }
               else beforebyteSQ=*inloc + *llul;

               
               //replace vr and vl of SQ by itemnumber
               uint32 itemnumber=1;
               attrstruct->r=CFSwapInt16(itemnumber/0x10000);
               attrstruct->l=CFSwapInt16(itemnumber%0x10000);

               *inloc += 12;//do not add *llul !

               
#pragma mark item level
               kloc+=8;
               uint8 *itembytes=kbuf+kloc;
               struct t4r2l2 *itemstruct=(struct t4r2l2*) itembytes;
               if (! read8(itembytes, &bytescount)) return false;

               
               //for each first attr fffee000 of any new item
              while ((*inloc < beforebyteSQ) && (CFSwapInt32(itemstruct->t)==fffee000)) //itemstart compulsory
               {
                  uint32 IQll = (itemstruct->l << 16) | itemstruct->r;
                  itemstruct->t=0x00000000;
                  itemstruct->r=IA;
                  itemstruct->l=REPERTOIRE_GL;
                  if (!appendkv(kbuf,kloc,false,kvIA,*inloc,8,false,(void*)&IAbytes)) return false;

                  uint64 beforebyteIT;//to be computed from after item start
                  *inloc+=8;
                  if (IQll==ffffffff) beforebyteIT=beforebyteSQ;
                  else if (beforebyteSQ==ffffffff) beforebyteIT=*inloc + *llul;
                  else if (*inloc + *llul > beforebyteSQ) {
                     E("%s","IT incomplete input");
                     return false;
                  }
                  else beforebyteIT=*inloc + *llul;

                  dicm2dckvDataset(
                        kbuf,
                        kloc,
                        true,
                        keycs,
                        lbuf,
                        llul,
                        vbuf,
                        fromStdin,
                        inloc,
                        (uint32)beforebyteIT,
                        fffee00d
                        );
                  //Atención!!! attr still is the first attr of the first item
                  //the attr of the recursion is not available
                  //kbuf kept the last attr read one level deeper than current attr

                  
                  //write IZ
                  if (CFSwapInt32(itemstruct->t)==fffee00d)
                  {
                     itemstruct->t=ffffffff;
                     itemstruct->r=IZ;
                     itemstruct->l=0x00;
                     if (!appendkv(kbuf,kloc,false,kvIZ,*inloc,8,false,(void*)&IZbytes)) return false;
                     if (! read8(itembytes, &bytescount)) return false;
                     *inloc+=8;
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
                     if (!appendkv(kbuf,kloc,false,kvIZ,*inloc,8,false,(void*)&IZbytes)) return false;
                     itemstruct->t=copyattr.t;
                     itemstruct->r=copyattr.r;
                     itemstruct->l=copyattr.l;
                  }
                  
                  //new item number
                  itemnumber+=1;
                  attrstruct->r=CFSwapInt16(itemnumber/0x10000);
                  attrstruct->l=CFSwapInt16(itemnumber%0x10000);

               }//end while item
               kloc-=8;
#pragma mark item level end

               attrstruct->r=SZ;
               attrstruct->l=0xFFFF;
//               if (! read8(itembytes, &bytescount)) return false;
               if (!appendkv(kbuf,kloc,false,kvSZ,*inloc,8,false,(void*)&SZbytes)) return false;

               
               //itemstruct may be SZ or post SQ
               if (CFSwapInt32(itemstruct->t)==fffee0dd)
               {
                  *inloc+=8;
                  if (! read8(attrbytes, &bytescount)) return false;
               }
               else
               {
                  attrstruct->t=itemstruct->t;
                  attrstruct->r=itemstruct->r;
                  attrstruct->l=itemstruct->l;
               }
            }
         } break;
            
         case 0xFFFF:return true;//end of buffer
            
         default:
         {
            if (attrstruct->t==0 && attrstruct->r==0 && attrstruct->l==0)
            {
               return true;
            }
            E("error unknown vr at index %llu %08x %c%c %d",*inloc, attrstruct->t,attrstruct->r % 0x100,attrstruct->r / 0x100,attrstruct->l);
            return false;
         }
            
      //---------
      }//end switch
   }//end while (*index < beforebyte)
   
   if (attrstruct->t == 0xFCFFFCFF)
   {
      NSLog(@"%llu 0xFFFCFFFC vll 4 bytes (00000000) to be removed",*inloc);
      if (fread(lbuf, 1,4,stdin)!=4) {
         E("%s","0xFFFCFFFC eof instead of vll");
         return false;
      }
   }

   return true;
}

