//
//  dckvapi.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-04-04.
//
#include <Foundation/Foundation.h>

#include "dckvapi.h"
#include "mdbx.h"
#include "log.h"


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




#pragma mark tx

static int rc;//mdbx return code
static MDBX_env *env=NULL;
static MDBX_txn *txn = NULL;
static MDBX_dbi dbi = 0;
static MDBX_cursor *cursor = NULL;
static char *dbpath;

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
)
{
#define FLAGS (DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN|DB_INIT_MPOOL|DB_CREATE|DB_THREAD)

  rc = mdbx_env_create(&env);
  if (rc != MDBX_SUCCESS) {
     E("mdbx_env_create: (%d) %s\n", rc, mdbx_strerror(rc));
     return false;
  }
   
  dbpath=malloc(0xFF);
  strcat(dbpath,dstdir);
  strcat(dbpath, "/");
  char *ibuf = malloc(*silen);
  memcpy(ibuf, vbuf+*siloc, *silen);
  strcat(dbpath, ibuf);
  rc = mdbx_env_open(env, dbpath,
                     MDBX_NOSUBDIR | MDBX_COALESCE | MDBX_LIFORECLAIM, 0664);
  if (rc != MDBX_SUCCESS) {
     E("mdbx_env_open: (%d) %s\n", rc, mdbx_strerror(rc));
     return false;
  }
  rc = mdbx_txn_begin(env, NULL, 0, &txn);
  if (rc != MDBX_SUCCESS) {
     E("mdbx_txn_begin: (%d) %s\n", rc, mdbx_strerror(rc));
     return false;
  }
  rc = mdbx_dbi_open(txn, NULL, 0, &dbi);
  if (rc != MDBX_SUCCESS) {
     E("mdbx_dbi_open: (%d) %s\n", rc, mdbx_strerror(rc));
     closetx(siidx);
     return false;
  }
   rc = mdbx_cursor_open(txn, dbi, &cursor);
   if (rc != MDBX_SUCCESS) {
      E("mdbx_cursor_open: (%d) %s\n", rc, mdbx_strerror(rc));
      closetx(siidx);
      return false;
   }
   I("#%d",*siidx);
   return true;
}


bool committx(sint16 *siidx)
{
   rc = mdbx_txn_commit(txn);
   if (rc)
   {
      E("mdbx_txn_commit: (%d) %s\n", rc, mdbx_strerror(rc));
      closetx(siidx);
      return false;
   }
   else
   {
      D("%s","txn commit");
#pragma mark move db to final dest
      closetx(siidx);
   }
   return true;
}


bool closetx(sint16 *siidx)
{
   if (cursor)
     mdbx_cursor_close(cursor);
   if (dbi)
   {
      mdbx_dbi_close(env, dbi);
      D("%s","close dbi");
   }
   if (txn)
   {
      mdbx_txn_abort(txn);
      D("%s","close txn");
   }
   if (env)
   {
      mdbx_env_close(env);
      D("%s","close env");
   }
   I("!#%d",*siidx);
   (*siidx)++;

   return true;
}

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
   MDBX_val key;
   key.iov_len = kloc + 8;
   key.iov_base = kbuf;
   MDBX_val data;
   
   if (vlen > 0xFFFE) //requires new temporary buffer
   {
      data.iov_len = vlen;
      char databytes[vlen];
      if (!(fread(databytes,1,vlen,stdin)!=vlen))
      {
         E("vll put read vl:%lu",vlen);
         return false;
      }
      data.iov_base = databytes;
   }
   else
   {
      if (fromStdin)
      {
         if ((vlen > 0) && !(fread(vbuf,1,vlen,stdin)!=vlen))
         {
            E("put read vl:%lu",vlen);
            return false;
         }
         data.iov_len = vlen;
         data.iov_base = vbuf;
      }
      else //from vbuf
      {
         data.iov_len = vlen;
         data.iov_base = vbuf;

      }
   }
  
   
   int putResult=mdbx_put(txn,dbi,&key,&data,MDBX_APPEND);
   if (putResult > 0)
   {
      E("put buf vl %lu",vlen);
      return false;
   }

   
   return true;
}
