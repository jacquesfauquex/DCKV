//
//  main.m
//  dckvparserrocksdb
//
//  Created by jacquesfauquex on 2024-02-12.
//

#import <Foundation/Foundation.h>
#import "ObjectiveRocks.h"


//short length value vr
const uint16 AE=0x4541;
const uint16 AS=0x5341;
const uint16 AT=0x5441;
const uint16 CS=0x5343;
const uint16 DA=0x4144;
const uint16 DS=0x5344;
const uint16 DT=0x5444;
const uint16 FD=0x4446;
const uint16 FL=0x4C46;
const uint16 IS=0x5349;
const uint16 LO=0x4f4c;
const uint16 LT=0x544c;
const uint16 PN=0x4e50;
const uint16 SH=0x4853;
const uint16 SL=0x4C53;
const uint16 SS=0x5353;
const uint16 ST=0x5453;
const uint16 TM=0x4d54;
const uint16 UI=0x4955;
const uint16 UL=0x4C55;
const uint16 US=0x5355;

//long length value vr
const uint16 OB=0x424F;
const uint16 OD=0x444F;
const uint16 OF=0x464F;
const uint16 OL=0x4C4F;
const uint16 OV=0x564F;
const uint16 OW=0x574F;
const uint16 SV=0x5653;
const uint16 UC=0x4355;
const uint16 UR=0x5255;
const uint16 UT=0x5455;
const uint16 UV=0x5655;

const uint16 UN=0x4E55;

const uint16 SQ=0x5153;

//propietary delimitation vr

const uint16 SA=0x0000;
const uint16 IA=0x2B2B;//++
const uint16 IZ=0x5F5F;//__
const uint16 SZ=0xFFFF;


struct ele {
   uint8 g;
   uint8 G;
   uint8 u;
   uint8 U;
   uint16 r;
   uint16 l;
};

void swaptag(struct ele *a)
{
   unsigned char swap;
   swap=a->g;
   a->g=a->G;
   a->G=swap;
   swap=a->u;
   a->u=a->U;
   a->U=swap;
}

uint32 letag(uint32 t)
{
   return
     ((t & 0xff) << 24)
   + ((t & 0xff00) << 16)
   + ((t & 0xff0000) >> 16)
   + ((t & 0xff000000) >> 24)
   ;
}

uint32 attrletag(struct ele *a)
{
   return (a->g << 24) + (a->G << 16) + (a->u << 8) + a->U;
}

NSData  *emptyData=nil;
const uint64 SZbytes=0xDDE0FEFF;//FFFEE0DD00000000
NSData  *SZdata=nil;
const uint64 IAbytes=0xFFFFFFFF00E0FEFF;//FFFEE000FFFFFFFF
NSData  *IAdata=nil;
const uint64 IZbytes=0x0DE0FEFF;//FFFEE00D00000000
NSData  *IZdata=nil;

const uint32 ffffffff=0xFFFFFFFF;
const uint32 fffee000=0xFFFEE000;
const uint32 fffee00d=0xFFFEE00D;
const uint32 fffee0dd=0xFFFEE0DD;

