%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.50"

\header {
  lsrtags = "expressive-marks"

  texidoc = "
Some composers write two slurs when they want legato chords.  This can
be achieved by setting @code{doubleSlurs}. 

"
  doctitle = "Using double slurs for legato chords"
} % begin verbatim
\relative c' {
  \set doubleSlurs = ##t
  <c e>4( <d f> <c e> <d f>)
}
