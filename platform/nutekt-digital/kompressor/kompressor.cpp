/*
 * File: kompressor.cpp
 *
 * RMS downward compressor/expander for Modulation FX, Delay FX and Reverb FX
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include <float.h>
#include "fxwrapper.h"
#include "osc_api.h"

#define MIN_THRESHOLD 80 //dB
#define ATTACK_TIME .01f //seconds
#define RELEASE_TIME .01f //seconds
#define RMS_TIME .1f //seconds
#define RMS_LEN (RMS_TIME * k_samplerate)

static float s_threshold;
static f32pair_t s_sum;
static f32pair_t s_rms;
static f32pair_t s_gain;
#ifdef FX_MODFX_SUB
static f32pair_t s_sum_sub;
static f32pair_t s_rms_sub;
static f32pair_t s_gain_sub;
#endif
static float s_gain_tgt;
static float s_attack;
static float s_release;
#ifndef FX_MODFX
static float s_wet;
#endif
static float s_len_recip;
static uint32_t s_pos;

FX_INIT
{
  s_threshold = .5f;
  s_sum = f32pair(0.f, 0.f);
  s_rms = f32pair(.5f, .5f);
  s_gain = f32pair(1.f, 1.f);
#ifdef FX_MODFX_SUB
  s_sum_sub = f32pair(0.f, 0.f);
  s_rms_sub = f32pair(.5f, .5f);
  s_gain_sub = f32pair(1.f, 1.f);
#endif
  s_gain_tgt = 1.f;
  s_attack = 0.f;
  s_release = 0.f;
#ifndef FX_MODFX 
  s_wet = .5f;
#endif
  s_len_recip = 1.f / RMS_LEN;
  s_pos = 0;
}

FX_PROCESS
{
  float gain_min = 1.f;
  float gain_max = s_gain_tgt;
  if (s_gain_tgt < 1.f) {
    gain_min = s_gain_tgt;
    gain_max = 1.f;
  }
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

    s_sum = f32pair_add(s_sum, f32pair_mul(valp, valp));
#ifdef FX_MODFX_SUB
    s_sum_sub = f32pair_add(s_sum, f32pair_mul(valp, valp));
#endif
    if (s_pos >= RMS_LEN) {
      s_pos = 0;
      s_rms = f32pair(fastpowf(s_sum.a * s_len_recip, .5f), fastpowf(s_sum.b * s_len_recip, .5f));
      s_sum = f32pair(0.f, 0.f);
#ifdef FX_MODFX_SUB
      s_rms_sub = f32pair(fastpowf(s_sum_sub.a * s_len_recip, .5f), fastpowf(s_sum_sub.b * s_len_recip, .5f));
      s_sum_sub = f32pair(0.f, 0.f);
#endif
    }
    s_pos++;

    if ((s_gain_tgt > 1.f && s_rms.a < s_threshold) || (s_gain_tgt < 1.f && s_rms.a > s_threshold)) {
      if ((valp.a < s_threshold && valp.a > 0.f) || (valp.a > s_threshold))
        valp.a = clipminf(0.f, (valp.a - s_threshold) * s_gain.a + s_threshold);
      else if ((valp.a > -s_threshold && valp.a < 0.f) || (valp.a < -s_threshold))
        valp.a = clipmaxf(0.f, (valp.a + s_threshold) * s_gain.a - s_threshold);
      s_gain.a += s_attack;
    } else
      s_gain.a -= s_release;

    if ((s_gain_tgt > 1.f && s_rms.b < s_threshold) || (s_gain_tgt < 1.f && s_rms.b > s_threshold)) {
      if ((valp.b < s_threshold && valp.b > 0.f) || (valp.b > s_threshold))
        valp.b = clipminf(0.f, (valp.b - s_threshold) * s_gain.b + s_threshold);
      else if ((valp.b > -s_threshold && valp.b < 0.f) || (valp.b < -s_threshold))
        valp.b = clipmaxf(0.f, (valp.b + s_threshold) * s_gain.b - s_threshold);
      s_gain.b += s_attack;
    } else
      s_gain.b -= s_release;

    s_gain.a = clipminmaxf(gain_min, s_gain.a, gain_max);
    s_gain.b = clipminmaxf(gain_min, s_gain.b, gain_max);
#ifdef FX_MODFX
    *(y++) = valp;
#else
    *x = f32pair_linint(s_wet, *x, valp);
#endif
#ifdef FX_MODFX_SUB
    valp = *x_sub;

    if ((s_gain_tgt > 1.f && s_rms_sub.a < s_threshold) || (s_gain_tgt < 1.f && s_rms_sub.a > s_threshold)) {
      if ((valp.a < s_threshold && valp.a > 0.f) || (valp.a > s_threshold))
        valp.a = clipminf(0.f, (valp.a - s_threshold) * s_gain_sub.a + s_threshold);
      else if ((valp.a > -s_threshold && valp.a < 0.f) || (valp.a < -s_threshold))
        valp.a = clipmaxf(0.f, (valp.a + s_threshold) * s_gain_sub.a - s_threshold);
      s_gain_sub.a += s_attack;
    } else
      s_gain_sub.a -= s_release;

    if ((s_gain_tgt > 1.f && s_rms_sub.b < s_threshold) || (s_gain_tgt < 1.f && s_rms_sub.b > s_threshold)) {
      if ((valp.b < s_threshold && valp.b > 0.f) || (valp.b > s_threshold))
        valp.b = clipminf(0.f, (valp.b - s_threshold) * s_gain_sub.b + s_threshold);
      else if ((valp.b > -s_threshold && valp.b < 0.f) || (valp.b < -s_threshold))
        valp.b = clipmaxf(0.f, (valp.b + s_threshold) * s_gain_sub.b - s_threshold);
      s_gain_sub.b += s_attack;
    } else
      s_gain_sub.b -= s_release;

    s_gain_sub.a = clipminmaxf(gain_min, s_gain_sub.a, gain_max);
    s_gain_sub.b = clipminmaxf(gain_min, s_gain_sub.b, gain_max);

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
    s_gain_tgt = valf * 2.f;
    if (s_gain_tgt == 0.f) //noise gate
      s_gain_tgt = FLT_MAX;
    else if (s_gain_tgt == 1.f) // dry
      s_gain_tgt = 1.f;
    else if (s_gain_tgt < 1.f) //expand - gain inf...1
      s_gain_tgt = 1.f / s_gain_tgt;
    else if (s_gain_tgt > 1.f) //compress - gain 1...0
      s_gain_tgt = 2.f - s_gain_tgt;
    break;
    s_attack = (s_gain_tgt - 1.f) / (ATTACK_TIME * k_samplerate);
    s_release = (s_gain_tgt - 1.f) / (RELEASE_TIME * k_samplerate);
#ifndef FX_MODFX
  case FX_PARAM_SHIFT_DEPTH:
    s_wet = valf;
    break;
#endif
  default:
    break;
  }
}
