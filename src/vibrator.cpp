/*
 * File: vibrato.cpp
 *
 * Simple vibrato with pitch shifter for Modulation FX, Delay FX and Reverb FX
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
#ifdef VIBRATORv2
static uint32_t s_lfo_index = 0;
#else
static float s_speed = 0.f;
#endif
static float s_read_pos;
static uint32_t s_write_pos;
static dsp::SimpleLFO s_lfo;
static __sdram f32pair_t s_loop[BUF_SIZE];
/*
static __sdram uint16_t s_prev_grain_a[BUF_SIZE];
static __sdram uint16_t s_prev_grain_b[BUF_SIZE];
static __sdram uint16_t s_next_grain_a[BUF_SIZE];
static __sdram uint16_t s_next_grain_b[BUF_SIZE];
static f32pair_t s_prev_val;
static uint16_t s_prev_grain_pos_a;
static uint16_t s_prev_grain_pos_b;
*/
#ifdef FX_MODFX_SUB
static __sdram f32pair_t s_loop_sub[BUF_SIZE];
#endif

#ifdef VIBRATORv2
inline __attribute__((optimize("Ofast"),always_inline))
float sine_bi(void) {
  return s_lfo.sine_bi();
}

inline __attribute__((optimize("Ofast"),always_inline))
float sine_uni(void) {
  return s_lfo.sine_uni();
}

inline __attribute__((optimize("Ofast"),always_inline))
float triangle_bi(void) {
  return s_lfo.triangle_bi();
}

inline __attribute__((optimize("Ofast"),always_inline))
float triangle_uni(void) {
  return s_lfo.triangle_uni();
}

inline __attribute__((optimize("Ofast"),always_inline))
float saw_bi(void) {
  return s_lfo.saw_bi();
}

inline __attribute__((optimize("Ofast"),always_inline))
float saw_uni(void) {
  return s_lfo.saw_uni();
}

inline __attribute__((optimize("Ofast"),always_inline))
float square_bi(void) {
  return s_lfo.square_bi();
}

inline __attribute__((optimize("Ofast"),always_inline))
float square_uni(void) {
  return s_lfo.square_uni();
}

inline __attribute__((optimize("Ofast"),always_inline))
float snh_bi(void) {
  static float snh, sq_old;
  float sq_new = s_lfo.square_uni();
  if (sq_new != sq_old) {
     sq_old = sq_new;
     snh = fx_white();
  }
  return snh;
}

inline __attribute__((optimize("Ofast"),always_inline))
float snh_uni(void) {
  static float snh, sq_old;
  float sq_new = s_lfo.square_uni();
  if (sq_new != sq_old) {
     sq_old = sq_new;
     snh = si_fabsf(fx_white());
  }
  return snh;
}

static float (*s_lfo_ptr[])() = {
  sine_uni,
  triangle_uni,
  saw_uni,
  square_uni,
  snh_uni,
  sine_bi,
  triangle_bi,
  saw_bi,
  square_bi,
  snh_bi
};
#endif

FX_INIT
{
  s_depth = 0.f;
  s_read_pos = 0.f;
  s_write_pos = 0;
  s_lfo.reset();
  s_lfo.setF0(0.f, k_samplerate_recipf);
  buf_clr_f32((float*)s_loop, BUF_SIZE * sizeof(f32pair_t)/sizeof(float));
#ifdef FX_MODFX_SUB
  buf_clr_f32((float*)s_loop_sub, BUF_SIZE * sizeof(f32pair_t)/sizeof(float));
#endif
  for (uint32_t i = 0; i < BUF_SIZE; i++) {
    s_prev_grain_a[i] = 0xFFFF;
    s_next_grain_a[i] = 0xFFFF;
    s_prev_grain_b[i] = 0xFFFF;
    s_next_grain_b[i] = 0xFFFF;
  }
  s_prev_grain_pos_a = 0xFFFF;
  s_prev_grain_pos_b = 0xFFFF;
}

