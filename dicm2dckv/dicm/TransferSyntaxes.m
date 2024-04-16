//
//  TransferSyntaxes.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-02-29.
//

#include <Foundation/Foundation.h>
#include "TransferSyntaxes.h"

const char *tsstr[]={
"1.2.840.10008.1.​20",        //Papyrus​3​Implicit​VR​Little​Endian      Retired
"1.2.840.10008.1.​2",         //Implicit​VR​Little​Endian
"1.2.840.10008.1.​2.​1",       //Explicit​VR​Little​Endian
"1.2.840.10008.1.​2.​1.98",    //Encapsulated​Uncompressed​Explicit​VR​Little​Endian
"1.2.840.10008.1.2.​1.​99",    //Deflated​Explicit​VR​Little​Endian
"1.2.840.10008.1.​2.​2",       //Explicit​VR​Big​Endian     Retired
"1.2.840.10008.1.2.​4.​50",    //JPEG​Baseline​8​Bit
"1.2.840.10008.1.2.​4.​51",    //JPEG​Extended​12​Bit
"1.2.840.10008.1.2.​4.​52",    //JPEG​Extended35      Retired
"1.2.840.10008.1.2.​4.​53",    //JPEG​Spectral​Selection​Non​Hierarchical68      Retired
"1.2.840.10008.1.2.​4.​54",    //JPEG​Spectral​Selection​Non​Hierarchical79      Retired
"1.2.840.10008.1.2.​4.​55",    //JPEG​Full​Progression​Non​Hierarchical1012      Retired
"1.2.840.10008.1.2.​4.​56",    //JPEG​Full​Progression​Non​Hierarchical1113      Retired
"1.2.840.10008.1.2.​4.​57",    //JPEG​Lossless
"1.2.840.10008.1.2.​4.​58",    //JPEG​Lossless​Non​Hierarchical​15      Retired
"1.2.840.10008.1.2.​4.​59",    //JPEG​Extended​Hierarchical​1618     Retired
"1.2.840.10008.1.2.​4.​60",    //JPEG​Extended​Hierarchical​1719     Retired
"1.2.840.10008.1.2.​4.​61",    //JPEG​Spectral​Selection​Hierarchical2022     Retired
"1.2.840.10008.1.2.​4.​62",    //JPEG​Spectral​Selection​Hierarchical2123     Retired
"1.2.840.10008.1.2.​4.​63",    //JPEG​Full​Progression​Hierarchical2426    Retired
"1.2.840.10008.1.2.​4.​64",    //JPEG​Full​Progression​Hierarchical2527    Retired
"1.2.840.10008.1.2.​4.​65",    //JPEG​Lossless​Hierarchical​28    Retired
"1.2.840.10008.1.2.​4.​66",    //JPEG​Lossless​Hierarchical​29    Retired
"1.2.840.10008.1.2.​4.​70",    //JPEG​Lossless​SV1
"1.2.840.10008.1.2.​4.​80",    //JPEG​LS​Lossless
"1.2.840.10008.1.2.​4.​81",    //JPEG​LS​Near​Lossless
"1.2.840.10008.1.2.​4.​90",    //JPEG​2000Lossless
"1.2.840.10008.1.2.​4.​91",    //JPEG​2000
"1.2.840.10008.1.2.​4.​92",    //JPEG​2000MCLossless
"1.2.840.10008.1.2.​4.​93",    //JPEG​2000MC
"1.2.840.10008.1.2.​4.​94",    //JPIP​Referenced
"1.2.840.10008.1.2.​4.​95",    //JPIP​Referenced​Deflate
"1.2.840.10008.1.2.​4.​100",  //MPEG2​MPML
"1.2.840.10008.1.2.​4.​100.1",//MPEG2​MPMLF
"1.2.840.10008.1.2.​4.​101",  //MPEG2​MPHL
"1.2.840.10008.1.2.​4.​101.1",//MPEG2​MPHLF
"1.2.840.10008.1.2.​4.​102",  //MPEG4​HP41
"1.2.840.10008.1.2.​4.​102.1",//MPEG4​HP41F
"1.2.840.10008.1.2.​4.​103",  //MPEG4​HP41BD
"1.2.840.10008.1.2.​4.​103.1",//MPEG4​HP41BDF
"1.2.840.10008.1.2.​4.​104",  //MPEG4​HP422D
"1.2.840.10008.1.2.​4.​104.1",//MPEG4​HP422DF
"1.2.840.10008.1.2.​4.​105",  //MPEG4​HP423D
"1.2.840.10008.1.2.​4.​105.1",//MPEG4​HP423DF
"1.2.840.10008.1.2.​4.​106",  //MPEG4​HP42STEREO
"1.2.840.10008.1.2.​4.​106.1",//MPEG4​HP42STEREOF
"1.2.840.10008.1.2.​4.107",  //HEVC​MP51
"1.2.840.10008.1.2.​4.108",  //HEVC​M10P51
"1.2.840.10008.1.2.​4.​201",  //HTJ2K​Lossless
"1.2.840.10008.1.2.​4.202",  //HTJ2K​Lossless​RPCL
"1.2.840.10008.1.2.​4.​203",  //HTJ2K
"1.2.840.10008.1.2.​4.​204",  //JPIP​HTJ2K​Referenced
"1.2.840.10008.1.2.​4.​205",  //JPIP​HTJ2K​Referenced​Deflate
"1.2.840.10008.1.​2.​5",      //RLE​Lossless
"1.2.840.10008.1.2.​6.​1",    //RFC​2557​MIME​Encapsulation
"1.2.840.10008.1.2.​6.​2",    //XML​Encoding
"1.2.840.10008.1.2.7.1",    //SMPTE​ST​211020​Uncompressed​Progressive​Active​Video
"1.2.840.10008.1.2.7.2",    //SMPTE​ST​211020​Uncompressed​Interlaced​Active​Video
"1.2.840.10008.1.2.7.3"     //SMPTE​ST​211030​PCMDigital​Audio
};

