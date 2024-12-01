// project: dicm2dckv
// targets: eDCKVinline
// file: dckvapi.m
// created by jacquesfauquex on 2024-04-04.

#include "edckvapi.h"
//#include <stdio.h> //incluido in dckvtypes.h


#pragma mark - overwrite read hooks
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



#pragma mark - static


static sqlite3      *db;
static char         *dberr = 0;
static int           dbrc  = 0;//return code

static int stepreturnstatus;

static sqlite3_stmt *eblake3stmt;
static sqlite3_stmt *einsertstmt;
static int currentEpk;
static sqlite3_stmt *sblake3stmt;
static sqlite3_stmt *sinsertstmt;
static int currentSpk;
static bool notRegistered;

static bool isexplicit;

static u32 vlenNoPadding;

//static size_t bytesWritten;

const char *space=" ";
const char *backslash = "\\";

//buffers ...
//start with 0xFFFF size
//and is reallocated with same increment in size each time the current space is filled up
static char *Ebuf;
static char *Sbuf;
static char *Ibuf;
static char *Pbuf;
static char *Nbuf;
static char *Cbuf;
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

static u32 utf8length;
static char utf8bytes[256];

//variables for E sqlite




static char sdate[]={ 0x0,0x0,0x0,0x0,0x0,0x0,0x0 };
static char stime[]={ 0x0,0x0,0x0,0x0,0x0,0x0,0x0 };
static char suidb64[44];
static u8   suidb64length;
static char snumber[17];//15 issuer type
static u8   snumberlength;
static char sclass[17];//15 issuer type
static u8   sclasslength;
static char smod[17];//15 issuer type
static u8   smodlength;
static char sdescc[256];//15 issuer type
static u8   sdesclength;


static char iuidb64[44];
static u8   iuidb64length;

static u64 prefix=0x00;
static blake3_hasher hasher;
static uint8_t hashbytes[BLAKE3_OUT_LEN];//32 bytes
static u8 *registeredhashbytes;//32 bytes

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