FX_PROCESS
{
#ifdef FX_MODFX
  f32pair_t * __restrict y = (f32pair_t*)yn;
#endif
#ifdef FX_MODFX_SUB
  f32pair_t * __restrict y_sub = (f32pair_t*)sub_yn;
#endif

#ifdef VIBRATORv2
  float (*func_lfo)() = s_lfo_ptr[s_lfo_index];
  if (s_lfo_index < 5) {

#ifdef FX_MODFX_SUB
  for (f32pair_t * __restrict x = (f32pair_t*)xn, * __restrict x_sub = (f32pair_t*)sub_xn; frames--; x++, x_sub++) {
#else
  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
#endif
#ifdef FX_MODFX_SUB
    *(y_sub++) = f32pair_mulscal(*x_sub, 1.f - (*func_lfo)() * s_depth);
#endif
#ifdef FX_MODFX
    *(y++) = f32pair_mulscal(*x, 1.f - (*func_lfo)() * s_depth);
#else
    *x = f32pair_mulscal(*x, 1.f - (*func_lfo)() * s_depth);
#endif
    s_lfo.cycle();
  }

  } else {
#endif

#ifdef FX_MODFX_SUB
  for (f32pair_t * __restrict x = (f32pair_t*)xn, * __restrict x_sub = (f32pair_t*)sub_xn; frames--; x++, x_sub++) {
#else
  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
#endif
/*
if (s_grain_count == 0) {
  *(y++) = 0.f;
} else {

}
*/
    uint32_t pos = (uint32_t)s_read_pos;
#ifdef FX_MODFX_SUB
    f32pair_t valp_sub = *x_sub;
    *(y_sub++) = f32pair_linint(s_read_pos - pos, s_loop_sub[pos], s_loop_sub[pos < BUF_SIZE ? pos + 1: 0]);
    s_loop_sub[s_write_pos] = valp_sub;
#endif
    f32pair_t valp = *x;
#ifdef FX_MODFX
    *(y++) = f32pair_linint(s_read_pos - pos, s_loop[pos], s_loop[pos < BUF_SIZE ? pos + 1: 0]);
#else
    *x = f32pair_linint(s_read_pos - pos, s_loop[pos], s_loop[pos < BUF_SIZE ? pos + 1: 0]);
#endif
    s_loop[s_write_pos] = valp;
/*
if (s_prev_val.a < 0.f && valp.a >= 0.f) {
  s_prev_grain_a[s_write_pos] = s_prev_grain_pos_a;
  if (s_prev_grain_pos_a != 0xFFFF)
    s_next_grain_a[s_prev_grain_pos_a] = s_write_pos;
  s_prev_grain_pos_a = s_write_pos;
} else {
  s_prev_grain_a[s_write_pos] = 0xFFFF;
  if (s_next_grain_a[s_write_pos] != 0xFFFF)
    s_prev_grain_a[s_next_grain_a[s_write_pos]] = 0xFFFF;
  s_next_grain_a[s_write_pos] = 0xFFFF;
}
if (s_prev_val.b < 0.f && valp.b >= 0.f) {
  s_prev_grain_b[s_write_pos] = s_prev_grain_pos_b;
  if (s_prev_grain_pos_b != 0xFFFF)
    s_next_grain_b[s_prev_grain_pos_b] = s_write_pos;
  s_prev_grain_pos_b = s_write_pos;
} else {
  s_prev_grain_b[s_write_pos] = 0xFFFF;
  if (s_next_grain_b[s_write_pos] != 0xFFFF)
    s_prev_grain_b[s_next_grain_a[s_write_pos]] = 0xFFFF;
  s_next_grain_b[s_write_pos] = 0xFFFF;
}
s_prev_val = valp;
*/
#ifdef VIBRATORv2
    s_read_pos += fastpowf(F_FACTOR, (*func_lfo)() * s_depth);
#else
    s_read_pos += fastpowf(F_FACTOR, s_speed + s_lfo.sine_bi() * s_depth);
#endif
    if ((uint32_t)s_read_pos >= BUF_SIZE)
      s_read_pos -= BUF_SIZE;
/*
if (s_prev_grain_a[(uint32_t)s_read_pos] != 0xFFFF && s_prev_grain_a[s_prev_grain_a[(uint32_t)s_read_pos]] == 0xFFFF) {
//  while (s_next_grain_a[(uint32_t)s_read_pos] != 0xFFFF)
    s_read_pos = s_next_grain_a[(uint32_t)s_read_pos];
} else if (s_next_grain_a[(uint32_t)s_read_pos] != 0xFFFF && s_next_grain_a[s_next_grain_a[(uint32_t)s_read_pos]] == 0xFFFF) {
//  while (s_prev_grain_a[(uint32_t)s_read_pos] != 0xFFFF)
    s_read_pos = s_prev_grain_a[(uint32_t)s_read_pos];
}
*/
    s_write_pos++;
    if (s_write_pos >= BUF_SIZE)
      s_write_pos = 0;
    s_lfo.cycle();
  }

#ifdef VIBRATORv2
  }
#endif
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
  case FX_PARAM_SHIFT_DEPTH:
#ifdef VIBRATORv2
    s_lfo_index = clipmaxf(si_floorf(valf * 10), 10 - 1);
#else
    s_speed = (valf - .5f) * MAX_SHIFT * 2; //pitch shift
#endif
    break;
#endif
  default:
    break;
  }
}
