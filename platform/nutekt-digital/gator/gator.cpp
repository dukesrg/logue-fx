/*
 * File: gator.cpp
 *
 * Kaossilator-style gate arpeggiator
 *
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "fxwrapper.h"
#include "osc_api.h"
#include "patterns.h"

static __sdram uint32_t s_pattern[PATTERN_SIZE];

static uint32_t s_pattern_index;
static bool s_restart;
static float s_sample_pos;
static uint32_t s_pattern_pos;
static uint32_t s_pattern_len;
static bool s_gate_a;
static bool s_attack_a;
static bool s_release_a;
static bool s_gate_b;
static bool s_attack_b;
static bool s_release_b;
#ifdef FX_MODFX_SUB
static bool s_gate_a_sub;
static bool s_attack_a_sub;
static bool s_release_a_sub;
static bool s_gate_b_sub;
static bool s_attack_b_sub;
static bool s_release_b_sub;
#endif

void unpack() {
  uint32_t pos = 0;
  uint8_t val;
  s_pattern_len = 0;
  for (uint32_t i = s_pattern_index; i--;)
    while(patterns[pos++]);
  while ((val = patterns[pos])) {
    for (val >>= 4; val--; s_pattern[s_pattern_len++] = 1);
    for (val = patterns[pos++] & 0x0F; val--; s_pattern[s_pattern_len++] = 0);
  }
}

void init() {
  s_sample_pos = 0.f;
  s_pattern_pos = 0;
  s_gate_a = false;
  s_attack_a = false;
  s_release_a = false;
  s_gate_b = false;
  s_attack_b = false;
  s_release_b = false;
#ifdef FX_MODFX_SUB
  s_gate_a_sub = false;
  s_attack_a_sub = false;
  s_release_a_sub = false;
  s_gate_b_sub = false;
  s_attack_b_sub = false;
  s_release_b_sub = false;
#endif
  unpack();
};

FX_INIT
{
  s_pattern_index = 1;
  init();
}

FX_PROCESS
{
  float quant = (k_samplerate * 60 / PATTERN_QUANTIZE) / fx_get_bpmf();
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

    if (si_fabsf(valp.a) < .001f) {
       if (s_attack_a) {
         s_gate_a = true;
         s_attack_a = false;
       }
       if (s_release_a) {
         s_gate_a = false;
         s_release_a = false;
       }
    }
    if (si_fabsf(valp.b) < .001f) {
       if (s_attack_b) {
         s_gate_b = true;
         s_attack_b = false;
       }
       if (s_release_b) {
         s_gate_b = false;
         s_release_b = false;
       }
    }

    if (!s_gate_a)
      valp.a = 0.f;
    if (!s_gate_b)
      valp.b = 0.f;

#ifdef FX_MODFX
    *(y++) = valp;
#else
    *x = valp;
#endif
#ifdef FX_MODFX_SUB
    valp = *x_sub;

    if (si_fabsf(valp.a) < .001f) {
       if (s_attack_a_sub) {
         s_gate_a_sub = true;
         s_attack_a_sub = false;
       }
       if (s_release_a_sub) {
         s_gate_a_sub = false;
         s_release_a_sub = false;
       }
    }
    if (si_fabsf(valp.b) < .001f) {
       if (s_attack_b_sub) {
         s_gate_b_sub = true;
         s_attack_b_sub = false;
       }
       if (s_release_b_sub) {
         s_gate_b_sub = false;
         s_release_b_sub = false;
       }
    }

    if (!s_gate_a_sub)
      valp.a = 0.f;
    if (!s_gate_b_sub)
      valp.b = 0.f;

    *(y_sub++) = valp;
#endif
    if (++s_sample_pos >= quant) {
      s_sample_pos -= quant;
      if (++s_pattern_pos >= s_pattern_len)
        s_pattern_pos = 0;
      if (s_pattern[s_pattern_pos]) {
        s_attack_a = !s_gate_a;
        s_attack_b = !s_gate_b;
#ifdef FX_MODFX_SUB
        s_attack_a_sub = !s_gate_a_sub;
        s_attack_b_sub = !s_gate_b_sub;
#endif
      } else {
        s_release_a = s_gate_a;
        s_release_b = s_gate_b;
#ifdef FX_MODFX_SUB
        s_release_a_sub = s_gate_a_sub;
        s_release_b_sub = s_gate_b_sub;
#endif
      }
    }
  }
}

FX_PARAM
{
  const float valf = q31_to_f32(value);
  switch (index) {
  case FX_PARAM_TIME:
    s_pattern_index = clipminmaxf(0.f, valf * PATTERN_COUNT, PATTERN_COUNT - 1);
    unpack();
    break;
  case FX_PARAM_DEPTH:
    if (s_restart != (valf > .5f)) {
      init();
      s_restart = !s_restart;
    }
    break;
#ifndef FX_MODFX
  case FX_PARAM_SHIFT_DEPTH:
    break;
#endif
  default:
    break;
  }
}
