/* Copyright (c) 2007-2008 CSIRO
   Copyright (c) 2007-2009 Xiph.Org Foundation
   Copyright (c) 2008 Gregory Maxwell
   Written by Jean-Marc Valin and Gregory Maxwell */
/**
  @file celt.h
  @brief Contains all the functions for encoding and decoding audio
 */

/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CELT_H
#define CELT_H

#include "opus_types.h"
#include "opus_defines.h"
#include "opus_custom.h"
#include "entenc.h"
#include "entdec.h"
#include "arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CELTEncoder OpusCustomEncoder
#define CELTDecoder OpusCustomDecoder
#define CELTMode OpusCustomMode

/** Encoder state
 @brief Encoder state
 */
struct OpusCustomEncoder {
   const OpusCustomMode *mode;     /**< Mode used by the encoder */
   int overlap;
   int channels;
   int stream_channels;

   int force_intra;
   int clip;
   int disable_pf;
   int complexity;
   int upsample;
   int start, end;

   opus_int32 bitrate;
   int vbr;
   int signalling;
   int constrained_vbr;      /* If zero, VBR can do whatever it likes with the rate */
   int loss_rate;

   /* encoder tuning */
   int tune_lowpass;
   int tune_trim;
   int intensity_start;
   int skip_high;
   int skip_low;
   int tune_trim_lower1;
   int tune_trim_lower2;
   int tune_trim_increase1;
   int tune_trim_increase2;
   int tune_spread_aggr;
   int tune_spread_medium;
   int tune_spread_light;

   /* Everything beyond this point gets cleared on a reset */
#define ENCODER_RESET_START rng

   opus_uint32 rng;
   int spread_decision;
   opus_val32 delayedIntra;
   int tonal_average;
   int lastCodedBands;
   int hf_average;
   int tapset_decision;

   int prefilter_period;
   opus_val16 prefilter_gain;
   int prefilter_tapset;
#ifdef RESYNTH
   int prefilter_period_old;
   opus_val16 prefilter_gain_old;
   int prefilter_tapset_old;
#endif
   int consec_transient;

   opus_val32 preemph_memE[2];
   opus_val32 preemph_memD[2];

   /* VBR-related parameters */
   opus_int32 vbr_reservoir;
   opus_int32 vbr_drift;
   opus_int32 vbr_offset;
   opus_int32 vbr_count;

#ifdef RESYNTH
   celt_sig syn_mem[2][2*MAX_PERIOD];
#endif

   celt_sig in_mem[1]; /* Size = channels*mode->overlap */
   /* celt_sig prefilter_mem[],  Size = channels*COMBFILTER_PERIOD */
   /* celt_sig overlap_mem[],  Size = channels*mode->overlap */
   /* opus_val16 oldEBands[], Size = 2*channels*mode->nbEBands */
};

#define _celt_check_mode_ptr_ptr(ptr) ((ptr) + ((ptr) - (const CELTMode**)(ptr)))

/* Encoder/decoder Requests */

#define CELT_SET_PREDICTION_REQUEST    10002
/** Controls the use of interframe prediction.
    0=Independent frames
    1=Short term interframe prediction allowed
    2=Long term prediction allowed
 */
#define CELT_SET_PREDICTION(x) CELT_SET_PREDICTION_REQUEST, __opus_check_int(x)

#define CELT_SET_INPUT_CLIPPING_REQUEST    10004
#define CELT_SET_INPUT_CLIPPING(x) CELT_SET_INPUT_CLIPPING_REQUEST, __opus_check_int(x)

#define CELT_GET_AND_CLEAR_ERROR_REQUEST   10007
#define CELT_GET_AND_CLEAR_ERROR(x) CELT_GET_AND_CLEAR_ERROR_REQUEST, __opus_check_int_ptr(x)

#define CELT_SET_CHANNELS_REQUEST    10008
#define CELT_SET_CHANNELS(x) CELT_SET_CHANNELS_REQUEST, __opus_check_int(x)


/* Internal */
#define CELT_SET_START_BAND_REQUEST    10010
#define CELT_SET_START_BAND(x) CELT_SET_START_BAND_REQUEST, __opus_check_int(x)

