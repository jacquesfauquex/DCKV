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
static sqlite3_stmt *iblake3stmt;
static sqlite3_stmt *ipinsertstmt;
static int currentIpk;

static bool isexplicit;

static u32 vlenNoPadding;

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

static u64 prefix=0x00;
static int iterator;
static blake3_hasher hasher;
static uint8_t hashbytes[BLAKE3_OUT_LEN];//32 bytes
static u8 *registeredhashbytes;//32 bytes

struct stat st={0};//for directory creation
static char dirpath[256];
static u8   dirpathlength;
static char filepath[330];
static FILE *fileptr;

//prefix components
static u8  sversion=0;
static u16 sopclassindex=0;

#pragma mark TODO
static u8  rversion=0;
static u16 rnumber=0;

static u8  iversion=0;
static u8  concat=0;

static u16 fnumber=0;


#pragma mark - methods

//pk
//procid
static u32  edate;
static char euidb64[44];
static u8   euidb64length;
static u32  pname[3];// patient name [0] offset [1] length [2] charset
static u32  pide[3];// patient id LO
static u32  pidr[3];// patient id issuer LO
static u32  pbirth;//
static u8   psex=0;// CS
static u32  eid[3];// study id
static u32  ean[3];// accession number
static u32  eal[3];// issuer local
static u32  eau[3];// issuer universal
static u32  eat[3];// issuer type
static u32  img[3];// realisationstatic
static u32  cda[3];// cda (reading)
static u32  req[3];// requesting
static u32  ref[3];// requesting
static u32  pay[3];// patient id LO
static u32  edesc[3];// patient id LO
static char ecode[256];// code^lexique^title
static u8   ecodelength;
static u8   ecodecharset;
//emoods

//fk
//pk
static u32  sdate;
static u32  stime;
static char suidb64[44];
static u8   suidb64length;
static u32  sxml[2];
//static u32  spdf[3];
static u16  snumber;//unsigned because we sum up 0x8000
//static u32  doctitle[3];
static u32  sopidx;
static u32  smod[2];
static u32  sdesc[3];
//sframes

