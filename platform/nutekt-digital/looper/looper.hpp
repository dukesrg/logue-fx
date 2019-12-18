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

#define MAX_MEM (2400 * 1024) //exact BPM limits, 32K reserved
#define BUF_SIZE (MAX_MEM / sizeof(f32pair_t))
#define BEAT_MAX 16 // maximum loop length, beats
#define BEAT_MIN 16 // minimum loop length, beat fractions
#define BEAT_STEPS 9 // = log2(BEAT_MAX * BEAT_MIN) + 1

enum {
  mix_mode_overwrite = 0,
  mix_mode_play,
  mix_mode_overdub,
  mix_mode_count
};

enum {
  record_format_stereo = 0,
  record_format_stereo_packed,
  record_format_mono,
  record_format_mono_packed,
  record_format_count
};

static dsp::DualDelayLine s_delay;

static __sdram f32pair_t s_delay_ram[BUF_SIZE];

static int s_beats;
static int s_mix_mode;
static int s_record_format;

static inline __attribute__((optimize("Ofast"),always_inline))
void looper_init(uint32_t platform, uint32_t api)
{
  s_delay.setMemory(s_delay_ram, BUF_SIZE);
  s_beats = BEAT_STEPS - 1;
  s_mix_mode = mix_mode_play;
  s_record_format = record_format_stereo;
}

static inline __attribute__((optimize("Ofast"),always_inline))
void looper_process(float *xn, uint32_t frames)
{
  float * __restrict x = xn;
  const float * x_e = x + 2 * frames;

// adjust length to the buffer limit
  int beats = s_beats;
  float val = (k_samplerate * 60 / BEAT_MIN) / fx_get_bpmf();
  float len;
  do {
    len = val * (1 << (beats--));
  } while (len > BUF_SIZE);

  f32pair_t valfp;

  switch(s_mix_mode) {
  case mix_mode_overwrite:
    for (; x != x_e; x+=2) {
      valfp = *(f32pair_t *)x;
      *(f32pair_t *)x = f32pair_linint(.5f, valfp, s_delay.read(len));
      s_delay.write(valfp);
    }
    break;
  case mix_mode_play:
    for (; x != x_e; x+=2) {
      *(f32pair_t *)x = f32pair_linint(.5f, *(f32pair_t *)x, s_delay.read(len));
    }
    break;
  case mix_mode_overdub:
    for (; x != x_e; x+=2) {
      valfp = f32pair_add(*(f32pair_t *)x, s_delay.read(len));
      *(f32pair_t *)x = f32pair_mulscal(valfp, .5f);
      s_delay.write(valfp);
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
  case k_user_revfx_param_shift_depth: // record format
    s_record_format = clipmaxf(si_floorf(valf * record_format_count), record_format_count - 1);
    break;
  default:
    break;
  }
}
