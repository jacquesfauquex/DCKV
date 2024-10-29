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
static bool isexplicit;

static unsigned long vlen2;

//static size_t bytesWritten;

const char *space=" ";
const char *backslash = "\\";

//buffers ...
//start with 0xFFFF size
//and is reallocated with same increment in size each time the current space is filled up
static u8 *Ebuf;
static u8 *Sbuf;
static u8 *Ibuf;
static u8 *Pbuf;
static u8 *Nbuf;
static u8 *Cbuf;
static u32 Eidx=0;
static u32 Sidx=0;
static u32 Iidx=0;
static u32 Pidx=0;
static u32 Nidx=0;
static u32 Cidx=0;
static u32 Emax=0;
static u32 Smax=0;
static u32 Imax=0;
static u32 Pmax=0;
static u32 Nmax=0;
static u32 Cmax=0;

static char pid[20];
static u8   pidlength;//int pid=getpid()
static char edate[8];//when study date is known (or current date if empty)
static unsigned char euidb64[44];
static u8   euidb64length;
static u64 prefix=0x00;
static blake3_hasher hasher;
static uint8_t hashbytes[BLAKE3_OUT_LEN];//32 bytes

struct stat st={0};//for directory creation
static char dirpath[256];
static u8   dirpathlength;
static char filepath[330];
static FILE *fileptr;

//prefix components
static u64 ps16=0;
static u64 pv8=0;
static u64 pi16=0;
static u64 pr16=0;
static u64 pc16=0;
static u64 pf16=0;


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
   isexplicit=*stidx==2;

   //static sopclass idx
   pc16=*soidx;

   //static base/pid path
   strcat(dirpath,dstdir);
   dirpathlength=strlen(dstdir);
   if (dirpath[dirpathlength-1]!='/') dirpath[dirpathlength++]='/';
   dirpath[dirpathlength]=0x00;
   if ((stat(dirpath, &st)==-1) && (mkdir(dirpath, 0777)==-1)) return false;
   sprintf(pid, "%d", getpid());
   pidlength=strlen(pid);
   if (pidlength==0) return false;
   memcpy(dirpath+dirpathlength, pid, pidlength);
   dirpathlength+=pidlength;
   dirpath[dirpathlength++]='/';
   dirpath[dirpathlength]=0x00;
   if ((stat(dirpath, &st)==-1) && (mkdir(dirpath, 0777)==-1)) return false;
   
   //resets
   euidb64length=0;
   
   //edate
   time_t rawtime;
   time ( &rawtime );
   struct tm * timeinfo;
   timeinfo = localtime ( &rawtime );
   strftime(edate,8,"%Y%m%d", timeinfo);
   
   //buffers init
   if (Emax==0) {
      Emax=0xFFFF;
      Ebuf=malloc(Emax);
   }
   Eidx=0;
   
   if (Smax==0) {
      Smax=0xFFFF;
      Sbuf=malloc(Smax);
   }
   Sidx=0;

   if (Pmax==0) {
      Pmax=0xFFFF;
      Pbuf=malloc(Pmax);
   }
   Pidx=0;

   if (Imax==0) {
      Imax=0xFFFF;
      Ibuf=malloc(Imax);
   }
   Iidx=0;
   
   if (isexplicit)
   {
      if (Nmax==0) {
         Nmax=0x3000000;
         Nbuf=malloc(Nmax);
      }
      Nidx=0;
   }
   else
   {
      if (Cmax==0) {
         Cmax=0x6000000;
         Cbuf=malloc(Cmax);
      }
      Cidx=0;
   }

   (*siidx)++;
   I("#%d",*siidx);
   return true;
}

#pragma mark -



