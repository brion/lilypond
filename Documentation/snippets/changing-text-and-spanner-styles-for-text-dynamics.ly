%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.17.15"

\header {
  lsrtags = "expressive-marks, tweaks-and-overrides"

  texidoc = "
The text used for crescendos and decrescendos can be changed by
modifying the context properties @code{crescendoText} and
@code{decrescendoText}.

The style of the spanner line can be changed by modifying the
@code{'style} property of @code{DynamicTextSpanner}.  The default value
is @code{'dashed-line}, and other possible values include @code{'line},
@code{'dotted-line} and @code{'none}.

"
  doctitle = "Changing text and spanner styles for text dynamics"
} % begin verbatim


\relative c'' {
  \set crescendoText = \markup { \italic { cresc. poco } }
  \set crescendoSpanner = #'text
  \override DynamicTextSpanner.style = #'dotted-line
  a2\< a
  a2 a
  a2 a
  a2 a\mf
}
