// project: dicm2dckv
// targets: eDCKVinline
// file: dckvapi.m
// created by jacquesfauquex on 2024-04-04.

#include "edckvapi.h"
#include "opjcompress.h"

extern char *DICMbuf;
extern u64 DICMidx;
extern uint8_t *kbuf;
static u32 DICMlen;
static u16  syntaxidx;
static u16  classidx;

struct stat st={0};//for directory creation
static char relativepath[256];
static u8 relativepathlength=0;
static FILE *fileptr;


static sqlite3      *db;
static char         *dberr = 0;
static int           dbrc  = 0;//return code

static int stepreturnstatus;
static bool notRegistered;

static sqlite3_stmt *eblake3stmt;
static sqlite3_stmt *einsertstmt;
static int currentEpk;

static sqlite3_stmt *sblake3stmt;
static sqlite3_stmt *sinsertstmt;
static int currentSpk;

static sqlite3_stmt *iblake3stmt;
static sqlite3_stmt *iinsertstmt;
static int currentIpk;

static sqlite3_stmt *iframepksstmt;
static sqlite3_stmt *finsertstmt;

static sqlite3_stmt *tinsertstmt;

static u32 vlenNoPadding;

//buffers ...
//start with 0xFFFF size
//and is reallocated with same increment in size each time the current space is filled up
static char *Ebuf;
static char *Sbuf;
static char *Ibuf;
static char *Pbuf;
static char *Fbuf;

static u32 Eidx=0;
static u32 Sidx=0;
static u32 Iidx=0;
static u32 Pidx=0;

static u32 Emax=0;
static u32 Smax=0;
static u32 Imax=0;
static u32 Pmax=0;
static u32 Fmax=0;

static u64 Fcidx=0;
static u64 Ffidx=0;
static u64 Fhidx=0;
static u64 Foidx=0;
static u64 Feidx=0;

static u32 utf8length;
static char utf8bytes[256];

static u64 prefix=0x00;
static int iterator;
static blake3_hasher hasher;
static uint8_t hashbytes[BLAKE3_OUT_LEN];//32 bytes
static u8 *registeredhashbytes;//32 bytes


//prefix components
static u8  sversion=0;

#pragma mark TODO
static u8  rversion=0;
static u16 rnumber=0;
static u8  iversion=0;
static u8  concat=0;

#pragma mark -
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
      E("einsertstmt error: %s\n", dberr);
      return false;
   }
   sqlite3_reset(einsertstmt);
   return true;
}

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
static u32  smod[2];
static u32  sdesc[3];
//sframes
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
   
   sqlite3_bind_int(sinsertstmt, 10, classidx);//
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
      E("sinsertstmt error: %s\n", dberr);
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

//fk
//pk
static char iuidb64[44];
static u8   iuidb64length;
static u16  inumber;//unsigned because we sum up 0x8000
static u16  ianumber;//unsigned because we sum up 0x8000
//classidx
static char itype[64];
static u8   itypelength;
static char icomment[64];
static u8   icommentlength;
static u16  iframes;
//pdckv
static u16  spp;//sample per plane = components
static u16  photocode;
static u16  rows;
static u16  cols;
static u16  alloc;
static u16  stored;
static u16  high;
static u16  pixrep;
static u16  planar;//0 = RGB del pixel; 1 = componentes RGB

