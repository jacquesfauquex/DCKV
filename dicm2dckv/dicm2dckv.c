// project: dicm2dckv
// file: dicm2dckv.c
// created by jacquesfauquex on 2024-04-04.

#include "dicm2dckv.h"
#include "dckvapi.h"

extern char *DICMbuf;
extern u64 DICMidx;
extern s16 siidx;

const u32 L00020002=0x00020002;
const u32 L00020003=0x00030002;
const u32 L00020010=0x00100002;

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


#pragma mark parsing
/*
 NON RECURSIVE
 read stream up to transfer syntax.
 finds sopclassidx
 finds soptsidx
 */
struct trcl attr;

bool dicmuptosopts(void)
{
   //read up to dicom version 0002001 (8+150 bytes)
   D("%s","waiting 158 bytes on stdin");
   if(!_DKVfread(158))return false;
   if (DICMidx!=158) return false;
   if (DICMbuf[128]!=0x44 || DICMbuf[129]!=0x49 || DICMbuf[130]!=0x43 || DICMbuf[131]!=0x4D) return false;
   
#pragma mark read sop object [so] (=sop class)
   if (_DKVfread( 8)!=8) return false;
   memcpy(&attr.t,DICMbuf+DICMidx-8,4);
   if (attr.t!=L00020002) return false;
   u16 solen=DICMbuf[DICMidx-2]+(DICMbuf[DICMidx-1]<<8);
   u64 soloc=DICMidx;
   if (_DKVfread( solen)!=solen){E("%s","bad 00020002");return false;}
   solen-=(DICMbuf[soloc + solen - 1]==0x0);//remove last 0x0
   u16 soidx=sopclassidx( DICMbuf+soloc, solen);
   if (soidx==0) return false;//no valid sopclass

#pragma mark read sop instance uid [si]
   if (_DKVfread( 8)!=8) return false;
   memcpy(&attr.t,DICMbuf+DICMidx-8,4);
   if (attr.t!=L00020003) return false;
   u16 silen=DICMbuf[DICMidx-2]+(DICMbuf[DICMidx-1]<<8);
   u64 siloc=DICMidx;
   if (_DKVfread( silen)!=silen){E("%s","bad 00020003");return false;}
   silen-=(DICMbuf[siloc + silen - 1]==0x0);//remove last 0x0


#pragma mark read transfer syntax
   if (_DKVfread( 8)!=8) return false;
   memcpy(&attr.t,DICMbuf+DICMidx-8,4);
   if (attr.t!=L00020010) return false;
   u16 stlen=DICMbuf[DICMidx-2]+(DICMbuf[DICMidx-1]<<8);
   u64 stloc=DICMidx;
   if (_DKVfread( stlen)!=stlen){E("%s","bad 00020010");return false;}
   stlen-=(DICMbuf[stloc + stlen - 1]==0x0);//remove last 0x0
   u16 stidx=tsidx( DICMbuf+stloc, stlen);
   if (stidx==0) return false;
   
   DICMidx += stlen;
   if (!_DKVcreate(
                 soloc,         // offset in valbyes for sop class
                 solen,         // length in valbyes for sop class
                 soidx,         // index in const char *scstr[]
                 siloc,         // offset in valbyes for sop instance uid
                 silen,         // length in valbyes for sop instance uid
                 stloc,         // offset in valbyes for transfer syntax
                 stlen,         // length in valbyes for transfer syntax
                 stidx          // index in const char *csstr[]
                 )) return false;
   return true;
}


