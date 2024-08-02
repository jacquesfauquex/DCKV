// dicm2dckv
// dckvtype.h

#ifndef dckvtype_h
#define dckvtype_h

#pragma mark - C

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef char                s8;//%c
typedef unsigned char       u8;//%c
typedef short              s16;//%hd
typedef unsigned short     u16;//%hu
typedef int                s32;//%d
typedef unsigned int       u32;//%u
typedef long long          s64;//%lld
typedef unsigned long long u64;//%llu


#pragma mark - main & log

enum exitValue{
   exitOK=0,
   errorArgs,
   errorIn,
   errorOutPath,
   errorWrite,
   errorCreateKV,
   errorLogLevel
};

//https://stackoverflow.com/questions/53522586/variadic-macro-calling-fprintf-how-to-add-arguments-to-va-args
//el if permite sumar los niveles más fundamentales
enum DIWEFenum {D,I,W,E,F};
extern enum DIWEFenum DIWEF;
bool loglevel(const char * logletter);

#define D(format, ...) do {           \
  if (DIWEF <= D){         \
    fprintf(stderr, (format), __VA_ARGS__); \
    fputc('\n', stderr);                      \
  }\
} while (0)


#define I(format, ...) do {           \
  if (DIWEF <= I){         \
    fprintf(stderr, (format), __VA_ARGS__); \
    fputc('\n', stderr);                      \
  }\
} while (0)


#define W(format, ...) do {           \
  if (DIWEF <= W){         \
    fprintf(stderr, (format), __VA_ARGS__); \
    fputc('\n', stderr);                      \
  }\
} while (0)


#define E(format, ...) do {           \
  if (DIWEF <= E){         \
    fprintf(stderr, (format), __VA_ARGS__); \
    fputc('\n', stderr);                      \
  }\
} while (0)


#define F(format, ...) do { fprintf(stderr, @"%@", __FAULT__); } while (0)



#pragma mark - repertoires

enum repertoireenum{
   
   REPERTOIRE_GL,
   ISO_IR100,
   ISO_IR101,
   ISO_IR109,
   ISO_IR110,
   ISO_IR148,
   ISO_IR126,
   ISO_IR127,
   
   ISO_IR192,
   RFC3986,
   ISO_IR13,
   ISO_IR144,
   ISO_IR138,
   ISO_IR166,
   GB18030,
   GBK,
   
   ISO2022IR6,
   ISO2022IR100,
   ISO2022IR101,
   ISO2022IR109,
   ISO2022IR110,
   ISO2022IR148,
   ISO2022IR126,
   ISO2022IR127,
   
   ISO2022IR87,
   ISO2022IR159,
   ISO2022IR13,
   ISO2022IR144,
   ISO2022IR138,
   ISO2022IR166,
   ISO2022IR149,
   ISO2022IR58
};


u32 repertoireidx( uint8_t *vbuf, u16 vallength );










#endif /* dckvtype_h */
