%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.46"

\header {
  lsrtags = "expressive-marks"

  texidoc = "
A contemporary glissando without a final note can be typeset using a
hidden note and cadenza timing.

"
  doctitle = "Contemporary glissando"
} % begin verbatim
\relative c'' {
  \time 3/4
  \override Glissando #'style = #'zigzag
  c4 c
  \cadenzaOn
  c4\glissando
  \hideNotes
  c,,4 
  \unHideNotes
  \cadenzaOff
  \bar "|"
}
