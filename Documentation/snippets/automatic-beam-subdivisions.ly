%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.20"

\header {
  lsrtags = "rhythms"

%% Translation of GIT committish: 0b55335aeca1de539bf1125b717e0c21bb6fa31b
  texidoces = "
Se pueden subdividir las barras automáticamente.  Estableciendo la
propiedad @code{subdivideBeams}, las barras se subdividen en
posiciones de pulso (tal y como se especifica en @code{beatLength}).

"
  doctitlees = "Subdivisiones de barra automáticas"

  texidoc = "
Beams can be subdivided automatically.  By setting the property
@code{subdivideBeams}, beams are subdivided at beat positions (as
specified in @code{beatLength}).

"
  doctitle = "Automatic beam subdivisions"
} % begin verbatim

\new Staff {
  \relative c'' {
    <<
      {
        \voiceOne
        \set subdivideBeams = ##t
        b32[ a g f c' b a g
        b32^"subdivide beams" a g f c' b a g]
      }
      \new Voice {
        \voiceTwo
        b32_"default"[ a g f c' b a g
        b32 a g f c' b a g]
      }
    >>
    \oneVoice
    \set beatLength = #(ly:make-moment 1 8)
    b32^"beatLength 1 8"[ a g f c' b a g]
    \set beatLength = #(ly:make-moment 1 16)
    b32^"beatLength 1 16"[ a g f c' b a g]
  }
}