bool iinsert(u64 prefix)
{
   /*
    0:Sfk
         1:pk
    2:iuid,
    3:idckv,
    4:iblake3,
    5:inumber,
    6:ianumber,
    7:classidx,
    8:itype
    9:syntaxidx
    10:icomment
    11:iframes, (0:no frame objects, 1:native, n:encoded)
    12:pdckv (private attributes)
    13:spp 00280002 US
    14:photocode 00280004 CS https://dicom.innolitics.com/ciods/rt-dose/image-pixel/00280004
    15:rows 00280010 US
    16:cols 00280011 US
    17:alloc 00280100 US
    18:stored 00280101 US
    19:high 00280102 US
    20:pixrep 00280103 US
    21:planar 00280006 US
    22:DICMrelpath
    */
   // int pk
   sqlite3_bind_int(iinsertstmt,  1, currentSpk);
   sqlite3_bind_text(iinsertstmt, 2, iuidb64,iuidb64length, NULL);

   //prefix attributes
   iterator=0;
   while (iterator<Iidx)
   {
      memcpy(Ibuf+iterator+1,&prefix,8);
      iterator+=Ibuf[iterator]+1;
      iterator+=Ibuf[iterator]+(Ibuf[iterator+1]*0x10)+(Ibuf[iterator+2]*0x100)+(Ibuf[iterator+3]*0x1000)+4;
   }
   sqlite3_bind_blob(iinsertstmt, 3, Ibuf, Iidx, NULL);//dckv

   sqlite3_bind_blob(iinsertstmt, 4, hashbytes,BLAKE3_OUT_LEN, NULL);
   sqlite3_bind_int(iinsertstmt,  5, inumber);
   sqlite3_bind_int(iinsertstmt,  6, ianumber);
   sqlite3_bind_int(iinsertstmt,  7, classidx);

   utf8(0,itype,0,itypelength,utf8bytes,0,&utf8length);
   sqlite3_bind_text(iinsertstmt, 8, utf8bytes,utf8length, SQLITE_TRANSIENT);
   itypelength=0;

   sqlite3_bind_int(iinsertstmt,  9, syntaxidx);

   utf8(0,icomment,0,icommentlength,utf8bytes,0,&utf8length);
   sqlite3_bind_text(iinsertstmt, 10, utf8bytes,utf8length, SQLITE_TRANSIENT);
   icommentlength=0;
   
   sqlite3_bind_int(iinsertstmt,  11, iframes);
   
   if (Pidx>0)//12
   {
      //prefix attributes
      iterator=0;
      while (iterator<Pidx)
      {
         memcpy(Pbuf+iterator+1,&prefix,8);
         iterator+=Pbuf[iterator]+1;
         iterator+=Pbuf[iterator]+(Pbuf[iterator+1]*0x10)+(Pbuf[iterator+2]*0x100)+(Pbuf[iterator+3]*0x1000)+4;
      }
      sqlite3_bind_blob(iinsertstmt, 12, Pbuf, Pidx, NULL);//dckv
   }

   sqlite3_bind_int( iinsertstmt, 13, spp);
   sqlite3_bind_int( iinsertstmt, 14, photocode);
   sqlite3_bind_int( iinsertstmt, 15, rows);
   sqlite3_bind_int( iinsertstmt, 16, cols);
   sqlite3_bind_int( iinsertstmt, 17, alloc);
   sqlite3_bind_int( iinsertstmt, 18, stored);
   sqlite3_bind_int( iinsertstmt, 19, high);
   sqlite3_bind_int( iinsertstmt, 20, pixrep);
   sqlite3_bind_int( iinsertstmt, 21, planar);
   sqlite3_bind_text(iinsertstmt, 22, relativepath,relativepathlength+iuidb64length, SQLITE_TRANSIENT);

   //finalize
   dbrc = sqlite3_step(iinsertstmt);
   if (dbrc != SQLITE_DONE )
   {
      E("iinsertstmt error: %s\n", dberr);
      return false;
   }
   sqlite3_reset(iinsertstmt);

   //get pk
   sqlite3_reset(iblake3stmt);
   sqlite3_bind_text(iblake3stmt, 1, iuidb64,iuidb64length, NULL);
   stepreturnstatus = sqlite3_step(iblake3stmt);
   currentIpk=sqlite3_column_int(iblake3stmt, 0);
   if (stepreturnstatus != SQLITE_ROW) return false;
   D("sqlite commit I pk: %d",currentIpk);
   return true;
}

static u16 fnumber;
static u64 cidx=0;
static u64 fidx=0;
static u64 hidx=0;
static u64 oidx=0;
static u64 zidx=0;

bool finsert(u64 prefix)
{
   /*
    0:Ifk
      1:pk
    2:fnumber
    3:DICMidx
    4:DICMlen
    5:syntaxidx
    6:compressed
    7:fast
    8:high
    9:original
    
    */
   // int pk
   sqlite3_bind_int( finsertstmt, 1, currentIpk);
   sqlite3_bind_int( finsertstmt, 2, fnumber);
   
#pragma mark TODO   add prefix
   sqlite3_bind_int( finsertstmt, 3, (int)DICMidx-DICMlen);
   sqlite3_bind_int( finsertstmt, 4, DICMlen);
   sqlite3_bind_int( finsertstmt, 5, syntaxidx);
   sqlite3_bind_blob(finsertstmt, 6, DICMbuf+cidx,(u32)(fidx-cidx), NULL);
   sqlite3_bind_blob(finsertstmt, 7, DICMbuf+fidx,(u32)(hidx-fidx), NULL);
   sqlite3_bind_blob(finsertstmt, 8, DICMbuf+hidx,(u32)(oidx-hidx), NULL);
   sqlite3_bind_blob(finsertstmt, 9, DICMbuf+oidx,(u32)(zidx-oidx), NULL);

   //finalize
   dbrc = sqlite3_step(finsertstmt);
   if (dbrc != SQLITE_DONE )
   {
      E("finsertstmt error: %s\n", dberr);
      return false;
   }
   sqlite3_reset(finsertstmt);
   return true;
}


