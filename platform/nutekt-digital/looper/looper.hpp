/*
 * File: looper.hpp
 *
 * Kaossilator-style loop recorder for Delay FX and Reverb FX
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "fx_api.h"
#include "fixed_math.h"
#include "float_math.h"
#include "osc_api.h"

#define XSTR(x) STR(x)
#define STR(x) #x

#if fxtype==delfx
  #include "userdelfx.h"
  #define FX_INIT DELFX_INIT
  #define FX_PROCESS DELFX_PROCESS
  #define FX_PARAM DELFX_PARAM
  #define FX_PARAM_TIME k_user_delfx_param_time
  #define FX_PARAM_DEPTH k_user_delfx_param_depth
  #define FX_PARAM_SHIFT_DEPTH k_user_delfx_param_shift_depth
#elif fxtype==revfx
  #include "userrevfx.h"
  #define FX_INIT REVFX_INIT
  #define FX_PROCESS REVFX_PROCESS
  #define FX_PARAM REVFX_PARAM
  #define FX_PARAM_TIME k_user_revfx_param_time
  #define FX_PARAM_DEPTH k_user_revfx_param_depth
  #define FX_PARAM_SHIFT_DEPTH k_user_revfx_param_shift_depth
#else
#error "Wrong FX type " XSTR(fxtype)
#endif
#pragma message  "Compiling FX type " XSTR(fxtype)

#include "delayline.hpp"

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

typedef union {
  float f;
  struct {
    q15_t a;
    q15_t b;
  } q;
} __attribute__ ((packed)) f32packed_t;

#define f32pack(f1,f2) (f32packed_t){.q={f32_to_q15(f1),f32_to_q15(f2)}}.f
#define f32unpack1(f1) (q15_to_f32(((f32packed_t){.f=f1}).q.a))
#define f32unpack2(f1) (q15_to_f32(((f32packed_t){.f=f1}).q.b))