//e static
static u32  edate;//9
static char euidb64[44];
static u8   euidb64length;
static u32  pname[3];//7 patient name [0] offset [1] length [2] charset
static u32  pide[3];//7 patient id LO
static u32  pidr[3];//8 patient id issuer LO
static u32  pbirth;//9
static u8   psex=0;//10 CS
static u32  eid[3];//11 study id
static u32  ean[3];//12 accession number
static u32  eal[3];//13 issuer local
static u32  eau[3];//14 issuer universal
static u32  eat[3];//15 issuer type
static u32  img[3];//16 realisationstatic u8   imglength;
static u32  cda[3];//17 cda (reading)
static u32  req[3];//18 requesting
static u32  ref[3];//19 requesting
static u32  pay[3];//20 patient id LO
static u32  edesc[3];//21 patient id LO
static char ecode[256];//22 code^lexique^title
static u8   ecodelength;
static u8   ecodecharset;
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
#pragma mark sqlite
   I("sqlite %s\n", sqlite3_libversion());
   
   //db path
   u8 argv2strlen=strlen(dstdir);
   char dbpath[argv2strlen+10];
   memcpy(dbpath,dstdir,argv2strlen);
   if (dbpath[argv2strlen-1]!='/') dbpath[argv2strlen++]='/';
   memcpy(dbpath+argv2strlen, "edckv.db", 8);
   dbpath[argv2strlen+8]=0x00;

   sqlite3 *db;
   char *dberr = 0;
   int   dbrc  = 0;//return code
   
   if (stat(dbpath, &st)==-1)
   {
#pragma mark todo create default db from a dedicated file in the  project
      I("create edckv.db in %s",dstdir);
      dbrc = sqlite3_open_v2(dbpath, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
      if (dbrc == SQLITE_OK)
      {
         if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS hola (id INTEGER PRIMARY KEY AUTOINCREMENT)", NULL, NULL, &dberr) != SQLITE_OK)
         {
             sqlite3_close(db);
             E("Table failed to create %s","hola");
         }
      }
   }
   else dbrc = sqlite3_open_v2(dbpath, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
   
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   
#pragma mark E sqlite stmt
//blake3 stmt
   char eblake3[]="SELECT pk, eblake3 FROM E WHERE euid=?;";
   dbrc = sqlite3_prepare_v2(db, eblake3, sizeof(eblake3), &eblake3stmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot initialize eblake3stmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
//insert stmt
   //char einsert[] = "INSERT INTO E(da,ui) VALUES(?,?)";
   char einsert[] = "INSERT INTO E(procid,edate,euid,edckv,eblake3,pname,pide,pidr,pbirth,psex,eid,ean,eal,eau,eat,img,cda,req,ref,pay,edesc,ecode) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
   dbrc=sqlite3_prepare(db, einsert, -1, &einsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare einsertStmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }

   
#pragma mark S sqlite stmt
//blake3 stmt
   char sblake3[]="SELECT pk, sblake3 FROM S WHERE suid=?;";
   dbrc = sqlite3_prepare_v2(db, sblake3, sizeof(sblake3), &sblake3stmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot initialize Sblake3stmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   char sinsert[] = "INSERT INTO S(fk,sdate,stime,suid,sdckv,sblake3,sxml,spdf,snumber,sclass,smod,sdesc,sicon,sframes) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
   dbrc=sqlite3_prepare(db, sinsert, -1, &sinsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare sinsertStmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }

   
   
   
   
   isexplicit=*stidx==2;

   //static sopclass idx
   pc16=*soidx;

   //static base/pid path
   strcat(dirpath,dstdir);
   dirpathlength=strlen(dstdir);
   if (dirpath[dirpathlength-1]!='/') dirpath[dirpathlength++]='/';
   //dirpath[dirpathlength]=0x00;
   //if ((stat(dirpath, &st)==-1) && (mkdir(dirpath, 0777)==-1)) return false;
   sprintf(dirpath+dirpathlength, "%d", getpid());
   dirpathlength+=intdecsize(getpid());
   dirpath[dirpathlength++]='/';
   dirpath[dirpathlength]=0x00;
   if ((stat(dirpath, &st)==-1) && (mkdir(dirpath, 0777)==-1)) return false;
   
   //resets
   euidb64length=0;
   
   
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


bool einsert()
{
   // int pk
   sqlite3_bind_int(einsertstmt,  1, getpid());
   sqlite3_bind_int(einsertstmt,  2, edate);
   sqlite3_bind_text(einsertstmt, 3, euidb64,euidb64length, NULL);
   sqlite3_bind_blob(einsertstmt, 4, Ebuf, Eidx, NULL);//3 dckv
   sqlite3_bind_blob(einsertstmt, 5, hashbytes,BLAKE3_OUT_LEN, NULL);
   
   utf8(pname[2],Ebuf,pname[0],pname[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt, 6, utf8bytes,utf8length, SQLITE_TRANSIENT);//patient name
   
   sqlite3_bind_text(einsertstmt, 7, Ebuf+pide[0],pide[1], NULL);//id extension
   sqlite3_bind_text(einsertstmt, 8, Ebuf+pidr[0],pidr[1], NULL);//id root (issuer)
   sqlite3_bind_int(einsertstmt,  9, pbirth);//patient birthdate
   sqlite3_bind_int(einsertstmt, 10, psex);//patient sex 1=M, 2=F, 9=O

   utf8(eid[2],Ebuf,eid[0],eid[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt, 11, utf8bytes,utf8length, SQLITE_TRANSIENT);

   utf8(ean[2],Ebuf,ean[0],ean[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,12, utf8bytes,utf8length, SQLITE_TRANSIENT);
 
   utf8(eal[2],Ebuf,eal[0],eal[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,13, utf8bytes,utf8length, SQLITE_TRANSIENT);

   utf8(eau[2],Ebuf,eau[0],eau[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,14, utf8bytes,utf8length, SQLITE_TRANSIENT);

   utf8(eat[2],Ebuf,eat[0],eat[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,15, utf8bytes,utf8length, SQLITE_TRANSIENT);

   utf8(img[2],Ebuf,img[0],img[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,16, utf8bytes,utf8length, SQLITE_TRANSIENT);

   utf8(cda[2],Ebuf,cda[0],cda[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,17, utf8bytes,utf8length, SQLITE_TRANSIENT);

   utf8(req[2],Ebuf,req[0],req[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,18, utf8bytes,utf8length, SQLITE_TRANSIENT);

   utf8(ref[2],Ebuf,ref[0],ref[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,19, utf8bytes,utf8length, SQLITE_TRANSIENT);

   utf8(pay[2],Ebuf,pay[0],pay[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,20, utf8bytes,utf8length, SQLITE_TRANSIENT);

   utf8(edesc[2],Ebuf,edesc[0],edesc[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,21, utf8bytes,utf8length, SQLITE_TRANSIENT);

   sqlite3_bind_text(einsertstmt,22, ecode,ecodelength, SQLITE_TRANSIENT);
   //mods

   dbrc = sqlite3_step(einsertstmt);
   if (dbrc != SQLITE_DONE )
   {
      fprintf(stderr, "einsertstmt error: %s\n", dberr);
      return false;
   }
   sqlite3_reset(einsertstmt);
   return true;
}


bool sinsert()
{
   /*
   // int pk
   sqlite3_bind_int(einsertstmt,  1, currentEpk);
   sqlite3_bind_int(einsertstmt,  2, atoi(sdate));
   sqlite3_bind_int(einsertstmt,  3, atoi(stime));
   sqlite3_bind_text(einsertstmt, 4, suidb64,suidb64length, NULL);
   sqlite3_bind_blob(einsertstmt, 5, Sbuf, Sidx, NULL);//3 dckv
   sqlite3_bind_blob(einsertstmt, 6, hashbytes,BLAKE3_OUT_LEN, NULL);
   sqlite3_bind_text(einsertstmt, 7, sxml,sxmllength, NULL);//
   sqlite3_bind_blob(einsertstmt, 8, spdf,spdflength, NULL);//
   sqlite3_bind_int(einsertstmt,  9, snumber);//
   sqlite3_bind_int(einsertstmt, 10, sclass);//
   sqlite3_bind_text(einsertstmt,11, smod,smodlength, NULL);
   sqlite3_bind_text(einsertstmt,12, sdesc,sdesclength, NULL);
   sqlite3_bind_blob(einsertstmt,13, sicon,siconlength, NULL);
   sqlite3_bind_int(einsertstmt, 14, 0);
   

   dbrc = sqlite3_step(einsertstmt);
   if (dbrc != SQLITE_DONE )
   {
      fprintf(stderr, "einsertstmt error: %s\n", dberr);
      return false;
   }
   sqlite3_reset(einsertstmt);
    */
   return true;
}


#pragma mark -



bool morebuf(enum kvfamily f, u32 vlen)
{
   char *newbuf;
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
   if (edate==0)
   {
      time_t rawtime;
      time ( &rawtime );
      struct tm * timeinfo;
      timeinfo = localtime ( &rawtime );
      strftime(dirpath+dirpathlength,8,"%Y%m%d", timeinfo);
   }
   else sprintf(dirpath+dirpathlength, "%d", edate);
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
#pragma mark E
      currentEpk=0;
      blake3_hasher_reset(&hasher);
      blake3_hasher_update(&hasher, Ebuf, Eidx);
      blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);

      //does euidb64[euidb64length] exist in sqlite3 and what is its blake3 ?
      //find currentEpk
      sqlite3_bind_text(eblake3stmt, 1, euidb64,euidb64length, NULL);
      stepreturnstatus = sqlite3_step(eblake3stmt);
      if (stepreturnstatus != SQLITE_ROW)
      {
         if (!einsert()) return false;
         //get pk
         sqlite3_reset(eblake3stmt);
         sqlite3_bind_text(eblake3stmt, 1, euidb64,euidb64length, NULL);
         stepreturnstatus = sqlite3_step(eblake3stmt);
         currentEpk=sqlite3_column_int(eblake3stmt, 0);
      }
      else //euidb64 exists in E
      {
         //more than one ROW?
         notRegistered=true;
         while (notRegistered)
         {
            registeredhashbytes=(u8 *)(sqlite3_column_blob(eblake3stmt, 1));
            //equals?
            if (memcmp(hashbytes, registeredhashbytes, BLAKE3_OUT_LEN)==0)
            {
               notRegistered=false;
               currentEpk=sqlite3_column_int(eblake3stmt, 0);
               continue;
            }
            //next answer?
            stepreturnstatus = sqlite3_step(eblake3stmt);
            notRegistered=(stepreturnstatus == SQLITE_ROW);
         }
         if (notRegistered)
         {
            if (!einsert()) return false;
            //get pk
            sqlite3_reset(eblake3stmt);
            sqlite3_bind_text(eblake3stmt, 1, euidb64,euidb64length, NULL);
            stepreturnstatus = sqlite3_step(eblake3stmt);
            currentEpk=sqlite3_column_int(eblake3stmt, 0);
         }
      }
      sqlite3_reset(eblake3stmt);
      D("sqlite commit E pk: %d",currentEpk);
      
   //(optional) write to filesystem
      
      // /studydate/.studyiuid/0000000000000000.
      snprintf(filepath+dirpathlength,17,"0000000000000000");
      filepath[dirpathlength+16]='.';
      //add blake3 to final name
      
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

//blake3
      currentSpk=0;
      blake3_hasher_reset(&hasher);
      blake3_hasher_update(&hasher, Sbuf, Sidx);
      blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);

//does suidb64[suidb64length] exist in sqlite3 and what is its blake3 ?
//find currentSpk
      sqlite3_bind_text(sblake3stmt, 1, suidb64,suidb64length, NULL);
      stepreturnstatus = sqlite3_step(sblake3stmt);
      if (stepreturnstatus != SQLITE_ROW)
      {
         if (!sinsert()) return false;
         //get pk
         sqlite3_reset(eblake3stmt);
         sqlite3_bind_text(eblake3stmt, 1, euidb64,euidb64length, NULL);
         stepreturnstatus = sqlite3_step(eblake3stmt);
         currentEpk=sqlite3_column_int(eblake3stmt, 0);
      }
      else //suidb64 exists in S
      {
         //more than one ROW?
         bool notRegistered=true;
         while (notRegistered)
         {
            registeredhashbytes=(u8 *)(sqlite3_column_blob(eblake3stmt, 1));
            //equals?
            if (memcmp(hashbytes, registeredhashbytes, BLAKE3_OUT_LEN)==0)
            {
               notRegistered=false;
               currentEpk=sqlite3_column_int(eblake3stmt, 0);
               continue;
            }
            //next answer?
            stepreturnstatus = sqlite3_step(eblake3stmt);
            notRegistered=(stepreturnstatus == SQLITE_ROW);
         }
         if (notRegistered)
         {
            if (!einsert()) return false;
            //get pk
            sqlite3_reset(eblake3stmt);
            sqlite3_bind_text(eblake3stmt, 1, euidb64,euidb64length, NULL);
            stepreturnstatus = sqlite3_step(eblake3stmt);
            currentEpk=sqlite3_column_int(eblake3stmt, 0);
         }
      }
      sqlite3_reset(eblake3stmt);
      D("sqlite commit E pk: %d",currentEpk);
      
   //(optional) write to filesystem





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
   sqlite3_finalize(eblake3stmt);
   sqlite3_finalize(einsertstmt);
   sqlite3_free(dberr);
   sqlite3_close(db);

   I("!#%d",*siidx);
   return true;
}

#pragma mark -

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
#pragma mark UID
      case kvUI://unique ID
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         Eidx+=vlen;
      };break;
         
      case kveuid://StudyInstanceUID
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         vlenNoPadding=  vlen - (Ebuf[Eidx+vlen-1] == 0);
         if (!ui2b64( Ebuf+Eidx, vlenNoPadding, euidb64, &euidb64length )) return false;
         
         Eidx+=vlen;
      };break;

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
      //   kvUN only private
      case kv01://OB OD OF OL OV OW SV UV
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         Eidx+=vlen;
      };break;
      case kvPN://person name
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         Eidx+=vlen;
      };break;

#pragma mark special

      case kvedate://StudyDate
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain study date
         //memcpy(edate, Ebuf+Eidx, vlen);
         edate=atoi(Ebuf+Eidx);
         
         Eidx+=vlen;
      };break;

      case kvpname://Patient name
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain
         pname[0]=Eidx;
         pname[1]=vlen;
         pname[2]=kbuf[kloc+6];

         Eidx+=vlen;
      };break;

      case kvpide://Patient id extension
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         pide[0]=Eidx;
         pide[1]=vlen;
         pide[2]=kbuf[kloc+6];

         Eidx+=vlen;
      };break;

      case kvpidr://Patient root id issuer
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         pidr[0]=Eidx;
         pidr[1]=vlen;
         pidr[2]=kbuf[kloc+6];;
         
         Eidx+=vlen;
      };break;

      case kvpbirth://Patient birthdate
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         //retain
         pbirth=atoi(Ebuf+Eidx);
         Eidx+=vlen;
      };break;

      case kvpsex://Patient sex
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         u8 sexchar=*(Ebuf+Eidx);
         if (sexchar=='M') psex=1;
         else if (sexchar=='F') psex=2;
         else if (sexchar=='O') psex=9;
         else psex=0;
         Eidx+=vlen;
      };break;

      case kveid://StudyID
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf

         //retain
         eid[0]=Eidx;
         eid[1]=vlen;
         eid[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;

      case kvean://AccessionNumber
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         ean[0]=Eidx;
         ean[1]=vlen;
         ean[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;

      case kveal://AccessionNumberIssuer local 00080051.00400031
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         memcpy(eal, Ebuf+Eidx, vlen);

         //retain static
         eal[0]=Eidx;
         eal[1]=vlen;
         eal[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;

      case kveau://AccessionNumberIssuer universal 00080051.00400032
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         eau[0]=Eidx;
         eau[1]=vlen;
         eau[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;

      case kveat://AccessionNumberType
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         eat[0]=Eidx;
         eat[1]=vlen;
         eat[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;


      case kvimg://InstitutionName (placed in exam instead of series)
      {
         if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf

         //retain
         img[0]=Eidx;
         img[1]=vlen;
         img[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;

      case kvcda://study CDA (reading)
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         cda[0]=Eidx;
         cda[1]=vlen;
         cda[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;

      case kvreq://study requesting
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         req[0]=Eidx;
         req[1]=vlen;
         req[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;
         
      case kvref://study referring
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         ref[0]=Eidx;
         ref[1]=vlen;
         ref[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;

         
      case kvpay://00101050 kvpay LO pay insurance plan identification
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         pay[0]=Eidx;
         pay[1]=vlen;
         pay[2]=kbuf[kloc+6];;

         Eidx+=vlen;
      };break;

      case kvedesc://study description
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         
         //retain static
         edesc[0]=Eidx;
         edesc[1]=vlen;
         edesc[2]=kbuf[kloc+6];

         Eidx+=vlen;
      };break;

      case kvecode://study code
      {
         if (fromStdin){if (edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
         //retain
         utf8(kbuf[kloc+6],Ebuf,Eidx,vlen,ecode,ecodelength,&utf8length);
         ecodelength+=utf8length;
         ecode[ecodelength++]='^';
         ecodecharset=kbuf[kloc+6];
         
         Eidx+=vlen;
      };break;

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
#pragma mark UID
      case kvUI://unique ID
      {
         if (fromStdin){if(edckvapi_fread(Sbuf+Sidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Sbuf+Sidx, vbuf, vlen);//from vbuf
         Sidx+=vlen;
      };break;
         
      case kvIS://SeriesInstanceUID
      {
         if (fromStdin){if(edckvapi_fread(Sbuf+Sidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Sbuf+Sidx, vbuf, vlen);//from vbuf
         
         vlenNoPadding=  vlen - (Sbuf[Sidx+vlen-1] == 0);
#pragma mark TODO         //series index
         //if (!ui2b64( Sbuf+Sidx, vlenNoPadding, euidb64, &euidb64length )) return false;
         
         Eidx+=vlen;
      };break;


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
         
      case kved://OB Encapsulated​Document 00420011
      {
         if (fromStdin){if(edckvapi_fread(Sbuf+Sidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Sbuf+Sidx, vbuf, vlen);//from vbuf
         
         //replace everything after last > with spaces
         vlenNoPadding=Sidx+vlen-1;
         while (Sbuf[vlenNoPadding]!='>')
         {
            Sbuf[vlenNoPadding]=' ';
            vlenNoPadding--;
         }
         
         Sidx+=vlen;
      };break;

         
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
#pragma mark UID
      case kvUI://unique ID
      {
         if (fromStdin){if(edckvapi_fread(Pbuf+Pidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Pbuf+Pidx, vbuf, vlen);//from vbuf
         Pidx+=vlen;
      };break;

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
#pragma mark UID
      case kvII://SOPInstanceUID
      case kvUI://unique ID
      {
         if (fromStdin){if(edckvapi_fread(Ibuf+Iidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Ibuf+Iidx, vbuf, vlen);//from vbuf
         Iidx+=vlen;
      };break;

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
#pragma mark UID
      case kvNM:
      case kvUI://unique ID (transfert syntax)
      {
         if (fromStdin){if(edckvapi_fread(Nbuf+Nidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Nbuf+Nidx, vbuf, vlen);//from vbuf
         Nidx+=vlen;
      };break;
         
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

      default: return false;
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
#pragma mark UID
      case kvUI://unique ID (transfert syntax)
      {
         if (fromStdin){if(edckvapi_fread(Cbuf+Cidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Cbuf+Cidx, vbuf, vlen);//from vbuf
         Cidx+=vlen;
      };break;

#pragma mark generic
      case kvUS://unsigned short
      case kvTS://LO LT SH ST 19 text short charset
      {
         if (fromStdin){if(edckvapi_fread(Nbuf+Nidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Nbuf+Nidx, vbuf, vlen);//from vbuf
         Nidx+=vlen;
      };break;

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
   //https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_A.4
   if (vlen!=0xFFFFFFFF) return false; //0x7FE00010 fragments vlen is undefined

   if (fromStdin)
   {
      /*
       our codification does not follow the standard
       u32 number of fragments
       u32 offset of fragment (repeated)
       u32 offset after last fragment
       fragments data
       */
      
      
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
      //fwriteIfile(false);
      
      //read fragment 1
      if (edckvapi_fread(&fragmentbytes, 1, 8, stdin)!=8) return false;
      
      while (fragmentstruct->t != 0xe0ddfffe)
      {
         if (fragmentstruct->t != 0xe000fffe) return false;
#pragma mark TODO fragment number
//Iprefix+=0x10000000000;
         Ibuf[Iidx++]=16;//key size
         //memcpy(Ibuf+Iidx, &Iprefix, 8);//copy Iprefix
         Iidx+=8;
         memcpy(Ibuf+Iidx, kbuf, 8);//copy key
         Iidx+=8;
         memcpy(Ibuf+Iidx, &(fragmentstruct->l), 4);//val length
         Iidx+=8;
         
         //write iBuffer and reset index
         //fwriteIfile(false);
         
         //D("%08lld %016llx %x %x\n",vloc,u64swap(Iprefix),fragmentstruct->t,fragmentstruct->l);
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
   return true;
}
