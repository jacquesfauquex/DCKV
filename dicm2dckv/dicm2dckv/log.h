//
//  ODLog.h
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-04-03.
//

#ifndef main_h
#define main_h
#include <Foundation/Foundation.h>
#include "stdarg.h"


enum exitValue{
   exitOK=0,
   errorArgs,
   errorIn,
   errorOutPath,
   errorWrite,
   errorCreateKV
};

//https://stackoverflow.com/questions/53522586/variadic-macro-calling-fprintf-how-to-add-arguments-to-va-args
//el if permite sumar los niveles más fundamentales
typedef NS_ENUM(int, DIWEFenum) {D,I,W,E,F};
static DIWEFenum DIWEF=D;

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


#endif /* main_h */
