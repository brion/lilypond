%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.17.15"

\header {
  lsrtags = "expressive-marks"

  texidoc = "
Dynamics that occur at, begin on, or end on the same note will be
vertically aligned.  To ensure that dynamics are aligned when they do
not occur on the same note, increase the @code{staff-padding} property
of the @code{DynamicLineSpanner} object.

"
  doctitle = "Vertically aligning dynamics across multiple notes"
} % begin verbatim


\relative c' {
  \override DynamicLineSpanner.staff-padding = #4
  c2\p f\mf
  g2\< b4\> c\!
}
