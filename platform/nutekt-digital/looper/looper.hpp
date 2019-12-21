/*
 * File: looper.hppOD
 *
 * Kaossilator-style loop recorder for Delay FX and Reverb FX
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "buffer_ops.h"
#include "fixed_math.h"
#include "float_math.h"
#include "fx_api.h"
#include "osc_api.h"

#define k_user_module_modfx_val 1
#define k_user_module_delfx_val 2
#define k_user_module_revfx_val 3
#define k_user_module_osc_val 4

#define VAL_(a) a##_val
#define VAL(a) VAL_(a)

#if VAL(USER_TARGET_MODULE) == k_user_module_delfx_val
  #pragma message "Compiling Delay FX"
  #include "userdelfx.h"
  #define FX_INIT void DELFX_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
  #define FX_PROCESS void DELFX_PROCESS(float *xn, uint32_t frames)
  #define FX_PARAM void DELFX_PARAM(uint8_t index, int32_t value)
  #define FX_PARAM_TIME k_user_delfx_param_time
  #define FX_PARAM_DEPTH k_user_delfx_param_depth
  #define FX_PARAM_SHIFT_DEPTH k_user_delfx_param_shift_depth
#elif VAL(USER_TARGET_MODULE) == k_user_module_revfx_val
  #pragma message "Compiling Reverb FX"
  #include "userrevfx.h"
  #define FX_INIT void REVFX_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
  #define FX_PROCESS void REVFX_PROCESS(float *xn, uint32_t frames)
  #define FX_PARAM void REVFX_PARAM(uint8_t index, int32_t value)
  #define FX_PARAM_TIME k_user_revfx_param_time
  #define FX_PARAM_DEPTH k_user_revfx_param_depth
  #define FX_PARAM_SHIFT_DEPTH k_user_revfx_param_shift_depth
#else
  #error "Wrong module"
#endif

#define MAX_MEM (2400 * 1024) // exact BPM limits, 32K reserved
#define BUF_SIZE (MAX_MEM / (sizeof(float) * 2)) // loop buffer size in samples, stereo
#define BEAT_MAX 16 // maximum loop length, beats
#define BEAT_MIN 16 // minimum loop length, beat fractions
#define BEAT_STEPS 9 // precalculated = log2(BEAT_MAX * BEAT_MIN) + 1

enum {
  mix_mode_overwrite = 0,
  mix_mode_play,
  mix_mode_overdub,
  mix_mode_count
};

enum {
  record_format_stereo = 0,
  record_format_stereo_packed,
  record_format_mono,
  record_format_mono_packed,
  record_format_count
};

typedef struct {
  q15_t a;
  q15_t b;
} __attribute__ ((packed)) q15pair_t;

#define f32pair_to_q15pair(fp) ((q15pair_t){f32_to_q15((fp).a),f32_to_q15((fp).b)})
#define q15pair_to_f32pair(qp) ((f32pair_t){q15_to_f32((qp).a),q15_to_f32((qp).b)})
#define q15pair_add(q1,q2) ((q15pair_t){q15add(q1.a,q2.a),q15add(q1.b,q2.b)})
