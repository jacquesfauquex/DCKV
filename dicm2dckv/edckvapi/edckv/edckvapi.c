// project: dicm2dckv
// targets: eDCKVinline
// file: dckvapi.m
// created by jacquesfauquex on 2024-04-04.

#include "edckvapi.h"
//#include <stdio.h> //incluido in dckvtypes.h


#pragma mark - possibility to overwrite any  read
size_t edckvapi_fread(
                     void * __restrict __ptr,
                     size_t __size,
                     size_t __nitems,
                     FILE * __restrict __stream
                     )
{
   return fread(__ptr,__size,__nitems,__stream);
}

//returns true when 8 bytes were read
u8 swapchar;
bool edckvapi_fread8(uint8_t *buffer, u64 *bytesReadRef)
{
   *bytesReadRef=edckvapi_fread(buffer, 1, 8, stdin);
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
static u16 i;//loop iterator (2-bytes vl buffer size)
static bool isexplicit;
static u64 prefix;

static unsigned long vlen2;
static unsigned long vstart;
static unsigned long vstop;
static unsigned long vtrim;

static size_t bytesWritten;

const char *space=" ";
const char *backslash = "\\";

//output files
static uint8_t *Ebuf;
static uint8_t *Sbuf;
static uint8_t *Ibuf;
static uint8_t *Pbuf;
static uint8_t *Nbuf;
static uint8_t *Cbuf;
static u32 Eidx=8;
static u32 Sidx=8;
static u32 Iidx=8;
static u32 Pidx=8;
static u32 Nidx=8;
static u32 Cidx=8;

static char * basedir;
static char * euid;
static char * edate;
static char * euiddir;

static char *Epath;
static char *Spath;
static char *Ppath;
static char *Ipath;
static char *Npath;
static char *Cpath;
static FILE *Efile;
static FILE *Sfile;
static FILE *Pfile;
static FILE *Ifile;
static FILE *Nfile;
static FILE *Cfile;

static u8 i0;
static u8 i1;

static u8 s0;
static u8 s1;

static u8 c0;
static u8 c1;


#pragma mark - methods

bool createedckv(
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
#pragma mark receive study date and uid or manage alternatives
   //uuid_t binuuid;
    /*
     * Generate a UUID. We're not done yet, though,
     * for the UUID generated is in binary format
     * (hence the variable name). We must 'unparse'
     * binuuid to get a usable 36-character string.
     */
    //uuid_generate_random(binuuid);

    /*
     * uuid_unparse() doesn't allocate memory for itself, so do that with
     * malloc(). 37 is the length of a UUID (36 characters), plus '\0'.
     */

    //char *uuid = malloc(37);
    //uuid_unparse(binuuid, uuid);
    
    // = printf("%s\n", uuid);
    // = puts(uuid);

    
   basedir=malloc(0xFF);
   strcat(basedir,dstdir);
   
   isexplicit=*stidx==2;

   c0=*soidx % 0x100;//static for use in commitdckv series prefix
   c1=*soidx / 0x100;

   //SOPInstanceUID
   char *sopibuf = malloc(*silen);
   memcpy(sopibuf, vbuf+*siloc+8, *silen);
   
   Ebuf=malloc(0xFFFF);

   Sbuf=malloc(0xFFFF);
   Spath=malloc(0xFF);
   strcat(Spath,dstdir);
   strcat(Spath, "/");
   strcat(Spath,sopibuf);
   strcat(Spath, "/s.kv");
   Sfile=fopen(Spath, "w");

   Pbuf=malloc(0xFFFF);
   Ppath=malloc(0xFF);
   strcat(Ppath,dstdir);
   strcat(Ppath, "/");
   strcat(Ppath,sopibuf);
   strcat(Ppath, "/p.kv");
   Pfile=fopen(Ppath, "w");

   Ibuf=malloc(0xFFFF);
   Ipath=malloc(0xFF);
   strcat(Ipath,dstdir);
   strcat(Ipath, "/");
   strcat(Ipath,sopibuf);
   strcat(Ipath, "/i.kv");
   Ifile=fopen(Ipath, "w");

   if (isexplicit)
   {
      Nbuf=malloc(0xFFFF);
      Npath=malloc(0xFF);
      strcat(Npath,dstdir);
      strcat(Npath, "/");
      strcat(Npath,sopibuf);
      strcat(Npath, "/n.kv");
      Nfile=fopen(Npath, "w");
   }
   else //iscompressed
   {
      Cbuf=malloc(0xFFFF);
      Cpath=malloc(0xFF);
      strcat(Cpath,dstdir);
      strcat(Cpath, "/");
      strcat(Cpath,sopibuf);
      strcat(Cpath, "/c.kv");
      Cfile=fopen(Cpath, "w");
   }

   free(sopibuf);
   (*siidx)++;
   I("#%d",*siidx);
   return true;
}



bool fwriteEfile()
{
   //00 00 00 00 00 00 00 00
   if (!Efile)
   {

      Epath=malloc(0xFF);
      strcat(Epath,basedir);
      if (euid)
      {
         strcat(Epath, "/");
         strcat(Epath,euid);
      }
      strcat(Epath, "/e.kv");
      Efile=fopen(Epath, "w");
   }
   if (fwrite(Ebuf, 1, Eidx, Efile)!=Eidx)
   {
      E("%s","fwriteEfile error");
      return false;
   }
   Eidx=0;
   return true;
}


bool fwriteSfile()
{
   //00 01 S1 S0 C1 C0 RR RR
#pragma mark TODO RR

   //adjust prefixes
   i=0;
   while (i < Sidx)
   {
      Sbuf[i+2]=s1;
      Sbuf[i+3]=s0;
      Sbuf[i+4]=c1;
      Sbuf[i+5]=c0;
      i+= 1 + Sbuf[i];//value length
      i+= 4 + Sbuf[i] + (Sbuf[i+1]<<2) + (Sbuf[i+2]<<4) + (Sbuf[i+3]<<6);//next key length
   }
   bytesWritten=fwrite(Sbuf, 1, Sidx, Sfile);
   if (bytesWritten!=Sidx)
   {
      E("fwriteSfile %d bytes error %zu",Sidx,bytesWritten);
      return false;
   }
   Sidx=0;
   return true;
}

bool fwritePfile()
{
   //00 02 S1 S0 I1 I0 00 00
   //adjust prefixes
   i=0;
   while (i < Pidx)
   {
      Pbuf[i+2]=s1;
      Pbuf[i+3]=s0;
      Pbuf[i+4]=i1;
      Pbuf[i+5]=i0;
      i+= 1 + Pbuf[i];//value length
      i+= 4 + Pbuf[i] + (Pbuf[i+1]*0x100) + (Pbuf[i+2]*0x10000) + (Pbuf[i+3]*0x1000000);//next key length
   }
   bytesWritten=fwrite(Pbuf, 1, Pidx, Pfile);
   if (bytesWritten!=Pidx)
   {
      E("fwritePfile %d bytes error %zu",Pidx,bytesWritten);
      return false;
   }
   Pidx=0;
   return true;
}

bool fwriteIfile()
{
   //00 03 S1 S0 I1 I0 00 00
   //adjust prefixes
   i=0;
   while (i < Iidx)
   {
      Ibuf[i+2]=s1;
      Ibuf[i+3]=s0;
      Ibuf[i+4]=i1;
      Ibuf[i+5]=i0;
      i+= 1 + Ibuf[i];//value length
      i+= 4 + Ibuf[i] + (Ibuf[i+1]<<2) + (Ibuf[i+2]<<4) + (Ibuf[i+3]<<6);//next key length
   }
   bytesWritten=fwrite(Ibuf, 1, Iidx, Ifile);
   if (bytesWritten!=Iidx)
   {
      E("fwriteIfile %d bytes error %zu",Iidx,bytesWritten);
      return false;
   }
   Iidx=0;
   return true;
}

bool fwriteNfile()
{
   //00 04 S1 S0 I1 I0 00 00
   //adjust prefixes
   i=0;
   while (i < Nidx)
   {
      Nbuf[i+2]=s1;
      Nbuf[i+3]=s0;
      Nbuf[i+4]=i1;
      Nbuf[i+5]=i0;
      i+= 1 + Nbuf[i];//value length
      i+= 4 + Nbuf[i] + (Nbuf[i+1]<<2) + (Nbuf[i+2]<<4) + (Nbuf[i+3]<<6);//next key length
   }
   bytesWritten=fwrite(Nbuf, 1, Nidx, Nfile);
   if (bytesWritten!=Nidx)
   {
      E("fwriteNfile %d bytes error %zu",Nidx,bytesWritten);
      return false;
   }
   Nidx=0;
   return true;
}

bool fwriteCfile()
{
   //00 05 S1 S0 I1 I0 FF FF
#pragma mark TODO FF

   //adjust prefixes
   i=0;//loop iterator
   while (i < Cidx)
   {
      Cbuf[i+2]=s1;
      Cbuf[i+3]=s0;
      Cbuf[i+4]=i1;
      Cbuf[i+5]=i0;
      i+= 1 + Cbuf[i];//value length
      i+= 4 + Cbuf[i] + (Cbuf[i+1]<<2) + (Cbuf[i+2]<<4) + (Cbuf[i+3]<<6);//next key length
   }
   bytesWritten=fwrite(Cbuf, 1, Cidx, Cfile);
   if (bytesWritten!=Cidx)
   {
      E("fwriteCfile %d bytes error %zu",Cidx,bytesWritten);
      return false;
   }
   Cidx=0;
   
   return true;
}

bool commitedckv(s16 *siidx)
{
   //https://github.com/jacquesfauquex/DCKV/wiki/eDCKV
   
   if ((Eidx > 8) && !fwriteEfile()) return false;
   if ((Sidx > 8) && !fwriteSfile()) return false;
   if ((Pidx > 8) && !fwritePfile()) return false;
   if ((Iidx > 8) && !fwriteIfile()) return false;
   if ((Nidx > 8) && !fwriteNfile()) return false;
   if ((Iidx > 8) && !fwriteIfile()) return false;

   if (isexplicit)
   {
      if ((Nidx > 8) && !fwriteNfile()) return false;
   }
   else //iscompressed
   {
      if ((Cidx > 8) && !fwriteCfile()) return false;
   }
   return closedckv(siidx);
}


bool closeedckv(s16 *siidx)
{
   fclose(Efile);
   if (Ebuf) free(Ebuf);
   free(Epath);

   fclose(Sfile);
   if (Sbuf) free(Sbuf);
   free(Spath);

   fclose(Pfile);
   if (Pbuf) free(Pbuf);
   free(Ppath);

   fclose(Ifile);
   if (Ibuf) free(Ibuf);
   free(Ipath);

   if (isexplicit)
   {
      fclose(Nfile);
      if (Nbuf) free(Nbuf);
      free(Npath);
   }
   else
   {
      fclose(Cfile);
      if (Cbuf) free(Cbuf);
      free(Cpath);
   }
   
   I("!#%d",*siidx);
   return true;
}



bool appendEXAMkv( //patient and study level attributes
   uint8_t            *kbuf,    //contextualized key value buffer
   u32                kloc,     //offset of current attribute in key
   bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
   enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
   u64                vloc,     //value location in input stream
   u32                vlen,     //value length
   bool               fromStdin,//value to be read, or already read in vbuf
   uint8_t            *vbuf     //buffer for values
)
{
   prefix=0x00;
   if ((vlen + 21 + kloc + Eidx > 0xFFFE) && !fwriteEfile()) return false;//freeing buffer necesary?

   //key length = key path length + 8 prefix + 8 current attribute
   //Eidx increased by 1
   Ebuf[Eidx++]=kloc+16;
   
   //prefix
   memcpy(Ebuf+Eidx, &prefix, 8);
   Eidx+=8;
   //key
   memcpy(Ebuf+Eidx, kbuf, kloc+8);
   Eidx+=kloc+8;
   
   //value length
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
         case kv01://OB OD OF OL OV OW SV UV
         {
            memcpy(Ebuf+Eidx, &vlen, 4);//value length
            Eidx+=4;

            //value
            if (vlen)
            {
               if (fromStdin)
               {
                  if (vlen > 0xFFFE)
                  {
                     u32 remaining=vlen;
                     while (remaining > 0xFFFE)
                     {
                        if (!fwriteEfile()) return false;
                        if (edckvapi_fread(Ebuf+Eidx,1,0xFFFE,stdin)!=0xFFFE) return false;
                        remaining-=0xFFFE;
                     }
                     if (remaining > 0)
                     {
                        if (!fwriteEfile()) return false;
                        if (edckvapi_fread(Ebuf+Eidx,1,remaining,stdin)!=remaining) return false;
                     }
                     if (!fwriteEfile()) return false;
                  }
                  else
                  {
                     if ((vlen + Eidx > 0xFFFE) && !fwriteEfile()) return false;//freeing buffer necesary?
                     if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;
                     Eidx+=vlen;
                  }
               }
               else //from vbuf
               {
                  if ((vlen + Eidx > 0xFFFE) && !fwriteEfile()) return false;//freeing buffer necesary?
                  memcpy(Ebuf+Eidx, vbuf, vlen);
                  Eidx+=vlen;
               }
            }
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
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
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
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
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
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
         case kvIN://InstitutionName
         case kvPN:
         {
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
   return true;
}


bool appendSERIESkv( //series level attributes. We add to this category the instance level attributes SR and encapsulatedCDA
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
)
{
   prefix=0x01;
   if ((vlen + 21 + kloc + Eidx > 0xFFFE) && !fwriteEfile()) return false;//freeing buffer necesary?

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
         case kved://OB Encapsulated​Document 00420011
         case kvfo://OV Extended​Offset​Table fragments offset 7FE00001
         case kvfl://OV Extended​Offset​TableLengths fragments offset 7FE00002
         case kvft://UV Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
         case kv01://OB OD OF OL OV OW SV UV

         case kvUN:
         {
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
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
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
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
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
            vlen2=vlen - (vbuf[vlen - 1] == 0);
            memcpy(Sbuf+Sidx, &vlen2, 4);
            Sidx+=4;
            memcpy(Sbuf+Sidx, vbuf, vlen2);
            Sidx+=vlen2;
         }break;
               
         case kvSm://Modality
         case kvAt://AccessionNumberType
         case kvIi://InstanceNumber
         case kvIa://AcquisitionNumber
         case kvTA:
         {
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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

         case kvIs://SeriesNumber
         {
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
           
           int Is;
           sscanf((char*)vbuf,"%d",&Is);
           s0=Is % 0x100;//static for use in commitdckv series prefix
           s1=Is / 0x100;
         }break;


            
         case kvdn://ST  DocumentTitle 00420010
         case kvHC://HL7InstanceIdentifier
         case kvEi://StudyID
         case kvAn://AccessionNumber
         case kvTS://valores representadas por texto
         case kvPN:
         {
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
   return true;
}


bool appendPRIVATEkv( //odd group and UN attributes
   uint8_t            *kbuf,    //contextualized key value buffer
   u32                kloc,     //offset of current attribute in key
   bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
   enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
   u64                vloc,     //value location in input stream
   u32                vlen,     //value length
   bool               fromStdin,//value to be read, or already read in vbuf
   uint8_t            *vbuf     //buffer for values
)
{
   prefix=0x02;
   if ((vlen + 21 + kloc + Eidx > 0xFFFE) && !fwriteEfile()) return false;//freeing buffer necesary?

   if (vlen==0){
      //value zero length
      Ibuf[Iidx++]=kloc+16;//key size
      memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
      Iidx+=8;
      memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
      Iidx+=kloc+8;
      memcpy(Ibuf+Iidx, &vlen, 4);//value length
      Iidx+=4;
      return true;
   }
   else {//value with contents
      switch (vrcat)
      {
         case kved://OB Encapsulated​Document 00420011
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
             if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
             memcpy(Ibuf+Iidx, &vlen, 4);
             Iidx+=4;
             memcpy(Ibuf+Iidx, vbuf, vlen);
             Iidx+=vlen;
         };break;

         case kv01://not representable
         {
            if ( u32swap(((u32*)kbuf)[0]) == 0x7FE00010)
            {
#pragma mark ·· 0x7FE00001
               if (vlen==0xFFFFFFFF) //fragments OB or OW
               {
#pragma mark ··· fragments
                  if (fromStdin)
                  {
                     u64 fragmentbytes=0;
                     struct t4l4 *fragmentstruct=(struct t4l4*) &fragmentbytes;
                     
                     
                     //fragmento 0
                     if (edckvapi_fread(&fragmentbytes, 1, 8, stdin)!=8) return false;
                     if (fragmentstruct->t != 0xe000fffe) return false;
                     //read (and ignore) neventually existing table
                     if (   (fragmentstruct->l > 0)
                         && (edckvapi_fread(vbuf,1,fragmentstruct->l,stdin) != fragmentstruct->l)
                        ) return false;
                     vloc+=20+fragmentstruct->l;

                     
                     //write iBuffer and reset index
                     fwriteIfile();
                     
                     //read fragment 1
                     if (edckvapi_fread(&fragmentbytes, 1, 8, stdin)!=8) return false;
                     
                     while (fragmentstruct->t != 0xe0ddfffe)
                     {
                        if (fragmentstruct->t != 0xe000fffe) return false;
                        prefix+=0x10000000000;
                        Ibuf[Iidx++]=16;//key size
                        memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
                        Iidx+=8;
                        memcpy(Ibuf+Iidx, kbuf, 8);//copy key
                        Iidx+=8;
                        memcpy(Ibuf+Iidx, &(fragmentstruct->l), 4);//val length
                        Iidx+=8;
                        
                        //write iBuffer and reset index
                        fwriteIfile();
                        
                        D("%08lld %016llx %x %x\n",vloc,u64swap(prefix),fragmentstruct->t,fragmentstruct->l);
                        if (fragmentstruct->l > 0)
                        {
                           size_t bytesremaing=fragmentstruct->l;
                           while ( bytesremaing > 0xFFFD)
                           {
                              if (edckvapi_fread(vbuf,1,0xFFFE,stdin)!=0xFFFE) return false;
                              if (fwrite(vbuf ,1, 0xFFFE , Ifile)!=0xFFFE) return false;
                              bytesremaing-=0xFFFE;
                           }
                           if (bytesremaing > 0)
                           {
                              if (edckvapi_fread(vbuf,1,bytesremaing,stdin)!=bytesremaing) return false;
                              if (fwrite(vbuf ,1, bytesremaing , Ifile)!=bytesremaing) return false;
                           }

                        }
                        vloc+=8+fragmentstruct->l;//174674 en lugar de 172954 (dif 1720)
                        if (edckvapi_fread(&fragmentbytes, 1, 8, stdin)!=8) return false;
                     }
                  }
                  else //already in buffer
                  {
                  }
               }
               else
               {
#pragma mark ··· native

               }
            }
            else
            {
#pragma mark ·· other vl

               Ibuf[Iidx++]=kloc+16;//key size
               memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
               Iidx+=8;
               memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
               Iidx+=kloc+8;
               if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               memcpy(Ibuf+Iidx, &vlen, 4);
               Iidx+=4;
               memcpy(Ibuf+Iidx, vbuf, vlen);
               Iidx+=vlen;
            }
         };break;

            
         case kvUN://not representable
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
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
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
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
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
            vlen2=vlen - (vbuf[vlen - 1] == 0);
            memcpy(Ibuf+Iidx, &vlen2, 4);
            Iidx+=4;
            memcpy(Ibuf+Iidx, vbuf, vlen2);
            Iidx+=vlen2;
         }break;

         case kvSm://Modality
         case kvAt://AccessionNumberType
         case kvIs://SeriesNumber
         case kvIa://AcquisitionNumber
         case kvTA:
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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

         case kvIi://InstanceNumber needed for prefix
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
           int Ii;
           sscanf((char*)vbuf,"%d",&Ii);
           i0=Ii % 0x100;
           i1=Ii / 0x100;
         }break;

         case kvdn://ST  DocumentTitle 00420010
         case kvHC://HL7InstanceIdentifier
         case kvEi://StudyID
         case kvAn://AccessionNumber
         case kvTS://valores representadas por texto
            
         case kvPN:
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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

   return true;}


bool appendDEFAULTkv( //any other instance level attribute
   uint8_t            *kbuf,    //contextualized key value buffer
   u32                kloc,     //offset of current attribute in key
   bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
   enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
   u64                vloc,     //value location in input stream
   u32                vlen,     //value length
   bool               fromStdin,//value to be read, or already read in vbuf
   uint8_t            *vbuf     //buffer for values
)
{
   prefix=0x03;
   if ((vlen + 21 + kloc + Eidx > 0xFFFE) && !fwriteEfile()) return false;//freeing buffer necesary?

   if (vlen==0){
      //value zero length
      Ibuf[Iidx++]=kloc+16;//key size
      memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
      Iidx+=8;
      memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
      Iidx+=kloc+8;
      memcpy(Ibuf+Iidx, &vlen, 4);//value length
      Iidx+=4;
      return true;
   }
   else {//value with contents
      switch (vrcat)
      {
         case kved://OB Encapsulated​Document 00420011
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
             if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
             memcpy(Ibuf+Iidx, &vlen, 4);
             Iidx+=4;
             memcpy(Ibuf+Iidx, vbuf, vlen);
             Iidx+=vlen;
         };break;

         case kv01://not representable
         {
            if ( u32swap(((u32*)kbuf)[0]) == 0x7FE00010)
            {
#pragma mark ·· 0x7FE00001
               if (vlen==0xFFFFFFFF) //fragments OB or OW
               {
#pragma mark ··· fragments
                  if (fromStdin)
                  {
                     u64 fragmentbytes=0;
                     struct t4l4 *fragmentstruct=(struct t4l4*) &fragmentbytes;
                     
                     
                     //fragmento 0
                     if (edckvapi_fread(&fragmentbytes, 1, 8, stdin)!=8) return false;
                     if (fragmentstruct->t != 0xe000fffe) return false;
                     //read (and ignore) neventually existing table
                     if (   (fragmentstruct->l > 0)
                         && (edckvapi_fread(vbuf,1,fragmentstruct->l,stdin) != fragmentstruct->l)
                        ) return false;
                     vloc+=20+fragmentstruct->l;

                     
                     //write iBuffer and reset index
                     fwriteIfile();
                     
                     //read fragment 1
                     if (edckvapi_fread(&fragmentbytes, 1, 8, stdin)!=8) return false;
                     
                     while (fragmentstruct->t != 0xe0ddfffe)
                     {
                        if (fragmentstruct->t != 0xe000fffe) return false;
                        prefix+=0x10000000000;
                        Ibuf[Iidx++]=16;//key size
                        memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
                        Iidx+=8;
                        memcpy(Ibuf+Iidx, kbuf, 8);//copy key
                        Iidx+=8;
                        memcpy(Ibuf+Iidx, &(fragmentstruct->l), 4);//val length
                        Iidx+=8;
                        
                        //write iBuffer and reset index
                        fwriteIfile();
                        
                        D("%08lld %016llx %x %x\n",vloc,u64swap(prefix),fragmentstruct->t,fragmentstruct->l);
                        if (fragmentstruct->l > 0)
                        {
                           size_t bytesremaing=fragmentstruct->l;
                           while ( bytesremaing > 0xFFFD)
                           {
                              if (edckvapi_fread(vbuf,1,0xFFFE,stdin)!=0xFFFE) return false;
                              if (fwrite(vbuf ,1, 0xFFFE , Ifile)!=0xFFFE) return false;
                              bytesremaing-=0xFFFE;
                           }
                           if (bytesremaing > 0)
                           {
                              if (edckvapi_fread(vbuf,1,bytesremaing,stdin)!=bytesremaing) return false;
                              if (fwrite(vbuf ,1, bytesremaing , Ifile)!=bytesremaing) return false;
                           }

                        }
                        vloc+=8+fragmentstruct->l;//174674 en lugar de 172954 (dif 1720)
                        if (edckvapi_fread(&fragmentbytes, 1, 8, stdin)!=8) return false;
                     }
                  }
                  else //already in buffer
                  {
                  }
               }
               else
               {
#pragma mark ··· native

               }
            }
            else
            {
#pragma mark ·· other vl

               Ibuf[Iidx++]=kloc+16;//key size
               memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
               Iidx+=8;
               memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
               Iidx+=kloc+8;
               if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
               memcpy(Ibuf+Iidx, &vlen, 4);
               Iidx+=4;
               memcpy(Ibuf+Iidx, vbuf, vlen);
               Iidx+=vlen;
            }
         };break;

            
         case kvUN://not representable
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
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
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
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
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
            vlen2=vlen - (vbuf[vlen - 1] == 0);
            memcpy(Ibuf+Iidx, &vlen2, 4);
            Iidx+=4;
            memcpy(Ibuf+Iidx, vbuf, vlen2);
            Iidx+=vlen2;
         }break;

         case kvSm://Modality
         case kvAt://AccessionNumberType
         case kvIs://SeriesNumber
         case kvIa://AcquisitionNumber
         case kvTA:
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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

         case kvIi://InstanceNumber needed for prefix
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
           int Ii;
           sscanf((char*)vbuf,"%d",&Ii);
           i0=Ii % 0x100;
           i1=Ii / 0x100;
         }break;

         case kvdn://ST  DocumentTitle 00420010
         case kvHC://HL7InstanceIdentifier
         case kvEi://StudyID
         case kvAn://AccessionNumber
         case kvTS://valores representadas por texto
            
         case kvPN:
         {
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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
            Ibuf[Iidx++]=kloc+16;//key size
            memcpy(Ibuf+Iidx, &prefix, 8);//copy prefix
            Iidx+=8;
            memcpy(Ibuf+Iidx, kbuf, kloc+8);//copy key
            Iidx+=kloc+8;
           if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
           
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

   return true;
}


bool appendNATIVEkv(
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
)
{
   prefix=0x04;
   if ((vlen + 21 + kloc + Nidx > 0xFFFE) && !fwriteEfile()) return false;//freeing buffer necesary?
   Ibuf[Iidx++]=kloc+16;//key size
   memcpy(Nbuf+Nidx, &prefix, 8);//copy prefix
   Nidx+=8;
   memcpy(Nbuf+Nidx, kbuf, kloc+8);//copy key
   Nidx+=kloc+8;
   if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
   
   //remove eventual 0x00 at the end of UID
   vlen2=vlen - (vbuf[vlen - 1] == 0);
   memcpy(Nbuf+Nidx, &vlen2, 4);
   Nidx+=4;
   memcpy(Nbuf+Nidx, vbuf, vlen2);
   Nidx+=vlen2;
   return true;
}
bool appendNATIVE01(
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
)
{
   prefix=0x04;
   memcpy(Nbuf+Nidx, &vlen, 4);//value length
   Nidx+=4;

   //value
   if (vlen)
   {
      if (fromStdin)
      {
         if (vlen > 0xFFFE)
         {
            u32 remaining=vlen;
            while (remaining > 0xFFFE)
            {
               if (!fwriteNfile()) return false;
               if (edckvapi_fread(Nbuf+Nidx,1,0xFFFE,stdin)!=0xFFFE) return false;
               remaining-=0xFFFE;
            }
            if (remaining > 0)
            {
               if (!fwriteNfile()) return false;
               if (edckvapi_fread(Nbuf+Nidx,1,remaining,stdin)!=remaining) return false;
            }
            if (!fwriteNfile()) return false;
         }
         else
         {
            if ((vlen + Nidx > 0xFFFE) && !fwriteNfile()) return false;//freeing buffer necesary?
            if (edckvapi_fread(Nbuf+Nidx,1,vlen,stdin)!=vlen) return false;
            Nidx+=vlen;
         }
      }
      else //from vbuf
      {
         if ((vlen + Nidx > 0xFFFE) && !fwriteNfile()) return false;//freeing buffer necesary?
         memcpy(Nbuf+Nidx, vbuf, vlen);
         Nidx+=vlen;
      }
   }

   return true;
}

bool appendCOMPRESSEDkv(
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
)
{
   prefix=0x05;
   if ((vlen + 21 + kloc + Cidx > 0xFFFE) && !fwriteCfile()) return false;//freeing buffer necesary?
   Cbuf[Cidx++]=kloc+16;//key size
   memcpy(Cbuf+Cidx, &prefix, 8);//copy prefix
   Cidx+=8;
   memcpy(Cbuf+Cidx, kbuf, kloc+8);//copy key
   Cidx+=kloc+8;
   if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
   
   //remove eventual 0x00 at the end of UID
   vlen2=vlen - (vbuf[vlen - 1] == 0);
   memcpy(Cbuf+Cidx, &vlen2, 4);
   Cidx+=4;
   memcpy(Cbuf+Cidx, vbuf, vlen2);
   Cidx+=vlen2;
   return true;

}
bool appendCOMPRESSED01(
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
)
{
   prefix=0x05;
   memcpy(Cbuf+Cidx, &vlen, 4);//value length
   Cidx+=4;

   //value
   if (vlen)
   {
      if (fromStdin)
      {
         if (vlen > 0xFFFE)
         {
            u32 remaining=vlen;
            while (remaining > 0xFFFE)
            {
               if (!fwriteCfile()) return false;
               if (edckvapi_fread(Cbuf+Cidx,1,0xFFFE,stdin)!=0xFFFE) return false;
               remaining-=0xFFFE;
            }
            if (remaining > 0)
            {
               if (!fwriteCfile()) return false;
               if (edckvapi_fread(Cbuf+Cidx,1,remaining,stdin)!=remaining) return false;
            }
            if (!fwriteCfile()) return false;
         }
         else
         {
            if ((vlen + Cidx > 0xFFFE) && !fwriteCfile()) return false;//freeing buffer necesary?
            if (edckvapi_fread(Cbuf+Cidx,1,vlen,stdin)!=vlen) return false;
            Cidx+=vlen;
         }
      }
      else //from vbuf
      {
         if ((vlen + Cidx > 0xFFFE) && !fwriteCfile()) return false;//freeing buffer necesary?
         memcpy(Cbuf+Cidx, vbuf, vlen);
         Cidx+=vlen;
      }
   }

   return true;
}
