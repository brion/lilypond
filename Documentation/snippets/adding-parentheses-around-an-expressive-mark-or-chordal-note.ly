%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.17.15"

\header {
  lsrtags = "expressive-marks, workaround"

  texidoc = "
The @code{\\parenthesize} function is a special tweak that encloses
objects in parentheses.  The associated grob is @code{ParenthesesItem}.


"
  doctitle = "Adding parentheses around an expressive mark or chordal note"
} % begin verbatim


\relative c' {
  c2-\parenthesize ->
  \override ParenthesesItem.padding = #0.1
  \override ParenthesesItem.font-size = #-4
  <d \parenthesize f a>2
}
