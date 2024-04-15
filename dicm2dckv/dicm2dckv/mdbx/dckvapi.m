//
//  dckvapi.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-04-04.
//
#include <Foundation/Foundation.h>

#include "dckvapi.h"
#include "ODLog.h"

#pragma mark tx
bool urltx(NSURL* url)
{
   //url para todos los db kv. Abre una tx.
   return false;
}


bool committx(void)
{
   //aplica a todos los kv
   return false;
}


bool canceltx(void)
{
   //aplica a todos los kv
   return false;
}


#pragma mark - db

//apertura kv
bool createdb(enum kvDBcategory kvdb)
{
   //0 -> no se creó, o ya existe, always kvCoerce mode
   return true;
}
#pragma mark todo (void*)&SZbytes)


bool reopendb(enum kvDBcategory kvdb)
{
   //0 -> no estaba abierto o no se pudo reabrir
   return false;
}


bool existsdb(enum kvDBcategory kvdb)
{
   return false;
}


#pragma mark - cw
//operaciones exclusivas para primera creación
//requiere que todas las enmiendas este clasificadas por key ascendientes
bool appendkv(
              enum kvDBcategory  kvdb,
              uint8_t            *kbuf,
              int                klen,
              BOOL               vll,
              enum kvVRcategory  vrcat,
              char               *vurl,
              unsigned long long vloc,
              unsigned long      vlen,
              NSInputStream      *vstream,
              uint8_t            *buFFFF
              )
{
   return false;
}

#pragma mark - ow
//operaciones de escritura sobre db preexistente reabierta



bool coercekv(
              enum kvDBcategory  kvdb,
              uint8_t            *kbuf,
              int                klen,
              uint8_t            *vbuf,
              unsigned long long vlen
              )
{
   return false;
}

bool coercek8v(
               enum kvDBcategory  kvdb,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long vlen
               )
{
   return false;
}


bool supplementkv(
                  enum kvDBcategory  kvdb,
                  uint8_t            *kbuf,
                  int                klen,
                  uint8_t            *vbuf,
                  unsigned long long vlen
                 )
{
   return false;
}


bool supplementk8v(
                   enum kvDBcategory  kvdb,
                   uint64             k8,
                   uint8_t            *vbuf,
                   unsigned long long vlen
                   )
{
   return false;
}



//operaciones remove (vlen is a pointer)
//requieren vbuf de 0xFFFFFFFF length,
//en el cual se escribe el valor borrado
//vlen máx 0xFFFFFFFF indica que el key no existía
bool removetkv(
               enum kvDBcategory  kvdb,
               uint8_t            *kbuf,
               int                klen,
               uint8_t            *vbuf,
               unsigned long long *vlen
              )
{
   return false;
}


bool removek8v(
               enum kvDBcategory  kvdb,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long *vlen
               )
{
   return false;
}

/*
id env
id db
id tx

int rc;

MDBX_env *env = NULL;
#define FLAGS (DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN|DB_INIT_MPOOL|DB_CREATE|DB_THREAD)

MDBX_dbi dbi = 0;
MDBX_val key, data;
MDBX_txn *txn = NULL;
MDBX_cursor *cursor = NULL;
char sval[32];

rc = mdbx_env_create(&env);
if (rc != MDBX_SUCCESS) {
 fprintf(stderr, "mdbx_env_create: (%d) %s\n", rc, mdbx_strerror(rc));
 goto bailout;
}
rc = mdbx_env_open(env, "/Users/jacquesfauquex/mdbx",
                  MDBX_NOSUBDIR | MDBX_COALESCE | MDBX_LIFORECLAIM, 0664);
if (rc != MDBX_SUCCESS) {
 fprintf(stderr, "mdbx_env_open: (%d) %s\n", rc, mdbx_strerror(rc));
 goto bailout;
}
rc = mdbx_txn_begin(env, NULL, 0, &txn);
if (rc != MDBX_SUCCESS) {
 fprintf(stderr, "mdbx_txn_begin: (%d) %s\n", rc, mdbx_strerror(rc));
 goto bailout;
}
rc = mdbx_dbi_open(txn, NULL, 0, &dbi);
if (rc != MDBX_SUCCESS) {
 fprintf(stderr, "mdbx_dbi_open: (%d) %s\n", rc, mdbx_strerror(rc));
 goto bailout;
}
key.iov_len = sizeof(int);
key.iov_base = sval;
data.iov_len = sizeof(sval);
data.iov_base = sval;
sprintf(sval, "%03x %d foo bar", 32, 3141592);
rc = mdbx_put(txn, dbi, &key, &data, 0);
if (rc != MDBX_SUCCESS) {
 fprintf(stderr, "mdbx_put: (%d) %s\n", rc, mdbx_strerror(rc));
 goto bailout;
}
rc = mdbx_txn_commit(txn);
if (rc) {
 fprintf(stderr, "mdbx_txn_commit: (%d) %s\n", rc, mdbx_strerror(rc));
 goto bailout;
}
txn = NULL;
rc = mdbx_txn_begin(env, NULL, MDBX_TXN_RDONLY, &txn);
if (rc != MDBX_SUCCESS) {
 fprintf(stderr, "mdbx_txn_begin: (%d) %s\n", rc, mdbx_strerror(rc));
 goto bailout;
}
rc = mdbx_cursor_open(txn, dbi, &cursor);
if (rc != MDBX_SUCCESS) {
 fprintf(stderr, "mdbx_cursor_open: (%d) %s\n", rc, mdbx_strerror(rc));
 goto bailout;
}
int found = 0;
while ((rc = mdbx_cursor_get(cursor, &key, &data, MDBX_NEXT)) == 0) {
 printf("key: %p %.*s, data: %p %.*s\n", key.iov_base, (int)key.iov_len,
        (char *)key.iov_base, data.iov_base, (int)data.iov_len,
        (char *)data.iov_base);
 found += 1;
}
if (rc != MDBX_NOTFOUND || found == 0) {
 fprintf(stderr, "mdbx_cursor_get: (%d) %s\n", rc, mdbx_strerror(rc));
 goto bailout;
} else {
 rc = MDBX_SUCCESS;
}
bailout:
if (cursor)
 mdbx_cursor_close(cursor);
if (txn)
 mdbx_txn_abort(txn);
if (dbi)
 mdbx_dbi_close(env, dbi);
if (env)
 mdbx_env_close(env);
return (rc != MDBX_SUCCESS) ? EXIT_FAILURE : EXIT_SUCCESS;
*/
