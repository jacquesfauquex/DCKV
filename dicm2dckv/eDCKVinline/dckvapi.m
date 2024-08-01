//
//  dckvapi.m
//  eDCKVinline
//

#include <Foundation/Foundation.h>

#include "dckvapi.h"

#include "eDCKVprefixtag.h"

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

#pragma mark static

FILE *outFile;

static unsigned long vlen2;
static unsigned long vstart;
static unsigned long vstop;
static unsigned long vtrim;

static u16 ESidx=0;
static char *dbpath;
static u16 Bcccc;//class index
static u16 Bffff;//frame
static s16 Bssss;//series number
static s16 Brrrr;//relative to series number
static s16 Biiii;//instance number
static uint8_t *Ebuf;
static uint8_t *Sbuf;
static uint8_t *Ibuf;
static u32 Eidx=0;
static u32 Sidx=0;
static u32 Iidx=0;
static size_t bytesWritten;

const char *space=" ";
const char *backslash = "\\";

bool createtx(
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
)
{
   Ebuf=malloc(0xFFFF);
   Sbuf=malloc(0xFFFFFF);
   Ibuf=malloc(0xFFFFFFFF);

   Bcccc=CFSwapInt16(*soidx);
   Bffff=0x0100;//frame
   Bssss=0x0100;//series number
   Brrrr=0x0000;//relative to series number
   Biiii=0x0100;//instance
   dbpath=malloc(0xFF);
   strcat(dbpath,dstdir);
   strcat(dbpath, "/");
   char *ibuf = malloc(*silen);
   memcpy(ibuf, vbuf+*siloc+8, *silen);
   strcat(dbpath,ibuf);
   strcat(dbpath, ".ekv");
   outFile=fopen(dbpath, "w");

   I("#%d",*siidx);
   return true;
}
bool committx(s16 *siidx)
{
#pragma mark fwrite
   //bytesWritten=fputs(&klen, outFile);
   bytesWritten=fwrite(Ebuf, 1, Eidx, outFile);
   bytesWritten=fwrite(Sbuf, 1, Sidx, outFile);
   bytesWritten=fwrite(Ibuf ,1, Iidx , outFile);
   return closetx(siidx);
}
bool closetx(s16 *siidx)
{
   fclose(outFile);
   //if (Ebuf) free(Ebuf);
   //if (Sbuf) free(Sbuf);
   //if (Ibuf) free(Ibuf);
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
   D("%d",vrcat);
   
   //no need for sequence and item delimiters
   if (vrcat==kvSA) return true;
   if (vrcat==kvIA) return true;
   if (vrcat==kvIZ) return true;
   if (vrcat==kvSZ) return true;
   
   //does the attribute belong to patient-study, series levels ?
   u64 prefix=edckvPrefix(
                             *(u32*)kbuf,//basetag
                             &ESidx,
                             Bcccc,
                             Bffff,
                             Bssss,
                             Brrrr,
                             Biiii
                             );
   
   
   if (prefix==0)//atributo paciente estudio
   {
#pragma mark Patient Study (E)
      //key
      Ebuf[Eidx++]=kloc+16;
      memcpy(Ebuf+Eidx, &prefix, 8);
      Eidx+=8;
      memcpy(Ebuf+Eidx, kbuf, kloc+8);
      Eidx+=kloc+8;
      if (vlen==0){
         memcpy(Ebuf+Eidx, &vlen, 4);
         Eidx+=4;
         return true;
      }
      else {//value with contents
         switch (vrcat)
         {
            case kvAl://AccessionNumberIssuer local 00080051.00400031
            case kvAu://AccessionNumberIssuer universal 00080051.00400032
            case kved://OB Encapsulated​Document 00420011
            case kvfo://OV Extended​Offset​Table fragments offset 7FE00001
            case kvfl://OV Extended​Offset​TableLengths fragments offset 7FE00002
            case kvft://UV Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
            case kv01://OB OD OF OL OV OW SV UV
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               memcpy(Ebuf+Eidx, &vlen, 4);
               Eidx+=4;
               memcpy(Ebuf+Eidx, vbuf, vlen);
               Eidx+=vlen;
            };break;

            case kvUN:
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               memcpy(Ebuf+Eidx, &vlen, 4);
               Eidx+=4;
               memcpy(Ebuf+Eidx, vbuf, vlen);
               Eidx+=vlen;
            };break;

                  
            case kvFD://floating point double
            case kvFL://floating point single
            case kvSL://signed long
            case kvSS://signed short
            case kvUL://unsigned long
            case kvUS://unsigned short
            case kvAT://attribute tag, 2 u16 hexa
            case kvEd://StudyDate
            case kvTP:
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               memcpy(Ebuf+Eidx, &vlen, 4);
               Eidx+=4;
               memcpy(Ebuf+Eidx, vbuf, vlen);
               Eidx+=vlen;
            };break;
               
            case kvII://SOPInstanceUID
            case kvIE://StudyInstanceUID
            case kvIS://SeriesInstanceUID
            case kvUI://unique ID
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               vlen2=vlen - (vbuf[vlen - 1] == 0);
               memcpy(Ebuf+Eidx, &vlen2, 4);
               Eidx+=4;
               memcpy(Ebuf+Eidx, vbuf, vlen2);
               Eidx+=vlen2;
            }break;

            case kvSm://Modality
            case kvAt://AccessionNumberType
            case kvIs://SeriesNumber
            case kvIi://InstanceNumber
            case kvIa://AcquisitionNumber
            case kvTA:
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen-1;
              //while( (vstop > vstart) && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Ebuf+Eidx, &vtrim, 4);
              Eidx+=4;
              memcpy(Ebuf+Eidx, vbuf+vstart, vtrim);
              Eidx+=vtrim;
            }break;

            case kvdn://ST  DocumentTitle 00420010
            case kvHC://HL7InstanceIdentifier
            case kvEi://StudyID
            case kvAn://AccessionNumber
            case kvTS:
               
            case kvPN:
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen-1;
              while( (vstop > vstart) && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Ebuf+Eidx, &vtrim, 4);
              Eidx+=4;
              memcpy(Ebuf+Eidx, vbuf+vstart, vtrim);
              Eidx+=vtrim;
            }break;
               
            case kvTL://UC
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen-1;
              while( (vstop > vstart) && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Ebuf+Eidx, &vtrim, 4);
              Eidx+=4;
              memcpy(Ebuf+Eidx, vbuf+vstart, vtrim);
              Eidx+=vtrim;
            }break;

            case kvTU://url encoded
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen-1;
              while( (vstop > vstart) && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Ebuf+Eidx, &vtrim, 4);
              Eidx+=4;
              memcpy(Ebuf+Eidx, vbuf+vstart, vtrim);
              Eidx+=vtrim;
            }break;

               
            default: return false;
         }
      }
   }
   else if (prefix & 0x0300000000000000) //I atributo instancia
   {
      Ibuf[Iidx++]=kloc+16;
      memcpy(Ibuf+Iidx, &prefix, 8);
      Iidx+=8;
      memcpy(Ibuf+Iidx, kbuf, kloc+8);
      Iidx+=kloc+8;
      if (vlen==0){
         //value zero length
         memcpy(Ibuf+Iidx, &vlen, 4);
         Iidx+=4;
         return true;
      }
      else {//value with contents
         switch (vrcat)
         {
            case kvAl://AccessionNumberIssuer local 00080051.00400031
            case kvAu://AccessionNumberIssuer universal 00080051.00400032
            case kved://OB Encapsulated​Document 00420011
            case kvfo://OV Extended​Offset​Table fragments offset 7FE00001
            case kvfl://OV Extended​Offset​TableLengths fragments offset 7FE00002
            case kvft://UV Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
            case kv01://not representable
            {
               if (vlen==0xFFFFFFFF)
               {
                  unsigned long long ulllen=0;
                  if (fromStdin)
                  {
#pragma mark TODO fragments
                     //write size insteda of data
                     size_t bytesread=0xFFFE;
                     while (bytesread==0xFFFE)
                     {
                        bytesread=fread(vbuf,1,0xFFFE,stdin);
                        ulllen+=bytesread;
                     }
                     u32 eightBytes=8;
                     memcpy(Ibuf+Iidx, &eightBytes, 4);
                     Iidx+=4;
                     memcpy(Ibuf+Iidx, &ulllen, 8);
                     Iidx+=8;
                  }
                  else
                  {
                     if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
                     memcpy(Ibuf+Iidx, &vlen, 4);
                     Iidx+=4;
                     memcpy(Ibuf+Iidx, vbuf, vlen);
                     Iidx+=vlen;
                  }
               }
            };break;

               
            case kvUN://not representable
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               memcpy(Ibuf+Iidx, &vlen, 4);
               Iidx+=4;
               memcpy(Ibuf+Iidx, vbuf, vlen);
               Iidx+=vlen;
            };break;

            case kvFD://floating point double
            case kvFL://floating point single
            case kvSL://signed long
            case kvSS://signed short
            case kvUL://unsigned long
            case kvUS://unsigned short
            case kvAT://attribute tag, 2 u16 hexa
            case kvEd://StudyDate
            case kvTP:
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               memcpy(Ibuf+Iidx, &vlen, 4);
               Iidx+=4;
               memcpy(Ibuf+Iidx, vbuf, vlen);
               Iidx+=vlen;
            };break;

            case kvII://SOPInstanceUID
            case kvIE://StudyInstanceUID
            case kvIS://SeriesInstanceUID
            case kvUI://unique ID
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               vlen2=vlen - (vbuf[vlen - 1] == 0);
               memcpy(Ibuf+Iidx, &vlen2, 4);
               Iidx+=4;
               memcpy(Ibuf+Iidx, vbuf, vlen2);
               Iidx+=vlen2;
            }break;

            case kvSm://Modality
            case kvAt://AccessionNumberType
            case kvIs://SeriesNumber
            case kvIi://InstanceNumber
            case kvIa://AcquisitionNumber
            case kvTA:
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen - 1;
              while(vstop > vstart && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Ibuf+Iidx, &vtrim, 4);
              Iidx+=4;
              memcpy(Ibuf+Iidx, vbuf+vstart, vtrim);
              Iidx+=vtrim;
            }break;

            case kvdn://ST  DocumentTitle 00420010
            case kvHC://HL7InstanceIdentifier
            case kvEi://StudyID
            case kvAn://AccessionNumber
            case kvTS://valores representadas por texto
               
            case kvPN:
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen - 1;
              while(vstop > vstart && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Ibuf+Iidx, &vtrim, 4);
              Iidx+=4;
              memcpy(Ibuf+Iidx, vbuf+vstart, vtrim);
              Iidx+=vtrim;
            }break;

            case kvTL://UC
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen - 1;
              while(vstop > vstart && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Ibuf+Iidx, &vtrim, 4);
              Iidx+=4;
              memcpy(Ibuf+Iidx, vbuf+vstart, vtrim);
              Iidx+=vtrim;
            }break;
               
            case kvTU:
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen - 1;
              while(vstop > vstart && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Ibuf+Iidx, &vtrim, 4);
              Iidx+=4;
              memcpy(Ibuf+Iidx, vbuf+vstart, vtrim);
              Iidx+=vtrim;
            }break;
               

            default: return false;
         }
      }
   }
   else //SZ atributo serie
   {
      Sbuf[Sidx++]=kloc+16;
      memcpy(Sbuf+Sidx, &prefix, 8);
      Sidx+=8;
      memcpy(Sbuf+Sidx, kbuf, kloc+8);
      Sidx+=kloc+8;
      if (vlen==0){
         memcpy(Sbuf+Sidx, &vlen, 4);
         Sidx+=4;
         return true;
      }
      else {//value with contents
         switch (vrcat)
         {
            case kvAl://AccessionNumberIssuer local 00080051.00400031
            case kvAu://AccessionNumberIssuer universal 00080051.00400032
            case kved://OB Encapsulated​Document 00420011
            case kvfo://OV Extended​Offset​Table fragments offset 7FE00001
            case kvfl://OV Extended​Offset​TableLengths fragments offset 7FE00002
            case kvft://UV Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
            case kv01://OB OD OF OL OV OW SV UV

            case kvUN:
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               memcpy(Sbuf+Sidx, &vlen, 4);
               Sidx+=4;
               memcpy(Sbuf+Sidx, vbuf, vlen);
               Sidx+=vlen;
            };break;

            case kvFD://floating point double
            case kvFL://floating point single
            case kvSL://signed long
            case kvSS://signed short
            case kvUL://unsigned long
            case kvUS://unsigned short
            case kvAT://attribute tag, 2 u16 hexa
            case kvEd://StudyDate
            case kvTP:
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               memcpy(Sbuf+Sidx, &vlen, 4);
               Sidx+=4;
               memcpy(Sbuf+Sidx, vbuf, vlen);
               Sidx+=vlen;
            };break;

            case kvII://SOPInstanceUID
            case kvIE://StudyInstanceUID
            case kvIS://SeriesInstanceUID
            case kvUI://unique ID
            {
               if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               vlen2=vlen - (vbuf[vlen - 1] == 0);
               memcpy(Sbuf+Sidx, &vlen2, 4);
               Sidx+=4;
               memcpy(Sbuf+Sidx, vbuf, vlen2);
               Sidx+=vlen2;
            }break;
                  
            case kvSm://Modality
            case kvAt://AccessionNumberType
            case kvIs://SeriesNumber
            case kvIi://InstanceNumber
            case kvIa://AcquisitionNumber
            case kvTA:
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen - 1;
              while(vstop > vstart && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Sbuf+Sidx, &vtrim, 4);
              Sidx+=4;
              memcpy(Sbuf+Sidx, vbuf+vstart, vtrim);
              Sidx+=vtrim;
            }break;

            case kvdn://ST  DocumentTitle 00420010
            case kvHC://HL7InstanceIdentifier
            case kvEi://StudyID
            case kvAn://AccessionNumber
            case kvTS://valores representadas por texto
            case kvIN://InstitutionName
            case kvPN:
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen - 1;
              while(vstop > vstart && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Sbuf+Sidx, &vtrim, 4);
              Sidx+=4;
              memcpy(Sbuf+Sidx, vbuf+vstart, vtrim);
              Sidx+=vtrim;
            }break;

            case kvTL://UC
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen - 1;
              while(vstop > vstart && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Sbuf+Sidx, &vtrim, 4);
              Sidx+=4;
              memcpy(Sbuf+Sidx, vbuf+vstart, vtrim);
              Sidx+=vtrim;
            }break;

            case kvTU://url encoded
            {
              if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
              
              // Trim leading space
              vstart=0;
              while(isspace((unsigned char)*(vbuf+vstart))) vstart++;
              vstop = vlen - 1;
              while(vstop > vstart && isspace((unsigned char)*(vbuf+vstop))) vstop--;
              vtrim=vstop-vstart+1;
              memcpy(Sbuf+Sidx, &vtrim, 4);
              Sidx+=4;
              memcpy(Sbuf+Sidx, vbuf+vstart, vtrim);
              Sidx+=vtrim;
            }break;

            default: return false;
         }
      }

   }

   return true;
}
