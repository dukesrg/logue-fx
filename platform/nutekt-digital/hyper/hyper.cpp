/*
 * File: hyper.cpp
 *
 * Unison for Delay FX and Reverb FX
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "hyper.hpp"
#include <algorithm>

#define F_FACTOR 1.0594631f //chromatic semitone frequency factor
#define MAX_DETUNE 1.f //maximum detune between neighbor unison voices in seitones
#define MAX_UNISON 12 //maximum unison pairs
#define BUF_SIZE (k_samplerate / 100)

static __sdram f32pair_t s_loop[BUF_SIZE];
static float s_speed[MAX_UNISON * 2];
static float s_read_pos[MAX_UNISON * 2];
static uint32_t s_write_pos;
static float s_unison;
static float s_wet;

FX_INIT
{
  buf_clr_f32((float*)s_loop, BUF_SIZE * sizeof(f32pair_t)/sizeof(float));
  std::fill_n(s_speed, MAX_UNISON * 2, 1.f);
  std::fill_n(s_read_pos, MAX_UNISON * 2, 0.f);
  s_write_pos = 0;
  s_unison = 2;
  s_wet = .5f;
}

FX_PROCESS
{
  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
    f32pair_t y = {0.f, 0.f};
    for (uint32_t i = 0; i < MAX_UNISON * 2; i++) {
      if (i < s_unison) {
        uint32_t pos = (uint32_t)s_read_pos[i];
//        y = f32pair_add(y, f32pair_linint(s_read_pos[i] - pos, s_loop[pos], s_loop[pos + 1 < BUF_SIZE ? pos : 0])); //too slow
        y = f32pair_add(y, s_loop[pos]);
      }
      s_read_pos[i] += s_speed[i];
      if ((uint32_t)s_read_pos[i] >= BUF_SIZE)
        s_read_pos[i] -= BUF_SIZE;
    };
    s_loop[s_write_pos] = *x;
    s_write_pos++;
    if (s_write_pos >= BUF_SIZE)
      s_write_pos = 0;
    *x = f32pair_linint(s_wet, *x, y);
  }
}

FX_PARAM
{
  float valf = q31_to_f32(value);
  switch (index) {
  case FX_PARAM_TIME: //detune value
    valf *= MAX_DETUNE;
    for (uint32_t i = 0; i < MAX_UNISON; i++) {
      s_speed[i * 2] = powf(F_FACTOR, valf * (i + 1));
      s_speed[i * 2 + 1] = 1.f / s_speed[i];
    }
    break;
  case FX_PARAM_DEPTH: //unison voices
    s_unison = 2 * (1 + clipmaxf(si_floorf(valf * MAX_UNISON), MAX_UNISON - 1));
    break;
  case FX_PARAM_SHIFT_DEPTH:
    s_wet = valf;
    break;
  default:
    break;
  }
}
