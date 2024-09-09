// project: dicm2dckv
// targets: eDCKVinline
// file: dckvapi.m
// created by jacquesfauquex on 2024-04-04.

#include "edckvapi.h"


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
static u32 Eidx=0;
static u32 Sidx=0;
static u32 Iidx=0;
static char *Epath;
static char *Spath;
static char *Ipath;
FILE *Efile;
FILE *Sfile;
FILE *Ifile;

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
   c0=*soidx % 0x100;//static for use in commitdckv series prefix
   c1=*soidx / 0x100;

   //files contents buffer
   Ebuf=malloc(0xFFFF);
   Sbuf=malloc(0xFFFF);
   Ibuf=malloc(0xFFFF);

   //SOPInstanceUID
   char *sopibuf = malloc(*silen);
   memcpy(sopibuf, vbuf+*siloc+8, *silen);

   //files path and opening
   Epath=malloc(0xFF);
   Spath=malloc(0xFF);
   Ipath=malloc(0xFF);

   strcat(Epath,dstdir);
   strcat(Epath, "/");
   strcat(Epath,sopibuf);
   strcat(Epath, ".ekv");
   Efile=fopen(Epath, "w");

   strcat(Spath,dstdir);
   strcat(Spath, "/");
   strcat(Spath,sopibuf);
   strcat(Spath, ".skv");
   Sfile=fopen(Spath, "w");

   strcat(Ipath,dstdir);
   strcat(Ipath, "/");
   strcat(Ipath,sopibuf);
   strcat(Ipath, ".ikv");
   Ifile=fopen(Ipath, "w");

   free(sopibuf);
   (*siidx)++;
   I("#%d",*siidx);
   return true;
}

bool fwriteIfile()
{
   if (Iidx > 0)
   {
      //00 03 SS SS II II FF FF
      //replace in Ibuf (0..Iidx):
      //SSSS by s0 y s1
      //IIII by i0 y i1
      u16 i=0;//loop iterator
      while (i < Iidx)
      {
         Ibuf[i+2]=s1;
         Ibuf[i+3]=s0;
         Ibuf[i+4]=i1;
         Ibuf[i+5]=i0;
         i+= 1 + Ibuf[i];//value length
         i+= 4 + Ibuf[i] + (Ibuf[i+1]*0x100) + (Ibuf[i+2]*0x10000) + (Sbuf[i+3]*0x1000000);//next key length
      }
      bytesWritten=fwrite(Ibuf, 1, Iidx, Ifile);
      Iidx=0;
   }
   return true;
}

bool commitedckv(s16 *siidx)
{
   //https://github.com/jacquesfauquex/DCKV/wiki/eDCKV
   
#pragma mark fwrite patient study
   bytesWritten=fwrite(Ebuf, 1, Eidx, Efile);
   
   
#pragma mark fwrite series
   //00 01 SS SS CC CC RR RR
   //replace in Sbuf (0..Sidx):
   //SSSS by Is
   //CCCC by CCCC
   u16 i=0;//loop iterator
   while (i < Sidx)
   {
      Sbuf[i+2]=s1;
      Sbuf[i+3]=s0;
      Sbuf[i+4]=c1;
      Sbuf[i+5]=c0;
      i+= 1 + Sbuf[i];//value length
      i+= 4 + Sbuf[i] + (Sbuf[i+1]*0x100) + (Sbuf[i+2]*0x10000) + (Sbuf[i+3]*0x1000000);//next key length
   }
   bytesWritten=fwrite(Sbuf, 1, Sidx, Sfile);

#pragma mark fwrite instance
   fwriteIfile();
   
   return closedckv(siidx);
}


bool closeedckv(s16 *siidx)
{
   fclose(Efile);
   fclose(Sfile);
   fclose(Ifile);
   if (Ebuf) free(Ebuf);
   if (Sbuf) free(Sbuf);
   if (Ibuf) free(Ibuf);
   free(Epath);
   free(Spath);
   free(Ipath);

   I("!#%d",*siidx);
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
   return edckvapi_fread(vbuf,1,vlen,stdin);
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
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
            memcpy(Ebuf+Eidx, &vlen, 4);
            Eidx+=4;
            memcpy(Ebuf+Eidx, vbuf, vlen);
            Eidx+=vlen;
         };break;

         case kvUN:
         {
            if (fromStdin && vlen && (edckvapi_fread(vbuf,1,vlen,stdin)!=vlen)) return false;
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
           sscanf(vbuf,"%d",&Is);
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



bool appendNATIVEkv( //bitmap representatio 7FE0,0010 and 0002,0010="1.2.840.10008.1.2.1"
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
   return true;
}



bool appendCOMPRESSEDkv( //bitmap representatio 7FE0,0010 any other  0002,0010
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
   return true;
}



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
         case kvAl://AccessionNumberIssuer local 00080051.00400031
         case kvAu://AccessionNumberIssuer universal 00080051.00400032
         case kved://OB Encapsulated​Document 00420011
         case kvfo://OV Extended​Offset​Table fragments offset 7FE00001
         case kvfl://OV Extended​Offset​TableLengths fragments offset 7FE00002
         case kvft://UV Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
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
           sscanf(vbuf,"%d",&Ii);
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
