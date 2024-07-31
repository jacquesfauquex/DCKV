//
//  dicm2dckv.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-02-28.
//

#include "dicm2dckv.h"


#include "dckvapi.h"

const uint32 B00080005=0x05000800;//charset

const uint32 B00080018=0x18000800;//kvII UI SOPInstanceUID
const uint32 B0020000D=0x0D002000;//kvIE UI StudyInstanceUID
const uint32 B0020000E=0x0E002000;//kvIS UI SeriesInstanceUID

const uint32 B0040E001=0x0E002000;//kvHC ST CDA root^extension

const uint32 B00080020=0x20000800;//kvEd DA StudyDate
const uint32 B00200010=0x10002000;//kvEi SH StudyID
const uint32 B00080060=0x60000800;//kvSm CS Modality

const uint32 B00200011=0x11002000;//kvIs IS SeriesNumber
const uint32 B00200012=0x12002000;//kvIa IS AcquisitionNumber
const uint32 B00200013=0x13002000;//kvIi IS InstanceNumber

const uint32 B00080050=0x50000800;//kvAn SH Accession​Number
const uint32 B00080051=0x51000800;//kvAn SH Accession​NumberIssuer
const uint32 B00080080=0x80000800;//kvIN LO InstitutionName

const uint32 B00400031=0x31004000;//kvAl UT Accession​Number local
const uint32 B00400032=0x32004000;//kvAu UT Accession​Number universal
const uint32 B00400033=0x33004000;//kvAt CS Accession​Number type

const uint32 B00081150=0x50110800;

const uint32 B00420010=0x10004200;//kvdn ST DocumentTitle
const uint32 B00420011=0x11004200;//kved OB EncapsulatedDocument

const uint32 B7FE00001=0x0100E07F;//kvfo Extended​Offset​Table
const uint32 B7FE00002=0x0200E07F;//kvfl Extended​Offset​TableLengths
const uint32 B7FE00003=0x0300E07F;//kvft Encapsulated​Pixel​Data​Value​Total​Length

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


const BOOL islong=true;//long length
const BOOL isshort=false;//short length
const BOOL fromstdin=true;
const BOOL frombuffer=false;
const BOOL firstattrread=true;
const BOOL firstattravailable=false;


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
BOOL dicmuptosopts(
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
   //read up to dicom version 0002001 (8+150 bytes)
   D("%s","waiting 158 bytes on stdin");
   *inloc=dckvapi_fread(kbuf, 1,158, stdin);
   if (*inloc!=158) return false;
   
   //check if 128-131 = DICM
   if (kbuf[128]!=0x44 || kbuf[129]!=0x49 || kbuf[130]!=0x43 || kbuf[131]!=0x4D) return false;
   
   //variables init
   struct t4r2l2 *attrstruct=(struct t4r2l2*) kbuf;//corresponding struct
   uint64 *attruint64=(uint64*) kbuf;
   uint16 bytescount;
   
#pragma mark read sop object [so] (=sop class)
   *soloc=*inloc;
   bytescount=dckvapi_fread(kbuf, 1, 8, stdin);
   if (bytescount!=8) return false;
   if ((*attruint64 % 0x100000000)!=0x00020002)
   {
      E("%08llx [no 00020002]",*attruint64);
      return false;
   }
   *inloc+=bytescount;
   *solen=attrstruct->l;
   bytescount=dckvapi_fread(vbuf+*inloc, 1, *solen, stdin);
   if  (bytescount!=*solen)//not enough bytes
   {
      E("%s","bad 00020002");
      return false;
   }
   *soidx=scidx( vbuf+*inloc, *solen - (vbuf[*inloc + *solen - 1]==0x0) );
   if (*soidx==0) return false;//no valid sopclass
   *inloc += *solen;

#pragma mark read sop instance uid [si]
   *siloc=*inloc;
   bytescount=dckvapi_fread(vbuf+*inloc, 1, 8, stdin);
   if (bytescount!=8) return false;
   memcpy(kbuf, vbuf+*inloc, bytescount);
   if ((*attruint64 % 0x100000000)!=0x00030002)
   {
      E("%s","no 00020003");
      return false;
   }
   *inloc+=bytescount;
   *silen=attrstruct->l;
   bytescount=dckvapi_fread(vbuf+*inloc, 1, *silen, stdin);
   if  (bytescount!=*silen)//not enough bytes
   {
      E("%s","bad 00020003");
      return false;
   }
   *inloc += *silen;

#pragma mark read transfer syntax
   *stloc=*inloc;
   bytescount=dckvapi_fread(vbuf+*inloc, 1, 8, stdin);
   if (bytescount!=8) return false;
   memcpy(kbuf, vbuf+*inloc, bytescount);
   if ((*attruint64 % 0x100000000)!=0x00100002)
   {
      E("%s","no 00020010");
      return false;
   }
   *inloc+=bytescount;
   *stlen=attrstruct->l;
   bytescount=dckvapi_fread(vbuf+*inloc, 1, *stlen, stdin);
   if  (bytescount!=*stlen)//not enough bytes
   {
      E("%s","no 00020010");
      return false;
   }
   *stidx=tsidx( vbuf+*inloc, *stlen - (vbuf[*inloc + *stlen - 1]==0x0) );
   if (*stidx==0) return false;
   
   *inloc += *stlen;
   return true;//(char*)vbuf+*siloc;
}

