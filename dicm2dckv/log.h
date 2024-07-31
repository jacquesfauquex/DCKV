// dicm2dckv
// log.h
#include <stdio.h>
#include "stdbool.h"

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
