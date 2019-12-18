#pragma once
/*
 * File: looper.hpp
 *
 * Kaossilator-style loop recorder for revfx
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "fx_api.h"
#include "float_math.h"
#include "osc_api.h"
#include "userrevfx.h"

#include "delayline.hpp"

#define MAX_MEM (2432 * 1024)
#define BUF_SIZE (MAX_MEM / (sizeof(float) * 2))
#define BEAT_STEPS 9 // 1/16 to 16

enum {
  mix_mode_overwrite = 0,
  mix_mode_play,
  mix_mode_overdub,
  mix_mode_count
};

static dsp::DelayLine s_delay;

static __sdram float s_delay_ram[BUF_SIZE * 2];

static int s_beats;
static int s_mix_mode;

static inline __attribute__((optimize("Ofast"),always_inline))
void looper_init(uint32_t platform, uint32_t api)
{
  s_delay.setMemory(s_delay_ram, BUF_SIZE * 2);
  s_beats = BEAT_STEPS - 1;
  s_mix_mode = mix_mode_play;
}

static inline __attribute__((optimize("Ofast"),always_inline))
void looper_process(float *xn, uint32_t frames)
{
  float * __restrict x = xn;
  const float * x_e = x + 2 * frames;

// adjust length to the buffer limit
  int beats = s_beats;
  float val = (k_samplerate * 60.f / 4.f) / fx_get_bpmf();
  float len;
  do {
    len = val * (1 << (beats--));
  } while (len > BUF_SIZE);

  switch(s_mix_mode) {
  case mix_mode_overwrite:
    for (; x != x_e; x++) {
      val = s_delay.read(len);
      s_delay.write(*x);
      *x = (*x + val) * .5f;
    }
    break;
  case mix_mode_play:
    for (; x != x_e; x++) {
      *x = (*x + s_delay.read(len)) * .5f;
    }
    break;
  case mix_mode_overdub:
    for (; x != x_e; x++) {
      val = *x + s_delay.read(len);
      s_delay.write(val);
      *x = val * .5f;
    }
    break;
  default:
    break;
  }
}

static inline __attribute__((optimize("Ofast"),always_inline))
void looper_param(uint8_t index, int32_t value)
{
  const float valf = q31_to_f32(value);
  switch (index) {
  case k_user_revfx_param_time: // beats length factor
    s_beats = clipmaxf(si_floorf(valf * BEAT_STEPS), BEAT_STEPS - 1);
    break;
  case k_user_revfx_param_depth: // record mode
    s_mix_mode = clipmaxf(si_floorf(valf * mix_mode_count), mix_mode_count - 1);
    break;
  case k_user_revfx_param_shift_depth:
    break;
  default:
    break;
  }
}