#define CELT_SET_END_BAND_REQUEST    10012
#define CELT_SET_END_BAND(x) CELT_SET_END_BAND_REQUEST, __opus_check_int(x)

#define CELT_GET_MODE_REQUEST    10015
/** Get the CELTMode used by an encoder or decoder */
#define CELT_GET_MODE(x) CELT_GET_MODE_REQUEST, _celt_check_mode_ptr_ptr(x)

#define CELT_SET_SIGNALLING_REQUEST    10016
#define CELT_SET_SIGNALLING(x) CELT_SET_SIGNALLING_REQUEST, __opus_check_int(x)

#define CELT_SET_TUNE_LOWPASS_REQUEST                10020
#define CELT_SET_TUNE_LOWPASS(x) CELT_SET_TUNE_LOWPASS_REQUEST, __opus_check_int(x)
#define CELT_SET_TUNE_TRIM_REQUEST                   10022
#define CELT_SET_TUNE_TRIM(x) CELT_SET_TUNE_TRIM_REQUEST, __opus_check_int(x)
#define CELT_SET_INTENSITY_START_REQUEST             10024
#define CELT_SET_INTENSITY_START(x) CELT_SET_INTENSITY_START_REQUEST, __opus_check_int(x)
#define CELT_SET_SKIP_LOW_REQUEST                    10026
#define CELT_SET_SKIP_LOW(x) CELT_SET_SKIP_LOW_REQUEST, __opus_check_int(x)
#define CELT_SET_SKIP_HIGH_REQUEST                   10028
#define CELT_SET_SKIP_HIGH(x) CELT_SET_SKIP_HIGH_REQUEST, __opus_check_int(x)
#define CELT_SET_TRIM_LOWER1_THRESH_REQUEST          10030
#define CELT_SET_TRIM_LOWER1_THRESH(x) CELT_SET_TRIM_LOWER1_THRESH_REQUEST, __opus_check_int(x)
#define CELT_SET_TRIM_LOWER2_THRESH_REQUEST          10032
#define CELT_SET_TRIM_LOWER2_THRESH(x) CELT_SET_TRIM_LOWER2_THRESH_REQUEST, __opus_check_int(x)
#define CELT_SET_TRIM_INCR1_THRESH_REQUEST           10034
#define CELT_SET_TRIM_INCR1_THRESH(x) CELT_SET_TRIM_INCR1_THRESH_REQUEST, __opus_check_int(x)
#define CELT_SET_TRIM_INCR2_THRESH_REQUEST           10036
#define CELT_SET_TRIM_INCR2_THRESH(x) CELT_SET_TRIM_INCR2_THRESH_REQUEST, __opus_check_int(x)
#define CELT_SET_SPREAD_AGGR_REQUEST                 10038
#define CELT_SET_SPREAD_AGGR(x) CELT_SET_SPREAD_AGGR_REQUEST, __opus_check_int(x)
#define CELT_SET_SPREAD_MEDIUM_REQUEST               10040
#define CELT_SET_SPREAD_MEDIUM(x) CELT_SET_SPREAD_MEDIUM_REQUEST, __opus_check_int(x)
#define CELT_SET_SPREAD_LIGHT_REQUEST                10042
#define CELT_SET_SPREAD_LIGHT(x) CELT_SET_SPREAD_LIGHT_REQUEST, __opus_check_int(x)

/* Encoder stuff */

int celt_encoder_get_size(int channels);

int celt_encode_with_ec(OpusCustomEncoder * OPUS_RESTRICT st, const opus_val16 * pcm, int frame_size, unsigned char *compressed, int nbCompressedBytes, ec_enc *enc);

int celt_encoder_init(CELTEncoder *st, opus_int32 sampling_rate, int channels);



/* Decoder stuff */

int celt_decoder_get_size(int channels);


int celt_decoder_init(CELTDecoder *st, opus_int32 sampling_rate, int channels);

int celt_decode_with_ec(OpusCustomDecoder * OPUS_RESTRICT st, const unsigned char *data, int len, opus_val16 * OPUS_RESTRICT pcm, int frame_size, ec_dec *dec);

#define celt_encoder_ctl opus_custom_encoder_ctl
#define celt_decoder_ctl opus_custom_decoder_ctl

#ifdef __cplusplus
}
#endif

#endif /* CELT_H */
