# logue-fx
Custom effects for Korg logue-sdk compatible synths

You can get the lastest precompiled versions of the effects in [releases](../../releases/) section.

### Project structure

* [lodue-sdk/](logue-sdk/) : My own logue-sdk fork with optimized makefiles and reduced project footprint.
* [inc/fxwrapper.h](inc/fxwrapper.h) : FX wrapper that allows to make all effect types from one source file.
* [src/](src/) : Effects source files.
* ...fx/ : Effects project files.


### Effects description
* Crusher - Bit crusher and decimator effect.
* Gator - Kaossilator-style gate arpeggiator.
* Hyper - unison effect inspired by supersaw/hypersaw.
* Kompressor - RMS downward compressor/expander.
* Looper - Kaossilator-style loop recorder.
* Vibrator - frequency vibrato and pitch shift effect.

### Effects Parameters
|Effect |time / A knob|depth / B knob|shift depth / FX button + B knob|
|-|-|-|-|
|Crusher|Sampling frequency - 48000...480Hz|Bit depth - 24...1 bit|Dry/wet mix|
|Gator|Pattern select - [KAOSSILATOR Parameter List](https://www.korg.com/us/support/download/manual/1/121/1774/)|Pattern restart - pass center point|-|
|Hyper|Detune - 0...1 semitone|Unision voices - 1 to 12 pairs, each pair detuned by additional +/- detune value|Dry/wet mix|
|Kompressor|Threshold level - -80...0dB|Ratio<br>- expand ∞:1...1:1 (left to center)<br>- compress 1:1...∞:1 (center to right)|Dry/wet mix|
|Looper|Loop length - linked to the current BPM and sticked to<br>16, 8, 4, 2, 1, ½, ¼, ⅛, 1⁄16 beats|Play/Record mode<br>- Overwrite (left third)<br>- Play (center third)<br>- Overdub (right third)|Record format <br>- Stereo (d100-d50)<br>- Stereo packed (d49-d0)<br>- Mono (w0-w49)<br>- Mono packed (w50-w100)|
|Vibrator|Vibrato rate - 0...30Hz|Vibrato depth - 0...1 semitone|Pitch shift - -12...+12 semitones|

### Notes
* Most of the effects for all three effect slots.
* The third effect parameter control is not available for Modulation FX thus several effects are limited in control in this slot.
* Due to NTS-1 SDRAM size limitation loop length automatically reduced depending on the record format and current BPM according to the table below:

|Record format|BPM < 75|75 ≤ BPM < 150|
|-|-|-|
|Stereo|4 beats max|8 beats max|
|Stereo packed|8 beats max|-
|Mono|8 beats max|-
|Mono packed|-|-