bool tinsert(void)
{
   /*
    0:Ifk
      1:pk
    2:blob BLOB
    */
   // int pk
   sqlite3_bind_int( tinsertstmt, 1, currentIpk);
   sqlite3_bind_blob(tinsertstmt, 2, DICMbuf+DICMidx-DICMlen,DICMlen, NULL);

   //finalize
   dbrc = sqlite3_step(tinsertstmt);
   if (dbrc != SQLITE_DONE )
   {
      E("tinsertstmt error: %s\n", dberr);
      return false;
   }
   sqlite3_reset(tinsertstmt);
   return true;
}
bool EDKVcreate(
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
   classidx=soidx;//class
   syntaxidx=stidx;//transfer syntax

#pragma mark sqlite
   I("sqlite %s\n", sqlite3_libversion());

   sqlite3 *db;
   //char *dberr = 0;
   int   dbrc  = 0;//return code
   
   if (stat("edckv.db", &st)==-1)
   {
      dbrc = sqlite3_open_v2("edckv.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
      if (dbrc == SQLITE_OK)
      {
#pragma mark todo create default db from a dedicated file in the  project
      }
   }
   else dbrc = sqlite3_open_v2("edckv.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
   
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   
#pragma mark E sqlite stmt(s)
//blake3 stmt
   char eblake3[]="SELECT pk, eblake3 FROM E WHERE euid=?;";
   dbrc = sqlite3_prepare_v2(db, eblake3, sizeof(eblake3), &eblake3stmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot initialize eblake3stmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   char einsert[] = "INSERT INTO E(procid,edate,euid,edckv,eblake3,pname,pide,pidr,pbirth,psex,eid,ean,eal,eau,eat,img,cda,req,ref,pay,edesc,ecode) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
   dbrc=sqlite3_prepare(db, einsert, -1, &einsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare einsertStmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }

   
#pragma mark S sqlite stmt(s)
//blake3 stmt
   char sblake3[]="SELECT pk, sblake3 FROM S WHERE suid=?;";
   dbrc = sqlite3_prepare_v2(db, sblake3, sizeof(sblake3), &sblake3stmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot initialize Sblake3stmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   char sinsert[] = "INSERT INTO S(Efk,sdate,stime,suid,sdckv,sblake3,sxml,spdf,snumber,sclass,smod,sdesc,sicon,sframes) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
   dbrc=sqlite3_prepare(db, sinsert, -1, &sinsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare sinsertStmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }


   
#pragma mark I+P sqlite stmt(s)
//blake3 stmt
   char iblake3[]="SELECT pk, iblake3 FROM I WHERE iuid=?;";
   dbrc = sqlite3_prepare_v2(db, iblake3, sizeof(iblake3), &iblake3stmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot initialize iblake3stmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   char iinsert[] = "INSERT INTO I(Sfk,iuid,idckv,iblake3,inumber,ianumber,iclass,itype,syntaxidx,icomment,iframes,pdckv,spp,photocode,rows,cols,alloc,stored,high,pixrep,planar,DICMrelpath) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
   dbrc=sqlite3_prepare(db, iinsert, -1, &iinsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare iinsertStmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }

   
   
#pragma mark F sqlite stmt(s)
//existing one
   /*
    Ifk
    pk
    fnumber
    fdckv BLOB
    DICMidx
    DICMlen
    syntaxidx
    compressed
    fast
    high
    original
    */
   char iframepks[]="SELECT pk, fnumber FROM F WHERE Ifk=?;";
   dbrc = sqlite3_prepare_v2(db, iframepks, sizeof(iframepks), &iframepksstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot initialize iframepksstmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   char finsertchars[] = "INSERT INTO F(Ifk,fnumber,DICMidx,DICMlen,syntaxidx,compressed,fast,high,original) VALUES(?,?,?,?,?,?,?,?,')";
   dbrc=sqlite3_prepare(db, finsertchars, -1, &finsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare finsertstmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   /*
   char fcinsertchars[] = "INSERT INTO F(Ifk,fnumber,fdckv,nativeurl,syntaxidx,compressed) VALUES(?,?,?,?,?,?)";
   dbrc=sqlite3_prepare(db, finsertchars, -1, &finsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare finsertstmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   char fcfhoinsertchars[] = "INSERT INTO F(Ifk,fnumber,fdckv,nativeurl,syntaxidx,compressed,fast,high,original) VALUES(?,?,?,?,?,?,?,?,?)";
   dbrc=sqlite3_prepare(db, finsertchars, -1, &finsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare finsertstmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
   */

   
   
#pragma mark T sqlite stmt(s)
//existing one
   /*
    Ifk
    pk
    blob BLOB
    */
   /*
   char iframepks[]="SELECT pk, fnumber FROM F WHERE Ifk=?;";
   dbrc = sqlite3_prepare_v2(db, iframepks, sizeof(iframepks), &iframepksstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot initialize iframepksstmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }
    */
   char tinsertchars[] = "INSERT INTO T(Ifk,blob) VALUES(?,?)";
   dbrc=sqlite3_prepare(db, tinsertchars, -1, &tinsertstmt, 0);
   if (dbrc != SQLITE_OK)
   {
      E( "Cannot prepare tinsertstmt: %s\n", sqlite3_errmsg(db));
      sqlite3_close_v2(db);
      exit(1);
   }

#pragma mark pid dir
   sprintf(relativepath, "%d", getpid());
   relativepathlength=intdecsize(getpid());
   relativepath[relativepathlength++]='/';
   relativepath[relativepathlength]=0x00;
   if ((stat(relativepath, &st)==-1) && (mkdir(relativepath, 0777)==-1)) return false;
   
#pragma mark resets
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
   
   if (Fmax==0) {
         Fmax=0x3000000;
         Fbuf=malloc(Fmax);
   }
   iframes=0;

   return true;
}


#pragma mark -



bool morebuf(enum EDKVfamily f, u32 vlen)
{
   char *newbuf;
   switch (f) {
      case EDKV:{
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
      case SDKV:{
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
      case PDKV:{
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
      case IDKV:{
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
      default:
         return false;
         break;
   }
   return true;
}



#pragma mark -

u64 sqliteESIP(void)
{
   //append ?offset=xxxx&len=yyy for each frame url
   
#pragma mark filepath
   if (edate==0)
   {
      time_t rawtime;
      time ( &rawtime );
      struct tm * timeinfo;
      timeinfo = localtime ( &rawtime );
      strftime(relativepath+relativepathlength,8,"%Y%m%d", timeinfo);
   }
   else sprintf(relativepath+relativepathlength, "%d", edate);
   relativepathlength+=8;
   relativepath[relativepathlength++]='/';
   relativepath[relativepathlength]=0x00;
   if ((stat(relativepath, &st)==-1) && (mkdir(relativepath, 0777)==-1)) return false;

   //euidb64
   if (euidb64length==0) return false;
   memcpy(relativepath+relativepathlength, euidb64, euidb64length);
   relativepathlength+=euidb64length;
   relativepath[relativepathlength++]='/';
   relativepath[relativepathlength]=0x00;
   if ((stat(relativepath, &st)==-1) && (mkdir(relativepath, 0777)==-1)) return false;
   //iuidb64
   if (iuidb64length==0) return false;
   memcpy(relativepath+relativepathlength, iuidb64, iuidb64length);
   relativepath[relativepathlength+iuidb64length]=0;


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
         if (!sinsert(0x10|sversion|u16swap(snumber)*0x100|rversion*0x1000000|u16swap(rnumber)*0x100000000|u16swap(classidx)*0x100000000000000)) return false;
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
            if (!sinsert(0x10|sversion|u16swap(snumber)*0x100|rversion*0x1000000|u16swap(rnumber)*0x100000000|u16swap(classidx)*0x100000000000000)) return false;
         }
      }
      //reset (not snumber y sversion que se usan para I N C)
      sqlite3_reset(sblake3stmt);
      Sidx=0;
   }
   
#pragma mark IP : 2/3 s SS SS iu II II CC CC
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
         if (!iinsert(0x20|sversion|u16swap(snumber)*0x100|iversion*0x100000|concat*0x1000000|u16swap(inumber)*0x100000000|u16swap(classidx)*0x100000000000000)) return false;
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
            if (!iinsert(0x20|sversion|u16swap(snumber)*0x100|iversion*0x100000|concat*0x1000000|u16swap(inumber)*0x100000000|u16swap(classidx)*0x100000000000000)) return false;
         }
      }
      sqlite3_reset(iblake3stmt);
      Iidx=0;
      Pidx=0;
   }
   return true;
}
bool EDKVcommit(bool hastrailing)
{
   if (relativepathlength<10) sqliteESIP();//no pixel handler called the method
   if (hastrailing)
   {
      I("%s","hastrailing");
      tinsert();
   }
#pragma mark write DICM
   relativepath[relativepathlength+iuidb64length]=0x00;
   fileptr=fopen(relativepath, "w");
   if (fileptr == NULL) return false;
   if (fwrite(DICMbuf ,1, DICMidx, fileptr)!=DICMidx) return false;
   fclose(fileptr);
   snumber=0;
   inumber=0;
   return _DKVclose();
}


bool EDKVclose(void)
{
   sqlite3_finalize(eblake3stmt);
   sqlite3_finalize(einsertstmt);
   sqlite3_free(dberr);
   sqlite3_close(db);
   DICMidx=0;
   return true;
}

#pragma mark -

bool appendEDKV(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Eidx > Emax) && !morebuf(EDKV,vlen)) return false;

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
   if (!_DKVfread(vlen)) return false;
   memcpy(Ebuf+Eidx, DICMbuf+DICMidx-vlen, vlen);
   switch (vrcat)
   {
      case kveuid: {//StudyInstanceUID
         if (!ui2b64( Ebuf+Eidx, vlen - (Ebuf[Eidx+vlen-1] == 0), euidb64, &euidb64length )) return false;
      };break;
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
      case kvedate: edate=atoi(Ebuf+Eidx);break;
      case kvpname: {//Patient name
         pname[0]=Eidx;
         pname[1]=vlen;
         pname[2]=kbuf[kloc+6];
      };break;
      case kvpide: {//Patient id extension
         pide[0]=Eidx;
         pide[1]=vlen;
         pide[2]=kbuf[kloc+6];
      };break;
      case kvpidr: {//Patient root id issuer
         pidr[0]=Eidx;
         pidr[1]=vlen;
         pidr[2]=kbuf[kloc+6];
      };break;
      case kvpbirth: pbirth=atoi(Ebuf+Eidx); break;
      case kvpsex: {//Patient sex
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
      case kveid: {//StudyID
         eid[0]=Eidx;
         eid[1]=vlen;
         eid[2]=kbuf[kloc+6];
      };break;
      case kvean: {//AccessionNumber
         ean[0]=Eidx;
         ean[1]=vlen;
         ean[2]=kbuf[kloc+6];
      };break;
      case kveal: {//AccessionNumberIssuer local 00080051.00400031
         eal[0]=Eidx;
         eal[1]=vlen;
         eal[2]=kbuf[kloc+6];
      };break;
      case kveau: {//AccessionNumberIssuer universal 00080051.00400032
         eau[0]=Eidx;
         eau[1]=vlen;
         eau[2]=kbuf[kloc+6];
      };break;
      case kveat: {//AccessionNumberType
         eat[0]=Eidx;
         eat[1]=vlen;
         eat[2]=kbuf[kloc+6];
      };break;
      case kvimg: {//InstitutionName (placed in exam instead of series)
         img[0]=Eidx;
         img[1]=vlen;
         img[2]=kbuf[kloc+6];
      };break;
      case kvcda: {//study CDA (reading)
         cda[0]=Eidx;
         cda[1]=vlen;
         cda[2]=kbuf[kloc+6];
      };break;
      case kvreq: {//study requesting
         req[0]=Eidx;
         req[1]=vlen;
         req[2]=kbuf[kloc+6];
      };break;
      case kvref: {//study referring
         ref[0]=Eidx;
         ref[1]=vlen;
         ref[2]=kbuf[kloc+6];
      };break;
      case kvpay: {//00101050 kvpay LO pay insurance plan identification
         pay[0]=Eidx;
         pay[1]=vlen;
         pay[2]=kbuf[kloc+6];
      };break;
      case kvedesc: {//study description
         edesc[0]=Eidx;
         edesc[1]=vlen;
         edesc[2]=kbuf[kloc+6];
      };break;
      case kvecode: {//study code
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

bool appendSDKV(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Sidx > Smax) && !morebuf(SDKV,vlen)) return false;

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
   if (!_DKVfread(vlen)) return false;
   memcpy(Sbuf+Sidx, DICMbuf+DICMidx-vlen, vlen);
   switch (vrcat)
   {
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
      case kvsuid: {//SeriesInstanceUID
         if (!ui2b64( Sbuf+Sidx, vlen - (Sbuf[Sidx+vlen-1] == 0), suidb64, &suidb64length )) return false;
      };break;
      case kvsdate: sdate=atoi(Sbuf+Sidx); break;
      case kvstime: stime=atoi(Sbuf+Sidx); break;
      case kvsdocument: {//OB Encapsulated​Document 00420011
         //replace everything after last > with spaces
         vlenNoPadding=Sidx+vlen-1;
         while (Sbuf[vlenNoPadding]!='>')
         {
            D("document tail (%d) %02X\n",vlenNoPadding,Sbuf[vlenNoPadding]);
            
            Sbuf[vlenNoPadding]=' ';
            vlenNoPadding--;
         }
         sxml[0]=Sidx;
         sxml[1]=vlenNoPadding-Sidx+1;
         
      } break;
      case kvsnumber: snumber=atoi(Sbuf+Sidx)+0x8000; break;
      case kvsmod: {//Modality
         smod[0]=Sidx;
         smod[1]=vlen;
      } break;
      case kvsdesc: {
         sdesc[0]=Sidx;
         sdesc[1]=vlen;
         sdesc[2]=kbuf[kloc+6];
      } break;
      default: return false;
   }
   Sidx+=vlen;
   return true;
}

bool appendPDKV(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Pidx > Pmax) && !morebuf(PDKV,vlen)) return false;
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
   if (!_DKVfread(vlen)) return false;
   memcpy(Pbuf+Pidx, DICMbuf+DICMidx-vlen, vlen);
   Pidx+=vlen;
   return true;
}

bool appendIDKV(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Iidx > Imax) && !morebuf(IDKV,vlen)) return false;

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
   if (!_DKVfread(vlen)) return false;
   memcpy(Ibuf+Iidx, DICMbuf+DICMidx-vlen, vlen);
   switch (vrcat)
   {
      case kvFD://floating point double
      case kvFL://floating point single
      case kvSL://signed long
      case kvUS://unsigned short
      case kvSS://signed short
      case kvUL://unsigned long
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
      case kvfo://OV 31 Extended​Offset​Table fragments offset 7FE00001
      case kvfl://OV 32 Extended​Offset​TableLengths fragments offset 7FE00002
      case kvft://UV 33 Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
           break;
      case kviuid: {//SOPInstanceUID
         if (!ui2b64( Ibuf+Iidx, vlen - (Ibuf[Iidx+vlen-1] == 0), iuidb64, &iuidb64length )) return false;
      };break;
      case kvitype: break;
      case kvspp:spp=Ibuf[Iidx]|(Ibuf[Iidx+1]<<8);break;// 00280002 US
      case kvframesnumber:iframes=atoi(Ibuf+Iidx);break;// 00280008 IS
      case kvrows:rows=Ibuf[Iidx]|(Ibuf[Iidx+1]<<8);break;// 00280010 US
      case kvcols:cols=Ibuf[Iidx]|(Ibuf[Iidx+1]<<8);break;// 00280011 US
      case kvalloc:alloc=Ibuf[Iidx]|(Ibuf[Iidx+1]<<8);break;// 00280100 US
      case kvstored:stored=Ibuf[Iidx]|(Ibuf[Iidx+1]<<8);break;// 00280101 US
      case kvhigh:high=Ibuf[Iidx]|(Ibuf[Iidx+1]<<8);break;// 00280102 US
      case kvpixrep:pixrep=Ibuf[Iidx]|(Ibuf[Iidx+1]<<8);break;// 00280103 US
      case kvplanar:planar=Ibuf[Iidx]|(Ibuf[Iidx+1]<<8);break;// 00280106 US
//10:icomment B00204000=0x00402000;//LT compression desc (image comment)
      case kvicomment: break;
//14:photocode 00280004 CS https://dicom.innolitics.com/ciods/rt-dose/image-pixel/00280004
      case kvphotocode: break;
      case kvinumber: inumber=atoi(Ibuf+Iidx)+0x8000; break;
      case kvianumber:ianumber=atoi(Ibuf+Iidx)+0x8000;//AcquisitionNumber
           break;
      default: return false;
   }
   Iidx+=vlen;
   return true;
}

#pragma mark - alternative calls
//https://dicom.nema.org/medical/dicom/current/output/html/part05.html#sect_A.4


bool appendpixelOF(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
   I("%s","appendpixelOF");
     return false;
}

bool appendpixelOD(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
   I("%s","appendpixelOD");
     return false;
}


bool appendpixelOB(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Iidx > Imax) && !morebuf(IDKV,vlen)) return false;

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

   if (iframes==0)iframes=1;
   if (!sqliteESIP()) return false;//create sql for E,S,I,P

   for (fnumber=1;fnumber <= iframes;fnumber++)
   {
      //standarize pixel representation (per component, unsigned int LE
      DICMlen=cols * rows * spp;
      if (!_DKVfread(DICMlen)) return false;
      cidx=DICMidx;
      //write 4times bigger normalized data after the read in buffer
      if (pixrep) //signed
      {
         if ((spp==1)||planar)
         {
            for (u64 i=DICMidx - DICMlen; i < DICMidx; i++)
            {
               DICMbuf[cidx++]=(char)DICMbuf[i];
               DICMbuf[cidx++]=0;
               DICMbuf[cidx++]=0;
               DICMbuf[cidx++]=0;
            }
         }
         else //multi comp pixels
         {
            u64 j;
            u64 compsize=cols * rows;
            for (u64 i=DICMidx - DICMlen; i < DICMidx; i+=spp)
            {
               for (j=0; j<spp; j++)
               {
                  DICMbuf[cidx+(compsize*j)]=(char)DICMbuf[i];
                  DICMbuf[cidx+(compsize*j)+1]=0;
                  DICMbuf[cidx+(compsize*j)+2]=0;
                  DICMbuf[cidx+(compsize*j)+3]=0;
               }
               cidx++;
            }
            cidx+=cols * rows * (spp -1);
         }
      }
      else //unsigned
      {
         if ((spp==1)||planar)
         {
            for (u64 i=DICMidx - DICMlen; i < DICMidx; i++)
            {
               DICMbuf[cidx++]=DICMbuf[i];
               DICMbuf[cidx++]=0;
               DICMbuf[cidx++]=0;
               DICMbuf[cidx++]=0;
            }
         }
         else //multi comp pixels
         {
            u64 j;
            u64 compsize=cols * rows;
            for (u64 i=DICMidx - DICMlen; i < DICMidx; i+=spp)
            {
               for (j=0; j<spp; j++)
               {
                  DICMbuf[cidx+(compsize*j)]=DICMbuf[i];
                  DICMbuf[cidx+(compsize*j)+1]=0;
                  DICMbuf[cidx+(compsize*j)+2]=0;
                  DICMbuf[cidx+(compsize*j)+3]=0;
               }
               cidx++;
            }
            cidx+=cols * rows * (spp -1);
         }
       }

      /*
      9:syntaxidx
      11:iframes, (0:no frame objects, 1:native, n:encoded)
      13:spp
      14:photocode
      15:rows
      16:cols
      17:alloc
      18:stored
      19:high
      20:pixrep
      21:planar
      */

      //compression cfho
      u64 fidx=0;//fast offset
      u64 hidx=0;//high offset
      u64 oidx=0;//original offset
      u64 zidx=0;//first byte after original
      if (!opj_cfho(photocode ,spp,rows,cols,stored,DICMidx,cidx,&fidx,&hidx,&oidx,&zidx))
      {
         E("%s","error");
      }
      
      
      
      //4 s SS SS iu II II FF FF
      if (!finsert(0x40|sversion|u16swap(snumber)*0x100|iversion*0x100000|concat*0x1000000|u16swap(inumber)*0x100000000|u16swap(fnumber)*0x100000000000000)) return false;
   }
   /*
     call compress for each frame
    
    bool finsert(u64 prefix)
    {
        0:Ifk
          1:pk
        2:fnumber
        //3:fdckv BLOB
        4:DICMidx
        5:DICMlen
        6:syntaxidx
 
    */
   if (!_DKVfread(vlen)) return false;

   
   
   
   memcpy(Ibuf+Iidx, DICMbuf+DICMidx-vlen, vlen);
   Iidx+=vlen;
   return true;

   
   
   
   
   
   
   

   return true;   D("%s","appendpixelOB");

   /*
    put first E S P I to sqlite to get pk
    trailing padding is another dependance of an instance
    
    */
   if (!sqliteESIP())return false;
   
   //freeing buffer necesary?
   //if ((vlen + 21 + kloc + Fidx > Fmax) && !morebuf(FDKV,vlen)) return false;

   //key length = key path length + 8 prefix + 8 current attribute
   //idx increased by 1
