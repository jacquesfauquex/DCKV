// project: dicm2dckv
// file: dicm2dckv.c
// created by jacquesfauquex on 2024-04-04.

#include "dicm2dckv.h"


#include "dckvapi.h"

const u32 B00080005=0x05000800;//charset

const u32 B00080100=0x00010800;//code
const u32 B00080102=0x02010800;//domain
const u32 B00080104=0x04010800;//title

const u32 B00080018=0x18000800;//kviuid UI SOPInstanceUID
const u32 B00080019=0x19000800;//pyramid

const u32 B0020000D=0x0D002000;//kveuid UI StudyInstanceUID
const u32 B0020000E=0x0E002000;//kvsuid UI SeriesInstanceUID

const u32 B0040E001=0x0E002000;//kvscdaid ST CDA root^extension

const u32 B00100010=0x10001000;//kvpname PN Patient nanme
const u32 B00100020=0x20001000;//kvpide LO Patient id
const u32 B00100021=0x21001000;//kvpidr LO Patient id issuer
const u32 B00100030=0x30001000;//kvpbirth DA Patient bBirthdate
const u32 B00100040=0x40001000;//kvpsex CS Patient sex

const u32 B00080020=0x20000800;//kvedate DA StudyDate
const u32 B00080021=0x21000800;//kvsdate DA SeriesDate
const u32 B00080031=0x31000800;//kvstime DA SeriesTime
const u32 B00200010=0x10002000;//kveid SH StudyID
const u32 B00081030=0x30100800;//kvedesc LO Study name
const u32 B00081032=0x32100800;//kvecode SQ Study code
const u32 B00080090=0x90000800;//kvref PN referring
const u32 B00321032=0x32103200;//kvreq PN requesting

const u32 B00081060=0x60100800;//kvcda PN CDA writer (reading)
const u32 B00101050=0x50101000;//kvpay LO pay insurance plan identification
const u32 B00080060=0x60000800;//kvsmod CS Modality

const u32 B00200011=0x11002000;//kvsnumber IS SeriesNumber
const u32 B00200012=0x12002000;//kvianumber IS AcquisitionNumber
const u32 B00200013=0x13002000;//kvinumber IS InstanceNumber
const u32 B00200242=0x42022000;//SOP​Instance​UID​Of​Concatenation​Source

const u32 B0008103E=0x3E100800;//kvedesc LO Series name

const u32 B00080050=0x50000800;//kvean SH Accession​Number
const u32 B00080051=0x51000800;//kvean SQ Accession​NumberIssuer
const u32 B00080080=0x80000800;//kvimg LO InstitutionName

const u32 B00400031=0x31004000;//kveal UT Accession​Number local
const u32 B00400032=0x32004000;//kveau UT Accession​Number universal
const u32 B00400033=0x33004000;//kveat CS Accession​Number type

const u32 B00081150=0x50110800;

const u32 B00420010=0x10004200;//kvsdoctitle ST DocumentTitle
const u32 B00420011=0x11004200;//kvsxml OB EncapsulatedDocument

const u32 B7FE00001=0x0100E07F;//kvfo Extended​Offset​Table
const u32 B7FE00002=0x0200E07F;//kvfl Extended​Offset​TableLengths
const u32 B7FE00003=0x0300E07F;//kvft Encapsulated​Pixel​Data​Value​Total​Length

const u32 B00020010=0x10000200;//kvC or kvC UI transfert syntax
const u32 B00082111=0x11210800;//kvC        ST derivation description

const u32 B00080008=0x08000800;//CS image type itype
const u32 B00204000=0x00402000;//LT compression desc (image comment)
const u32 B00280002=0x02002800;//US spp
const u32 B00280004=0x04002800;//CS photocode (photometric interpretation)
const u32 B00280010=0x10002800;//US rows
const u32 B00280011=0x11002800;//US cols
const u32 B00280100=0x00012800;//US alloc
const u32 B00280101=0x01012800;//US stored
const u32 B00280102=0x02012800;//US high
const u32 B00280103=0x03012800;//US pixrep
const u32 B00280106=0x06012800;//US planar

const u32 B7FE00008=0x0800E07F;//kvN OF
const u32 B7FE00009=0x0900E07F;//kvN OD
const u32 B7FE00010=0x1000E07F;//kvN OB or OW or kvC