bool morebuf(enum kvfamily f, u32 vlen)
{
   u8 *newbuf;
   switch (f) {
      case kvE:{
         if (vlen > 0xFF00) {
            newbuf=realloc(Ebuf,Emax+vlen+0x0FFF);
            if (newbuf == NULL) return false;
            Emax+=vlen+0x0FFF;
         } else {
            newbuf=realloc(Ebuf,Emax+0xFFFF);
            if (newbuf == NULL) return false;
            Emax+=0xFFFF;
         }
         Ebuf=newbuf;
      } break;
      case kvS:{
         if (vlen > 0xFF00) {
            newbuf=realloc(Sbuf,Smax+vlen+0x0FFF);
            if (newbuf == NULL) return false;
            Smax+=vlen+0x0FFF;
         } else {
            newbuf=realloc(Sbuf,Smax+0xFFFF);
            if (newbuf == NULL) return false;
            Smax+=0xFFFF;
         }
         Sbuf=newbuf;
      } break;
      case kvP:{
         if (vlen > 0xFF00) {
            newbuf=realloc(Pbuf,Pmax+vlen+0x0FFF);
            if (newbuf == NULL) return false;
            Pmax+=vlen+0x0FFF;
         } else {
            newbuf=realloc(Pbuf,Pmax+0xFFFF);
            if (newbuf == NULL) return false;
            Pmax+=0xFFFF;
         }
         Pbuf=newbuf;
      } break;
      case kvI:{
         if (vlen > 0xFF00) {
            newbuf=realloc(Ibuf,Imax+vlen+0x0FFF);
            if (newbuf == NULL) return false;
            Imax+=vlen+0x0FFF;
         } else {
            newbuf=realloc(Ibuf,Imax+0xFFFF);
            if (newbuf == NULL) return false;
            Imax+=0xFFFF;
         }
         Ibuf=newbuf;
      } break;
      case kvN:{
         if (vlen > 0xFF00) {
            newbuf=realloc(Nbuf,Nmax+vlen+0x00FF);
            if (newbuf == NULL) return false;
            Nmax+=vlen+0x00FF;
         } else {
            newbuf=realloc(Nbuf,Nmax+0xFFFF);
            if (newbuf == NULL) return false;
            Nmax+=0xFFFF;
         }
         Nbuf=newbuf;
      } break;
      case kvC:{
         if (vlen > 0xFF00) {
            newbuf=realloc(Cbuf,Cmax+vlen+0x00FF);
            if (newbuf == NULL) return false;
            Cmax+=vlen+0x00FF;
         } else {
            newbuf=realloc(Cbuf,Cmax+0xFFFF);
            if (newbuf == NULL) return false;
            Cmax+=0xFFFF;
         }
         Cbuf=newbuf;
      } break;
      default:
         return false;
         break;
   }
   return true;
}



#pragma mark -

