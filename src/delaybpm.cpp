/*
 * File: delaybpm.cpp
 *
 * BPM-aligned delay for Delay FX and Reverb FX
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "fxwrapper.h"
#include "osc_api.h"

#define MAX_MEM (2400 * 1024) // exact BPM limits, 32K reserved
#define BUF_SIZE (MAX_MEM / sizeof(f32pair_t)) // loop buffer size in samples, stereo
#define BEAT_MAX 16 // maximum loop length, beats
#define BEAT_MIN 16 // minimum loop length, beat fractions
#define BEAT_STEPS 9 // precalculated = log2(BEAT_MAX * BEAT_MIN) + 1

static __sdram f32pair_t s_delay_ram[BUF_SIZE];
static uint32_t s_write_pos = 0;
static uint32_t s_beats = 4;
static float s_mix = .5f;
static float s_depth = .5f;

FX_INIT
{

}

FX_PROCESS
{
  f32pair_t valf;
  float dry = s_mix > .5f ? 2.f - s_mix * 2.f : 1.f;
  float wet = s_mix < .5f ? s_mix * 2.f: 1.f;
  uint32_t pos = (int32_t)((k_samplerate * 60 / BEAT_MIN) / fx_get_bpmf()) << s_beats;

  while (pos > BUF_SIZE)
    pos >>= 1;

  if (pos > s_write_pos)
    pos -= BUF_SIZE;

  pos = s_write_pos - pos;

  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
    valf = f32pair_mulscal(s_delay_ram[pos++], s_depth);
    s_delay_ram[s_write_pos++] = f32pair_add(valf, *x);
    *x = f32pair_add(f32pair_mulscal(*x, dry), f32pair_mulscal(valf, wet));

    if (s_write_pos >= BUF_SIZE)
      s_write_pos = 0;

    if (pos >= BUF_SIZE)
      pos = 0;
  }
}

FX_PARAM
{
  const float valf = q31_to_f32(value);
  switch (index) {
  case FX_PARAM_TIME: // delay beats
    s_beats = clipmaxf(si_floorf(valf * BEAT_STEPS), BEAT_STEPS - 1);
    break;
  case FX_PARAM_DEPTH: // delay depth & feedback
    s_depth = valf;
    break;
  case FX_PARAM_SHIFT_DEPTH: // dry/wet balance
    s_mix = valf;
    break;
  default:
    break;
  }
}
