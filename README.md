# logue-fx
Custom effects for Korg logue-sdk compatible synths

This projects depends on customized logue-sdk version

# Crusher

This is a bit crusher and decimator effect for Modulation FX, Delay FX and Reverb FX.

## Parameters
* Sampling frequency (time / A knob) - 48000...480Hz sample rate reduce
* Bit depth (depth / B knob) - 24...1 bit resolution reduce
* Dry/wet mix (shift depth / FX button + B knob)

## Limitations
Dry/wet parameter is not available for Modulation FX.
On the Korg NTS-1 the analog input noise got aplificated by very low bit resolution setting which produces very nasty effect.
To avoid this either:
* make sure actual signal is present on audio input, not silence
* route audio input after the effect

# Gator

This is a Kaossilator-style gate arpeggiator.

## Parameters
* Pattern select (time / A knob) - refer to [KAOSSILATOR Parameter List](https://www.korg.com/us/support/download/manual/1/121/1774/) for patterns
* Pattern restart (depth / B knob) - pass center point to restart pattern for manual beat sync

# Hyper

This is a unison effect inspired by supersaw/hypersaw for Modulation FX, Delay FX and Reverb FX.

## Parameters
* Detune (time / A knob) - detune value in 0...1 semitone
* Unision voices (depth / B knob) - 1 to 12 pairs, each pair detuned by additional +/- detune value
* Dry/wet mix (shift depth / FX button + B knob)

# Kompressor

This is RMS downward compressor/expander for Modulation FX, Delay FX and Reverb FX.

## Parameters
* Threshold level (time / A knob) - -80...0dB
* Ratio (depth / B knob) - (noise gate)∞:1...(expand)...1:1...(compress)...∞:1(limiter)
* Dry/wet mix (shift depth / FX button + B knob)

# Looper

This is a Kaossilator-style loop recorded for Delay FX and Reverb FX.

## Parameters
* Loop length (time / A knob) - linked to the current BPM and sticked to 16, 8, 4, 2, 1, ½, ¼, ⅛, 1⁄16 beats
* Play/Record mode (depth / B knob)
  * Overwrite (left third) - 50/50 mixed input and delay buffer passed to the output, input rewrites the delay buffer
  * Play (center third) - 50/50 mixed input and delay buffer passed to the output, delay buffer untouched
  * Overdub (right third) - 50/50 mixed input and delay buffer passed to the output, input added to the delay buffer
* Record format (shift depth / FX button + B knob)
  * Stereo (d100-d50) - full quality
  * Stereo packed (d49-d0) - reduced quality
  * Mono (w0-w49) - 50/50 channel mix, full quality
  * Mono packed (w50-w100) - 50/50 channel mix, reduced quality

Due to  SDRAM size limitation loop length automatically limited depending on the record format and current BPM according to the table:

|Record format|BPM < 75|75 ≤ BPM < 150|
|-|-|-|
|Stereo|4 beats max|8 beats max|
|Stereo packed|8 beats max|-
|Mono|8 beats max|-
|Mono packed|-|-

# Vibrator

This is a vibrato effect with pitch shift for Modulation FX, Delay FX and Reverb FX.

## Parameters
* Vibrato rate (time / A knob) - 0...30 Hz
* Vibrato depth (depth / B knob) - 0...1 semitone
* Pitch shift (shift depth / FX button + B knob) - -12...+12 semitones
