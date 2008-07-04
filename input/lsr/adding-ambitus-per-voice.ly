%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.50"

\header {
  lsrtags = "pitches, staff-notation, vocal-music"

  doctitlees = "Añadir un ámbito por voz"
  texidoces = "
Se puede añadir un ámbito por cada voz. En este caso, el ámbito se
debe desplazar manualmente para evitar colisiones.

"

  texidoc = "
Ambitus can be added per voice. In this case, the ambitus must be moved
manually to prevent collisions. 

"
  doctitle = "Adding ambitus per voice"
} % begin verbatim
\new Staff <<
  \new Voice \with {
    \consists "Ambitus_engraver"
  } \relative c'' {
    \override Ambitus #'X-offset = #2.0
    \voiceOne
    c4 a d e
    f1
  }
  \new Voice \with {
    \consists "Ambitus_engraver"
  } \relative c' {
    \voiceTwo
    es4 f g as
    b1
  }
>>
