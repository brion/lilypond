%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.50"

\header {
  lsrtags = "rhythms"

  texidoc = "
The Scheme function @code{set-time-signature}, in combination with the
@code{Measure_grouping_engraver}, creates measure grouping signs. Such
signs ease reading rhythmically complex modern music. In the following
example, the 9/8 measure is subdivided into 2, 2, 2 and 3 beats. This
is passed to @code{set-time-signature} as the third argument @code{(2 2
2 3)}.



"
  doctitle = "Conducting signs, measure grouping signs"
} % begin verbatim
\score {
  \relative c'' {
    #(set-time-signature 9 8 '(2 2 2 3))
    #(revert-auto-beam-setting '(end * * 9 8) 3 8)
    #(override-auto-beam-setting '(end 1 8 9 8) 1 4)
    #(override-auto-beam-setting '(end 1 8 9 8) 2 4)
    #(override-auto-beam-setting '(end 1 8 9 8) 3 4)
    g8 g d d g g a( bes g) |
    #(set-time-signature 5 8 '(3 2))
    a4. g4
  }
  \layout {
    \context {
      \Staff
      \consists "Measure_grouping_engraver"
    }
  }
}