BOOL dicm2dckvInstance(
   const char * dstdir,
   uint8_t *kbuf,     // buffer matriz de creación de nuevos keys por diferencial
   uint8_t *vbuf,     // lectura del valor del atributo
   uint8_t *lbuf,
   uint32 *vlen,      // buffer lectura 4-bytes ll de atributos largos
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
   sint16 *siidx          // instance count
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
   
   if (*soidx>0) //part 10
   {
      const uint64 key00020002=0x0000495502000200;
      if(!appendkv((uint8_t*)&key00020002,0,isshort,kvUI, *soloc, *solen,frombuffer,vbuf+*soloc+8)) return false;
      const uint64 key00020003=0x0000495503000200;
      if(!appendkv((uint8_t*)&key00020003,0,isshort,kvUI, *siloc, *silen,frombuffer,vbuf+*siloc+8)) return false;
      const uint64 key00020010=0x0000495510000200;
      if(!appendkv((uint8_t*)&key00020010,0,isshort,kvUI, *stloc, *stlen,frombuffer,vbuf+*stloc+8)) return false;

      if (   dicm2dckvDataset(
                  kbuf,
                  0,          //kloc
                  firstattrread,
                  0,          //keycs
                  lbuf,
                  vlen,
                  vbuf,
                  fromstdin,
                  inloc,
                  beforebyte, //beforebyte
                  beforetag  //beforetag
                 )
          && committx(siidx)
          ) return true;
   }
   else //pure dataset
   {
      if (dicm2dckvDataset(
                     kbuf,
                     0,          //kloc
                     false,      //readfirstattr
                     0,          //keycs
                     lbuf,
                     vlen,
                     vbuf,
                     true,       //fromStdin
                     inloc,
                     beforebyte, //beforebyte
                     beforetag  //beforetag
                    ) && committx(siidx)) return true;
   }
   closetx(siidx);
   return false;
}



