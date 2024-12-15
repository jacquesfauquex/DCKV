// project: dicm2dckv
// target: dicmstructdump
// file: dckvapi.m
// created by jacquesfauquex on 2024-04-04.

/*potencialmente multivalue: AE AS AT CS DA DS DT FD FL IS LO OW PN SH SV TM UC UI UV*/

#include "dckvapi.h"

size_t dckvapi_fread(
                     void * __restrict __ptr,
                     size_t __size,
                     size_t __nitems,
                     FILE * __restrict __stream
                     )
{
   return fread(__ptr,__size,__nitems,__stream);
}

u8 swapchar;


//returns true when 8 bytes were read
bool dckvapi_fread8(uint8_t *buffer, u64 *bytesReadRef)
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

const char *space=" ";
const char *backslash = "\\";

#pragma mark obligatorios formales
bool createdckv(
   const char * dstdir,
   uint8_t    * vbuf,
   u64 *soloc,         // offset in valbyes for sop class
   u16 *solen,         // length in valbyes for sop class
   u16 *soidx,         // index in const char *scstr[]
   u64 *siloc,         // offset in valbyes for sop instance uid
   u16 *silen,         // length in valbyes for sop instance uid
   u64 *stloc,         // offset in valbyes for transfer syntax
   u16 *stlen,         // length in valbyes for transfer syntax
   u16 *stidx,         // index in const char *csstr[]
   s16 *siidx          // SOPinstance index
){
   (*siidx)++;
   I("#%d",*siidx);
   printf("     144 %s\n","00020001 OB 0000 {156,2}");
   return true;
}
bool commitdckv(s16 *siidx){
   return closedckv(siidx);
}
bool closedckv(s16 *siidx){
   I("!#%d",*siidx);
   
   return true;
}


#pragma mark - parseo y agregado