//ts_papyrus3ile = 0 = código de error
uint8 tsidx( uint8_t *valbytes, uint16 vallength )
{
   uint8 idx=0x0;//verification / error
   
   switch (vallength){
      case 17: idx=ts_;break;//Implicit​VR​Little​Endian
      case 19:{
         switch (valbytes[18]) {
            case 0x31: idx=ts_1;break;//Explicit​VR​Little​Endian
            case 0x32: idx=ts_2;break;//Explicit​VR​Big​Endian     Retired
            case 0x35: idx=ts_5;break;//RLE​Lossless
            default : idx=ts_papyrus3ile;break;//error
         }};break;
      case 22:{
         switch ((valbytes[20]<<8)+valbytes[21]) {
            case 0x3938: idx=ts_1_98;break;//Encapsulated​Uncompressed​Explicit​VR​Little​Endian
            case 0x3939: idx=ts_1_99;break;//Deflated​Explicit​VR​Little​Endian
            case 0x3035: idx=ts_4_50;break;//JPEG​Baseline​8​Bit
            case 0x3135: idx=ts_4_51;break;//JPEG​Extended​12​Bit
            case 0x3235: idx=ts_4_52;break;//JPEG​Extended35      Retired
            case 0x3335: idx=ts_4_53;break;//JPEG​Spectral​Selection​Non​Hierarchical68      Retired
            case 0x3435: idx=ts_4_54;break;//JPEG​Spectral​Selection​Non​Hierarchical79      Retired
            case 0x3535: idx=ts_4_55;break;//JPEG​Full​Progression​Non​Hierarchical1012      Retired
            case 0x3635: idx=ts_4_56;break;//JPEG​Full​Progression​Non​Hierarchical1113      Retired
            case 0x3735: idx=ts_4_57;break;//JPEG​Lossless
            case 0x3835: idx=ts_4_58;break;//JPEG​Lossless​Non​Hierarchical​15      Retired
            case 0x3935: idx=ts_4_59;break;//JPEG​Extended​Hierarchical​1618     Retired
            case 0x3036: idx=ts_4_60;break;//JPEG​Extended​Hierarchical​1719     Retired
            case 0x3136: idx=ts_4_61;break;//JPEG​Spectral​Selection​Hierarchical2022     Retired
            case 0x3236: idx=ts_4_62;break;//JPEG​Spectral​Selection​Hierarchical2123     Retired
            case 0x3336: idx=ts_4_63;break;//JPEG​Full​Progression​Hierarchical2426    Retired
            case 0x3436: idx=ts_4_64;break;//JPEG​Full​Progression​Hierarchical2527    Retired
            case 0x3536: idx=ts_4_65;break;//JPEG​Lossless​Hierarchical​28    Retired
            case 0x3636: idx=ts_4_66;break;//JPEG​Lossless​Hierarchical​29    Retired
            case 0x3037: idx=ts_4_70;break;//JPEG​Lossless​SV1
            case 0x3038: idx=ts_4_80;break;//JPEG​LS​Lossless
            case 0x3138: idx=ts_4_81;break;//JPEG​LS​Near​Lossless
            case 0x3039: idx=ts_4_90;break;//JPEG​2000Lossless
            case 0x3139: idx=ts_4_91;break;//JPEG​2000
            case 0x3239: idx=ts_4_92;break;//JPEG​2000MCLossless
            case 0x3339: idx=ts_4_93;break;//JPEG​2000MC
            case 0x3439: idx=ts_4_94;break;//JPIP​Referenced
            case 0x3539: idx=ts_4_95;break;//JPIP​Referenced​Deflate
            case 0x2031:{
               switch (valbytes[18]){
                  case 0x36: idx=ts_6_1;break;//RFC​2557​MIME​Encapsulation
                  case 0x37: idx=ts_7_1;break;//SMPTE​ST​211020​Uncompressed​Progressive​Active​Video
                  default: idx=ts_papyrus3ile;break;//error
               }};break;
            case 0x2032:{
               switch (valbytes[18]){
                  case 0x36: idx=ts_6_2;break;//XML​Encoding
                  case 0x37: idx=ts_7_2;break;//SMPTE​ST​211020​Uncompressed​Interlaced​Active​Video
                  default: idx=ts_papyrus3ile;break;//error
               }};break;
            case 0x2033: idx=ts_7_3;break;//SMPTE​ST​211030​PCMDigital​Audio
            default    : idx=ts_papyrus3ile;break;//error
         }};break;
      case 23:{
         switch (valbytes[20]){
            case 31:{
               switch (valbytes[22]){
                  case 0x30:idx=ts_4_100 ;break;//MPEG2​MPML
                  case 0x31:idx=ts_4_101 ;break;//MPEG2​MPHL
                  case 0x32:idx=ts_4_102 ;break;//MPEG4​HP41
                  case 0x33:idx=ts_4_103 ;break;//MPEG4​HP41BD
                  case 0x34:idx=ts_4_104 ;break;//MPEG4​HP422D
                  case 0x35:idx=ts_4_105 ;break;//MPEG4​HP423D
                  case 0x36:idx=ts_4_106 ;break;//MPEG4​HP42STEREO
                  case 0x37:idx=ts_4_107 ;break;//HEVC​MP51
                  case 0x38:idx=ts_4_108 ;break;//HEVC​M10P51
                  default: idx=ts_papyrus3ile;break;//error
               }};break;
            case 32:{
               switch (valbytes[22]){
                  case 0x31:idx=ts_4_201 ;break;//HTJ2K​Lossless
                  case 0x32:idx=ts_4_202 ;break;//HTJ2K​Lossless​RPCL
                  case 0x33:idx=ts_4_203 ;break;//HTJ2K
                  case 0x34:idx=ts_4_204 ;break;//JPIP​HTJ2K​Referenced
                  case 0x35:idx=ts_4_205 ;break;//JPIP​HTJ2K​Referenced​Deflate
                  default: idx=ts_papyrus3ile;break;//error
               }};break;
            default: idx=ts_papyrus3ile;break;//error
         }};break;
      case 25:{
         switch (valbytes[22]){
            case 0x30:idx=ts_4_100_1 ;break;//MPEG2​MPMLF
            case 0x31:idx=ts_4_101_1 ;break;//MPEG2​MPHLF
            case 0x32:idx=ts_4_102_1 ;break;//MPEG4​HP41F
            case 0x33:idx=ts_4_103_1 ;break;//MPEG4​HP41BDF
            case 0x34:idx=ts_4_104_1 ;break;//MPEG4​HP422DF
            case 0x35:idx=ts_4_105_1 ;break;//MPEG4​HP423DF
            case 0x36:idx=ts_4_106_1 ;break;//MPEG4​HP42STEREOF
            default: idx=ts_papyrus3ile;break;//error
         }
      };break;
      default: idx=ts_papyrus3ile;break;//error
   };
   
   if (strncmp(tsstr[idx],(char*)valbytes,strlen(tsstr[idx]))) return idx;
   else return 0;//error (verification)
}


NSString *tsname( uint16 tsidx)
{
   return [NSString stringWithUTF8String:tsstr[tsidx]];
}