BOOL dicm2dckvDataset(
   uint8_t *kbuf,
   unsigned long kloc,
   BOOL readfirstattr,
   uint16 keycs,
   uint8_t *lbuf,
   uint32 *vlen,
   uint8_t *vbuf,
   BOOL fromStdin,
   uint64 *inloc,
   uint32 beforebyte,
   uint32 beforetag
)
{
   //inits
   unsigned long bytescount=0;
   //uint16 vl=0;//keeps vl while overwritting it in kbuf
   uint8 *attrbytes=kbuf+kloc;//subbuffer for attr reading
   struct t4r2l2 *attrstruct=(struct t4r2l2*) attrbytes;//corresponding struct
   
   //lbuf=kbuf+kloc+8;//subbuffer for ll reading
   //vlen=(uint32*)lbuf;
   
   //uint32 *ll=(uint32*)llbytes;//corresponding uin32

   if (readfirstattr && (! dckvapi_fread8(attrbytes, &bytescount))) return false;
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
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvFD,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
         case FL://floating point single
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvFL,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
         case SL://signed long
         {
            *vlen=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvSL,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
         case SS://signed short
         {
            *vlen=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvSS,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
         case UL://unsigned long
         {
            *vlen=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvUL,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
        case US://unsigned short
         {
            *vlen=attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvUS,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl tag code
         case AT://attribute tag
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvAT,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl oid code
         case UI://unique ID
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            
            switch (attrstruct->t) {
               case B00080018: if (!appendkv(kbuf,kloc,isshort,kvII,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               case B0020000D: if (!appendkv(kbuf,kloc,isshort,kvIE,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               case B0020000E: if (!appendkv(kbuf,kloc,isshort,kvIS,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               case B00081150:{
                  if (*vlen && (dckvapi_fread(vbuf, 1,*vlen,stdin)!=*vlen)) return false;
                  
                  uint16 sopclassidx=scidx( vbuf, *vlen - (vbuf[*vlen - 1]==0x0) );
                  if (sopclassidx==0x00)
                  {
                     E("bad sop class %s",[[[NSString alloc]initWithData:[NSData dataWithBytes:vbuf length:*vlen] encoding:NSASCIIStringEncoding]  cStringUsingEncoding:NSASCIIStringEncoding]);
                     return false;
                  }
                  else attrstruct->l=sopclassidx;
                  if (!appendkv(kbuf,kloc,isshort,kvUI,*inloc,*vlen,frombuffer,vbuf)) return false;
               } break;
                  
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvUI,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
#pragma mark vl ascii code
         case CS://coded string
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            switch (attrstruct->t) {
               case B00080060: if (!appendkv(kbuf,kloc,isshort,kvSm,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               case B00400033:{ //kvAt CS Accession​Number type
                  uint32 *itemtag=(uint32 *)kbuf;
                  if (*itemtag==B00080051)
                  {
                     if (!appendkv(kbuf,kloc,isshort,kvAt,*inloc,*vlen,fromStdin,vbuf)) return false;
                  }
                  else
                  {
                     if (!appendkv(kbuf,kloc,isshort,kvTA,*inloc,*vlen,fromStdin,vbuf)) return false;
                  }
               }break;
               case B00080005:{
                  if (*vlen && (dckvapi_fread(vbuf, 1,*vlen,stdin)!=*vlen)) return false;
                  uint16 repidxs=repertoireidx(vbuf,*vlen);
                  if (repidxs==0x09)
                  {
                     E("bad repertoire %s",[[[NSString alloc]initWithData:[NSData dataWithBytes:vbuf length:*vlen] encoding:NSASCIIStringEncoding]  cStringUsingEncoding:NSASCIIStringEncoding]);
                     return false;
                  }
                  else
                  {
                     keycs=(keycs & 0x8000) | repidxs;
                     attrstruct->l=repidxs;
                  }
                  if (!appendkv(kbuf,kloc,isshort,kvTA,*inloc,*vlen,frombuffer,vbuf)) return false;
               } break;
                  
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTA,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            

#pragma mark vl ascii
         case AS://age string
         case DT://date time
         case TM://time
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvTP,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
         case DA://date
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            switch (attrstruct->t) {
               case B00080020:
                  if (!appendkv(kbuf,kloc,isshort,kvEd,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTP,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

         case AE://application entity
         case DS://decimal string
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            //charset
            if (!appendkv(kbuf,kloc,isshort,kvTA,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

         case IS://integer string
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            switch (attrstruct->t) {
               case B00200011:
                  if (!appendkv(kbuf,kloc,isshort,kvIs,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00200012:
                  if (!appendkv(kbuf,kloc,isshort,kvIa,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00200013:
                  if (!appendkv(kbuf,kloc,isshort,kvIi,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTA,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

            
#pragma mark vl charset
         case LO://long string
         case LT://long text
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            switch (attrstruct->t) {
               case B00080080://institution name
                  if (!appendkv(kbuf,kloc,isshort,kvIN,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTS,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

         case SH://short string
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            switch (attrstruct->t) {
               case B00200010:
                  if (!appendkv(kbuf,kloc,isshort,kvEi,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00080050:
                  if (!appendkv(kbuf,kloc,isshort,kvAn,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTS,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;


            
         case ST://short text
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            switch (attrstruct->t) {
               case B0040E001://kvHC ST CDA root^extension
                  if (!appendkv(kbuf,kloc,isshort,kvHC,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00420010://kvdn ST DocumentTitle 00420010
                  if (!appendkv(kbuf,kloc,isshort,kvdn,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTS,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl person
         case PN://person name
         {
            *vlen=(uint32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            if (!appendkv(kbuf,kloc,isshort,kvPN,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

#pragma mark vll bin
            
            
         case OB://other byte
         {
            attrstruct->l=REPERTOIRE_GL;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(uint32*)lbuf;
            switch (attrstruct->t) {
               case B00420011:
                  if (!appendkv(kbuf,kloc,isshort,kved,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

            
         case OD://other double
         case OF://other float
         case OL://other long
         case OW://other word
         case SV://signed 64-bit very long
         {
            attrstruct->l=REPERTOIRE_GL;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(uint32*)lbuf;
            if (!appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return true;//false;

            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;

         } break;


            
         case OV://other 64-bit very long
         {
            attrstruct->l=REPERTOIRE_GL;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(uint32*)lbuf;
            switch (attrstruct->t) {
               case B7FE00001:
                  if (!appendkv(kbuf,kloc,isshort,kvfo,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B7FE00002:
                  if (!appendkv(kbuf,kloc,isshort,kvfl,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }

            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
         case UV://unsigned 64-bit very long
         {
            attrstruct->l=REPERTOIRE_GL;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(uint32*)lbuf;
            switch (attrstruct->t) {
               case B7FE00003:
                  if (!appendkv(kbuf,kloc,isshort,kvft,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }

            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;

         } break;


#pragma mark vll charset
         case UC://unlimited characters
         {
            attrstruct->l=keycs;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(uint32*)lbuf;
            if (!appendkv(kbuf,kloc,islong,kvTL,*inloc,*vlen,fromStdin,vbuf)) return false;

            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;

         } break;

         case UT://unlimited text
         {
            attrstruct->l=keycs;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(uint32*)lbuf;
            switch (attrstruct->t) {
               case B00400031:
                  if (!appendkv(kbuf,kloc,islong,kvAl,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00400032:
                  if (!appendkv(kbuf,kloc,islong,kvAu,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,islong,kvTL,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;

         } break;

            
#pragma mark vll RFC3986
         case UR://universal resrcurl identifier/locator
         {
            attrstruct->l=ISO_IR192;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(uint32*)lbuf;
            if (!appendkv(kbuf,kloc,islong,kvTU,*inloc,*vlen,fromStdin,vbuf)) return false;
            
            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
            
         } break;

      //---------
#pragma mark UN
         case UN://unknown
         {
            // https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_6.2.2
             //5. The Value Length Field of VR UN may contain Undefined Length (FFFFFFFFH), in which case the contents can be assumed to be encoded with Implicit VR. See Section 7.5.1 to determine how to parse Data Elements with an Undefined Length.
            attrstruct->l=REPERTOIRE_GL;
            
            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(uint32*)lbuf;
            if (!appendkv(kbuf,kloc,islong,kvUN,*inloc,*vlen,fromStdin,vbuf)) return false;

            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
            
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


            if (!appendkv(kbuf,kloc,islong,kvSA,*inloc,0,frombuffer,vbuf)) return false;

            //read length
            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(uint32*)lbuf;

            if (*vlen==0)
            {
               attrstruct->r=SZ;
               attrstruct->l=0xFFFF;
               if (!appendkv(kbuf,kloc,isshort,kvSZ,*inloc,0,fromStdin,(void*)&SZbytes)) return false;

               //read nextattr
               *inloc += 12;//do not add *vlen !
               if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
            }
            else //SQ *vlen!=0
            {
               *inloc += 12;
               //SQ length
               uint64 beforebyteSQ;
               if (*vlen==ffffffff) beforebyteSQ=beforebyte;//undefined length
               else if (beforebyte==ffffffff) beforebyteSQ= *inloc + *vlen;//undefined length container
               else if (*inloc + *vlen > beforebyte) {
                  E("%s","SQ incomplete input");
                  return false;
               }
               else beforebyteSQ=*inloc + *vlen;

               
               //replace vr and vl of SQ by itemnumber
               uint32 itemnumber=1;
               attrstruct->r=CFSwapInt16(itemnumber/0x10000);
               attrstruct->l=CFSwapInt16(itemnumber%0x10000);


               
#pragma mark item level
               kloc+=8;
               uint8 *itembytes=kbuf+kloc;
               struct t4r2l2 *itemstruct=(struct t4r2l2*) itembytes;
               if (! dckvapi_fread8(itembytes, &bytescount)) return false;

               
               //for each first attr fffee000 of any new item
              while ((*inloc < beforebyteSQ) && (CFSwapInt32(itemstruct->t)==fffee000)) //itemstart compulsory
               {
                  uint32 IQll = (itemstruct->l << 16) | itemstruct->r;
                  itemstruct->t=0x00000000;
                  itemstruct->r=IA;
                  itemstruct->l=REPERTOIRE_GL;
                  if (!appendkv(kbuf,kloc,isshort,kvIA,*inloc,0,fromStdin,(void*)&IAbytes)) return false;

                  uint64 beforebyteIT;//to be computed from after item start
                  *inloc+=8;
                  if (IQll==ffffffff) beforebyteIT=beforebyteSQ;
                  else if (beforebyteSQ==ffffffff) beforebyteIT=*inloc + *vlen;
                  else if (*inloc + IQll > beforebyteSQ) {
                     E("%s","IT incomplete input");
                     return false;
                  }
                  else beforebyteIT=*inloc + IQll;

                  dicm2dckvDataset(
                        kbuf,
                        kloc,
                        firstattrread,
                        keycs,
                        lbuf,
                        vlen,
                        vbuf,
                        fromstdin,
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
                     if (!appendkv(kbuf,kloc,isshort,kvIZ,*inloc,0,fromStdin,(void*)&IZbytes)) return false;
                     if (! dckvapi_fread8(itembytes, &bytescount)) return false;
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
                     if (!appendkv(kbuf,kloc,isshort,kvIZ,*inloc,0,fromStdin,(void*)&IZbytes)) return false;
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
//               if (! dckvapi_fread8(itembytes, &bytescount)) return false;
               if (!appendkv(kbuf,kloc,isshort,kvSZ,*inloc,0,fromStdin,(void*)&SZbytes)) return false;

               
               //itemstruct may be SZ or post SQ
               if (CFSwapInt32(itemstruct->t)==fffee0dd)
               {
                  *inloc+=8;
                  if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
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
      attrstruct->l=REPERTOIRE_GL;

      lbuf=kbuf+kloc+8;//subbuffer for ll reading
      if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
         E("%s","stream end instead of vll");
         return false;
      }
      *vlen=*(uint32*)lbuf;

      if ((*vlen > 0) && !appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return false;
   }

   return true;
}