extern uint8_t *kbuf;
bool dicm2dckvDataset(
                      u32 kloc,           // offset actual en el búfer matriz (cambia con el nivel de recursión)
                      struct trcl *attr, // true:read desde stream. false:ya está en kbuf
                      u16 keycs,          // key charset
                      u64 beforebyte,     // limite superior de lectura
                      u32 beforetag       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
)
{
   while (
       (DICMidx < beforebyte)
    && (attr->t < beforetag)
   )
   {
      switch (attr->r) {
         case FD: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvFD,attr->l)) return false; } break;
         case FL: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvFL,attr->l)) return false; } break;
         case SL: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvSL,attr->l)) return false; } break;
         case SS: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvSS,attr->l)) return false; } break;
         case UL: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvUL,attr->l)) return false; } break;
         case US: {
            attr->c=REPERTOIRE_GL;
            switch (   attr->t) {
               case B00280002: if (!_DKVappend(kloc,kvspp,   attr->l)) return false; break;//spp
               case B00280010: if (!_DKVappend(kloc,kvrows,  attr->l)) return false; break;//rows
               case B00280011: if (!_DKVappend(kloc,kvcols,  attr->l)) return false; break;//cols
               case B00280100: if (!_DKVappend(kloc,kvalloc, attr->l)) return false; break;//alloc
               case B00280101: if (!_DKVappend(kloc,kvstored,attr->l)) return false; break;//stored
               case B00280102: if (!_DKVappend(kloc,kvhigh,  attr->l)) return false; break;//high
               case B00280103: if (!_DKVappend(kloc,kvpixrep,attr->l)) return false; break;//pixrep
               case B00280106: if (!_DKVappend(kloc,kvplanar,attr->l)) return false; break;//planar
               default:        if (!_DKVappend(kloc,kvUS,    attr->l)) return false;
            }
         } break;
         case AT: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvAT,attr->l)) return false; } break;
         case UI: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00080018: if (!_DKVappend(kloc,kviuid,attr->l)) return false; break;
               case B0020000D: if (!_DKVappend(kloc,kveuid,attr->l)) return false; break;
               case B0020000E: if (!_DKVappend(kloc,kvsuid,attr->l)) return false; break;
               case B00080019: if (!_DKVappend(kloc,kvpuid,attr->l)) return false; break;//pyramid
               case B00200242: if (!_DKVappend(kloc,kvcuid,attr->l)) return false; break;//SOP​Instance​UID​Of​Concatenation​Source
               //case B00081150:
               default:        if (!_DKVappend(kloc,kvUI,  attr->l)) return false;break;
            }
         } break;
         case AS:
         case DT: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvTP,attr->l)) return false; } break;
         case DA: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00080020: if (!_DKVappend(kloc,kvedate, attr->l)) return false;break;
               case B00080021: if (!_DKVappend(kloc,kvsdate, attr->l)) return false;break;
               case B00100030: if (!_DKVappend(kloc,kvpbirth,attr->l)) return false;break;
               default:        if (!_DKVappend(kloc,kvTP,    attr->l)) return false;break;
            }
         } break;
         case TM: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00080031: if (!_DKVappend(kloc,kvstime,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kvTP,   attr->l)) return false; break;
            }
         } break;
         case CS: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00100040: if (!_DKVappend(kloc,kvpsex,     attr->l)) return false; break;
               case B00400033: { //kveat CS Accession​Number type
                  u32 *itemtag=(u32 *)kbuf;
                  if (*itemtag==B00080051)
                  {
                     if (!_DKVappend(kloc,kveat,attr->l)) return false;
                  }
                  else
                  {
                     if (!_DKVappend(kloc,kvTA,attr->l)) return false;
                  }
               } break;
               case B00080060: if (!_DKVappend(kloc,kvsmod,     attr->l)) return false; break;
               case B00080008: if (!_DKVappend(kloc,kvitype,    attr->l)) return false; break;
               case B00280004: if (!_DKVappend(kloc,kvphotocode,attr->l)) return false; break;
                  // https://dicom.innolitics.com/ciods/rt-dose/image-pixel/00280004
               case B00080005: {
                  if (!_DKVappend(kloc,kvTA,attr->l)) return false;
                  u16 repidxs=repertoireidx(DICMbuf+DICMidx-attr->l,attr->l);
                  if (repidxs==0x09)
                  {
                     E("bad repertoire %.*s",attr->l,DICMbuf+DICMidx-attr->l);
                     return false;
                  }
                  else
                  {
                     keycs=(keycs & 0x8000) | repidxs;
                     attr->c=repidxs;
                  }
               } break;
               default:        if (!_DKVappend(kloc,kvTA,       attr->l)) return false; break;
            }
         } break;
         case AE:
         case DS: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvTA,attr->l)) return false; } break;
         case IS: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00200011:if (!_DKVappend(kloc,kvsnumber, attr->l)) return false; break;
               case B00200012:if (!_DKVappend(kloc,kvianumber,attr->l)) return false; break;
               case B00200013:if (!_DKVappend(kloc,kvinumber, attr->l)) return false; break;
               default:       if (!_DKVappend(kloc,kvTA,      attr->l)) return false; break;
            }
         } break;
         case LO:
         case LT: {
            attr->c=keycs;
            switch (attr->t) {
               case B00101050: if (!_DKVappend(kloc,kvpay,     attr->l)) return false; break;//insurance plan identification
               case B00100020: if (!_DKVappend(kloc,kvpide,    attr->l)) return false; break;//patient id
               case B00100021: if (!_DKVappend(kloc,kvpidr,    attr->l)) return false; break;//patient id issuer
               case B00080080: if (!_DKVappend(kloc,kvimg,     attr->l)) return false; break;//institution name
               case B00081030: if (!_DKVappend(kloc,kvedesc,   attr->l)) return false; break;//study description
               case B0008103E: if (!_DKVappend(kloc,kvsdesc,   attr->l)) return false; break;//series description
               case B00204000: if (!_DKVappend(kloc,kvicomment,attr->l)) return false; break;//image comment
               case B00080104: {
                  //find CODE tag
                  u8 *codebytes=kbuf+kloc-8;
                  struct t4r2l2 *codestruct=(struct t4r2l2*) codebytes;
                  switch (codestruct->t) {
                     case B00081032:if (!_DKVappend(kloc,kvecode,attr->l)) return false;break;//study description
                     default:       if (!_DKVappend(kloc,kvTS,   attr->l)) return false;break;
                  }
               } break;
               default:        if (!_DKVappend(kloc,kvTS,      attr->l)) return false;break;
            }
         } break;
         case SH: {
            attr->c=keycs;
            switch (attr->t) {
               case B00200010: if (!_DKVappend(kloc,kveid,   attr->l)) return false; break;
               case B00080050: if (!_DKVappend(kloc,kvean,   attr->l)) return false; break;
#pragma mark code
               case B00080100:
               case B00080102: {
                  //find CODE tag
                  u8 *codebytes=kbuf+kloc-8;//subbuffer for attr reading
                  struct t4r2l2 *codestruct=(struct t4r2l2*) codebytes;
                  switch (codestruct->t) {
                     //study description
                     case B00081032: if (!_DKVappend(kloc,kvecode,attr->l)) return false; break;
                     default:        if (!_DKVappend(kloc,kvTS,   attr->l)) return false; break;
                  }
               } break;
               default:        if (!_DKVappend(kloc,kvTS,    attr->l)) return false; break;
            }
         } break;
         case ST: {
            attr->c=keycs;
            switch (attr->t) {
               case B0040E001: if (!_DKVappend(kloc,kvscdaid,   attr->l)) return false; break;//kvscdaid ST CDA root^extension
               case B00420010: if (!_DKVappend(kloc,kvsdoctitle,attr->l)) return false; break;//kvsdoctitle ST DocumentTitle 00420010
               default:        if (!_DKVappend(kloc,kvTS,       attr->l)) return false; break;
            }
         } break;
         case PN: {
            attr->c=keycs;
            switch (attr->t) {
               case B00100010: if (!_DKVappend(kloc,kvpname,attr->l)) return false; break;
               case B00080090: if (!_DKVappend(kloc,kvref,  attr->l)) return false; break;//referring
               case B00081060: if (!_DKVappend(kloc,kvcda,  attr->l)) return false; break;//reading
               case B00321032: if (!_DKVappend(kloc,kvreq,  attr->l)) return false; break;//requesting
               default:        if (!_DKVappend(kloc,kvPN,   attr->l)) return false; break;
            }
         } break;
         case OB: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00420011: if (!_DKVappend(kloc,kvsdocument,attr->l)) return false; break;
