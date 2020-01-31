/*
 * File: crusher.cpp
 *
 * Bit crusher and decimator for Modulation FX, Delay FX and Reverb FX
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "fx_api.h"
#include "fxwrapper.h"
#include "osc_api.h"

#define MIN_RATE 480 //minimum sampling rate

static float s_rate;
static float s_samples;
static float s_bits;
static float s_bits_recip;
static f32pair_t s_prev;
static f32pair_t s_val;
#ifdef FX_MODFX_SUB
static f32pair_t s_prev_sub;
static f32pair_t s_val_sub;
#endif
#ifndef FX_MODFX
static float s_wet;
#endif

FX_INIT
{
  s_rate = 1.f;
  s_samples = 0.f;
  s_bits_recip = 0.f;
  s_val = f32pair(0.f, 0.f);
#ifdef FX_MODFX_SUB
  s_val_sub = f32pair(0.f, 0.f);
#endif
#ifndef FX_MODFX
  s_wet = .5f;
#endif
}

FX_PROCESS
{
#ifdef FX_MODFX
  f32pair_t * __restrict y = (f32pair_t*)yn;
#endif
#ifdef FX_MODFX_SUB
  f32pair_t * __restrict y_sub = (f32pair_t*)sub_yn;
  for (f32pair_t * __restrict x = (f32pair_t*)xn, * __restrict x_sub = (f32pair_t*)sub_xn; frames--; x++, x_sub++) {
#else
  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
#endif
    if (s_samples >= s_rate) {
      s_samples -= s_rate;
      s_val = f32pair_linint(s_samples, s_prev, *x);
#ifdef FX_MODFX_SUB
      s_val_sub = f32pair_linint(s_samples, s_prev_sub, *x_sub);
#endif
    }
    s_samples++;
    s_prev = *x;
    f32pair_t valp = s_val;
#ifdef FX_MODFX_SUB
    s_prev_sub = *x_sub;
    f32pair_t valp_sub = s_val_sub;
#endif
    if (s_bits_recip != 0.f) {
      valp = f32pair_mulscal(valp, s_bits);
      valp.a = valp.a < 0.f ? floorf(valp.a) : ceilf(valp.a);
      valp.b = valp.b < 0.f ? floorf(valp.b) : ceilf(valp.b);
      valp = f32pair_mulscal(valp, s_bits_recip);
#ifdef FX_MODFX_SUB
      valp_sub = f32pair_mulscal(valp_sub, s_bits);
      valp_sub.a = valp_sub.a < 0.f ? floorf(valp_sub.a) : ceilf(valp_sub.a);
      valp_sub.b = valp_sub.b < 0.f ? floorf(valp_sub.b) : ceilf(valp_sub.b);
      valp_sub = f32pair_mulscal(valp_sub, s_bits_recip);
#endif
    }
#ifdef FX_MODFX_SUB
    *(y_sub++) = valp_sub;
#endif
#ifdef FX_MODFX
    *(y++) = valp;
#else
    *x = f32pair_linint(s_wet, *x, valp);
#endif
  }
}

FX_PARAM
{
  float valf = q31_to_f32(value);
  switch (index) {
  case FX_PARAM_TIME: //sampling rate reduce
    s_rate = clipmaxf(1.f / (1.f - valf), k_samplerate / MIN_RATE);
    break;
  case FX_PARAM_DEPTH: //bit resolution reduce
    s_bits = fx_bitresf(valf);
    s_bits_recip = 1.f / s_bits;
    break;
#ifndef FX_MODFX
  case FX_PARAM_SHIFT_DEPTH:
    s_wet = valf;
    break;
#endif
  default:
    break;
  }
}