//fk
//pk
static char iuidb64[44];
static u8   iuidb64length;
static u16  inumber;//unsigned because we sum up 0x8000
static u16  ianumber;//unsigned because we sum up 0x8000
//iframes

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


   
#pragma mark I+P sqlite stmt
//blake3 stmt
   char iblake3[]="SELECT pk, iblake3 FROM I WHERE iuid=?;";
   dbrc = sqlite3_prepare_v2(db, iblake3, sizeof(iblake3), &iblake3stmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot initialize iblake3stmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   char ipinsert[] = "INSERT INTO I(fk,iuid,idckv,iblake3,inumber,ianumber,iclass,iframes,pdckv) VALUES(?,?,?,?,?,?,?,?,?)";
   dbrc=sqlite3_prepare(db, ipinsert, -1, &ipinsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare ipinsertStmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }

   
   
   
   isexplicit=*stidx==2;

   //static sopclass idx
   sopclassindex=*soidx;
   sopidx=*soidx;
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
   edate=0;
   sqlite3_bind_text(einsertstmt, 3, euidb64,euidb64length, NULL);
   sqlite3_bind_blob(einsertstmt, 4, Ebuf, Eidx, NULL);//3 dckv
   sqlite3_bind_blob(einsertstmt, 5, hashbytes,BLAKE3_OUT_LEN, NULL);
   
   utf8(pname[2],Ebuf,pname[0],pname[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt, 6, utf8bytes,utf8length, SQLITE_TRANSIENT);//patient name
   pname[0]=pname[1]=pname[2]=0;
   
   sqlite3_bind_text(einsertstmt, 7, Ebuf+pide[0],pide[1], NULL);//id extension
   pide[0]=pide[1]=0;
   
   sqlite3_bind_text(einsertstmt, 8, Ebuf+pidr[0],pidr[1], NULL);//id root (issuer)
   pidr[0]=pidr[1]=0;

   sqlite3_bind_int(einsertstmt,  9, pbirth);//patient birthdate
   pbirth=0;
   
   sqlite3_bind_int(einsertstmt, 10, psex);//patient sex 1=M, 2=F, 9=O
   psex=0;
   
   utf8(eid[2],Ebuf,eid[0],eid[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt, 11, utf8bytes,utf8length, SQLITE_TRANSIENT);
   eid[0]=eid[1]=eid[2]=0;

   utf8(ean[2],Ebuf,ean[0],ean[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,12, utf8bytes,utf8length, SQLITE_TRANSIENT);
   ean[0]=ean[1]=ean[2]=0;

   utf8(eal[2],Ebuf,eal[0],eal[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,13, utf8bytes,utf8length, SQLITE_TRANSIENT);
   eal[0]=eal[1]=eal[2]=0;

   utf8(eau[2],Ebuf,eau[0],eau[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,14, utf8bytes,utf8length, SQLITE_TRANSIENT);
   eau[0]=eau[1]=eau[2]=0;

   utf8(eat[2],Ebuf,eat[0],eat[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,15, utf8bytes,utf8length, SQLITE_TRANSIENT);
   eat[0]=eat[1]=eat[2]=0;

   utf8(img[2],Ebuf,img[0],img[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,16, utf8bytes,utf8length, SQLITE_TRANSIENT);
   img[0]=img[1]=img[2]=0;

   utf8(cda[2],Ebuf,cda[0],cda[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,17, utf8bytes,utf8length, SQLITE_TRANSIENT);
   cda[0]=cda[1]=cda[2]=0;

   utf8(req[2],Ebuf,req[0],req[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,18, utf8bytes,utf8length, SQLITE_TRANSIENT);
   req[0]=req[1]=req[2]=0;

   utf8(ref[2],Ebuf,ref[0],ref[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,19, utf8bytes,utf8length, SQLITE_TRANSIENT);
   ref[0]=ref[1]=ref[2]=0;

   utf8(pay[2],Ebuf,pay[0],pay[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,20, utf8bytes,utf8length, SQLITE_TRANSIENT);
   pay[0]=pay[1]=pay[2]=0;

   utf8(edesc[2],Ebuf,edesc[0],edesc[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(einsertstmt,21, utf8bytes,utf8length, SQLITE_TRANSIENT);
   edesc[0]=edesc[1]=edesc[2]=0;

   sqlite3_bind_text(einsertstmt,22, ecode,ecodelength, SQLITE_TRANSIENT);
   ecodelength=0;
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


bool sinsert(u64 prefix)
{
   // int pk
   sqlite3_bind_int(sinsertstmt,  1, currentEpk);
   
   sqlite3_bind_int(sinsertstmt,  2, sdate);
   sdate=0;
   
   sqlite3_bind_int(sinsertstmt,  3, stime);
   stime=0;
   
   sqlite3_bind_text(sinsertstmt, 4, suidb64,suidb64length, NULL);
   
   iterator=0;
   while (iterator<Sidx)
   {
      memcpy(Sbuf+iterator+1,&prefix,8);
      iterator+=Sbuf[iterator]+1;
      iterator+=Sbuf[iterator]+(Sbuf[iterator+1]*0x10)+(Sbuf[iterator+2]*0x100)+(Sbuf[iterator+3]*0x1000)+4;
   }
   sqlite3_bind_blob(sinsertstmt, 5, Sbuf, Sidx, NULL);//3 dckv
   
   sqlite3_bind_blob(sinsertstmt, 6, hashbytes,BLAKE3_OUT_LEN, NULL);
   
   
   sqlite3_bind_text(sinsertstmt, 7, Sbuf+sxml[0],sxml[1], NULL);
   //sqlite3_bind_blob(sinsertstmt, 8, spdf,spdflength, NULL);
   sxml[0]=sxml[1]=0;
   
   sqlite3_bind_int(sinsertstmt,  9, snumber);
   //snumber=0;used for prefix, reseted at the end of the commit
   
   sqlite3_bind_int(sinsertstmt, 10, sopidx);//
   sqlite3_bind_text(sinsertstmt,11, Sbuf+smod[0],smod[1], NULL);
   smod[0]=smod[1]=0;
   
   utf8(sdesc[2],Sbuf,sdesc[0],sdesc[1],utf8bytes,0,&utf8length);
   sqlite3_bind_text(sinsertstmt,12, utf8bytes,utf8length, SQLITE_TRANSIENT);
   sdesc[0]=sdesc[1]=sdesc[2]=0;
   
   //sqlite3_bind_blob(sinsertstmt,13, sicon,siconlength, NULL);
   sqlite3_bind_int(sinsertstmt, 14, 0);
   

   dbrc = sqlite3_step(sinsertstmt);
   if (dbrc != SQLITE_DONE )
   {
      fprintf(stderr, "sinsertstmt error: %s\n", dberr);
      return false;
   }
   sqlite3_reset(sinsertstmt);

   //get pk
   sqlite3_reset(sblake3stmt);
   sqlite3_bind_text(sblake3stmt, 1, suidb64,suidb64length, NULL);
   stepreturnstatus = sqlite3_step(sblake3stmt);
   currentSpk=sqlite3_column_int(sblake3stmt, 0);
   if (stepreturnstatus != SQLITE_ROW) return false;
   D("sqlite commit S pk: %d",currentSpk);
   return true;
}


bool ipinsert(u64 prefix)
{
   /*
         0:pk
    1:fk, enlace a S.pk
    2:iuid,
    3:idckv,
    4:iblake3,
    5:inumber,
    6:ianumber,
    7:iclass,
    8:iframes, (0:no frame objects, 1:native, n:compressed)
    9:pdckv (private attributes)
    */
   
   // int pk
   sqlite3_bind_int(ipinsertstmt,  1, currentSpk);
   
   sqlite3_bind_text(ipinsertstmt, 2, iuidb64,iuidb64length, NULL);
   
   //prefix attributes
   iterator=0;
   while (iterator<Iidx)
   {
      memcpy(Ibuf+iterator+1,&prefix,8);
      iterator+=Ibuf[iterator]+1;
      iterator+=Ibuf[iterator]+(Ibuf[iterator+1]*0x10)+(Ibuf[iterator+2]*0x100)+(Ibuf[iterator+3]*0x1000)+4;
   }
   sqlite3_bind_blob(ipinsertstmt, 3, Ibuf, Iidx, NULL);//dckv
   
   sqlite3_bind_blob(ipinsertstmt, 4, hashbytes,BLAKE3_OUT_LEN, NULL);
   
   sqlite3_bind_int(ipinsertstmt,  5, inumber);
   
   sqlite3_bind_int(ipinsertstmt,  6, ianumber);
   
   sqlite3_bind_int(ipinsertstmt, 7, sopidx);

   sqlite3_bind_int(ipinsertstmt, 8, (Nidx>0));//number of frames modified when transfer syntaxes other than native are used
   
   if (Pidx>0)
   {
      //prefix attributes
      iterator=0;
      while (iterator<Pidx)
      {
         memcpy(Pbuf+iterator+1,&prefix,8);
         iterator+=Pbuf[iterator]+1;
         iterator+=Pbuf[iterator]+(Pbuf[iterator+1]*0x10)+(Pbuf[iterator+2]*0x100)+(Pbuf[iterator+3]*0x1000)+4;
      }
      sqlite3_bind_blob(ipinsertstmt, 3, Pbuf, Pidx, NULL);//dckv
   }

   dbrc = sqlite3_step(ipinsertstmt);
   if (dbrc != SQLITE_DONE )
   {
      fprintf(stderr, "ipinsertstmt error: %s\n", dberr);
      return false;
   }
   sqlite3_reset(ipinsertstmt);

   //get pk
   sqlite3_reset(iblake3stmt);
   sqlite3_bind_text(iblake3stmt, 1, iuidb64,iuidb64length, NULL);
   stepreturnstatus = sqlite3_step(iblake3stmt);
   currentIpk=sqlite3_column_int(iblake3stmt, 0);
   if (stepreturnstatus != SQLITE_ROW) return false;
   D("sqlite commit I pk: %d",currentIpk);
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

   
   

#pragma mark E 0000000000000000
   if (Eidx>0)
   {
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
         D("sqlite commit E pk: %d",currentEpk);
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
            D("sqlite commit E pk: %d",currentEpk);
         }
      }
      sqlite3_reset(eblake3stmt);

      Eidx=0;
   }
   
#pragma mark S : 1 s SS SS 0r RR RR CC CC
   if (Sidx>0)
   {
      //blake3 computed in the same conditions without prefix variability
      blake3_hasher_reset(&hasher);
      blake3_hasher_update(&hasher, Sbuf, Sidx);
      blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);

//does suidb64[suidb64length] exist in sqlite3 and what is its blake3 ?
//find currentSpk
      currentSpk=0;
      sversion=0;
      sqlite3_bind_text(sblake3stmt, 1, suidb64,suidb64length, NULL);
      stepreturnstatus = sqlite3_step(sblake3stmt);
      if (stepreturnstatus != SQLITE_ROW)
      {
         if (!sinsert(0x10|sversion|u16swap(snumber)*0x100|rversion*0x1000000|u16swap(rnumber)*0x100000000|u16swap(sopclassindex)*0x100000000000000)) return false;
      }
      else //suidb64 exists in S
      {
         //more than one ROW?
         bool notRegistered=true;
         while (notRegistered)
         {
            registeredhashbytes=(u8 *)(sqlite3_column_blob(sblake3stmt, 1));
            //equals?
            if (memcmp(hashbytes, registeredhashbytes, BLAKE3_OUT_LEN)==0)
            {
               notRegistered=false;
               currentSpk=sqlite3_column_int(sblake3stmt, 0);
               continue;
            }
            sversion++;
            //next answer?
            stepreturnstatus = sqlite3_step(sblake3stmt);
            notRegistered=(stepreturnstatus == SQLITE_ROW);
         }
         if (notRegistered)
         {
            if (!sinsert(0x10|sversion|u16swap(snumber)*0x100|rversion*0x1000000|u16swap(rnumber)*0x100000000|u16swap(sopclassindex)*0x100000000000000)) return false;
         }
      }
      //reset (not snumber y sversion que se usan para I N C)
      sqlite3_reset(sblake3stmt);
      Sidx=0;
   }
   
#pragma mark I : 2 s SS SS iu II II CC CC
   //u iversion (relates the frames of instances into a same volume)
   //versions of the same instance with diferent quality have diferent i
   if (Iidx>0 || Pidx>0)
   {
      //blake3
      blake3_hasher_reset(&hasher);
      blake3_hasher_update(&hasher, Ibuf, Iidx);
      blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);

      //does iuidb64[iuidb64length] exist in sqlite3 and what is its blake3 ?
      //find currentIpk
      currentIpk=0;
      iversion=0;
      concat=0;
      sqlite3_bind_text(iblake3stmt, 1, iuidb64,iuidb64length, NULL);
      stepreturnstatus = sqlite3_step(iblake3stmt);
      if (stepreturnstatus != SQLITE_ROW)
      {
         if (!ipinsert(0x20|sversion|u16swap(snumber)*0x100|iversion*0x100000|concat*0x1000000|u16swap(inumber)*0x100000000|u16swap(sopclassindex)*0x100000000000000)) return false;
      }
      else //iuidb64 exists in I
      {
         //more than one ROW?
         bool notRegistered=true;
         while (notRegistered)
         {
            registeredhashbytes=(u8 *)(sqlite3_column_blob(iblake3stmt, 1));
                  //equals?
            if (memcmp(hashbytes, registeredhashbytes, BLAKE3_OUT_LEN)==0)
            {
               notRegistered=false;
               currentIpk=sqlite3_column_int(iblake3stmt, 0);
               continue;
            }
            
            //next answer?
            stepreturnstatus = sqlite3_step(iblake3stmt);
            notRegistered=(stepreturnstatus == SQLITE_ROW);
         }
         if (notRegistered)
         {
            if (!ipinsert(0x20|sversion|u16swap(snumber)*0x100|iversion*0x100000|concat*0x1000000|u16swap(inumber)*0x100000000|u16swap(sopclassindex)*0x100000000000000)) return false;
         }
      }
      sqlite3_reset(iblake3stmt);
      Iidx=0;
      Pidx=0;
   }
   


   if (isexplicit)
   {
#pragma mark N 4 s SS SS iu II II 00 00
      //u iversion (relates the frames of instances into a same volume)
      //l concat (refers to distinct slices of the volumes)
      //versions of the same instance with diferent quality have diferent i
      if (Nidx>0)
      {
         //replace prefix in buffer
         iterator=0;
         prefix=0x40|sversion|u16swap(snumber)*0x100|iversion*0x100000|concat*0x1000000|u16swap(inumber)*0x100000000;
         while (iterator<Nidx)
         {
            memcpy(Nbuf+iterator+1,&prefix,8);
            iterator+=Nbuf[iterator]+1;
            iterator+=Nbuf[iterator]+(Nbuf[iterator+1]*0x10)+(Nbuf[iterator+2]*0x100)+(Nbuf[iterator+3]*0x1000)+4;
         }
         //      snprintf(filepath+dirpathlength,17, "2%1x%04hX%02hhX%04hX%04hX",sversion,snumber,concatconcat,inumber
         snprintf(filepath+dirpathlength,17, "4%1x%04hX%1x%1x%04hX0000",sversion,snumber,iversion,concat,inumber);
         filepath[dirpathlength+16]='.';
         //add blake3 to final name
         blake3_hasher_reset(&hasher);
         blake3_hasher_update(&hasher, Nbuf, Nidx);
         blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);
         for (iterator = 0; iterator < BLAKE3_OUT_LEN; iterator++) {
            sprintf(filepath+dirpathlength+17+iterator+iterator ,"%02x", hashbytes[iterator]);
         }
         filepath[dirpathlength+17+iterator+iterator]=0x00;
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
#pragma mark C 5 s SS SS iu II II FF FF
      //u iversion (relates the frames of instances into a same volume)
      //l concat (refers to distinct slices of the volumes)
      //versions of the same instance with diferent quality have diferent i
      if (Cidx>0)
      {
         //replace prefix in buffer
         iterator=0;
         
         prefix=0x50|sversion|u16swap(snumber)*0x100|iversion*0x100000|concat*0x1000000|u16swap(inumber)*0x100000000|u16swap(fnumber)*0x100000000000000;
         while (iterator<Cidx)
         {
            memcpy(Cbuf+iterator+1,&prefix,8);
            iterator+=Cbuf[iterator]+1;
            iterator+=Cbuf[iterator]+(Cbuf[iterator+1]*0x10)+(Cbuf[iterator+2]*0x100)+(Cbuf[iterator+3]*0x1000)+4;
         }
         snprintf(filepath+dirpathlength,17, "5%1x%04hX%1x%1x%04hX%04hX",sversion,snumber,iversion,concat,inumber,fnumber);
         filepath[dirpathlength+16]='.';
         //add blake3 to final name
         blake3_hasher_reset(&hasher);
         blake3_hasher_update(&hasher, Cbuf, Cidx);
         blake3_hasher_finalize(&hasher, hashbytes, BLAKE3_OUT_LEN);
         for (iterator = 0; iterator < BLAKE3_OUT_LEN; iterator++) {
            sprintf(filepath+dirpathlength+17+iterator+iterator ,"%02x", hashbytes[iterator]);
         }
         filepath[dirpathlength+17+iterator+iterator]=0x00;
         D("%s\n%s\n",dirpath,filepath);
         fileptr=fopen(filepath, "w");
         if (fileptr == NULL) return false;
         if (fwrite(Cbuf ,1, Cidx , fileptr)!=Cidx) return false;
         fclose(fileptr);
         
         //reset
         Cidx=0;
      }
   }
   
   
   snumber=0;
   inumber=0;
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

   if (fromStdin){if(edckvapi_fread(Ebuf+Eidx,1,vlen,stdin)!=vlen) return false;}
   else memcpy(Ebuf+Eidx, vbuf, vlen);//from vbuf
   
   switch (vrcat)
   {
      case kveuid://StudyInstanceUID
      {
         if (!ui2b64( Ebuf+Eidx, vlen - (Ebuf[Eidx+vlen-1] == 0), euidb64, &euidb64length )) return false;
      };break;

#pragma mark generic
      case kvUI://unique ID
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
      case kvPN://person name
         break;

      case kvedate: edate=atoi(Ebuf+Eidx);
         ;break;

      case kvpname://Patient name
      {
         pname[0]=Eidx;
         pname[1]=vlen;
         pname[2]=kbuf[kloc+6];
      };break;

      case kvpide://Patient id extension
      {
         pide[0]=Eidx;
         pide[1]=vlen;
         pide[2]=kbuf[kloc+6];
      };break;

      case kvpidr://Patient root id issuer
      {
         pidr[0]=Eidx;
         pidr[1]=vlen;
         pidr[2]=kbuf[kloc+6];
      };break;

      case kvpbirth: pbirth=atoi(Ebuf+Eidx);
         break;

      case kvpsex://Patient sex
      {
         switch (*(Ebuf+Eidx)) {
            case 'M':psex=1;
               break;
            case 'F':psex=2;
               break;
            case 'O':psex=9;
               break;
            default:psex=0;
               break;
         }
      };break;

      case kveid://StudyID
      {
         eid[0]=Eidx;
         eid[1]=vlen;
         eid[2]=kbuf[kloc+6];
      };break;

      case kvean://AccessionNumber
      {
         ean[0]=Eidx;
         ean[1]=vlen;
         ean[2]=kbuf[kloc+6];
      };break;

      case kveal://AccessionNumberIssuer local 00080051.00400031
      {
         eal[0]=Eidx;
         eal[1]=vlen;
         eal[2]=kbuf[kloc+6];
      };break;

      case kveau://AccessionNumberIssuer universal 00080051.00400032
      {
         eau[0]=Eidx;
         eau[1]=vlen;
         eau[2]=kbuf[kloc+6];
      };break;

      case kveat://AccessionNumberType
      {
         eat[0]=Eidx;
         eat[1]=vlen;
         eat[2]=kbuf[kloc+6];
      };break;


      case kvimg://InstitutionName (placed in exam instead of series)
      {
         img[0]=Eidx;
         img[1]=vlen;
         img[2]=kbuf[kloc+6];
      };break;

      case kvcda://study CDA (reading)
      {
         cda[0]=Eidx;
         cda[1]=vlen;
         cda[2]=kbuf[kloc+6];
      };break;

      case kvreq://study requesting
      {
         req[0]=Eidx;
         req[1]=vlen;
         req[2]=kbuf[kloc+6];
      };break;
         
      case kvref://study referring
      {
         ref[0]=Eidx;
         ref[1]=vlen;
         ref[2]=kbuf[kloc+6];
      };break;

         
      case kvpay://00101050 kvpay LO pay insurance plan identification
      {
         pay[0]=Eidx;
         pay[1]=vlen;
         pay[2]=kbuf[kloc+6];
      };break;

      case kvedesc://study description
      {
         edesc[0]=Eidx;
         edesc[1]=vlen;
         edesc[2]=kbuf[kloc+6];
      };break;

      case kvecode://study code
      {
         utf8(kbuf[kloc+6],Ebuf,Eidx,vlen,ecode,ecodelength,&utf8length);
         ecodelength+=utf8length;
         ecode[ecodelength++]='^';
         ecodecharset=kbuf[kloc+6];
      };break;

      default: return false;
   }
   Eidx+=vlen;
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

   if (fromStdin){if(edckvapi_fread(Sbuf+Sidx,1,vlen,stdin)!=vlen) return false;}
   else memcpy(Sbuf+Sidx, vbuf, vlen);//from vbuf
   switch (vrcat)
   {
      case kvsuid://SeriesInstanceUID
      {
         if (!ui2b64( Sbuf+Sidx, vlen - (Sbuf[Sidx+vlen-1] == 0), suidb64, &suidb64length )) return false;
      };break;

#pragma mark generic
      case kvUI://unique ID
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

      case kvsdoctitle://ST  DocumentTitle 00420010
      case kvscdaid://HL7InstanceIdentifier
         break;

#pragma mark special
      
      case kvsdate: sdate=atoi(Sbuf+Sidx);
         break;
      case kvstime: stime=atoi(Sbuf+Sidx);
           break;

      case kvsxml://OB Encapsulated​Document 00420011
      {
         //replace everything after last > with spaces
         vlenNoPadding=Sidx+vlen-1;
         while (Sbuf[vlenNoPadding]!='>')
         {
            printf("%d",Sbuf[vlenNoPadding]);
            
            Sbuf[vlenNoPadding]=' ';
            vlenNoPadding--;
         }
         sxml[0]=Sidx;
         sxml[1]=vlenNoPadding-Sidx+1;
         
      } break;

      case kvsnumber: snumber=atoi(Sbuf+Sidx)+0x8000;
           break;
         
      case kvsmod://Modality
      {
         smod[0]=Sidx;
         smod[1]=vlen;
      } break;

      case kvsdesc:
      {
         sdesc[0]=Sidx;
         sdesc[1]=vlen;
         sdesc[2]=kbuf[kloc+6];
      } break;

      default: return false;
   }
   Sidx+=vlen;
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


   if (fromStdin){if(edckvapi_fread(Ibuf+Iidx,1,vlen,stdin)!=vlen) return false;}
   else memcpy(Ibuf+Iidx, vbuf, vlen);//from vbuf
   switch (vrcat)
   {
#pragma mark UID
      case kviuid://SOPInstanceUID
      {
         if (!ui2b64( Ibuf+Iidx, vlen - (Ibuf[Iidx+vlen-1] == 0), iuidb64, &iuidb64length )) return false;
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
      case kvUI://unique ID
#pragma mark special
      case kvfo://OV 31 Extended​Offset​Table fragments offset 7FE00001
      case kvfl://OV 32 Extended​Offset​TableLengths fragments offset 7FE00002
      case kvft://UV 33 Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
           break;

      case kvinumber: inumber=atoi(Sbuf+Sidx)+0x8000;
           break;

      case kvianumber:ianumber=atoi(Sbuf+Sidx)+0x8000;//AcquisitionNumber
           break;

      default: return false;
   }
   Iidx+=vlen;
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
