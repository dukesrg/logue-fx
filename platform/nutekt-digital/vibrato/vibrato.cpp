/*
 * File: vibrato.cpp
 *
 * Simple vibrato with pitch shifter for Delay FX and Reverb FX
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "vibrato.hpp"
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
static __sdram f32pair_t s_loop[BUF_SIZE];
static dsp::SimpleLFO s_lfo;

FX_INIT
{
  s_depth = 0.f;
  s_speed = 1.f;
  s_read_pos = 0.f;
  s_write_pos = 0;
  buf_clr_f32((float*)s_loop, BUF_SIZE * sizeof(f32pair_t)/sizeof(float));
  s_lfo.reset();
  s_lfo.setF0(0.f, k_samplerate_recipf);
}

FX_PROCESS
{
  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
    f32pair_t y = *x;
    uint32_t pos = (uint32_t)s_read_pos;
    *x = f32pair_linint(s_read_pos - pos, s_loop[pos], s_loop[pos < BUF_SIZE ? pos : 0]);
    s_read_pos += powf(F_FACTOR, s_speed + s_lfo.sine_bi() * s_depth);
    if ((uint32_t)s_read_pos >= BUF_SIZE)
      s_read_pos -= BUF_SIZE;
    s_lfo.cycle();
    s_loop[s_write_pos] = y;
    s_write_pos++;
    if (s_write_pos >= BUF_SIZE)
      s_write_pos = 0;
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
  case FX_PARAM_SHIFT_DEPTH: //pitch shift
    s_speed = (valf - .5f) * MAX_SHIFT * 2;
    break;
  default:
    break;
  }
}
