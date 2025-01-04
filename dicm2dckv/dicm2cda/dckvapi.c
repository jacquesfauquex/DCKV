// project: dicm2dckv
// target: dicm2cda
// file: dckvapi.c
// created by jacquesfauquex on 2024-04-04.

#include "dckvapi.h"

extern char *DICMbuf;
extern u64 DICMidx;
extern s16 siidx;
extern uint8_t *kbuf;

static FILE *outFile;

static char *dbpath;
const char *backslash = "\\";

u32 _DKVfread(u32 Baskedfor)
{
   u64 Breceived=fread(DICMbuf+DICMidx,1,Baskedfor,stdin);
   if (Breceived>0xFFFFFFFF)return 0;
   DICMidx+=Breceived;
   return (u32)Breceived;
}

//returns true when 8(+4) bytes were read
bool _DKVfreadattr(u8 kloc)
{
   if (fread(DICMbuf+DICMidx,1,8,stdin)!=8)
   {
      if (ferror(stdin)) E("%s","stdin error");
      return false;
   }
   
   //group LE>BE
   kbuf[kloc]=DICMbuf[DICMidx+1];
   kbuf[kloc+1]=DICMbuf[DICMidx];
   //element LE>BE
   kbuf[kloc+2]=DICMbuf[DICMidx+3];
   kbuf[kloc+3]=DICMbuf[DICMidx+2];
   //vr vl copied (LE)
   kbuf[kloc+4]=DICMbuf[DICMidx+4];
   kbuf[kloc+5]=DICMbuf[DICMidx+5];
   kbuf[kloc+6]=DICMbuf[DICMidx+6];
   kbuf[kloc+7]=DICMbuf[DICMidx+7];

   switch ((DICMbuf[DICMidx+5]<<8)|(DICMbuf[DICMidx+4])) {
      case OB://other byte
      case OW://other word
      case OD://other double
      case OF://other float
      case OL://other long
      case SV://signed 64-bit very long
      case OV://other 64-bit very long
      case UV://unsigned 64-bit very long
      case UC://unlimited characters
      case UT://unlimited text
      case UR://universal resrcurl identifier/locator
      case SQ://sequence
      {
         DICMidx+=8;
         if (fread(DICMbuf+DICMidx,1,4,stdin)!=4)
         {
            if (ferror(stdin)) E("%s","stdin error");
            return false;
         }
         memcpy(kbuf+kloc+8, DICMbuf+DICMidx, 4);
         DICMidx+=4;
      }break;
      default:
      {
         //IA,IZ,SZ require postprocessing in dicm2dckv
         DICMidx+=8;
         memcpy(kbuf+kloc+8, DICMbuf+DICMidx-2, 2);
         kbuf[kloc+10]=0;
         kbuf[kloc+11]=0;
      }break;
   }
   
   return true;
}


bool _DKVcreate(
   u64 soloc,         // offset in valbyes for sop class
   u16 solen,         // length in valbyes for sop class
   u16 soidx,         // index in const char *scstr[]
   u64 siloc,         // offset in valbyes for sop instance uid
   u16 silen,         // length in valbyes for sop instance uid
   u64 stloc,         // offset in valbyes for transfer syntax
   u16 stlen,         // length in valbyes for transfer syntax
   u16 stidx          // index in const char *csstr[]
){
   dbpath=malloc(0xFF);
   char *ibuf = malloc(silen);
   memcpy(ibuf, DICMbuf+siloc, silen);
   strcat(dbpath,ibuf);
   strcat(dbpath, ".dscd.xml");
   outFile=fopen(dbpath, "w");
   return true;
}
bool _DKVcommit(void){
   return _DKVclose();
}
bool _DKVclose(void){
   fclose(outFile);
   return true;
}

bool _DKVappend(int kloc, enum kvVRcategory  vrcat, u32 vlen)
{
   switch (vrcat) {
      case kvSA:
      case kvSZ:
      case kvIA:
      case kvIZ: break;
      case kvsdoctitle: { //ST  DocumentTitle 00420010
         if (vlen > 0)
         {
            //charset -> utf-8
            u32 repidx=kbuf[kloc+6] + (kbuf[kloc+7] << 8);
            u32 charstart=(u32)DICMidx;
            u32 utf8length=0;
            if (_DKVfread(vlen)!=vlen) return false;
            utf8(repidx,DICMbuf,charstart,vlen,DICMbuf,(u32)DICMidx,&utf8length);
            printf( "%.*s\n", utf8length,DICMbuf+DICMidx );
         }
      } break;
      case kvsdocument: { //OB encapsulaed document
         if (_DKVfread(vlen)!=vlen) return false;
         u32 documentlength=vlen - (DICMbuf[DICMidx-1]==0);//last char 0x00 ?
         if (!fwrite(DICMbuf+DICMidx-vlen ,1, documentlength , outFile)) return false;
      } break;
      default:if (_DKVfread(vlen)!=vlen) return false;break;
   }
   return true;
}