#pragma mark TODO
                  /*
               case B7FE00010:
               {
                  //native
                   //nativeencoded
                   //frames
                   //encodedframes
                  
                  if (sopclassidxisframes(soidx))
                  {
                     if(stidx==2)
                     {
                        if (!_DKVappend(kloc,islong,kvframesOB,DICMidx,vlen,fromstdin)) return false;
                     }
                     else
                     {
                        if(!_DKVappend(kloc,islong,kvframesOC,DICMidx,vlen,fromstdin)) return false;
                     }
                  }
                  else
                  {
                     if(stidx==2)
                     {
                        if (!_DKVappend(kloc,islong,kvnativeOB,DICMidx,vlen,fromstdin)) return false;
                     }
                     else
                     {
                        if(!_DKVappend(kloc,islong,kvnativeOC,DICMidx,vlen,fromstdin)) return false;
                     }
                  }
               } break;
                   */
               default:        if (!_DKVappend(kloc,kv01,       attr->l)) return false; break;
            }
         } break;
         case OW: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00010: if (!_DKVappend(kloc,kvnativeOW,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,      attr->l)) return false; break;
            }
         } break;
         case OD: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00010: if (!_DKVappend(kloc,kvnativeOD,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,      attr->l)) return false; break;
            }
         } break;
         case OF: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00010: if (!_DKVappend(kloc,kvnativeOF,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,      attr->l)) return false; break;
            }
         } break;
         case OL:
         case SV: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kv01,attr->l)) return false; } break;
         case OV: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00001: if (!_DKVappend(kloc,kvfo,attr->l)) return false; break;
               case B7FE00002: if (!_DKVappend(kloc,kvfl,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,attr->l)) return false; break;
            }
         } break;
         case UV: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00003: if (!_DKVappend(kloc,kvft,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,attr->l)) return false; break;
            }
         } break;
         case UC: { attr->c=keycs;         if (!_DKVappend(kloc,kvTL,attr->l)) return false; } break;
         case UT: {
            attr->c=keycs;
            switch (attr->t) {
               case B00400031: if (!_DKVappend(kloc,kveal,attr->l)) return false; break;
               case B00400032: if (!_DKVappend(kloc,kveau,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kvTL, attr->l)) return false; break;
            }
         } break;
         case UR: { attr->c=ISO_IR192;     if (!_DKVappend(kloc,kvTU, attr->l)) return false; } break;//RFC3986
