# Loop recorder

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
