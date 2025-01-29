//
//  opj_compress.h
//  dicm2dckv
//
//  Created by jacquesfauquex on 2025-01-29.
//

#ifndef opj_compress_h
#define opj_compress_h

BOOL opj_cfho(
          u16  photocode,
          u16  spp,
          u16  rows,
          u16  cols,
          u16  stored,
          u64  dkvs,//source idx
          u64  dkvc,//compressed idx
          u64 *dkvf,//fast idx
          u64 *dkvh,//high idx
          u64 *dkvo,//original idx
          u64 *dkve//end idx
)

#endif /* opj_compress_h */


/* compression parameters */
//opj_set_default_encoder_parameters(&parameters);
/**
Set encoding parameters to default values, that means :
<ul>
    <li>Lossless
    <li>1 tile
<li>Size of precinct : 2^15 x 2^15 (means 1 precinct)
<li>Size of code-block : 64 x 64
    <li>Number of resolutions: 6
<li>No SOP marker in the codestream
<li>No EPH marker in the codestream
    <li>No sub-sampling in x or y direction
<li>No mode switch activated
    <li>Progression order: LRCP
<li>No index file
<li>No ROI upshifted
<li>No offset of the origin of the image
<li>No offset of the origin of the tiles
<li>Reversible DWT 5-3
</ul>
@param parameters Compression parameters

 
//---------------------------------------
 
 /*
 REPLACE THIS STRUCTURE BY DIRECT CALLS
  
 struct opj_codec_private {
    m_codec_data {
        struct opj_compression m_compression {
              OPJ_BOOL(* opj_start_compress)(void *p_codec,
                                             struct opj_stream_private * cio,
                                             struct opj_image * p_image,
                                             struct opj_event_mgr * p_manager);

              OPJ_BOOL(* opj_encode)(void * p_codec,
                                     struct opj_stream_private *p_cio,
                                     struct opj_event_mgr * p_manager);

              OPJ_BOOL(* opj_write_tile)(void * p_codec,
                                         OPJ_UINT32 p_tile_index,
                                         OPJ_BYTE * p_data,
                                         OPJ_UINT32 p_data_size,
                                         struct opj_stream_private * p_cio,
                                         struct opj_event_mgr * p_manager);

              OPJ_BOOL(* opj_end_compress)(void * p_codec,
                                           struct opj_stream_private * p_cio,
                                           struct opj_event_mgr * p_manager);

              void (* opj_destroy)(void * p_codec);

              OPJ_BOOL(* opj_setup_encoder)(void * p_codec,
                                            opj_cparameters_t * p_param,
                                            struct opj_image * p_image,
                                            struct opj_event_mgr * p_manager);

              OPJ_BOOL(* opj_encoder_set_extra_options)(void * p_codec,
                      const char* const* p_options,
                      struct opj_event_mgr * p_manager);
        }
     }
    void * m_codec;
    opj_event_mgr_t m_event_mgr;
    OPJ_BOOL is_decompressor;
    void (*opj_dump_codec)(void * p_codec, OPJ_INT32 info_flag,FILE* output_stream);
    opj_codestream_info_v2_t* (*opj_get_codec_info)(void* p_codec);
    opj_codestream_index_t* (*opj_get_codec_index)(void* p_codec);
    OPJ_BOOL(*opj_set_threads)(void * p_codec, OPJ_UINT32 num_threads);
  }
  */
