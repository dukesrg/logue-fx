/*
 * File: vibrato.cpp
 *
 * Simple vibrato with pitch shifter for MNodulation FX, Delay FX and Reverb FX
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "buffer_ops.h"
#include "fxwrapper.h"
#include "osc_api.h"
#include "simplelfo.hpp"

#define MAX_RATE 30.f //maximum LFO rate
#define MAX_DEPTH 1.f //maximum LFO depth in semitones
#define MAX_SHIFT 12 //maximum frequency shift spread in semitones
#define BUF_SIZE (k_samplerate / 100)
#define F_FACTOR 1.0594631f //chromatic semitone frequency factor

static float s_depth;
static float s_speed;
static float s_read_pos;
static uint32_t s_write_pos;
static dsp::SimpleLFO s_lfo;
static __sdram f32pair_t s_loop[BUF_SIZE];
#ifdef FX_MODFX_SUB
static __sdram f32pair_t s_loop_sub[BUF_SIZE];
#endif

FX_INIT
{
  s_depth = 0.f;
  s_speed = 0.f;
  s_read_pos = 0.f;
  s_write_pos = 0;
  s_lfo.reset();
  s_lfo.setF0(0.f, k_samplerate_recipf);
  buf_clr_f32((float*)s_loop, BUF_SIZE * sizeof(f32pair_t)/sizeof(float));
#ifdef FX_MODFX_SUB
  buf_clr_f32((float*)s_loop_sub, BUF_SIZE * sizeof(f32pair_t)/sizeof(float));
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
    uint32_t pos = (uint32_t)s_read_pos;
#ifdef FX_MODFX_SUB
    f32pair_t valp_sub = *x_sub;
    *(y_sub++) = f32pair_linint(s_read_pos - pos, s_loop_sub[pos], s_loop_sub[pos < BUF_SIZE ? pos : 0]);
    s_loop_sub[s_write_pos] = valp_sub;
#endif
    f32pair_t valp = *x;
#ifdef FX_MODFX
    *(y++) = f32pair_linint(s_read_pos - pos, s_loop[pos], s_loop[pos < BUF_SIZE ? pos : 0]);
#else
    *x = f32pair_linint(s_read_pos - pos, s_loop[pos], s_loop[pos < BUF_SIZE ? pos : 0]);
#endif
    s_loop[s_write_pos] = valp;
    s_read_pos += fastpowf(F_FACTOR, s_speed + s_lfo.sine_bi() * s_depth);
    if ((uint32_t)s_read_pos >= BUF_SIZE)
      s_read_pos -= BUF_SIZE;
    s_write_pos++;
    if (s_write_pos >= BUF_SIZE)
      s_write_pos = 0;
    s_lfo.cycle();
  }
}

FX_PARAM
{
  const float valf = q31_to_f32(value);
  switch (index) {
  case FX_PARAM_TIME: //vibrato rate
    s_lfo.setF0(valf * MAX_RATE, k_samplerate_recipf);
    break;
  case FX_PARAM_DEPTH: //vibrato depth
    s_depth = valf * MAX_DEPTH;
    break;
#ifndef FX_MODFX
  case FX_PARAM_SHIFT_DEPTH: //pitch shift
    s_speed = (valf - .5f) * MAX_SHIFT * 2;
    break;
#endif
  default:
    break;
  }
}
