// project: dicm2dckv
// target: dicmstructdump
// file: dckvapi.c
// created by jacquesfauquex on 2024-04-04.

/*potencialmente multivalue: AE AS AT CS DA DS DT FD FL IS LO OW PN SH SV TM UC UI UV*/

#include "dckvapi.h"

extern char *DICMbuf;
extern u64 DICMidx;
extern s16 siidx;
extern uint8_t *kbuf;

u32 _DKVfread(u32 Baskedfor)
{
   u64 Breceived=fread(DICMbuf+DICMidx,1,Baskedfor,stdin);
   if (Breceived>0xFFFFFFFF)return 0;
   DICMidx+=Breceived;
   return (u32)Breceived;
}

u8 swapchar;


//returns true when 8 bytes were read
bool _DKVfreadtag(u8 kloc,struct trcl *attrpointer)
{
   if (fread(DICMbuf+DICMidx,1,8,stdin)!=8)
   {
      if (ferror(stdin)) E("%s","stdin error");
      return false;
   }
   //group
   kbuf[kloc]=DICMbuf[DICMidx+1];
   kbuf[kloc+1]=DICMbuf[DICMidx];
   //element
   kbuf[kloc+2]=DICMbuf[DICMidx+3];
   kbuf[kloc+3]=DICMbuf[DICMidx+2];
   //vr
   kbuf[kloc+4]=DICMbuf[DICMidx+4];
   kbuf[kloc+5]=DICMbuf[DICMidx+5];
   //cs from attr
   kbuf[kloc+6]=(attrpointer->c)%0x100;
   kbuf[kloc+7]=(attrpointer->c)/0x100;

   //attr
   attrpointer->t= DICMbuf[DICMidx+1]<<24
                  |DICMbuf[DICMidx  ]<<16
                  |DICMbuf[DICMidx+3]<<8
                  |DICMbuf[DICMidx+2];
   attrpointer->r= DICMbuf[DICMidx+5]<<8
                  |DICMbuf[DICMidx+4];
   
   switch (attrpointer->r) {
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

         attrpointer->l= DICMbuf[DICMidx+3]<<24
                        |DICMbuf[DICMidx+2]<<16
                        |DICMbuf[DICMidx+1]<<8
                        |DICMbuf[DICMidx  ];
         DICMidx+=4;
      }break;
      default:
      {
         attrpointer->l= DICMbuf[DICMidx+7]<<8
                        |DICMbuf[DICMidx+6];
         DICMidx+=8;
      }break;
   }
   
   return true;
}

bool _DKVfread4(u32* ll)
{
   if (fread(DICMbuf+DICMidx,1,4,stdin)!=4)
   {
      if (ferror(stdin)) E("%s","stdin error");
      return false;
   }
   memcpy(ll, DICMbuf+DICMidx, 4);
   DICMidx+=4;
   return true;
}

const char *space=" ";
const char *backslash = "\\";

#pragma mark obligatorios formales

bool _DKVcreate(
   u64 soloc,         // offset in valbyes for sop class
   u16 solen,         // length in valbyes for sop class
   u16 soidx,         // index in const char *scstr[]
   u64 siloc,         // offset in valbyes for sop instance uid
   u16 silen,         // length in valbyes for sop instance uid
   u64 stloc,         // offset in valbyes for transfer syntax
   u16 stlen,         // length in valbyes for transfer syntax
   u16 stidx          // index in const char *csstr[]
)
{
   //static

   D("dump create #%d",siidx);
   printf("144 %s\n","00020001 OB 0000 {156,2}");
   printf("%llu 00020002 UI 0000 \"%s\" [%hu]\n",soloc,DICMbuf+soloc,soidx);
   printf("%llu 00020003 UI 0000 \"%s\"\n",siloc,DICMbuf+siloc);
   printf("%llu 00020010 UI 0000 \"%s\" [%hu]\n",stloc,DICMbuf+stloc,stidx);
   /*
    printf("%8lld%*s%02X%02X%02X%02X ",vloc,(int)(((kloc/8)*9)+8),space, kbuf[kloc-4], kbuf[kloc-3], kbuf[kloc-2], kbuf[kloc-1]);
    
   const u64 key00020002=0x0000495502000200;
   if(!_DKVappend((uint8_t*)&key00020002,0,isshort,kvUI, soloc, solen,frombuffer,DICMbuf+soloc)) return false;
   const u64 key00020003=0x0000495503000200;
   if(!_DKVappend((uint8_t*)&key00020003,0,isshort,kvUI, siloc, silen,frombuffer,DICMbuf+siloc)) return false;
   const u64 key00020010=0x0000495510000200;
   if(!_DKVappend((uint8_t*)&key00020010,0,isshort,kvUI, stloc, stlen,frombuffer,DICMbuf+stloc)) return false;
   
   //do not write transfert syntax (which is always explicit little endian) in dicm2dckv
   */
   return true;
}
bool _DKVcommit(void)
{
   FILE *fileptr=fopen("dicmstructdump.dcm", "w");
   if (fileptr == NULL) return false;
   if (fwrite(DICMbuf ,1, DICMidx , fileptr)!=DICMidx) return false;
   fclose(fileptr);
   return _DKVclose();
}
bool _DKVclose(void)
{
   I("!#%d", siidx);
   return true;
}


