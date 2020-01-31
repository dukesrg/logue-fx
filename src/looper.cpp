/*
 * File: looper.cpp
 *
 * Kaossilator-style loop recorder for Delay FX and Reverb FX
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "buffer_ops.h"
#include "fxwrapper.h"
#include "osc_api.h"

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

typedef struct {
  q15_t a;
  q15_t b;
} __attribute__ ((packed)) q15pair_t;

#define f32pair_to_q15pair(fp) ((q15pair_t){f32_to_q15((fp).a),f32_to_q15((fp).b)})
#define q15pair_to_f32pair(qp) ((f32pair_t){q15_to_f32((qp).a),q15_to_f32((qp).b)})
#define q15pair_add(q1,q2) ((q15pair_t){q15add(q1.a,q2.a),q15add(q1.b,q2.b)})

#define MAX_MEM (2400 * 1024) // exact BPM limits, 32K reserved
#define BUF_SIZE (MAX_MEM / (sizeof(float) * 2)) // loop buffer size in samples, stereo
#define BEAT_MAX 16 // maximum loop length, beats
#define BEAT_MIN 16 // minimum loop length, beat fractions
#define BEAT_STEPS 9 // precalculated = log2(BEAT_MAX * BEAT_MIN) + 1

static __sdram float s_loop_ram[BUF_SIZE * 2];
static uint32_t s_loop_pos;
static uint32_t s_beats;
static uint32_t s_mix_mode;
static uint32_t s_record_format;

FX_INIT
{
  buf_clr_f32(s_loop_ram, BUF_SIZE * 2);
//@todo: Sync start (and end) position if sync/arpeggiator information is available
  s_loop_pos = 0;
  s_beats = BEAT_STEPS - 1;
  s_mix_mode = mix_mode_play;
  s_record_format = record_format_stereo;
}

FX_PROCESS
{
  uint32_t maxlen, len = (uint32_t)((k_samplerate * 60 / BEAT_MIN) / fx_get_bpmf()) << s_beats;
  f32pair_t fp, *yfp = (f32pair_t*)s_loop_ram;
  q15pair_t qp, *yqp = (q15pair_t*)s_loop_ram;
  float f, *yf = s_loop_ram;
  q15_t q, *yq = (q15_t*)s_loop_ram;

  maxlen = BUF_SIZE;
  if (s_record_format == record_format_mono_packed)
    maxlen *= 4;
  else if(s_record_format != record_format_stereo)
    maxlen *= 2;

  while (len > maxlen)
    len >>= 1;

  for (f32pair_t * __restrict x = (f32pair_t*)xn; frames--; x++) {
    while (s_loop_pos >= len)
        s_loop_pos -= len;

    switch (s_record_format) {
    default:
    case record_format_stereo:
      fp = yfp[s_loop_pos];
      if (s_mix_mode == mix_mode_overwrite)
        yfp[s_loop_pos] = *x;
      else if (s_mix_mode == mix_mode_overdub)
        yfp[s_loop_pos] = f32pair_add(fp, *x);
      break;
    case record_format_stereo_packed:
      qp = yqp[s_loop_pos];
      fp = q15pair_to_f32pair(qp);
      if (s_mix_mode == mix_mode_overwrite)
        yqp[s_loop_pos] = f32pair_to_q15pair(*x);
      else if (s_mix_mode == mix_mode_overdub)
        yqp[s_loop_pos] = q15pair_add(qp, f32pair_to_q15pair(*x));
      break;
    case record_format_mono:
      f = yf[s_loop_pos];
      fp = (f32pair_t){f, f};
      if (s_mix_mode == mix_mode_overwrite)
        yf[s_loop_pos] = (x->a + x->b) * .5f;
      else if (s_mix_mode == mix_mode_overdub)
        yf[s_loop_pos] = f + (x->a + x->b) * .5f;
      break;
    case record_format_mono_packed:
      q = yq[s_loop_pos];
      fp = q15pair_to_f32pair(((q15pair_t){q, q}));
      if (s_mix_mode == mix_mode_overwrite)
        yq[s_loop_pos] = f32_to_q15((x->a + x->b) * .5f);
      else if (s_mix_mode == mix_mode_overdub)
        yq[s_loop_pos] = q15add(q, f32_to_q15((x->a + x->b) * .5f));
      break;
    }

    *x = f32pair_linint(.5f, *x, fp);
    s_loop_pos++;
  }
}

FX_PARAM
{
  const float valf = q31_to_f32(value);
  uint32_t val;
  switch (index) {
  case FX_PARAM_TIME: // beats length factor
    s_beats = clipmaxf(si_floorf(valf * BEAT_STEPS), BEAT_STEPS - 1);
    break;
  case FX_PARAM_DEPTH: // record mode
    s_mix_mode = clipmaxf(si_floorf(valf * mix_mode_count), mix_mode_count - 1);
    break;
  case FX_PARAM_SHIFT_DEPTH: // record format
    val = clipmaxf(si_floorf(valf * record_format_count), record_format_count - 1);
    if (val != s_record_format) {
      s_record_format = val;
//@todo: Resample & replicate instead of clear
//@todo: Check buffer cleared properly for q15
      buf_clr_f32(s_loop_ram, BUF_SIZE * 2);
    }
    break;
  default:
    break;
  }
}
