//
//  dckvapi.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-04-04.
//

/*potencialmente multivalue: AE AS AT CS DA DS DT FD FL IS LO OW PN SH SV TM UC UI UV*/
#include <Foundation/Foundation.h>

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

uint8 swapchar;


//returns true when 8 bytes were read
BOOL dckvapi_fread8(uint8_t *buffer, unsigned long *bytesReadRef)
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
bool createtx(
   const char * dstdir,
   uint8_t    * vbuf,
   uint64 *soloc,         // offset in valbyes for sop class
   uint16 *solen,         // length in valbyes for sop class
   uint16 *soidx,         // index in const char *scstr[]
   uint64 *siloc,         // offset in valbyes for sop instance uid
   uint16 *silen,         // length in valbyes for sop instance uid
   uint64 *stloc,         // offset in valbyes for transfer syntax
   uint16 *stlen,         // length in valbyes for transfer syntax
   uint16 *stidx,         // index in const char *csstr[]
   sint16 *siidx          // SOPinstance index
){
   I("#%d",*siidx);
   printf("     144 %s\n","00020001 OB 0000 {156,2}");
   return true;
}
bool committx(sint16 *siidx){
   return closetx(siidx);
}
bool closetx(sint16 *siidx){
   I("!#%d",*siidx);
   (*siidx)++;
   return true;
}


#pragma mark - parseo y agregado

bool appendkv(
              uint8_t            *kbuf,
              unsigned long      kloc,
              BOOL               vlenisl,
              enum kvVRcategory  vrcat,
              unsigned long long vloc,
              unsigned long      vlen,
              BOOL               fromStdin,
              uint8_t            *vbuf
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
   
   
   
   //uint64* attruint64=(uint64*) kbuf+kloc;
   //printf("%*s%016llX\n",kloc+kloc+1,space, CFSwapInt64(*attruint64));
   uint32* t=(uint32*)kbuf+(kloc/4);
   uint32* i=0;
   if (kloc > 0) i=(uint32*) kbuf+(kloc/4)-4;
   uint16* l=(uint16*) kbuf+((kloc/2)+3);
   uint8 v=*(kbuf+(kloc+4));
   uint8 r=*(kbuf+(kloc+5));
      
   if (vlenisl)
   {
#pragma mark long length
      if (vlen==0xFFFFFFFF)
      {
         unsigned long long ulllen=0;
         if (fromStdin)
         {
            size_t bytesread=0xFFFE;
            while (bytesread==0xFFFE)
            {
               bytesread=fread(vbuf,1,0xFFFE,stdin);
               ulllen+=bytesread;
            }
         }
         else if (!fromStdin)
         {
            //contents already in vbuf
         }
         switch (vrcat) {
             
            case kvAl://AccessionNumberIssuer local 00080051.00400031
            case kvAu://AccessionNumberIssuer universal 00080051.00400032
            case kvTL://UC

            case kvTU://UR
            
            case kved://OB Encapsulated​Document 00420011
            case kvfo://OV Extended​Offset​Table fragments offset 7FE00001
            case kvfl://OV Extended​Offset​TableLengths fragments offset 7FE00002
            case kvft://UV Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
            case kv01://OB OD OF OL OV OW SV UV
               
            case kvUN:
               printf("%08X %c%c %04X {%llu,%llu}\n",CFSwapInt32(*t),v,r,*l,vloc+12,ulllen);
               break;
               
            case kvSA://SQ head
            {
               printf("%08X00000000\n", CFSwapInt32(*t));
            }break;

            default: return false;
         }

      }
      else
      {
#pragma mark short length
         
         if (fromStdin && vlen)
         {
            unsigned long ll=vlen;
            while (ll>0xFFFE)
            {
               if (fread(vbuf,1,0xFFFE,stdin)!=0xFFFE) return false;
               ll-=0xFFFE;
            }
            if (ll && (!feof(stdin)) && (fread(vbuf,1,ll,stdin)!=ll)) return false;
         }
         else if (!fromStdin)
         {
            //contents already in vbuf
         }
         
         
         switch (vrcat) {
             
            case kvAl://AccessionNumberIssuer local 00080051.00400031
            case kvAu://AccessionNumberIssuer universal 00080051.00400032
            case kvTL://UC

            case kvTU://UR
               
            case kv01://OB OD OF OL OV OW SV UV
               
            case kvUN:
               printf("%08X %c%c %04X {%llu,%lu}\n",CFSwapInt32(*t),v,r,*l,vloc+12,vlen);
               break;
               
            case kvSA://SQ head
            {
               printf("%08X00000000\n", CFSwapInt32(*t));
            }break;

            default: return false;
         }
      }
   }
   else //vl
   {
      if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
      
      switch (vrcat) {
         case kvFD://floating point double
         {
            printf("%08X %c%c %04X",CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               double *v=(double*)vbuf;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>3); idx++)
               {
                  printf(" %f",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvFL://floating point single
         {
            printf("%08X %c%c %04X",CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               float *v=(float*)vbuf;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>2); idx++)
               {
                  printf(" %f",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvSL://signed long
         {
            printf("%08X %c%c %04X",CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               sint32 *v=(sint32*)vbuf;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>2); idx++)
               {
                  printf(" %d",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvSS://signed short
         {
            printf("%08X %c%c %04X",CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               sint16 *v=(sint16*)vbuf;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>1); idx++)
               {
                  printf(" %hd",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvUL://unsigned long
         {
            printf("%08X %c%c %04X",CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               uint32 *v=(uint32*)vbuf;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>2); idx++)
               {
                  printf(" %u",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvUS://unsigned short
         {
            printf("%08X %c%c %04X",CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               uint16 *v=(uint16*)vbuf;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>1); idx++)
               {
                  printf(" %hu",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvAT://attribute tag
         {
            printf("%08X %c%c %04X",CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               uint16 *v=(uint16*)vbuf;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>2); idx+=2)
               {
                  printf(" %04x%04x",v[idx],v[idx+1]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvII://SOPInstanceUID
         case kvIE://StudyInstanceUID
         case kvIS://SeriesInstanceUID
         case kvUI://unique ID
         {
            printf("%08X %c%c %04X",CFSwapInt32(*t),v,r,*l);
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
            
         case kvEd://StudyDate
         case kvTP:
            
         case kvSm://Modality
         case kvAt://AccessionNumberType
         case kvIs://SeriesNumber
         case kvIi://InstanceNumber
         case kvIa://AcquisitionNumber
         case kvTA://AE DS IS CS
            
         case kvdn://ST  DocumentTitle 00420010
         case kvHC://HL7InstanceIdentifier
         case kvEi://StudyID
         case kvAn://AccessionNumber
         case kvIN://InstitutionName
         case kvTS://LO LT SH ST
            
         case kvPN:
         {
            printf("%08X %c%c %04X",CFSwapInt32(*t),v,r,*l);
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
            
         case kvIA://item head
         {
            printf("%08X %c%c %04X\n",CFSwapInt32(*t),v,r,*l);
         }break;
            
         case kvIZ://item tail
         {
            printf("%08X %c%c %04X\n",CFSwapInt32(*t),v,r,*l);
         }break;
            
         case kvSZ://SQ tail
         {
            printf("%08XFFFFFFFF\n", CFSwapInt32(*t));
         }break;

         default: return false;
      }
   }

   return true;
}