bool commitedckv(s16 *siidx)
{
   memcpy(dirpath+dirpathlength, edate, 8);
   dirpathlength+=8;
   dirpath[dirpathlength++]='/';
   dirpath[dirpathlength]=0x00;
   if ((stat(dirpath, &st)==-1) && (mkdir(dirpath, 0777)==-1)) return false;

   //.euidb64
   if (euidb64length==0) return false;
   //dirpath[dirpathlength++]='.';
   memcpy(dirpath+dirpathlength, euidb64, euidb64length);
   dirpathlength+=euidb64length;
   dirpath[dirpathlength++]='/';
   dirpath[dirpathlength]=0x00;
   if ((stat(dirpath, &st)==-1) && (mkdir(dirpath, 0777)==-1)) return false;
   memcpy(filepath,dirpath,dirpathlength);
   size_t i;
   int j;
   
   

   
   if (Eidx>0)
   {
      // /studydate/.studyiuid/0000000000000000.
      snprintf(filepath+dirpathlength,17,"0000000000000000");
      filepath[dirpathlength+16]='.';
      //add blake3 to final name
      blake3_hasher_reset(&hasher);
      blake3_hasher_update(&hasher, Ebuf, Eidx);
      blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);
      for (i = 0; i < BLAKE3_OUT_LEN; i++) {
         sprintf(filepath+dirpathlength+17+i+i ,"%02x", hashbytes[i]);
      }
      filepath[dirpathlength+17+i+i]=0x00;
      D("%s\n%s\n",dirpath,filepath);
      fileptr=fopen(filepath, "w");
      if (fileptr == NULL) return false;
      if (fwrite(Ebuf ,1, Eidx , fileptr)!=Eidx) return false;
      fclose(fileptr);
      Eidx=0;
   }
   
   if (Sidx>0)
   {
      //01SSSS00RRRRCCCC
      //replace prefix in buffer
      j=0;
      prefix=1|u16swap(ps16)*0x100|u16swap(pr16)*0x100000000|u16swap(pc16)*0x1000000000000;
      while (j<Sidx)
      {
         memcpy(Sbuf+j+1,&prefix,8);
         j+=Sbuf[j]+1;
         j+=Sbuf[j]+(Sbuf[j+1]*0x10)+(Sbuf[j+2]*0x100)+(Sbuf[j+3]*0x1000)+4;
      }
      // /studydate/.studyiuid/01SSSS00RRRRCCCC.
      snprintf(filepath+dirpathlength,17,"01%04llX00%04llX%04llX",ps16,pr16,pc16);
      filepath[dirpathlength+16]='.';
      //add blake3 to final name
      blake3_hasher_reset(&hasher);
      blake3_hasher_update(&hasher, Sbuf, Sidx);
      blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);
      for (i = 0; i < BLAKE3_OUT_LEN; i++) {
         sprintf(filepath+dirpathlength+17+i+i ,"%02x", hashbytes[i]);
      }
      filepath[dirpathlength+17+i+i]=0x00;
      D("%s\n%s\n",dirpath,filepath);
      fileptr=fopen(filepath, "w");
      if (fileptr == NULL) return false;
      if (fwrite(Sbuf ,1, Sidx , fileptr)!=Sidx) return false;
      fclose(fileptr);
      
      //reset
      Sidx=0;
   }
   
   if (Pidx>0)
   {
      //02SSSSVVIIIICCCC
      //replace prefix in buffer
      j=0;
      prefix=2|u16swap(ps16)*0x100|pv8*0x1000000|u16swap(pi16)*0x100000000|u16swap(pc16)*0x1000000000000;
      while (j<Pidx)
      {
         memcpy(Pbuf+j+1,&prefix,8);
         j+=Pbuf[j]+1;
         j+=Pbuf[j]+(Pbuf[j+1]*0x10)+(Pbuf[j+2]*0x100)+(Pbuf[j+3]*0x1000)+4;
      }

      // /studydate/.studyiuid/02SSSSVVIIIICCCC.
      snprintf(filepath+dirpathlength,17, "02%04llX%02llX%04llX%04llX",ps16,pv8,pi16,pc16);
      filepath[dirpathlength+16]='.';
      //add blake3 to final name
      blake3_hasher_reset(&hasher);
      blake3_hasher_update(&hasher, Pbuf, Pidx);
      blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);
      for (i = 0; i < BLAKE3_OUT_LEN; i++) {
         sprintf(filepath+dirpathlength+17+i+i ,"%02x", hashbytes[i]);
      }
      filepath[dirpathlength+17+i+i]=0x00;
      D("%s\n%s\n",dirpath,filepath);
      fileptr=fopen(filepath, "w");
      if (fileptr == NULL) return false;
      if (fwrite(Pbuf ,1, Pidx , fileptr)!=Pidx) return false;
      fclose(fileptr);
      
      //reset
      Pidx=0;
   }
   
   if (Iidx>0)
   {
      //03SSSSVVIIII0000
      //replace prefix in buffer
      j=0;
      prefix=3|u16swap(ps16)*0x100|pv8*0x1000000|u16swap(pi16)*0x100000000;
      while (j<Iidx)
      {
         memcpy(Ibuf+j+1,&prefix,8);
         j+=Ibuf[j]+1;
         j+=Ibuf[j]+(Ibuf[j+1]*0x10)+(Ibuf[j+2]*0x100)+(Ibuf[j+3]*0x1000)+4;
      }
      // /studydate/.studyiuid/03SSSSVVIIII0000.
      snprintf(filepath+dirpathlength,17, "03%04llX%02llX%04llX0000",ps16,pv8,pi16);
      filepath[dirpathlength+16]='.';
      //add blake3 to final name
      blake3_hasher_reset(&hasher);
      blake3_hasher_update(&hasher, Sbuf, Sidx);
      blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);
      for (i = 0; i < BLAKE3_OUT_LEN; i++) {
         sprintf(filepath+dirpathlength+17+i+i ,"%02x", hashbytes[i]);
      }
      filepath[dirpathlength+17+i+i]=0x00;
      D("%s\n%s\n",dirpath,filepath);
      fileptr=fopen(filepath, "w");
      if (fileptr == NULL) return false;
      if (fwrite(Ibuf ,1, Iidx , fileptr)!=Iidx) return false;
      fclose(fileptr);
      
      //reset
      Iidx=0;
   }
   
   if (isexplicit)
   {
      if (Nidx>0)
      {
         //04SSSSVVIIII0000
         //replace prefix in buffer
         j=0;
         prefix=4|u16swap(ps16)*0x100|pv8*0x1000000|u16swap(pi16)*0x100000000;
         while (j<Nidx)
         {
            memcpy(Nbuf+j+1,&prefix,8);
            j+=Nbuf[j]+1;
            j+=Nbuf[j]+(Nbuf[j+1]*0x10)+(Nbuf[j+2]*0x100)+(Nbuf[j+3]*0x1000)+4;
         }
         // /studydate/.studyiuid/04SSSSVVIIII0000.
         snprintf(filepath+dirpathlength,17, "04%04llX%02llX%04llX0000",ps16,pv8,pi16);
         filepath[dirpathlength+16]='.';
         //add blake3 to final name
         blake3_hasher_reset(&hasher);
         blake3_hasher_update(&hasher, Nbuf, Nidx);
         blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);
         for (i = 0; i < BLAKE3_OUT_LEN; i++) {
            sprintf(filepath+dirpathlength+17+i+i ,"%02x", hashbytes[i]);
         }
         filepath[dirpathlength+17+i+i]=0x00;
         D("%s\n%s\n",dirpath,filepath);
         fileptr=fopen(filepath, "w");
         if (fileptr == NULL) return false;
         if (fwrite(Nbuf ,1, Nidx , fileptr)!=Nidx) return false;
         fclose(fileptr);
         
         //reset
         Nidx=0;
      }
   }
   else //iscompressed
   {
      if (Cidx>0)
      {
         //05SSSSVVIIIIFFFF
         //replace prefix in buffer
         j=0;
         prefix=5|u16swap(ps16)*0x100|pv8*0x1000000|u16swap(pi16)*0x100000000|u16swap(pf16)*0x1000000000000;
         while (j<Cidx)
         {
            memcpy(Cbuf+j+1,&prefix,8);
            j+=Cbuf[j]+1;
            j+=Cbuf[j]+(Cbuf[j+1]*0x10)+(Cbuf[j+2]*0x100)+(Cbuf[j+3]*0x1000)+4;
         }
         // /studydate/.studyiuid/05SSSSVVIIIIFFFF.
         snprintf(filepath+dirpathlength,17, "05%04llX%02llX%04llX%04llX",ps16,pv8,pi16,pf16);
         filepath[dirpathlength+16]='.';
         //add blake3 to final name
         blake3_hasher_reset(&hasher);
         blake3_hasher_update(&hasher, Cbuf, Cidx);
         blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);
         for (i = 0; i < BLAKE3_OUT_LEN; i++) {
            sprintf(filepath+dirpathlength+17+i+i ,"%02x", hashbytes[i]);
         }
         filepath[dirpathlength+17+i+i]=0x00;
         D("%s\n%s\n",dirpath,filepath);
         fileptr=fopen(filepath, "w");
         if (fileptr == NULL) return false;
         if (fwrite(Cbuf ,1, Cidx , fileptr)!=Cidx) return false;
         fclose(fileptr);
         
         //reset
         Cidx=0;
      }
   }
   return closedckv(siidx);
}


