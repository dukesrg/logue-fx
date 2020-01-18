/*
 * File: Kompressor.cpp
 *
 * Peak downward compressor/expander for Modulation FX, Delay FX and Reverb FX
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include <float.h>
#include "fxwrapper.h"

#define MIN_THRESHOLD 80 //dB

static float s_threshold;
static float s_gain;
#ifndef FX_MODFX
static float s_wet;
#endif

FX_INIT
{
  s_threshold = .5f;
  s_gain = 1.f;
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
    f32pair_t valp = *x;

    if ((s_gain > 1.f && valp.a > 0.f && valp.a < s_threshold) || (s_gain < 1.f && valp.a > s_threshold))
      valp.a = clipminf(0.f, (valp.a - s_threshold) * s_gain + s_threshold);
    else if ((s_gain > 1.f && valp.a < 0.f && valp.a > -s_threshold) || (s_gain < 1.f && valp.a < -s_threshold))
      valp.a = clipmaxf(0.f, (valp.a + s_threshold) * s_gain - s_threshold);

    if ((s_gain > 1.f && valp.b > 0.f && valp.b < s_threshold) || (s_gain < 1.f && valp.b > s_threshold))
      valp.b = clipminf(0.f, (valp.b - s_threshold) * s_gain + s_threshold);
    else if ((s_gain > 1.f && valp.b < 0.f && valp.b > -s_threshold) || (s_gain < 1.f && valp.b < -s_threshold))
      valp.b = clipmaxf(0.f, (valp.b + s_threshold) * s_gain - s_threshold);
#ifdef FX_MODFX
    *(y++) = valp;
#else
    *x = f32pair_linint(s_wet, *x, valp);
#endif
#ifdef FX_MODFX_SUB
    valp = *x_sub;

    if ((s_gain > 1.f && valp.a > 0.f && valp.a < s_threshold) || (s_gain < 1.f && valp.a > s_threshold))
      valp.a = clipminf(0.f, (valp.a - s_threshold) * s_gain + s_threshold);
    else if ((s_gain > 1.f && valp.a < 0.f && valp.a > -s_threshold) || (s_gain < 1.f && valp.a < -s_threshold))
      valp.a = clipmaxf(0.f, (valp.a + s_threshold) * s_gain - s_threshold);

    if ((s_gain > 1.f && valp.b > 0.f && valp.b < s_threshold) || (s_gain < 1.f && valp.b > s_threshold))
      valp.b = clipminf(0.f, (valp.b - s_threshold) * s_gain + s_threshold);
    else if ((s_gain > 1.f && valp.b < 0.f && valp.b > -s_threshold) || (s_gain < 1.f && valp.b < -s_threshold))
      valp.b = clipmaxf(0.f, (valp.b + s_threshold) * s_gain - s_threshold);

    *(y_sub++) = valp;
#endif
  }
}

FX_PARAM
{
  float valf = q31_to_f32(value);
  switch (index) {
  case FX_PARAM_TIME: //threshold
    s_threshold = fasterdbampf((valf - 1.f) * MIN_THRESHOLD);
    break;
  case FX_PARAM_DEPTH: //gain
    s_gain = valf * 2.f;
    if (s_gain == 0.f) //noise gate
      s_gain = FLT_MAX;
    else if (s_gain < 1.f) //expand
      s_gain = 1.f / s_gain;
    else if (s_gain > 1.f) //compress
      s_gain = 2.f - s_gain;
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