/*
   Fbuf[Fidx++]=kloc+16;
   
   //prefix
   memcpy(Fbuf+Fidx, &prefix, 8);
   Fidx+=8;
   
   //key
   memcpy(Fbuf+Fidx, kbuf, kloc+8);
   Fidx+=kloc+8;
   
   //value length
   memcpy(Fbuf+Fidx, &vlen, 4);
   Fidx+=4;
   if (vlen==0) return true;
   if (!_DKVfread(vlen)) return false;
   memcpy(Fbuf+Fidx, DICMbuf+DICMidx-vlen, vlen);
   Fidx+=vlen;
*/
   return false;
}

bool appendpixelOW(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
   if (iframes==0)iframes=1;
   if (!sqliteESIP())return false;
   /*
   13:spp
   14:photocode
   15:rows
   16:cols
   17:alloc
   18:stored
   19:high
   20:pixrep
   21:planar
   */
   
   if ( (spp==1) && (alloc==16) )
   {
      //always explicit
      D("%s","appendpixelOW");
      /*not compressed one channel */
      for (fnumber=1;fnumber <= iframes;fnumber++)
      {
         DICMlen=cols * rows * 2;
         if (!_DKVfread(DICMlen)) return false;
        //4 s SS SS iu II II FF FF
         if (!finsert(0x40|sversion|u16swap(snumber)*0x100|iversion*0x100000|concat*0x1000000|u16swap(inumber)*0x100000000|u16swap(fnumber)*0x100000000000000)) return false;
      }
   }
   else
   {
      E("%s","appendpixelOW");
      return false;
   }
   return true;
}