const u64 SZbytes=0xDDE0FEFF;//FFFEE0DD00000000
const u64 IAbytes=0xFFFFFFFF00E0FEFF;//FFFEE000FFFFFFFF
const u64 IZbytes=0x0DE0FEFF;//FFFEE00D00000000
const u32 ffffffff=0xFFFFFFFF;
const u32 fffee000=0xfffee000;
const u32 fffee00d=0xfffee00d;
const u32 fffee0dd=0xfffee0dd;

//short length value vr
const u16 AE=0x4541;
const u16 AS=0x5341;
const u16 AT=0x5441;
const u16 CS=0x5343;
const u16 DA=0x4144;
const u16 DS=0x5344;
const u16 DT=0x5444;
const u16 FD=0x4446;
const u16 FL=0x4C46;
const u16 IS=0x5349;
const u16 LO=0x4f4c;
const u16 LT=0x544c;
const u16 PN=0x4e50;
const u16 SH=0x4853;
const u16 SL=0x4C53;
const u16 SS=0x5353;
const u16 ST=0x5453;
const u16 TM=0x4d54;
const u16 UI=0x4955;
const u16 UL=0x4C55;
const u16 US=0x5355;

//long length value vr
const u16 OB=0x424F;
const u16 OD=0x444F;
const u16 OF=0x464F;
const u16 OL=0x4C4F;
const u16 OV=0x564F;
const u16 OW=0x574F;
const u16 SV=0x5653;
const u16 UC=0x4355;
const u16 UR=0x5255;
const u16 UT=0x5455;
const u16 UV=0x5655;

const u16 UN=0x4E55;

const u16 SQ=0x5153;

//propietary delimitation vr

const u16 SA=0x0000;
const u16 IA=0x2B2B;//++
const u16 IZ=0x5F5F;//__
const u16 SZ=0xFFFF;


const bool islong=true;//long length
const bool isshort=false;//short length
const bool fromstdin=true;
const bool frombuffer=false;
const bool firstattrread=true;
const bool firstattravailable=false;


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
bool dicmuptosopts(
   uint8_t *kbuf, // buffer matriz de creación de nuevos keys por diferencial
   uint8_t *vbuf, // lectura del valor del atributo returns with sopiuid
   u64 *inloc, // current stdin byte index
   u64 *soloc, // offset in valbyes for sop class
   u16 *solen, // length in valbyes for sop class
   u16 *soidx, // index in const char *scstr[]
   u64 *siloc, // offset in valbyes for sop instance uid
   u16 *silen, // length in valbyes for sop instance uid
   u64 *stloc, // offset in valbyes for transfer syntax
   u16 *stlen, // length in valbyes for transfer syntax
   u16 *stidx  // index in const char *csstr[]
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
   u64 *attruint64=(u64*) kbuf;
   u16 bytescount;
   
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
   *soidx=sopclassidx( vbuf+*inloc, *solen - (vbuf[*inloc + *solen - 1]==0x0) );
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

bool dicm2dckvInstance(
   const char * dstdir,
   uint8_t *kbuf,     // buffer matriz de creación de nuevos keys por diferencial
   uint8_t *vbuf,     // lectura del valor del atributo
   uint8_t *lbuf,
   u32 *vlen,      // buffer lectura 4-bytes ll de atributos largos
   u64 *inloc,           // offstet en stream
   u32 beforebyte,     // limite superior de lectura
   u32 beforetag,       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
   u64 *soloc,         // offset in valbyes for sop class
   u16 *solen,         // length in valbyes for sop class
   u16 *soidx,         // index in const char *scstr[]
   u64 *siloc,         // offset in valbyes for sop instance uid
   u16 *silen,         // length in valbyes for sop instance uid
   u64 *stloc,         // offset in valbyes for transfer syntax
   u16 *stlen,         // length in valbyes for transfer syntax
   u16 *stidx,         // index in const char *csstr[]
   s16 *siidx          // instance count
)
{
   if (!createdckv(
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
      const u64 key00020002=0x0000495502000200;
      if(!appendkv((uint8_t*)&key00020002,0,isshort,kvUI, *soloc, *solen,frombuffer,vbuf+*soloc+8)) return false;
      const u64 key00020003=0x0000495503000200;
      if(!appendkv((uint8_t*)&key00020003,0,isshort,kvUI, *siloc, *silen,frombuffer,vbuf+*siloc+8)) return false;
      const u64 key00020010=0x0000495510000200;
      if(!appendkv((uint8_t*)&key00020010,0,isshort,kvUI, *stloc, *stlen,frombuffer,vbuf+*stloc+8)) return false;
      
      //do not write transfert syntax (which is always explicit little endian) in dicm2dckv
      
      if (!dicm2dckvDataset(
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
                            beforetag,  //beforetag
                            soidx,
                            stidx
                            )
          ) return false;
      if (!commitdckv(siidx)) return false;
      return true;
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
                     beforetag,  //beforetag
                     soidx,
                     stidx
                    ) && commitdckv(siidx)) return true;
   }
   closedckv(siidx);
   return false;
}



