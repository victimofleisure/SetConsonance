# SetConsonance

This Windows console app performs various functions related to composing atonal chord progressions with balanced Gray interval sets. It's related to the [BalaGrayIter](https://github.com/victimofleisure/BalaGrayIter) project and my paper [Granular 12-Tone Harmony via Mixed-Radix Balanced Gray Codes: Interval Sets and the BalaGray Solver](https://doi.org/10.5281/zenodo.16041343).

The main functions are:
* Harmonizes the chords of a balanced Gray solution, by reducing each chord to a prime form pitch class set, and then matching that pitch class set to a predefined harmonization consisting of a scale, key, and mode. The harmonizations can be limited to heptatonic scales only if desired.
* Outputs a series of scales that harmonize with the chords of a balanced Gray solution. The scales can then be used to generate melodies. The scales are output in the file format of the [Polymeter MIDI Sequencer](https://victimofleisure.github.io/Polymeter/).
* Finds the optimal spacing of an incomplete interval set, by generating all its possible spacings, harmonizing each spacing, and then assigning a score based on how many of the resulting pitch class sets are consonant (tonic or subdominant).
* Creates a tone map that shows which tones are available given a specified balanced Gray solution for an interval set. Notes that would cause chromatic pitch clusters or doubling are eliminated, and scale tones are numbered. The tone map is a plain text file. This function is useful for choosing suitable bass or melody notes to accompany a balanced Gray chord progression.
* Creates an HTML table similar to the tone map described above, with chord, bass, and melody tones indicated in various colors.

The app also includes verification features. It tests its table of harmonizations to ensure that they produce the specified pitch class sets. Each pitch class set can have one or two harmonizations, depending on whether its plain (A) and inverted (B) forms differ. Pitch class sets that include chromatic clusters are excluded, as the balanced Gray method intrinsically avoids them. The app can also verify its pitch class set definitions against the Wikipedia definitions, which are included in a 7-Zip file. The app uses Rahn's packing, as does the Wikipedia pitch class set list.

The app compiles cleanly in Visual Studio 2012 and probably later versions too. It's a bit messy and contains hard-coded paths.

# Links

* [Chris Korda software](https://victimofleisure.github.io/software)
