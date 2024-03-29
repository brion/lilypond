%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.17.15"

\header {
  lsrtags = "fretted-strings"

  texidoc = "
The direction of stems is controlled the same way in tablature as in
traditional notation. Beams can be made horizontal, as shown in this
example.

"
  doctitle = "Stem and beam behavior in tablature"
} % begin verbatim


\new TabStaff {
  \relative c {
    \tabFullNotation
    g16 b d g b d g b
    \stemDown
    \override Beam.concaveness = #10000
    g,,16 b d g b d g b
  }
}
