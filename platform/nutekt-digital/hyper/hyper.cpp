/*
 * File: hyper.cpp
 *
 * Unison for Modulation FX, Delay FX and Reverb FX
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "buffer_ops.h"
#include "fxwrapper.h"
#include "osc_api.h"
#include <algorithm>

#define F_FACTOR 1.0594631f //chromatic semitone frequency factor
#define MAX_DETUNE 1.f //maximum detune between neighbor unison voices in seitones
#define MAX_UNISON 12 //maximum unison pairs
#define BUF_SIZE (k_samplerate / 100)

static __sdram f32pair_t s_loop[BUF_SIZE];
#ifdef FX_MODFX_SUB
static __sdram f32pair_t s_loop_sub[BUF_SIZE];
#endif
static float s_speed[MAX_UNISON * 2];
static float s_read_pos[MAX_UNISON * 2];
static uint32_t s_write_pos;
static float s_unison;
#ifndef FX_MODFX
static float s_wet;
#endif

FX_INIT
{
  buf_clr_f32((float*)s_loop, BUF_SIZE * sizeof(f32pair_t)/sizeof(float));
#ifdef FX_MODFX_SUB
  buf_clr_f32((float*)s_loop_sub, BUF_SIZE * sizeof(f32pair_t)/sizeof(float));
#endif
  std::fill_n(s_speed, MAX_UNISON * 2, 1.f);
  std::fill_n(s_read_pos, MAX_UNISON * 2, 0.f);
  s_write_pos = 0;
  s_unison = 2;
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
    f32pair_t valp_sub = f32pair(0.f, 0.f);
#else
  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
#endif
    f32pair_t valp = f32pair(0.f, 0.f);
    for (uint32_t i = 0; i < MAX_UNISON * 2; i++) {
      if (i < s_unison) {
        uint32_t pos = (uint32_t)s_read_pos[i];
//        valp = f32pair_add(valp, f32pair_linint(s_read_pos[i] - pos, s_loop[pos], s_loop[pos + 1 < BUF_SIZE ? pos : 0])); //too slow
        valp = f32pair_add(valp, s_loop[pos]);
#ifdef FX_MODFX_SUB
        valp_sub = f32pair_add(valp_sub, s_loop_sub[pos]);
#endif
      }
      s_read_pos[i] += s_speed[i];
      if ((uint32_t)s_read_pos[i] >= BUF_SIZE)
        s_read_pos[i] -= BUF_SIZE;
    };
    s_loop[s_write_pos] = *x;
#ifdef FX_MODFX_SUB
    s_loop_sub[s_write_pos] = *x_sub;
#endif
    s_write_pos++;
    if (s_write_pos >= BUF_SIZE)
      s_write_pos = 0;
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
  case FX_PARAM_TIME: //detune value
    valf *= MAX_DETUNE;
    for (uint32_t i = 0; i < MAX_UNISON; i++) {
      s_speed[i * 2] = fastpowf(F_FACTOR, valf * (i + 1));
      s_speed[i * 2 + 1] = 1.f / s_speed[i * 2];
    }
    break;
  case FX_PARAM_DEPTH: //unison voices
    s_unison = 2 * (1 + clipmaxf(si_floorf(valf * MAX_UNISON), MAX_UNISON - 1));
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