#pragma mark SQ
         /*
         case SQ://sequence
         {
            //register SQ con vr 0000 y length undefined
            kbuf[kloc+0x8]=0xff;
            kbuf[kloc+0x9]=0xff;
            kbuf[kloc+0xA]=0xff;
            kbuf[kloc+0xB]=0xff;
            attr->r=SA;
            attr->c=REPERTOIRE_GL;


            if (!_DKVappend(kloc,islong,kvSA,DICMidx,0,frombuffer)) return false;

            //read length
            if (!_DKVfread4(&vlen)) {
               E("%s","stream end instead of vll");
               return false;
            }
            if (vlen==0)
            {
               attr->r=SZ;
               attr->l=0xFFFF;
               if (!_DKVappend(kloc,isshort,kvSZ,DICMidx,0,fromstdin)) return false;//,(void*)&SZbytes
               //do not add vlen !
               if (! _DKVfreadtag(kloc, attr)) return false;
            }
            else //SQ vlen!=0
            {
               //SQ length
               u64 beforebyteSQ;
               if (vlen==ffffffff) beforebyteSQ=beforebyte;//undefined length
               else if (beforebyte==ffffffff) beforebyteSQ= DICMidx + vlen;//undefined length container
               else if (DICMidx + vlen > beforebyte) {
                  E("%s","SQ incomplete input");
                  return false;
               }
               else beforebyteSQ=DICMidx + vlen;

               
               //replace vr and vl of SQ by itemnumber
               u32 itemnumber=1;
               attr->r=u16swap(itemnumber/0x10000);
               attr->l=u16swap(itemnumber%0x10000);


               
#pragma mark item level
               kloc+=8;
               u8 *itembytes=kbuf+kloc;
               struct trcl *   itemattr=(struct trcl*) itembytes;
               if (! _DKVfreadtag(    itemattr)) return false;

               
               //for each first attr fffee000 of any new item
              while ((DICMidx < beforebyteSQ) && (u32swap(   itemattr->t)==fffee000)) //itemstart compulsory
               {
                  u32 IQll = (   itemattr->l << 16) |    itemattr->r;
                     itemattr->t=0x00000000;
                     itemattr->r=IA;
                     itemattr->l=REPERTOIRE_GL;
                  if (!_DKVappend(kloc,isshort,kvIA,DICMidx,0,fromstdin)) return false;//,(void*)&IAbytes)
                  u64 beforebyteIT;//to be computed from after item start
                  if (IQll==ffffffff) beforebyteIT=beforebyteSQ;
                  else if (beforebyteSQ==ffffffff) beforebyteIT=DICMidx + vlen;
                  else if (DICMidx + IQll > beforebyteSQ) {
                     E("%s","IT incomplete input");
                     return false;
                  }
                  else beforebyteIT=DICMidx + IQll;

                  dicm2dckvDataset(
                        kloc,
                        readfirstattr,
                        keycs,
                        (u32)beforebyteIT,
                        fffee00d
                        );
                  //Atención!!! attr still is the first attr of the first item
                  //the attr of the recursion is not available
                  //kbuf kept the last attr read one level deeper than current attr

                  
                  //write IZ
                  if (u32swap(   itemattr->t)==fffee00d)
                  {
                        itemattr->t=ffffffff;
                        itemattr->r=IZ;
                        itemattr->l=0x00;
                     if (!_DKVappend(kloc,isshort,kvIZ,DICMidx,0,fromstdin)) return false;//,(void*)&IZbytes
                     if (! _DKVfreadtag(   itemattr)) return false;
                  }
                  else
                  {
                     struct t4r2l2 copyattr;
                     copyattr.t=   itemattr->t;
                     copyattr.r=   itemattr->r;
                     copyattr.l=   itemattr->l;
                        itemattr->t=ffffffff;
                        itemattr->r=IZ;
                        itemattr->l=0x0;
                     if (!_DKVappend(kloc,isshort,kvIZ,DICMidx,0,fromstdin)) return false;//,(void*)&IZbytes
                        itemattr->t=copyattr.t;
                        itemattr->r=copyattr.r;
                        itemattr->l=copyattr.l;
                  }
                  
                  //new item number
                  itemnumber+=1;
                  attr->r=u16swap(itemnumber/0x10000);
                  attr->l=u16swap(itemnumber%0x10000);

               }//end while item
               kloc-=8;
#pragma mark item level end

               attr->r=SZ;
               attr->l=0xFFFF;
//               if (! _DKVfread8(itembytes, &bytescount)) return false;
               if (!_DKVappend(kloc,isshort,kvSZ,DICMidx,0,fromstdin)) return false;//,(void*)&SZbytes)

               
               //   itemattr may be SZ or post SQ
               if (u32swap(   itemattr->t)==fffee0dd)
               {
                  if (! _DKVfreadtag(attr)) return false;
               }
               else
               {
                  attr->t=   itemattr->t;
                  attr->r=   itemattr->r;
                  attr->l=   itemattr->l;
               }
            }
         } break;
          */
         case 0xFFFF:return true;//end of buffer
            
         default:
         {
            if (attr->t==0 && attr->r==0 && attr->l==0)
            {
               return true;
            }
            E("error unknown vr at index %llu %08x %c%c %d",DICMidx, attr->t,attr->r % 0x100,attr->r / 0x100,attr->l);
            return false;
         }
      //---------
