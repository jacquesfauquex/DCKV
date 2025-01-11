// project: dicm2dckv
// file: dicm2dckv.c
// created by jacquesfauquex on 2024-04-04.

#include "dicm2dckv.h"
#include "dckvapi.h"

extern char *DICMbuf;
extern u64 DICMidx;

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
const u32 B00280008=0x08002800;//IS numberOfFrames
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


/*
 NON RECURSIVE
 read stream up to transfer syntax.
 finds sopclassidx
 finds soptsidx
 */
bool dicmuptosopts(void)
{
   //read up to dicom version 0002001 (8+150 bytes)
   //D("%s","waiting 158 bytes on stdin");
   if(!_DKVfread(158))return false;
   if (DICMidx!=158) return false;
   if (DICMbuf[128]!=0x44 || DICMbuf[129]!=0x49 || DICMbuf[130]!=0x43 || DICMbuf[131]!=0x4D) return false;
   
   struct trcl tosptsattr;
#pragma mark read sop object [so] (=sop class)
   if (!_DKVfread(8)) return false;
   memcpy(&tosptsattr.t,DICMbuf+DICMidx-8,4);
   if (tosptsattr.t!=L00020002) return false;
   u16 solen=DICMbuf[DICMidx-2]+(DICMbuf[DICMidx-1]<<8);
   u64 soloc=DICMidx;
   if (!_DKVfread(solen)){E("%s","bad 00020002");return false;}
   solen-=(DICMbuf[soloc + solen - 1]==0x0);//remove last 0x0
   u16 soidx=sopclassidx( DICMbuf+soloc, solen);
   if (soidx==0) return false;//no valid sopclass

#pragma mark read sop instance uid [si]
   if (!_DKVfread(8)) return false;
   memcpy(&tosptsattr.t,DICMbuf+DICMidx-8,4);
   if (tosptsattr.t!=L00020003) return false;
   u16 silen=DICMbuf[DICMidx-2]+(DICMbuf[DICMidx-1]<<8);
   u64 siloc=DICMidx;
   if (!_DKVfread( silen)){E("%s","bad 00020003");return false;}
   silen-=(DICMbuf[siloc + silen - 1]==0x0);//remove last 0x0


#pragma mark read transfer syntax
   if (!_DKVfread(8)) return false;
   memcpy(&tosptsattr.t,DICMbuf+DICMidx-8,4);
   if (tosptsattr.t!=L00020010) return false;
   u16 stlen=DICMbuf[DICMidx-2]+(DICMbuf[DICMidx-1]<<8);
   u64 stloc=DICMidx;
   if (!_DKVfread(stlen)){E("%s","bad 00020010");return false;}
   stlen-=(DICMbuf[stloc + stlen - 1]==0x0);//remove last 0x0
   u16 stidx=tsidx( DICMbuf+stloc, stlen);
   if (stidx==0) return false;

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
                      struct trcl *attr,  // read in advance
                      u16 keycs,          // key charset
                      u64 beforebyte,     // limite superior de lectura
                      u32 beforetag       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
)
{
   while (
       (DICMidx < beforebyte)
    && (u32swap(attr->t) < beforetag)
   )
   {
      switch (attr->r) {
         case FD: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvFD,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
         case FL: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvFL,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
         case SL: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvSL,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
         case SS: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvSS,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
         case UL: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvUL,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
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
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case AT: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvAT,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
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
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case AS:
         case DT: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvTP,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
         case DA: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00080020: if (!_DKVappend(kloc,kvedate, attr->l)) return false;break;
               case B00080021: if (!_DKVappend(kloc,kvsdate, attr->l)) return false;break;
               case B00100030: if (!_DKVappend(kloc,kvpbirth,attr->l)) return false;break;
               default:        if (!_DKVappend(kloc,kvTP,    attr->l)) return false;break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case TM: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00080031: if (!_DKVappend(kloc,kvstime,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kvTP,   attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
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
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case AE:
         case DS: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kvTA,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
         case IS: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00200011:if (!_DKVappend(kloc,kvsnumber, attr->l)) return false; break;
               case B00200012:if (!_DKVappend(kloc,kvianumber,attr->l)) return false; break;
               case B00200013:if (!_DKVappend(kloc,kvinumber, attr->l)) return false; break;
               case B00280008:if (!_DKVappend(kloc,kvframesnumber, attr->l)) return false; break;
               default:       if (!_DKVappend(kloc,kvTA,      attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
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
                  u32 *containertag=(u32*) (kbuf+kloc-8);
                  switch (*containertag) {
                     case B00081032:if (!_DKVappend(kloc,kvecode,attr->l)) return false;break;//study description
                     default:       if (!_DKVappend(kloc,kvTS,   attr->l)) return false;break;
                  }
               } break;
               default:        if (!_DKVappend(kloc,kvTS,      attr->l)) return false;break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
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
                  u32 *containertag=(u32*) (kbuf+kloc-8);
                  switch (*containertag) {
                     //study description
                     case B00081032: if (!_DKVappend(kloc,kvecode,attr->l)) return false; break;
                     default:        if (!_DKVappend(kloc,kvTS,   attr->l)) return false; break;
                  }
               } break;
               default:        if (!_DKVappend(kloc,kvTS,    attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case ST: {
            attr->c=keycs;
            switch (attr->t) {
               case B0040E001: if (!_DKVappend(kloc,kvscdaid,   attr->l)) return false; break;//kvscdaid ST CDA root^extension
               case B00420010: if (!_DKVappend(kloc,kvsdoctitle,attr->l)) return false; break;//kvsdoctitle ST DocumentTitle 00420010
               default:        if (!_DKVappend(kloc,kvTS,       attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
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
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
            
         case OF: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00010: if (!_DKVappend(kloc,kvpixelOF,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,      attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case OD: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00010: if (!_DKVappend(kloc,kvpixelOD,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,      attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case OB: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B00420011: if (!_DKVappend(kloc,kvsdocument,attr->l)) return false; break;
               case B7FE00010: if (!_DKVappend(kloc,kvpixelOB,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,       attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case OW: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00010: if (!_DKVappend(kloc,kvpixelOW,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,      attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case OL: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00010: if (!_DKVappend(kloc,kvpixelOL,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,      attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case OV: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00001: if (!_DKVappend(kloc,kvfo,attr->l)) return false; break;
               case B7FE00002: if (!_DKVappend(kloc,kvfl,attr->l)) return false; break;
               case B7FE00010: if (!_DKVappend(kloc,kvpixelOV,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
            
         case SV: { attr->c=REPERTOIRE_GL; if (!_DKVappend(kloc,kv01,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
         case UV: {
            attr->c=REPERTOIRE_GL;
            switch (attr->t) {
               case B7FE00003: if (!_DKVappend(kloc,kvft,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kv01,attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case UC: { attr->c=keycs;         if (!_DKVappend(kloc,kvTL,attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;
         case UT: {
            attr->c=keycs;
            switch (attr->t) {
               case B00400031: if (!_DKVappend(kloc,kveal,attr->l)) return false; break;
               case B00400032: if (!_DKVappend(kloc,kveau,attr->l)) return false; break;
               default:        if (!_DKVappend(kloc,kvTL, attr->l)) return false; break;
            }
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
         case UR: { attr->c=ISO_IR192;     if (!_DKVappend(kloc,kvTU, attr->l)) return false; if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}} break;//RFC3986
#pragma mark SQ
         case SQ://sequence
         {
            u64 beforebyteSQ;
            if      (attr->l==0xFFFFFFFF) beforebyteSQ=beforebyte;//SQ undefined, byte limit will be of the dataset
            else if (beforebyte==0xFFFFFFFF) beforebyteSQ= DICMidx + attr->l;//dataset undefined, byte limit will be of the SQ
            else if (DICMidx + attr->l > beforebyte) {
               E("%s","SQ incomplete input");
               return false;
            }
            else beforebyteSQ=DICMidx + attr->l;


            if (!_DKVappend(kloc,kvSA, attr->l)) return false;


#pragma mark SQ empty
            if (attr->l==0)
            {
               if (!_DKVappend(kloc,kvSZ, attr->l)) return false;
            }
            else
            {
#pragma mark SQ item number
               u32 *itemnumber=(u32 *)(kbuf+kloc+4);
               *itemnumber=u32swap(1);
               
#pragma mark item level
               kloc+=8;
               //for each first attr fffee000 of any new item
               if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
               struct trcl * itemattr=(struct trcl*) (kbuf+kloc);
               u32 *delimiter=(u32*) (kbuf+kloc);//tag
               while ((DICMidx < beforebyteSQ) && (*delimiter==0x00E0FEFF)) //itemstart compulsory
               {
                  //for each item
                  delimiter++;//length
                  u64 beforebyteIT;//to be computed from after item start
                  if (*delimiter==0xFFFFFFFF) beforebyteIT=beforebyteSQ;
                  else if (beforebyteSQ==0xFFFFFFFF) beforebyteIT=DICMidx + *delimiter;
                  else if (DICMidx + *delimiter > beforebyteSQ) {
                     E("%s","IT incomplete input");
                     return false;
                  }
                  else beforebyteIT=DICMidx + *delimiter;

                  if (!_DKVappend(kloc,kvIA, 0)) return false;
                  if (!_DKVfreadattr(kloc))return false;
                  if (!dicm2dckvDataset(
                      kloc,
                      itemattr,
                      keycs,
                      (u32)beforebyteIT,
                      0xfffee00d
                      )) return false;
                  //write IZ
                  if (u32swap(   itemattr->t)==0xfffee00d)
                  {
                        itemattr->t=0xFFFFFFFF;
                        itemattr->r=IZ;
                        itemattr->l=0;
                     if (!_DKVappend(kloc,kvIZ, itemattr->l)) return false;
                     if (!_DKVfreadattr(kloc))  {attr->t=u32swap(beforetag);attr->r=0xFFFF;};
                  }
                  else
                  {
                     if (itemattr->r==0xFFFF) return false;//could not read next itemattr with item dataset
                     if (!_DKVappend(kloc,kvIZ, itemattr->l)) return false;
                  }
                  *itemnumber=u32swap(u32swap(*itemnumber)+1);
               }//end while item
               kloc-=8;
#pragma mark item level end
               if (!_DKVappend(kloc,kvSZ, 0)) return false;

               //   itemattr may be SZ or post SQ
               if (u32swap(itemattr->t)==0xfffee0dd)
               {
                  if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
               }
               else
               {
                  //should not read new attr after end of switch
                  //should transfer itemattr to attr instead
                  attr->t=   itemattr->t;
                  attr->r=   itemattr->r;
                  attr->l=   itemattr->l;
               }
            }
         } break;

         case UN://unknown
         {
            // https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_6.2.2
             //5. The Value Length Field of VR UN may contain Undefined Length (FFFFFFFFH), in which case the contents can be assumed to be encoded with Implicit VR. See Section 7.5.1 to determine how to parse Data Elements with an Undefined Length.
            attr->l=REPERTOIRE_GL;
            if (!_DKVappend(kloc,kvUN, attr->l)) return false;
            if (! _DKVfreadattr(kloc)) {attr->t=u32swap(beforetag);attr->r=0xFFFF;}
         } break;
            
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
      }//end switch
   }//end while (*index < beforebyte)
   
   if (attr->t == 0xFCFFFCFF)
   {
      if (attr->l==0) DICMidx-=12;
      else
      {
         //trailing padding
         attr->c=REPERTOIRE_GL;
   #pragma mark TODO sqlite T
         if (!_DKVfread(attr->l)) {
            E("%s","trailling padding");
            return false;
         }
      }
   }      
   return true;
}

struct trcl * baseattr;
bool dicm2dckvInstance(
   u64 beforebyte,     // limite superior de lectura
   u32 beforetag       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
)
{
   baseattr=(struct trcl*) kbuf;
   
   if (!_DKVfreadattr(0))return false;
   if (dicm2dckvDataset(
       0,          //kloc
       baseattr,
       0,          //keycs
       beforebyte, //beforebyte
       beforetag  //beforetag
      ) && _DKVcommit(baseattr)) return true;
   _DKVclose();
   return false;
}