bool closeedckv(s16 *siidx)
{
   I("!#%d",*siidx);
   return true;
}


#pragma mark -


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
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Eidx > Emax) && !morebuf(kvE,vlen)) return false;

   //key length = key path length + 8 prefix + 8 current attribute
   //idx increased by 1
   Ebuf[Eidx++]=kloc+16;
   
   //prefix
   memcpy(Ebuf+Eidx, &prefix, 8);
   Eidx+=8;
   
   //key
   memcpy(Ebuf+Eidx, kbuf, kloc+8);
   Eidx+=kloc+8;
   
   //value length
   memcpy(Ebuf+Eidx, &vlen, 4);
   Eidx+=4;
   if (vlen==0) return true;
   //value with contents
   switch (vrcat)
   {
#pragma mark generic
      case kvUI://unique ID
      {
         if (fromStdin){if(edckvapi_fread(vbuf,1,vlen,stdin)!=vlen) return false;}
         vlen2=  vlen - (vbuf[vlen+3] == 0);
         memcpy(Ebuf+Eidx-4, &vlen2, 4);
         memcpy(Ebuf+Eidx, vbuf, vlen2);
         Eidx+=vlen2;
      }break;
         
      case kvFD://floating point double
      case kvFL://floating point single
      case kvSL://signed long
      case kvSS://signed short
      case kvUL://unsigned long
      case kvUS://unsigned short
      case kvAT://attribute tag, 2 u16 hexa
      case kvTP://AS DT TM DA 11 text short ascii pair length
      case kvTA://AE DS IS CS 13 text short ascii
      case kvTS://LO LT SH ST 19 text short charset
      case kvTL://UC UT 25 text long charset
      case kvTU://url encoded
      case kvPN://person name
      //   kvUN only private
      case kv01://OB OD OF OL OV OW SV UV
#pragma mark special
      case kvIN://InstitutionName (placed in exam instead of series)
      case kvEi://StudyID
      case kvAt://AccessionNumberType
      case kvAn://AccessionNumber
      case kvAl://AccessionNumberIssuer local 00080051.00400031
      case kvAu://AccessionNumberIssuer universal 00080051.00400032
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         Eidx+=vlen;
      };break;

      case kvEd://StudyDate
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         //retain study date
         memcpy(edate, Ebuf+Eidx, vlen);
         Eidx+=vlen;
      };break;


      case kvIE://StudyInstanceUID
      {
         if (fromStdin){if(edckvapi_fread(vbuf+4,1,vlen,stdin)!=vlen) return false;}
         vlen2=  vlen - (vbuf[vlen+3] == 0);
         memcpy(Ebuf+Eidx-4, &vlen2, 4);
         memcpy(Ebuf+Eidx, vbuf, vlen2);
         Eidx+=vlen2;
         //retain study uid
         if (!ui2b64( vbuf, vlen2, euidb64, &euidb64length )) return false;
      }break;
         
      default: return false;
   }
   return true;
}