#pragma mark UN
         case UN://unknown
         {
            // https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_6.2.2
             //5. The Value Length Field of VR UN may contain Undefined Length (FFFFFFFFH), in which case the contents can be assumed to be encoded with Implicit VR. See Section 7.5.1 to determine how to parse Data Elements with an Undefined Length.
            attr->l=REPERTOIRE_GL;
            if (!_DKVappend(kloc,kvUN, attr->l)) return false;
            
         } break;


      //---------
      }//end switch
      if (! _DKVfreadtag(kloc, attr)) return false;
   }//end while (*index < beforebyte)
   
   if (attr->t == 0xFCFFFCFF)
   {
      attr->c=REPERTOIRE_GL;
#pragma mark TODO
      /*
      if (!_DKVfread4(&vlen)) {
         E("%s","stream end instead of vll");
         return false;
      }
      if ((vlen > 0) && !_DKVappend(kloc,islong,kv01,DICMidx,vlen,fromstdin)) return false;
      */
   }
   return true;
}


bool dicm2dckvInstance(
   u64 beforebyte,     // limite superior de lectura
   u32 beforetag       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
)
{
   if (!_DKVfreadtag(*kbuf, &attr))return false;
   if (dicm2dckvDataset(
       0,          //kloc
       &attr,
       0,          //keycs
       beforebyte, //beforebyte
       beforetag  //beforetag
      ) && _DKVcommit()) return true;
   _DKVclose();
   return false;
}

