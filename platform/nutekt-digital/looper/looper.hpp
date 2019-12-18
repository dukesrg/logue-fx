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
//#include "fixed_math.h"
#include "float_math.h"
#include "osc_api.h"
#include "userrevfx.h"

#include "delayline.hpp"

#define MAX_MEM (2400 * 1024) //exact BPM limits, 32K reserved
#define BUF_SIZE (MAX_MEM / (sizeof(float) * 2))
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
/*
typedef struct {
  q15_t a;
  q15_t b;
} q15pair_t;

typedef union {
  float f;
  q15pair_t q;
} q15f32_t;

#define f32x2_to_q15(f1, f2) (q15pair_t){f32_to_q15(f1), f32_to_q15(f2)}
*/
static dsp::DelayLine s_delay;

static __sdram float s_delay_ram[BUF_SIZE * 2];

static int s_beats;
static int s_mix_mode;
static int s_record_format;

static inline __attribute__((optimize("Ofast"),always_inline))
void looper_init(uint32_t platform, uint32_t api)
{
  s_delay.setMemory(s_delay_ram, BUF_SIZE * 2);
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

  float valf, valf2;
  switch (s_record_format) {
  case record_format_stereo:
  case record_format_stereo_packed: //ToDo
    len *= 2;
    switch (s_mix_mode) {
    case mix_mode_overwrite:
      for (; x != x_e; x) {
        valf = s_delay.read(len);
        s_delay.write(*x);
        *(x++) = (*x + valf) * .5f;
      }
      break;
    case mix_mode_play:
      for (; x != x_e;) {
        valf = s_delay.read(len);
        *(x++) = (*x + valf) * .5f;
      }
      break;
    case mix_mode_overdub:
      for (; x != x_e; x++) {
        valf = s_delay.read(len);
        *(x++) = (*x + valf) * .5f;
        s_delay.write(valf);
      }
      break;
    default:
      break;
    }
    break;
  case record_format_mono:
  case record_format_mono_packed: //ToDo
    switch (s_mix_mode) {
    case mix_mode_overwrite:
      for (; x != x_e; x) {
        valf = s_delay.read(len);
        s_delay.write((*x + x[1]) * .5f);
        *(x++) = (*x + valf) * .5f;
        *(x++) = (*x + valf) * .5f;
      }
      break;
    case mix_mode_play:
      for (; x != x_e;) {
        valf = s_delay.read(len);
        *(x++) = (*x + valf) * .5f;
        *(x++) = (*x + valf) * .5f;
      }
      break;
    case mix_mode_overdub:
      for (; x != x_e; x) {
        valf = s_delay.read(len) ;
        valf += (*x + x[1]) * .5f;
        *(x++) = (*x + valf) * .5f;
        *(x++) = (*x + valf) * .5f;
        s_delay.write(valf);
      }
      break;
    default:
      break;
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
