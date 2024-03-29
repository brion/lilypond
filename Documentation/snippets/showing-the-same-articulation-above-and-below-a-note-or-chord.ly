%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.17.15"

\header {
  lsrtags = "expressive-marks, tweaks-and-overrides, version-specific"

  texidoc = "
By default, LilyPond does not allow the same articulation (e.g., an
accent, a fermata, a flageolet, etc.) to be displayed above and below a
note. For example, c4_\\fermata^\\fermata will only show a fermata
below. The fermata above will simply be ignored. However, one can stick
scripts (just like fingerings) inside a chord, which means it is
possible to have as many articulations as desired. This approach has
the advantage that it ignores the stem and positions the articulation
relative to the note head. This can be seen in the case of the
flageolets in the snippet. To mimic the behaviour of scripts outside a
chord, 'add-stem-support would be required. So, the solution is to
write the note as a chord and add the articulations inside the <...>.
The direction will always be above, but one can tweak this via a
\\tweak: @code{<c-\\tweak direction #DOWN-\\fermata^\\fermata>}

"
  doctitle = "Showing the same articulation above and below a note or chord"
} % begin verbatim


% The same as \flageolet, just a little smaller
smallFlageolet =
#(let ((m (make-articulation "flageolet")))
   (set! (ly:music-property m 'tweaks)
         (acons 'font-size -2
                (ly:music-property m 'tweaks)))
   m)

\relative c' {
  s4^"Wrong:"
  c4_\fermata^\fermata % The second fermata is ignored!
  <e d'>4^\smallFlageolet_\smallFlageolet

  s4^"Works if written inside a chord:"
  <e_\smallFlageolet d'^\smallFlageolet>4
  <e_\flageolet d'^\flageolet>4
  <e_\smallFlageolet^\smallFlageolet>4
  <e_\fermata^\fermata>4
}
