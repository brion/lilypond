%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.17.15"

\header {
  lsrtags = "vocal-music"

  texidoc = "
If LilyPond does not think there is space for a hyphen, it will be
omitted.  The behaviour can be overridden with the
@code{minimum-distance} property of @code{LyricHyphen}.

"
  doctitle = "Forcing hyphens to be shown"
} % begin verbatim


\relative c'' {
  c32 c c c
  c32 c c c
  c32 c c c
  c32 c c c
}
\addlyrics {
  syl -- lab word word
  \override LyricHyphen.minimum-distance = #1.0
  syl -- lab word word
  \override LyricHyphen.minimum-distance = #2.0
  syl -- lab word word
  \revert LyricHyphen.minimum-distance
  syl -- lab word word
}