#pragma mark - parseo y agregado

bool _DKVappend(u32 kloc,enum kvVRcategory vrcat,u32 vlen)
{
   switch (kloc) {
      case 0: printf("%llu %02X%02X%02X%02X %c%c %04X ",DICMidx,kbuf[kloc],kbuf[kloc+1],kbuf[kloc+2],kbuf[kloc+3],kbuf[kloc+4],kbuf[kloc+5],kbuf[kloc+6] + (kbuf[kloc+7] << 8));break;
      default:printf("%llu  %02X%02X%02X%02X %02X%02X%02X%02X %c%c %04X ",DICMidx, kbuf[kloc-4], kbuf[kloc-3], kbuf[kloc-2], kbuf[kloc-1],kbuf[kloc],kbuf[kloc+1],kbuf[kloc+2],kbuf[kloc+3],kbuf[kloc+4],kbuf[kloc+5],kbuf[kloc+6] + (kbuf[kloc+7] << 8));
   }
   switch (vrcat) {
      case kvSA:printf("00000000\n");break;
      case kvSZ:printf("FFFFFFFF\n");break;//SQ tail

      case kv01://OB OD OF OL OV OW SV UV
      case kvsdocument://OB Encapsulated​Document 00420011 xml cda o pdf
      case kvnativeOB: //OB 0x7FE00010
      case kvnativeOW: //OW 0x7FE00010
      case kvnativeOD: //OD 0x7FE00009
      case kvnativeOF: //OF 0x7FE00008
      case kvnativeOC: //OB 0x7E000010
      case kvframesOB: //OB 0x7E000010
      case kvframesOC: //OB 0x7E000010
      case kvfo://OV Extended​Offset​Table fragments offset 7FE00001
      case kvfl://OV Extended​Offset​TableLengths fragments offset 7FE00002
      case kvft://UV Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
      case kvTL://UC
      case kveal://UT AccessionNumberIssuer local 00080051.00400031
      case kveau://UT AccessionNumberIssuer universal 00080051.00400032
      case kvTU://UR
      case kvUN:
         printf("{%llu,%u}\n",DICMidx-vlen,vlen);break;
         
      case kvFD://floating point double
      {
         if (vlen > 0)
         {
            printf(" (");
            double d;
            if (_DKVfread(vlen)!=vlen) return false;
            for (u16 idx=DICMidx-vlen; idx<DICMidx; idx+=8)
            {
               memcpy(&d, DICMbuf+idx, 8);
               printf(" %f",d);
            }
            printf(" )");
         }
         printf("\n");
      }break;
         
      case kvFL://floating point single
      {
         if (vlen > 0)
         {
            printf(" (");
            float f;
            if (_DKVfread(vlen)!=vlen) return false;
            for (u16 idx=DICMidx-vlen; idx<DICMidx; idx+=4)
            {
               memcpy(&f, DICMbuf+idx, 4);
               printf(" %f",f);
            }
            printf(" )");
         }
         printf("\n");
      }break;
         
      case kvSL://signed long
      {
         if (vlen > 0)
         {
            printf(" (");
            s32 s4B;
            if (_DKVfread(vlen)!=vlen) return false;
            for (u16 idx=DICMidx-vlen; idx<DICMidx; idx+=4)
            {
               memcpy(&s4B, DICMbuf+idx, 4);
               printf(" %d",s4B);
            }
            printf(" )");
         }
         printf("\n");
      }break;
         
      case kvSS://signed short
      {
         if (vlen > 0)
         {
            printf(" (");
            s16 s2B;
            if (_DKVfread(vlen)!=vlen) return false;
            for (u16 idx=DICMidx-vlen; idx<DICMidx; idx+=2)
            {
               memcpy(&s2B, DICMbuf+idx, 2);
               printf(" %hd",s2B);
            }
            printf(" )");
         }
         printf("\n");
      }break;
         
      case kvUL://unsigned long
      {
         if (vlen > 0)
         {
            printf(" (");
            u32 u4B;
            if (_DKVfread(vlen)!=vlen) return false;
            for (u16 idx=DICMidx-vlen; idx<DICMidx; idx+=4)
            {
               memcpy(&u4B, DICMbuf+idx, 4);
               printf(" %u",u4B);
            }
            printf(" )");
         }
         printf("\n");
      }break;
         
      case kvUS://unsigned short
      case kvspp://7 US
      case kvrows://8 US
      case kvcols://9 US
      case kvalloc://10 US
      case kvstored://11 US
      case kvhigh://12 US
      case kvpixrep://13 US
      case kvplanar://14 US
      {
         if (vlen > 0)
         {
            printf(" (");
            u16 u2B;
            if (_DKVfread(vlen)!=vlen) return false;
            for (u16 idx=DICMidx-vlen; idx<DICMidx; idx+=2)
            {
               memcpy(&u2B, DICMbuf+idx, 2);
               printf(" %hu",u2B);
            }
            printf(" )");
         }
         printf("\n");
      }break;
         
      case kvAT://attribute tag
      {
         if (vlen > 0)
         {
            printf(" (");
            if (_DKVfread(vlen)!=vlen) return false;
            for (u16 idx=DICMidx-vlen; idx<DICMidx; idx+=2)
            {
               printf(" %04x%04x",*DICMbuf+idx,*DICMbuf+idx+1);
            }
            printf(" )");
         }
         printf("\n");
      }break;
         
      case kvUI://unique ID
      case kveuid://StudyInstanceUID
      case kvsuid://SeriesInstanceUID
      case kviuid://SOPInstanceUID
      case kvpuid://00080019 PyramidUID
      case kvcuid://SOP​Instance​UID​Of​Concatenation​Source
         
      case kvTP:
      case kvpbirth://Patient birthdate
      case kvedate://StudyDate
      case kvsdate://SeriesDate
      case kvstime://SeriesTime

      case kvTA://AE DS IS CS
      case kvpsex://CS patient sex
      case kveat://AccessionNumberType
      case kvsmod://Modality
      case kvitype://CS 00080008 ImageType
      case kvphotocode://CS
      case kvsnumber://SeriesNumber
      case kvianumber://AcquisitionNumber
      case kvinumber://InstanceNumber

      case kvTS://LO LT SH ST
      case kvpay://LO insurance
      case kvpide://SH patient id extension
      case kvpidr://LO patient id root issuer
      case kvimg://InstitutionName
      case kvedesc://LO Study name
      case kveid://StudyID
      case kvean://AccessionNumber
      case kvecode://SQ/SH Study code 00080100,00080102
      case kvsdesc://LO Series name
      case kvscdaid://ST HL7InstanceIdentifier 0040E001  root^extension
      case kvsdoctitle://ST  DocumentTitle 00420010
      case kvicomment://LO

      case kvPN:
      case kvpname://PN patient name
      case kvcda://PN CDA
      case kvref://PN referring
      case kvreq://PN requesting
      {
         if (vlen > 0)
         {
            if (_DKVfread(vlen)!=vlen) return false;
            printf( "\"%.*s\"\n", vlen,DICMbuf+DICMidx-vlen );
         }
         else printf("\"\"\n");
      } break;
         
      case kvIA://item head
      case kvIZ://item tail
         break;
         
      default: return false;
   }
   return true;
}