bool appendpixelOL(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
   if (vlen!=0xFFFFFFFF) return false; //0x7FE00010 fragments vlen is undefined

      /*
       our codification does not follow the standard
       u32 number of fragments
       u32 offset of fragment (repeated)
       u32 offset after last fragment
       fragments data
       */
      
/*
      u64 fragmentbytes=0;
      struct t4l4 *fragmentstruct=(struct t4l4*) &fragmentbytes;
      
      
      //fragmento 0
      if (EDKVfread(&fragmentbytes, 1, 8, stdin)!=8) return false;
      if (fragmentstruct->t != 0xe000fffe) return false;
      //read (and ignore) neventually existing table
      if (   (fragmentstruct->l > 0)
          && (EDKVfread(vbuf,1,fragmentstruct->l,stdin) != fragmentstruct->l)
         ) return false;
      vloc+=20+fragmentstruct->l;

      
      //write iBuffer and reset index
      //fwriteIfile(false);
      
      //read fragment 1
      if (EDKVfread(&fragmentbytes, 1, 8, stdin)!=8) return false;
      
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
               if (EDKVfread(vbuf,1,0xFFFE,stdin)!=0xFFFE) return false;
#pragma mark TODO                               if (fwrite(vbuf ,1, 0xFFFE , Ifile)!=0xFFFE) return false;
               bytesremaing-=0xFFFE;
            }
            if (bytesremaing > 0)
            {
               if (EDKVfread(vbuf,1,bytesremaing,stdin)!=bytesremaing) return false;
#pragma mark TODO                               if (fwrite(vbuf ,1, bytesremaing , Ifile)!=bytesremaing) return false;
            }

         }
         vloc+=8+fragmentstruct->l;//174674 en lugar de 172954 (dif 1720)
         if (EDKVfread(&fragmentbytes, 1, 8, stdin)!=8) return false;
      }
*/
   return false;
}