void parse(
           uint8_t *keybytes,
           uint8 keydepth,
           uint16 keycs,
           
           uint8_t *valbytes,
           uint8_t *llbytes,
           uint32 *ll,

           NSInputStream *stream,
           uint64 *idx,
           uint32 beforebyte,
           uint32 beforetag,
           
           RocksDB *db,
           NSError *error
           )
{
   uint8 *attrbytes=keybytes+keydepth;
   struct ele *attrstruct=(struct ele*) attrbytes;
   if ([stream read:attrbytes maxLength:8]!=8) {
      NSLog(@"error");
      //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
      return;
   }
   swaptag(attrstruct);
   
   while ((*idx < beforebyte) &&  ( attrletag(attrstruct) < beforetag))
   {
      switch (attrstruct->r) {
#pragma mark vl bin
         case FD://floating point double
         case FL://floating point single
         case SL://signed long
         case SS://signed short
         case UL://unsigned long
         case US://unsigned short
         {
            if (attrstruct->l > 0){
               if ([stream read:valbytes maxLength:attrstruct->l]!=attrstruct->l) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:attrstruct->l freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + attrstruct->l;
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;
#pragma mark vl tag code
         case AT://attribute tag
         {
            if (attrstruct->l > 0){
               if ([stream read:valbytes maxLength:attrstruct->l]!=attrstruct->l) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:attrstruct->l freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + attrstruct->l;
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;
#pragma mark vl ascii code
         case CS://coded string
         {
            if (attrstruct->l > 0){
               if ([stream read:valbytes maxLength:attrstruct->l]!=attrstruct->l) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:attrstruct->l freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + attrstruct->l;
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;
#pragma mark vl oid code
         case UI://unique ID
         {
            if (attrstruct->l > 0){
               if ([stream read:valbytes maxLength:attrstruct->l]!=attrstruct->l) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:attrstruct->l freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + attrstruct->l;
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;
#pragma mark vl ascii
         case AE://application entity
         case AS://age string
         case DA://date
         case DS://decimal string
         case DT://date time
         case IS://integer string
         case TM://time
         {
            if (attrstruct->l > 0){
               if ([stream read:valbytes maxLength:attrstruct->l]!=attrstruct->l) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:attrstruct->l freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db 
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + attrstruct->l;
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;
#pragma mark vl charset
         case LO://long string
         case LT://long text
         case SH://short string
         case ST://short text
         {
            if (attrstruct->l > 0){
               if ([stream read:valbytes maxLength:attrstruct->l]!=attrstruct->l) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:attrstruct->l freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + attrstruct->l;
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;
#pragma mark vl person
         case PN://person name
         {
            if (attrstruct->l > 0){
               if ([stream read:valbytes maxLength:attrstruct->l]!=attrstruct->l) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:attrstruct->l freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
             
            *idx += 8 + attrstruct->l;
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;
#pragma mark vll bin
         case OB://other byte
         case OD://other double
         case OF://other float
         case OL://other long
         case OV://other 64-bit very long
         case OW://other word
         case SV://signed 64-bit very long
         case UV://unsigned 64-bit very long
         {
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:*ll freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else {
               //loop read into NSMutableData
               NSMutableData *lldata=[NSMutableData dataWithLength:*ll];
               while (*ll>0xFFFF)
               {
                  if ([stream read:valbytes maxLength:0xFFFF]!=0xFFFF) {
                     //NSLog(@"error");
                     return;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               }
               [lldata appendBytes:valbytes length: *ll];
               [db
                setData:lldata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
               
             
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;
#pragma mark vll charset
         case UC://unlimited characters
         case UR://universal resource identifier/locator
         case UT://unlimited text
         {
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:*ll freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else {
               //loop read into NSMutableData
               NSMutableData *lldata=[NSMutableData dataWithLength:*ll];
               while (*ll>0xFFFF)
               {
                  if ([stream read:valbytes maxLength:0xFFFF]!=0xFFFF) {
                     //NSLog(@"error");
                     return;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               }
               [lldata appendBytes:valbytes length: *ll];
               [db
                setData:lldata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;

      //---------
#pragma mark UN
         case UN://unknown
         {
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return;
            }
            *idx += 12 + *ll;

            if (*ll==0) {
               [db
                setData:emptyData
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            else if (*ll < 0x10000){
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:*ll freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else {
               //loop read into NSMutableData
               NSMutableData *lldata=[NSMutableData dataWithLength:*ll];
               while (*ll>0xFFFF)
               {
                  if ([stream read:valbytes maxLength:0xFFFF]!=0xFFFF) {
                     //NSLog(@"error");
                     return;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if ([stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return;
               }
               [lldata appendBytes:valbytes length: *ll];
               [db
                setData:lldata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
            if ([stream read:attrbytes maxLength:8]!=8) {
               NSLog(@"error");
               //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
               return;
            }
            swaptag(attrstruct);
         } break;
            
      //---------
#pragma mark SQ
         case SQ://sequence
         {
            //register SA in rocksdb
            keybytes[keydepth+0x8]=0xff;
            keybytes[keydepth+0x9]=0xff;
            keybytes[keydepth+0xA]=0xff;
            keybytes[keydepth+0xB]=0xff;
            NSData *SQdata=[NSData dataWithBytes:keybytes+keydepth length:12];
            attrstruct->r=SA;
            attrstruct->l=0x0;
            [db
             setData:SQdata
             forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
             error:&error
            ];
            if ([stream read:llbytes maxLength:4]!=4) {
               //NSLog(@"error");
               return;
            }
            if (*ll==0)
            {
               //read nextattr
               *idx += 12;//do not add *ll !
               if ([stream read:attrbytes maxLength:8]!=8) {
                  NSLog(@"error");
                  //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
                  return;
               }
               swaptag(attrstruct);
            }
            else //SQ *ll!=0
            {
               //SQ length
               uint64 beforebyteSQ;
               if (*ll==ffffffff) beforebyteSQ=beforebyte;
               else if (beforebyte==ffffffff) beforebyteSQ= *idx + *ll;
               else if (*idx + *ll > beforebyte) {
                  NSLog(@"incomplete input");
                  return;
               }
               else beforebyteSQ=*idx + *ll;

               
               //replace vr and vl of SQ by itemnumber
               uint32 itemnumber=1;
               uint8_t *inb=(uint8_t*)&itemnumber;
               attrstruct->r= (*(inb+2) << 8) + *(inb+3);
               attrstruct->l= (*(inb+0) << 8) + *(inb+1);

               *idx += 12;//do not add *ll !

               
#pragma mark item level
               keydepth+=8;
               uint8 *itembytes=keybytes+keydepth;
               struct ele *itemstruct=(struct ele*) itembytes;
               if ([stream read:itembytes maxLength:8]!=8) {
                  NSLog(@"error");
                  //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
                  return;
               }
               swaptag(itemstruct);//first item attr should be fffee000

               uint32 itemtag=attrletag(itemstruct);
               //for each first attr fffee000 of any new item
               while ((*idx < beforebyteSQ) && (itemtag==fffee000)) //itemstart compulsory
               {
                  uint32 IQll = (itemstruct->l << 16) | itemstruct->r;
                  itemstruct->g=0x0;
                  itemstruct->G=0x0;
                  itemstruct->u=0x0;
                  itemstruct->U=0x0;
                  itemstruct->r=IA;
                  itemstruct->l=0x0;
                  [db
                   setData:IAdata
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];

                  //
                  uint64 beforebyteIT;//to be computed from after item start
                  *idx+=8;
                  if (IQll==ffffffff) beforebyteIT=beforebyteSQ;
                  else if (beforebyteSQ==ffffffff) beforebyteIT=*idx + *ll;
                  else if (*idx + *ll > beforebyteSQ) {
                     NSLog(@"incomplete input");
                     return;
                  }
                  else beforebyteIT=*idx + *ll;

                  parse(
                        keybytes,
                        keydepth,
                        keycs,
                        valbytes,
                        llbytes,
                        ll,
                        stream,
                        idx,
                        (uint32)beforebyteIT,
                        0xFFFEE00D,
                        db,
                        error
                        );
                  //Atención!!! attr still is the first attr of the first item
                  //the attr of the recursion is not available
                  //keybytes kept the last attr read one level deeper than current attr

                  
                  //write IZ to db
                  if (attrletag(itemstruct)==fffee00d)
                  {
                     itemstruct->g=0xff;
                     itemstruct->G=0xff;
                     itemstruct->u=0xff;
                     itemstruct->U=0xff;
                     itemstruct->r=IZ;
                     itemstruct->l=0x00;
                     [db
                      setData:IZdata
                      forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                      error:&error
                     ];
                     *idx+=8;
                     if ([stream read:itembytes maxLength:8]!=8) {
                        NSLog(@"error");
                        //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
                        return;
                     }
                     swaptag(itemstruct);
                  }
                  else
                  {
                     struct ele copyattr;
                     copyattr.g=itemstruct->g;
                     copyattr.G=itemstruct->G;
                     copyattr.u=itemstruct->u;
                     copyattr.U=itemstruct->U;
                     copyattr.r=itemstruct->r;
                     copyattr.l=itemstruct->l;
                     itemstruct->g=0xff;
                     itemstruct->G=0xff;
                     itemstruct->u=0xff;
                     itemstruct->U=0xff;
                     itemstruct->r=IZ;
                     itemstruct->l=0x0;
                     [db
                      setData:IZdata
                      forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                      error:&error
                     ];
                     itemstruct->g=copyattr.g;
                     itemstruct->G=copyattr.G;
                     itemstruct->u=copyattr.u;
                     itemstruct->U=copyattr.U;
                     itemstruct->r=copyattr.r;
                     itemstruct->l=copyattr.l;
                  }
                  
                  //new item number
                  itemnumber+=1;
                  attrstruct->r= (*(inb+2) << 8) + *(inb+3);
                  attrstruct->l= (*(inb+0) << 8) + *(inb+1);

                  itemtag=attrletag(itemstruct);
               }//end while item
               keydepth-=8;
#pragma mark item level end

               attrstruct->r=SZ;
               attrstruct->l=0x0;
               [db
                setData:SZdata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];

               
               //itemstruct may be SZ or post SQ
               if (attrletag(itemstruct)==fffee0dd)
               {
                  *idx+=8;
                  if ([stream read:attrbytes maxLength:8]!=8) {
                     NSLog(@"error");
                     //[NSException raise:@"STREAM_ERROR" format:@"%@", [stream streamError]];
                     return;
                  }
                  swaptag(attrstruct);
               }
               else
               {
                  attrstruct->g=itemstruct->g;
                  attrstruct->G=itemstruct->G;
                  attrstruct->u=itemstruct->u;
                  attrstruct->U=itemstruct->U;
                  attrstruct->r=itemstruct->r;
                  attrstruct->l=itemstruct->l;
               }
            }
 
#pragma mark test db (to be eliminated)
            /*
            RocksDBIterator *iterator = [db iterator];
            [iterator enumerateKeysUsingBlock:^(NSData *k, BOOL *stop) {NSLog(@"%@", k.description);}];
            [iterator close];
             */
         } break;
            
         default:
         {
            NSLog(@"error unknown vr");
            return;
         }
            
      //---------
      }//end switch
   }//end while (*index < beforebyte)
    
}

int main(int argc, const char * argv[]) {
   int returnstatus=0;
   NSError *error=nil;
   NSDate *start=[NSDate date];
   emptyData=[NSData data];
   SZdata=[NSData dataWithBytesNoCopy:(void*)&SZbytes length:8];
   IAdata=[NSData dataWithBytesNoCopy:(void*)&IAbytes length:8];
   IZdata=[NSData dataWithBytesNoCopy:(void*)&IZbytes length:8];

   @autoreleasepool {
      RocksDB *db = [RocksDB databaseAtPath:@"/Users/jacquesfauquex/rocksdb" andDBOptions:^(RocksDBOptions *options) {
         options.createIfMissing = YES;
      }];
      
      //https://stackoverflow.com/questions/19165134/correct-portable-way-to-interpret-buffer-as-a-struct
      uint8_t *keybytes = malloc(144);
      uint8_t *valbytes = malloc(0xFFFF);//max vl
      //additional ll and llvaluedata read ll attrs of the stream
      uint8_t *llbytes = malloc(4);
      uint32 *ll = (uint32*) llbytes ;
      uint64 streamindex=144;
      uint64 *idx=&streamindex;

//img
/*
      NSInputStream *stream1=[NSInputStream inputStreamWithFileAtPath:@"/Users/jacquesfauquex/dcvvparser/dcvvparser/img.dcm"];
      [stream1 open];
      streamindex=144;
      if (![stream1 hasBytesAvailable]) NSLog(@"no byte disponible");
      else if ([stream1 read:keybytes maxLength:*idx]!=*idx) NSLog(@"could not chop preamble");
      else
      {
         parse(
            keybytes,
            0,
            0,
            valbytes,
            llbytes,
            ll,
            stream1,
            idx,
            0xFFFFFFFF,
            0x7FE00010,
            db,
            error
            );
      }
      [stream1 close];
*/
//sr
      NSInputStream *stream2=[NSInputStream inputStreamWithFileAtPath:@"/Users/jacquesfauquex/dcvvparser/dcvvparser/sr.dcm"];
      [stream2 open];
      streamindex=144;

      if (![stream2 hasBytesAvailable]) NSLog(@"no byte dispo");
      else if ([stream2 read:keybytes maxLength:*idx]!=*idx) NSLog(@"could not chop preamble");
      else {
         parse(
               keybytes,
               0,
               0,
               valbytes,
               llbytes,
               ll,
               stream2,
               idx,
               0xFFFFFFFF,
               0x7FE00010,
               db,
               error
            );
      }
      [stream2 close];

      
      RocksDBIterator *iterator = [db iterator];
      [iterator enumerateKeysUsingBlock:^(NSData *k, BOOL *stop) {fprintf(stderr, "%s\n",k.description.UTF8String);}];
      [iterator close];
      
      [db close];
      
      if (error) NSLog(@"%@",error.description);
      
      free(keybytes);
      free(valbytes);
      free(llbytes);

   }
   NSLog(@"elapsed %f", [[NSDate date]timeIntervalSinceDate:start]);
   return returnstatus;
}
