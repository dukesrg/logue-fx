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

#define MIN_RATE 480 //minimum sampling rate
#define MAX_RATE k_samplerate //maximum sampling rate
#define MIN_BITS 1 //minimum bits
#define MAX_BITS 24 //maximum bits

static float s_rate;
static float s_rate_recip;
static float s_count;
static f32pair_t s_acc;
static f32pair_t s_val;
static float s_bits;
static float s_wet;
static float s_weight;

FX_INIT
{
  s_rate = 1.f;
  s_rate_recip = 1.f;
  s_count = 1.f;
  s_acc = f32pair(0.f, 0.f);
  s_val = f32pair(0.f, 0.f);
  s_bits = MAX_BITS - 1;
  s_wet = .5f;
  s_weight = 0.f;
}

FX_PROCESS
{
  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
    if (s_count < s_rate) {
      s_acc = f32pair_add(s_acc, f32pair_mulscal(*x, s_rate_recip));
    } else {
      s_count -= s_rate;
      s_val = f32pair_add(s_acc, f32pair_mulscal(*x, 1.f - s_count));
      s_acc = f32pair_mulscal(*x, s_count);
    }
    f32pair_t y = s_val;
    s_count++;
    if (s_bits < MAX_BITS) {
      if (y.a > 0.f)
        y.a = ceilf(y.a * s_bits) / s_bits;
      else if (y.a < 0.f)
        y.a = floorf(y.a * s_bits) / s_bits;
      if (y.b > 0.f)
        y.b = ceilf(y.b * s_bits) / s_bits;
      else if (y.b < 0.f)
        y.b = floorf(y.b * s_bits) / s_bits;
    }
    *x = f32pair_linint(s_wet, *x, y);
/*
    float y = *x * s_bits;
    if (y > 0.f)
      y = ceilf(y);
    else if (y < 0.f)
      y = floorf(y);
    *x = linintf(s_wet, *x, y / s_bits);
*/
//    uint32_t a = (*(uint32_t *)&x->a) & s_mask;
//    uint32_t b = (*(uint32_t *)&x->b) & s_mask;
//    *x = f32pair_linint(s_wet, *x, f32pair(*(float *)&a, *(float *)&b));
  }
}

FX_PARAM
{
  float valf = q31_to_f32(value);
  switch (index) {
  case FX_PARAM_TIME: //frequency reduce
    s_rate = k_samplerate / (MAX_RATE - (valf * (MAX_RATE - MIN_RATE)));
    s_rate_recip = 1 / s_rate;
    break;
  case FX_PARAM_DEPTH: //bit resolution reduce
    s_bits = powf(2.f, MAX_BITS - (valf * (MAX_BITS - MIN_BITS)) - 1);
    break;
  case FX_PARAM_SHIFT_DEPTH:
    s_wet = valf;
    break;
  default:
    break;
  }
}