bool appendkv(
              uint8_t           *kbuf,
              u32                kloc,
              bool               vlenisl,
              enum kvVRcategory  vrcat,
              u64                vloc,
              u32                vlen,
              bool               fromStdin,
              uint8_t           *vbuf
              )
{
   //loc+datasetspace+itemspace+item
   switch (kloc) {
      case 0: printf("%8lld ",vloc);
         break;
      case 8:
      {
         printf("%8lld         %02X%02X%02X%02X ",vloc, kbuf[kloc-4], kbuf[kloc-3], kbuf[kloc-2], kbuf[kloc-1]);
      }
         break;
      default:
      {
         printf("%8lld%*s%02X%02X%02X%02X ",vloc,(int)(((kloc/8)*9)+8),space, kbuf[kloc-4], kbuf[kloc-3], kbuf[kloc-2], kbuf[kloc-1]);
      }
   }
   
   
   
   //u64* attruint64=(u64*) kbuf+kloc;
   //printf("%*s%016llX\n",kloc+kloc+1,space, CFSwapInt64(*attruint64));
   u32* t=(u32*)kbuf+(kloc/4);
   u32* i=0;
   if (kloc > 0) i=(u32*) kbuf+(kloc/4)-4;
   u16* l=(u16*) kbuf+((kloc/2)+3);
   u8 v=*(kbuf+(kloc+4));
   u8 r=*(kbuf+(kloc+5));
      
   if (vlenisl)
   {
#pragma mark long length
      if (fromStdin)
      {
         u32 toberead=vlen;
         while (toberead>0xFFFE)
         {
            if (fread(vbuf,1,0xFFFE,stdin)!=0xFFFE) return false;
            toberead-=0xFFFE;
         }
         if (toberead && (!feof(stdin)) && (fread(vbuf,1,toberead,stdin)!=toberead)) return false;
      }
      switch (vrcat) {
             
         case kv01://OB OD OF OL OV OW SV UV
            
         case kvsdocument://55 OB Encapsulated​Document 00420011 xml cda o pdf
         case kvnative: //56 OB 0x7FE00010
         case kvencoded: //57 OB 0x7E000010
         case kvnativeOW: //58 OW 0x7FE00010
         case kvnativeOD: //59 OD 0x7FE00009
         case kvnativeOF: //60 OF 0x7FE00008
         case kvfo://OV Extended​Offset​Table fragments offset 7FE00001
         case kvfl://OV Extended​Offset​TableLengths fragments offset 7FE00002
         case kvft://UV Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
         case kvTL://UC
         case kveal://UT AccessionNumberIssuer local 00080051.00400031
         case kveau://UT AccessionNumberIssuer universal 00080051.00400032
         case kvTU://UR
            break;
            
         case kvSA:printf("%08X00000000\n", u32swap(*t));break;
            
         case kvUN:printf("%08X %c%c %04X {%llu,%u}\n", u32swap(*t),v,r,*l,vloc+12,vlen);break;

         default: return false;
      }

   }
   else //vl
   {
      if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
      
      switch (vrcat) {
         case kvFD://floating point double
         {
            printf("%08X %c%c %04X",u32swap(*t),v,r,*l);
            if (vlen > 0)
            {
               double *v=(double*)vbuf;
               printf(" (");
               for (u16 idx=0; idx<(vlen>>3); idx++)
               {
                  printf(" %f",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvFL://floating point single
         {
            printf("%08X %c%c %04X",u32swap(*t),v,r,*l);
            if (vlen > 0)
            {
               float *v=(float*)vbuf;
               printf(" (");
               for (u16 idx=0; idx<(vlen>>2); idx++)
               {
                  printf(" %f",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvSL://signed long
         {
            printf("%08X %c%c %04X",u32swap(*t),v,r,*l);
            if (vlen > 0)
            {
               s32 *v=(s32*)vbuf;
               printf(" (");
               for (u16 idx=0; idx<(vlen>>2); idx++)
               {
                  printf(" %d",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvSS://signed short
         {
            printf("%08X %c%c %04X",u32swap(*t),v,r,*l);
            if (vlen > 0)
            {
               s16 *v=(s16*)vbuf;
               printf(" (");
               for (u16 idx=0; idx<(vlen>>1); idx++)
               {
                  printf(" %hd",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvUL://unsigned long
         {
            printf("%08X %c%c %04X",u32swap(*t),v,r,*l);
            if (vlen > 0)
            {
               u32 *v=(u32*)vbuf;
               printf(" (");
               for (u16 idx=0; idx<(vlen>>2); idx++)
               {
                  printf(" %u",v[idx]);
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
            printf("%08X %c%c %04X",u32swap(*t),v,r,*l);
            if (vlen > 0)
            {
               u16 *v=(u16*)vbuf;
               printf(" (");
               for (u16 idx=0; idx<(vlen>>1); idx++)
               {
                  printf(" %hu",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvAT://attribute tag
         {
            printf("%08X %c%c %04X",u32swap(*t),v,r,*l);
            if (vlen > 0)
            {
               u16 *v=(u16*)vbuf;
               printf(" (");
               for (u16 idx=0; idx<(vlen>>2); idx+=2)
               {
                  printf(" %04x%04x",v[idx],v[idx+1]);
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
         {
            printf("%08X %c%c %04X",u32swap(*t),v,r,*l);
            if (vlen > 0)
            {
               printf(" (");
               vbuf[vlen]=0x0;//terminate even uid lists
               char *token = strtok((char *)vbuf, backslash);
               while( token != NULL ) {
                     printf( " %s", token );
                     token = strtok(NULL, backslash);
               }
               printf(" )");
            }
            printf("\n");
         }break;
         
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
            printf("%08X %c%c %04X",u32swap(*t),v,r,*l);
            if (vlen > 0)
            {
               printf(" (");
               vbuf[vlen]=0x0;//terminate even uid lists
               char *token = strtok((char *)vbuf, backslash);
               while( token != NULL ) {
                     printf( " «%s»", token );
                     token = strtok(NULL, backslash);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvIA:printf("%08X %c%c %04X\n",u32swap(*t),v,r,*l);break;//item head
         case kvIZ:printf("%08X %c%c %04X\n",u32swap(*t),v,r,*l);break;//item tail
         case kvSZ:printf("%08XFFFFFFFF\n", u32swap(*t));break;//SQ tail
         default: return false;
      }
   }

   return true;
}
