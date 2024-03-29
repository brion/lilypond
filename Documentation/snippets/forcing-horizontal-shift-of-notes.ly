%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.17.15"

\header {
  lsrtags = "simultaneous-notes, tweaks-and-overrides"

  texidoc = "
When the typesetting engine cannot cope, the following syntax can be
used to override typesetting decisions. The units of measure used here
are staff spaces.

"
  doctitle = "Forcing horizontal shift of notes"
} % begin verbatim


\relative c' <<
  {
    <d g>2 <d g>
  }
  \\
  {
    <b f'>2
    \once \override NoteColumn.force-hshift = #1.7
    <b f'>2
  }
>>
