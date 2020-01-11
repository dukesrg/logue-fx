/*
 * File: crusher.cpp
 *
 * Bit crusher and decimator for Delay FX and Reverb FX
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "crusher.hpp"
#include "fx_api.h"

#define MIN_RATE 480 //minimum sampling rate

static float s_rate;
static float s_samples;
static f32pair_t s_prev;
static f32pair_t s_val;
static float s_bits;
static float s_bits_recip;
static float s_wet;

FX_INIT
{
  s_rate = 1.f;
  s_samples = 0.f;
  s_val = f32pair(0.f, 0.f);
  s_bits_recip = 0.f;
  s_wet = .5f;
}

FX_PROCESS
{
  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
    if (s_samples >= s_rate) {
      s_samples -= s_rate;
      s_val = f32pair_linint(s_samples, s_prev, *x);
    }
    s_prev = *x;
    s_samples++;
    f32pair_t y = s_val;
    if (s_bits_recip != 0.f) {
      y = f32pair_mulscal(y, s_bits);
      y.a = y.a < 0.f ? floorf(y.a) : ceilf(y.a);
      y.b = y.b < 0.f ? floorf(y.b) : ceilf(y.b);
      y = f32pair_mulscal(y, s_bits_recip);
    }
    *x = f32pair_linint(s_wet, *x, y);
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
  case FX_PARAM_SHIFT_DEPTH:
    s_wet = valf;
    break;
  default:
    break;
  }
}