bool dicm2dckvDataset(
   uint8_t *kbuf,
   u32 kloc,           // offset actual en el búfer matriz (cambia con el nivel de recursión)
   bool readfirstattr, // true:read desde stream. false:ya está en kbuf
   u16 keycs,          // key charset
   uint8_t *lbuf,
   u32 *vlen,          // buffer lectura 4-bytes ll de atributos largos
   uint8_t *vbuf,      // lectura del valor del atributo
   bool fromStdin,     // ... o from vbuf
   u64 *inloc,
   u32 beforebyte,
   u32 beforetag,       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
   u16 *soidx,     // index in const char *scstr[]
  u16 *stidx      // index in const char *csstr[]
)
{
   //inits
   u64 bytescount=0;
   //u16 vl=0;//keeps vl while overwritting it in kbuf
   u8 *attrbytes=kbuf+kloc;//subbuffer for attr reading
   struct t4r2l2 *attrstruct=(struct t4r2l2*) attrbytes;//corresponding struct
   if (readfirstattr && (! dckvapi_fread8(attrbytes, &bytescount))) return false;
   while (
       (*inloc < beforebyte)
    && (u32swap(attrstruct->t) < beforetag)
   )
   {
      switch (attrstruct->r) {
#pragma mark vl num
         case FD://floating point double
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            
            switch (attrstruct->t) {
               case B7FE00008:
                  if (!appendkv(kbuf,kloc,isshort,kvnativeOD,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvFD,*inloc,*vlen,fromStdin,vbuf)) return false;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
         case FL://floating point single
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            
            switch (attrstruct->t) {
               case B7FE00009:
                  if (!appendkv(kbuf,kloc,isshort,kvnativeOF,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvFL,*inloc,*vlen,fromStdin,vbuf)) return false;
            }
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
            
            switch (attrstruct->t) {
               case B00280002://spp
                  if(!appendkv(kbuf,kloc,isshort,kvspp,*inloc,*vlen,fromStdin,vbuf)) return false; break;
             
               case B00280010://rows
                  if(!appendkv(kbuf,kloc,isshort,kvrows,*inloc,*vlen,fromStdin,vbuf)) return false; break;
             
               case B00280011://cols
                  if(!appendkv(kbuf,kloc,isshort,kvcols,*inloc,*vlen,fromStdin,vbuf)) return false; break;
             
               case B00280100://alloc
                  if(!appendkv(kbuf,kloc,isshort,kvalloc,*inloc,*vlen,fromStdin,vbuf)) return false; break;
             
               case B00280101://stored
                  if(!appendkv(kbuf,kloc,isshort,kvstored,*inloc,*vlen,fromStdin,vbuf)) return false; break;
             
               case B00280102://high
                  if(!appendkv(kbuf,kloc,isshort,kvhigh,*inloc,*vlen,fromStdin,vbuf)) return false; break;
             
               case B00280103://pixrep
                  if(!appendkv(kbuf,kloc,isshort,kvpixrep,*inloc,*vlen,fromStdin,vbuf)) return false; break;
             
               case B00280106://planar
                    if(!appendkv(kbuf,kloc,isshort,kvplanar,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvUS,*inloc,*vlen,fromStdin,vbuf)) return false;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
            
#pragma mark vl tag code
         case AT://attribute tag
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvAT,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

            
#pragma mark vl oid code
         case UI://unique ID
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            
            switch (attrstruct->t) {
               case B00080018: if (!appendkv(kbuf,kloc,isshort,kviuid,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               case B0020000D: if (!appendkv(kbuf,kloc,isshort,kveuid,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               case B0020000E: if (!appendkv(kbuf,kloc,isshort,kvsuid,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               case B00080019: if (!appendkv(kbuf,kloc,isshort,kvpuid,*inloc,*vlen,fromStdin,vbuf)) return false; break;//pyramid
               case B00200242: if (!appendkv(kbuf,kloc,isshort,kvcuid,*inloc,*vlen,fromStdin,vbuf)) return false; break;//SOP​Instance​UID​Of​Concatenation​Source
               case B00081150:{
                  if (*vlen && (dckvapi_fread(vbuf, 1,*vlen,stdin)!=*vlen)) return false;
                  u16 scidx=sopclassidx( vbuf, *vlen - (vbuf[*vlen - 1]==0x0) );
                  if (scidx==0x00)
                  {
                     E("bad sop class %.*s",(int)vlen,vbuf);
                     return false;
                  }
                  else attrstruct->l=scidx;
                  if (!appendkv(kbuf,kloc,isshort,kvUI,*inloc,*vlen,frombuffer,vbuf)) return false;
               } break;
                  
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvUI,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
            
#pragma mark vl ascii par
         case AS://age string
         case DT://date time
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            if (!appendkv(kbuf,kloc,isshort,kvTP,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
         case DA://date
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            switch (attrstruct->t) {
               case B00080020:
                  if (!appendkv(kbuf,kloc,isshort,kvedate,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00080021:
                  if (!appendkv(kbuf,kloc,isshort,kvsdate,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00100030:
                  if (!appendkv(kbuf,kloc,isshort,kvpbirth,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTP,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
         case TM://time
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            switch (attrstruct->t) {
               case B00080031:
                  if (!appendkv(kbuf,kloc,isshort,kvstime,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTP,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
             *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;

#pragma mark vl ascii par/even
         case CS://coded string
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            switch (attrstruct->t) {
               case B00100040: if (!appendkv(kbuf,kloc,isshort,kvpsex,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               case B00400033:{ //kveat CS Accession​Number type
                  u32 *itemtag=(u32 *)kbuf;
                  if (*itemtag==B00080051)
                  {
                     if (!appendkv(kbuf,kloc,isshort,kveat,*inloc,*vlen,fromStdin,vbuf)) return false;
                  }
                  else
                  {
                     if (!appendkv(kbuf,kloc,isshort,kvTA,*inloc,*vlen,fromStdin,vbuf)) return false;
                  }
               }break;
               case B00080060: if (!appendkv(kbuf,kloc,isshort,kvsmod,*inloc,*vlen,fromStdin,vbuf)) return false; break;
                  
               case B00080008: if (!appendkv(kbuf,kloc,isshort,kvitype,*inloc,*vlen,fromStdin,vbuf)) return false; break;
               case B00280004: if (!appendkv(kbuf,kloc,isshort,kvphotocode,*inloc,*vlen,fromStdin,vbuf)) return false; break;
                  // https://dicom.innolitics.com/ciods/rt-dose/image-pixel/00280004

               case B00080005:{
                  if (*vlen && (dckvapi_fread(vbuf, 1,*vlen,stdin)!=*vlen)) return false;
                  u16 repidxs=repertoireidx(vbuf,*vlen);
                  if (repidxs==0x09)
                  {
                     E("bad repertoire %.*s",(int)vlen,vbuf);
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
         case AE://application entity
         case DS://decimal string
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            //charset
            if (!appendkv(kbuf,kloc,isshort,kvTA,*inloc,*vlen,fromStdin,vbuf)) return false;
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
         case IS://integer string
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=REPERTOIRE_GL;
            switch (attrstruct->t) {
               case B00200011:
                  if (!appendkv(kbuf,kloc,isshort,kvsnumber,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00200012:
                  if (!appendkv(kbuf,kloc,isshort,kvianumber,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00200013:
                  if (!appendkv(kbuf,kloc,isshort,kvinumber,*inloc,*vlen,fromStdin,vbuf)) return false;
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
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            switch (attrstruct->t) {
               case B00101050://insurance plan identification
                  if (!appendkv(kbuf,kloc,isshort,kvpay,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00100020://patient id
                  if (!appendkv(kbuf,kloc,isshort,kvpide,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00100021://patient id issuer
                  if (!appendkv(kbuf,kloc,isshort,kvpidr,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00080080://institution name
                  if (!appendkv(kbuf,kloc,isshort,kvimg,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00081030://study description
                  if (!appendkv(kbuf,kloc,isshort,kvedesc,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;

               case B0008103E://series description
                  if (!appendkv(kbuf,kloc,isshort,kvsdesc,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;

               case B00204000://image comment
               {
                  if (!appendkv(kbuf,kloc,isshort,kvicomment,*inloc,*vlen,fromStdin,vbuf)) return false;
               }
                  break;

               case B00080104://find CODE tag
               {
                  u8 *codebytes=kbuf+kloc-8;//subbuffer for attr reading
                  struct t4r2l2 *codestruct=(struct t4r2l2*) codebytes;
                  switch (codestruct->t) {
                     //study description
                     case B00081032:if (!appendkv(kbuf,kloc,isshort,kvecode,*inloc,*vlen,fromStdin,vbuf)) return false;break;
                     default: if (!appendkv(kbuf,kloc,isshort,kvTS,*inloc,*vlen,fromStdin,vbuf)) return false;break;
                  }
               } break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTS,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
         case SH://short string
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            switch (attrstruct->t) {
               case B00200010:
                  if (!appendkv(kbuf,kloc,isshort,kveid,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00080050:
                  if (!appendkv(kbuf,kloc,isshort,kvean,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
#pragma mark code
               case B00080100:
               case B00080102:
               {
                  //find CODE tag
                  u8 *codebytes=kbuf+kloc-8;//subbuffer for attr reading
                  struct t4r2l2 *codestruct=(struct t4r2l2*) codebytes;
                  switch (codestruct->t) {
                        
                     //study description
                     case B00081032:if (!appendkv(kbuf,kloc,isshort,kvecode,*inloc,*vlen,fromStdin,vbuf)) return false;
                        break;
                        
                     default: if (!appendkv(kbuf,kloc,isshort,kvTS,*inloc,*vlen,fromStdin,vbuf)) return false;
                        break;
                  }
               } break;

               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTS,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
         case ST://short text
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            switch (attrstruct->t) {
               case B0040E001://kvscdaid ST CDA root^extension
                  if (!appendkv(kbuf,kloc,isshort,kvscdaid,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00420010://kvsdoctitle ST DocumentTitle 00420010
                  if (!appendkv(kbuf,kloc,isshort,kvsdoctitle,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvTS,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 8 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
         case PN://person name
         {
            *vlen=(u32)attrstruct->l;//length is then replaced in K by encoding
            attrstruct->l=keycs;
            switch (attrstruct->t) {
               case B00100010:
                  if (!appendkv(kbuf,kloc,isshort,kvpname,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00080090://referring
                  if (!appendkv(kbuf,kloc,isshort,kvref,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00081060://reading
                  if (!appendkv(kbuf,kloc,isshort,kvcda,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00321032://requesting
                  if (!appendkv(kbuf,kloc,isshort,kvreq,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               default:
                  if (!appendkv(kbuf,kloc,isshort,kvPN,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
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
            *vlen=*(u32*)lbuf;
            switch (attrstruct->t) {
               case B00420011:
                  if (!appendkv(kbuf,kloc,islong,kvsdocument,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B7FE00010://one or two bytes
               {
                  if(*stidx==2)
                  {
                     if (!appendkv(kbuf,kloc,islong,kvnative,*inloc,*vlen,fromStdin,vbuf)) return false;
                  }
                  else
                  {
                     if(!appendkv(kbuf,kloc,islong,kvencoded,*inloc,*vlen,fromStdin,vbuf)) return false;
                  }
               } break;
               default:
                  if (!appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
            }
            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
         } break;
         case OW://other word
         {
            attrstruct->l=REPERTOIRE_GL;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(u32*)lbuf;
            switch (attrstruct->t) {
               case B7FE00010:
               {
                  if (!appendkv(kbuf,kloc,islong,kvnativeOW,*inloc,*vlen,fromStdin,vbuf)) return false;
               } break;
               default:
                  if (!appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return true;//false;
                  break;
            }
            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;

         } break;
         case OD://other double
         {
            attrstruct->l=REPERTOIRE_GL;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(u32*)lbuf;
            switch (attrstruct->t) {
               case B7FE00010:
               {
                  if (!appendkv(kbuf,kloc,islong,kvnativeOD,*inloc,*vlen,fromStdin,vbuf)) return false;
               } break;
               default:
                  if (!appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return true;//false;
                  break;
            }
            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;

         } break;
         case OF://other float
         {
            attrstruct->l=REPERTOIRE_GL;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(u32*)lbuf;
            switch (attrstruct->t) {
               case B7FE00010:
               {
                  if (!appendkv(kbuf,kloc,islong,kvnativeOF,*inloc,*vlen,fromStdin,vbuf)) return false;
               } break;
               default:
                  if (!appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return true;//false;
                  break;
            }
            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;

         } break;
         case OL://other long
         case SV://signed 64-bit very long
         {
            attrstruct->l=REPERTOIRE_GL;

            lbuf=kbuf+kloc+8;//subbuffer for ll reading
            if (dckvapi_fread(lbuf, 1,4,stdin)!=4) {
               E("%s","stream end instead of vll");
               return false;
            }
            *vlen=*(u32*)lbuf;
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
            *vlen=*(u32*)lbuf;
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
            *vlen=*(u32*)lbuf;
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
            *vlen=*(u32*)lbuf;
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
            *vlen=*(u32*)lbuf;
            switch (attrstruct->t) {
               case B00400031:
                  if (!appendkv(kbuf,kloc,islong,kveal,*inloc,*vlen,fromStdin,vbuf)) return false;
                  break;
               case B00400032:
                  if (!appendkv(kbuf,kloc,islong,kveau,*inloc,*vlen,fromStdin,vbuf)) return false;
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
            *vlen=*(u32*)lbuf;
            if (!appendkv(kbuf,kloc,islong,kvTU,*inloc,*vlen,fromStdin,vbuf)) return false;
            
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
            *vlen=*(u32*)lbuf;

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
               u64 beforebyteSQ;
               if (*vlen==ffffffff) beforebyteSQ=beforebyte;//undefined length
               else if (beforebyte==ffffffff) beforebyteSQ= *inloc + *vlen;//undefined length container
               else if (*inloc + *vlen > beforebyte) {
                  E("%s","SQ incomplete input");
                  return false;
               }
               else beforebyteSQ=*inloc + *vlen;

               
               //replace vr and vl of SQ by itemnumber
               u32 itemnumber=1;
               attrstruct->r=u16swap(itemnumber/0x10000);
               attrstruct->l=u16swap(itemnumber%0x10000);


               
#pragma mark item level
               kloc+=8;
               u8 *itembytes=kbuf+kloc;
               struct t4r2l2 *itemstruct=(struct t4r2l2*) itembytes;
               if (! dckvapi_fread8(itembytes, &bytescount)) return false;

               
               //for each first attr fffee000 of any new item
              while ((*inloc < beforebyteSQ) && (u32swap(itemstruct->t)==fffee000)) //itemstart compulsory
               {
                  u32 IQll = (itemstruct->l << 16) | itemstruct->r;
                  itemstruct->t=0x00000000;
                  itemstruct->r=IA;
                  itemstruct->l=REPERTOIRE_GL;
                  if (!appendkv(kbuf,kloc,isshort,kvIA,*inloc,0,fromStdin,(void*)&IAbytes)) return false;

                  u64 beforebyteIT;//to be computed from after item start
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
                        (u32)beforebyteIT,
                        fffee00d,
                        soidx,
                        stidx
                        );
                  //Atención!!! attr still is the first attr of the first item
                  //the attr of the recursion is not available
                  //kbuf kept the last attr read one level deeper than current attr

                  
                  //write IZ
                  if (u32swap(itemstruct->t)==fffee00d)
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
                  attrstruct->r=u16swap(itemnumber/0x10000);
                  attrstruct->l=u16swap(itemnumber%0x10000);

               }//end while item
               kloc-=8;
#pragma mark item level end

               attrstruct->r=SZ;
               attrstruct->l=0xFFFF;
//               if (! dckvapi_fread8(itembytes, &bytescount)) return false;
               if (!appendkv(kbuf,kloc,isshort,kvSZ,*inloc,0,fromStdin,(void*)&SZbytes)) return false;

               
               //itemstruct may be SZ or post SQ
               if (u32swap(itemstruct->t)==fffee0dd)
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
            *vlen=*(u32*)lbuf;
            if (!appendkv(kbuf,kloc,islong,kvUN,*inloc,*vlen,fromStdin,vbuf)) return false;

            *inloc += 12 + *vlen;
            if (! dckvapi_fread8(attrbytes, &bytescount)) return false;
            
         } break;


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
      *vlen=*(u32*)lbuf;

      if ((*vlen > 0) && !appendkv(kbuf,kloc,islong,kv01,*inloc,*vlen,fromStdin,vbuf)) return false;
   }

   return true;
}