bool appendpixelOV(int kloc,enum kvVRcategory vrcat,u32 vlen)
{
/*
   //freeing buffer necesary?
   if ((vlen + 21 + kloc + Fidx > Fmax) && !morebuf(FDKV,vlen)) return false;

   //key length = key path length + 8 prefix + 8 current attribute
   //idx increased by 1
   Fbuf[Fidx++]=kloc+16;

   //prefix
   memcpy(Fbuf+Fidx, &prefix, 8);
   Fidx+=8;
   
   //key
   memcpy(Fbuf+Fidx, kbuf, kloc+8);
   Fidx+=kloc+8;
   
   //value length
   memcpy(Fbuf+Fidx, &vlen, 4);
   Fidx+=4;
   if (vlen==0) return true;
   //value with contents
   switch (vrcat)
   {
#pragma mark UID
      case kvUI://unique ID (transfert syntax)
      {
         if (fromStdin){if(EDKVfread(Fbuf+Fidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Fbuf+Fidx, vbuf, vlen);//from vbuf
         Fidx+=vlen;
      };break;
         
#pragma mark generic
      case kvUS://unsigned short
      case kvTS://LO LT SH ST 19 text short charset
      case kv01://OB OD OF OL OV OW SV UV
#pragma mark special
      case kvnativeOB://40 0x7FE00010: //OB
      case kvnativeOW://41 0x7FE00010: //OW
      case kvnativeOF://42 0x7FE00008: //OF float
      case kvnativeOD://43 0x7FE00009: //OD double
      {
         if (fromStdin){if(EDKVfread(Fbuf+Fidx,1,vlen,stdin)!=vlen) return false;}
         else memcpy(Fbuf+Fidx, vbuf, vlen);//from vbuf
         Fidx+=vlen;
      };break;

      default: return false;
   }
 */
   return false;
}


#pragma mark - trailing padding attribute