bool appendSERIESkv(//seWe add to this category instance level attributes SR and encapsulatedCDA
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
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Sidx > Smax) && !morebuf(kvS,vlen)) return false;

   //key length = key path length + 8 prefix + 8 current attribute
   //idx increased by 1
   Sbuf[Sidx++]=kloc+16;

   //prefix
   memcpy(Sbuf+Sidx, &prefix, 8);
   Sidx+=8;
   
   //key
   memcpy(Sbuf+Sidx, kbuf, kloc+8);
   Sidx+=kloc+8;
   
   //value length
   memcpy(Sbuf+Sidx, &vlen, 4);
   Sidx+=4;
   if (vlen==0) return true;
   //value with contents
   switch (vrcat)
   {
#pragma mark generic
      case kvFD://floating point double
      case kvFL://floating point single
      case kvSL://signed long
      case kvSS://signed short
      case kvUL://unsigned long
      case kvUS://unsigned short
      case kvAT://attribute tag, 2 u16 hexa
      case kvTP://AS DT TM DA 11 text short ascii pair length
      case kvTA://AE DS IS CS 13 text short ascii
      case kvTS://LO LT SH ST 19 text short charset
      case kvTL://UC UT 25 text long charset
      case kvTU://url encoded
      case kvPN://person name
      //   kvUN only private
      case kv01://OB OD OF OL OV OW SV UV
#pragma mark special
      case kvdn://ST  DocumentTitle 00420010
      case kvHC://HL7InstanceIdentifier
      case kvSm://Modality
      {
         if (fromStdin){if(edckvapi_fread(Sbuf+Sidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Sbuf+Sidx, vbuf, vlen);//from vbuf
         Sidx+=vlen;
      };break;

      case kvUI://unique ID
      case kved://OB Encapsulated​Document 00420011
      case kvIS://SeriesInstanceUID
      {
         if (fromStdin){if(edckvapi_fread(vbuf,1,vlen,stdin)!=vlen) return false;}
         vlen2=  vlen - (vbuf[vlen+3] == 0);
         memcpy(Sbuf+Sidx-4, &vlen2, 4);
         memcpy(Sbuf+Sidx, vbuf, vlen2);
         Sidx+=vlen2;
      }break;

      case kvIs://SeriesNumber
      {
         if (fromStdin){if(edckvapi_fread(Sbuf+Sidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Sbuf+Sidx, vbuf, vlen);//from vbuf
         Sidx+=vlen;

         //retain series number
         s16 Shd;
         sscanf((char*)vbuf,"%hd",&Shd);
         ps16=Shd+0x8000;
      }break;

      default: return false;
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
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Pidx > Pmax) && !morebuf(kvP,vlen)) return false;

   //key length = key path length + 8 prefix + 8 current attribute
   //idx increased by 1
   Pbuf[Pidx++]=kloc+16;
   
   //prefix
   memcpy(Pbuf+Pidx, &prefix, 8);
   Pidx+=8;
   
   //key
   memcpy(Pbuf+Pidx, kbuf, kloc+8);
   Pidx+=kloc+8;
   
   //value length
   memcpy(Pbuf+Pidx, &vlen, 4);
   Pidx+=4;
   if (vlen==0) return true;
   //value with contents
   switch (vrcat)
   {
#pragma mark generic
      case kvFD://floating point double
      case kvFL://floating point single
      case kvSL://signed long
      case kvSS://signed short
      case kvUL://unsigned long
      case kvUS://unsigned short
      case kvAT://attribute tag, 2 u16 hexa
      case kvTP://AS DT TM DA 11 text short ascii pair length
      case kvTA://AE DS IS CS 13 text short ascii
      case kvTS://LO LT SH ST 19 text short charset
      case kvTL://UC UT 25 text long charset
      case kvTU://url encoded
      case kvPN://person name
      //   kvUN only private
      case kv01://OB OD OF OL OV OW SV UV
      {
         if (fromStdin){if(edckvapi_fread(Pbuf+Pidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Pbuf+Pidx, vbuf, vlen);//from vbuf
         Pidx+=vlen;
      };break;

      case kvUI://unique ID
      {
         if (fromStdin){if(edckvapi_fread(vbuf,1,vlen,stdin)!=vlen) return false;}
         vlen2=  vlen - (vbuf[vlen+3] == 0);
         memcpy(Pbuf+Pidx-4, &vlen2, 4);
         memcpy(Pbuf+Pidx, vbuf, vlen2);
         Pidx+=vlen2;
      }break;

      default: return false;

   }

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
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Iidx > Imax) && !morebuf(kvI,vlen)) return false;

   //key length = key path length + 8 prefix + 8 current attribute
   //idx increased by 1
   Ibuf[Iidx++]=kloc+16;
   
   //prefix
   memcpy(Ibuf+Iidx, &prefix, 8);
   Iidx+=8;
   
   //key
   memcpy(Ibuf+Iidx, kbuf, kloc+8);
   Iidx+=kloc+8;
   
   //value length
   memcpy(Ibuf+Iidx, &vlen, 4);
   Iidx+=4;
   if (vlen==0) return true;
   //value with contents


   switch (vrcat)
   {
#pragma mark generic
      case kvII://SOPInstanceUID
      case kvUI://unique ID
      {
         if (fromStdin){if(edckvapi_fread(vbuf,1,vlen,stdin)!=vlen) return false;}
         vlen2=  vlen - (vbuf[vlen+3] == 0);
         memcpy(Ibuf+Iidx-4, &vlen2, 4);
         memcpy(Ibuf+Iidx, vbuf, vlen2);
         Iidx+=vlen2;
      }break;
         
      case kvFD://floating point double
      case kvFL://floating point single
      case kvSL://signed long
      case kvSS://signed short
      case kvUL://unsigned long
      case kvUS://unsigned short
      case kvAT://attribute tag, 2 u16 hexa
      case kvTP://AS DT TM DA 11 text short ascii pair length
      case kvTA://AE DS IS CS 13 text short ascii
      case kvTS://LO LT SH ST 19 text short charset
      case kvTL://UC UT 25 text long charset
      case kvTU://url encoded
      case kvPN://person name
      //   kvUN only private
      case kv01://OB OD OF OL OV OW SV UV
#pragma mark special
      case kvfo://OV 31 Extended​Offset​Table fragments offset 7FE00001
      case kvfl://OV 32 Extended​Offset​TableLengths fragments offset 7FE00002
      case kvft://UV 33 Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
      case kvIa://AcquisitionNumber
      {
         if (fromStdin){if(edckvapi_fread(Ibuf+Iidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ibuf+Iidx, vbuf, vlen);//from vbuf
         Iidx+=vlen;
      };break;

      case kvIi://InstanceNumber needed for Iprefix
      {
         if (fromStdin){if(edckvapi_fread(Ibuf+Iidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ibuf+Iidx, vbuf, vlen);//from vbuf
         Iidx+=vlen;

         //retain series number
         s16 Ihd;
         sscanf((char*)vbuf,"%hd",&Ihd);
         pi16=u16swap(Ihd+0x8000);
      }break;

      default: return false;
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
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Nidx > Nmax) && !morebuf(kvN,vlen)) return false;

   //key length = key path length + 8 prefix + 8 current attribute
   //idx increased by 1
   Nbuf[Nidx++]=kloc+16;

   //prefix
   memcpy(Nbuf+Nidx, &prefix, 8);
   Nidx+=8;
   
   //key
   memcpy(Nbuf+Nidx, kbuf, kloc+8);
   Nidx+=kloc+8;
   
   //value length
   memcpy(Nbuf+Nidx, &vlen, 4);
   Nidx+=4;
   if (vlen==0) return true;
   //value with contents
   switch (vrcat)
   {
#pragma mark generic
      case kvUS://unsigned short
      case kvTS://LO LT SH ST 19 text short charset
      case kv01://OB OD OF OL OV OW SV UV
#pragma mark special
      case kvNB://40 0x7FE00010: //OB
      case kvNW://41 0x7FE00010: //OW
      case kvNF://42 0x7FE00008: //OF float
      case kvND://43 0x7FE00009: //OD double
      {
         if (fromStdin){if(edckvapi_fread(Nbuf+Nidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Nbuf+Nidx, vbuf, vlen);//from vbuf
         Nidx+=vlen;
      };break;

      case kvUI://unique ID (transfert syntax)
      {
         if (fromStdin){if(edckvapi_fread(vbuf,1,vlen,stdin)!=vlen) return false;}
         vlen2=  vlen - (vbuf[vlen+3] == 0);
         memcpy(Nbuf+Nidx-4, &vlen2, 4);
         memcpy(Nbuf+Nidx, vbuf, vlen2);
         Nidx+=vlen2;
      }break;

      default: return false;
   }
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
/*
   //Nprefix=0x04;
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
               if (!fwriteNfile(false)) return false;
               if (edckvapi_fread(Nbuf+Nidx,1,0xFFFE,stdin)!=0xFFFE) return false;
               remaining-=0xFFFE;
            }
            if (remaining > 0)
            {
               if (!fwriteNfile(false)) return false;
               if (edckvapi_fread(Nbuf+Nidx,1,remaining,stdin)!=remaining) return false;
            }
            if (!fwriteNfile(false)) return false;
         }
         else
         {
            if ((vlen + Nidx > 0xFFFE) && !fwriteNfile(false)) return false;//freeing buffer necesary?
            if (edckvapi_fread(Nbuf+Nidx,1,vlen,stdin)!=vlen) return false;
            Nidx+=vlen;
         }
      }
      else //from vbuf
      {
         if ((vlen + Nidx > 0xFFFE) && !fwriteNfile(false)) return false;//freeing buffer necesary?
         memcpy(Nbuf+Nidx, vbuf, vlen);
         Nidx+=vlen;
      }
   }
*/
   return true;
}

/*
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
                  fwriteIfile(false);
                  
                  //read fragment 1
                  if (edckvapi_fread(&fragmentbytes, 1, 8, stdin)!=8) return false;
                  
                  while (fragmentstruct->t != 0xe0ddfffe)
                  {
                     if (fragmentstruct->t != 0xe000fffe) return false;
#pragma mark TODO fragment number
//Iprefix+=0x10000000000;
                     Ibuf[Iidx++]=16;//key size
                     memcpy(Ibuf+Iidx, &Iprefix, 8);//copy Iprefix
                     Iidx+=8;
                     memcpy(Ibuf+Iidx, kbuf, 8);//copy key
                     Iidx+=8;
                     memcpy(Ibuf+Iidx, &(fragmentstruct->l), 4);//val length
                     Iidx+=8;
                     
                     //write iBuffer and reset index
                     fwriteIfile(false);
                     
                     D("%08lld %016llx %x %x\n",vloc,u64swap(Iprefix),fragmentstruct->t,fragmentstruct->l);
                     if (fragmentstruct->l > 0)
                     {
                        size_t bytesremaing=fragmentstruct->l;
                        while ( bytesremaing > 0xFFFD)
                        {
                           if (edckvapi_fread(vbuf,1,0xFFFE,stdin)!=0xFFFE) return false;
#pragma mark TODO                               if (fwrite(vbuf ,1, 0xFFFE , Ifile)!=0xFFFE) return false;
                           bytesremaing-=0xFFFE;
                        }
                        if (bytesremaing > 0)
                        {
                           if (edckvapi_fread(vbuf,1,bytesremaing,stdin)!=bytesremaing) return false;
#pragma mark TODO                               if (fwrite(vbuf ,1, bytesremaing , Ifile)!=bytesremaing) return false;
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
*/
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
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Cidx > Cmax) && !morebuf(kvC,vlen)) return false;

   //key length = key path length + 8 prefix + 8 current attribute
   //idx increased by 1
   Cbuf[Cidx++]=kloc+16;

   //prefix
   memcpy(Cbuf+Cidx, &prefix, 8);
   Cidx+=8;
   
   //key
   memcpy(Cbuf+Cidx, kbuf, kloc+8);
   Cidx+=kloc+8;
   
   //value length
   memcpy(Cbuf+Cidx, &vlen, 4);
   Cidx+=4;
   if (vlen==0) return true;
   //value with contents
   switch (vrcat)
   {
#pragma mark generic
      case kvUS://unsigned short
      case kvTS://LO LT SH ST 19 text short charset
      case kv01://OB OD OF OL OV OW SV UV
#pragma mark special
      case kvCB://40 0x7FE00010: //OB
      {
         if (fromStdin){if(edckvapi_fread(Nbuf+Nidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Nbuf+Nidx, vbuf, vlen);//from vbuf
         Nidx+=vlen;
      };break;

      case kvUI://unique ID (transfert syntax)
      {
         if (fromStdin){if(edckvapi_fread(vbuf,1,vlen,stdin)!=vlen) return false;}
         vlen2=  vlen - (vbuf[vlen+3] == 0);
         memcpy(Nbuf+Nidx-4, &vlen2, 4);
         memcpy(Nbuf+Nidx, vbuf, vlen2);
         Nidx+=vlen2;
      }break;

      default: return false;
   }
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
/*
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
               if (!fwriteCfile(false)) return false;
               if (edckvapi_fread(Cbuf+Cidx,1,0xFFFE,stdin)!=0xFFFE) return false;
               remaining-=0xFFFE;
            }
            if (remaining > 0)
            {
               if (!fwriteCfile(false)) return false;
               if (edckvapi_fread(Cbuf+Cidx,1,remaining,stdin)!=remaining) return false;
            }
            if (!fwriteCfile(false)) return false;
         }
         else
         {
            if ((vlen + Cidx > 0xFFFE) && !fwriteCfile(false)) return false;//freeing buffer necesary?
            if (edckvapi_fread(Cbuf+Cidx,1,vlen,stdin)!=vlen) return false;
            Cidx+=vlen;
         }
      }
      else //from vbuf
      {
         if ((vlen + Cidx > 0xFFFE) && !fwriteCfile(false)) return false;//freeing buffer necesary?
         memcpy(Cbuf+Cidx, vbuf, vlen);
         Cidx+=vlen;
      }
   }
*/
   return true;
}
