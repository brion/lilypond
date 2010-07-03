% Do not edit this file; it is automatically
% generated from Documentation/snippets/new
% This file is in the public domain.
%% Note: this file works from version 2.13.17
\version "2.13.20"

\header {
%% Translation of GIT committish: 0b55335aeca1de539bf1125b717e0c21bb6fa31b
  texidoces = "
Para añadir digitaciones a las tablaturas, utilice una combinación de
@code{\\markup} y @code{\\finger}.
"

  doctitlees = "Añadir digitaciones a las tablaturas"


%% Translation of GIT committish: 96ee692447057131395fe4c1f9fe55805b710aa6
  texidocfr = "Ajout de doigtés à des tablatures"

  doctitlefr = "
L'ajout de doigtés à des tablatures s'obtient en conjuguant des
@code{\\markup} et des @code{\\finger}.
"

  lsrtags = "fretted-strings"
  texidoc = "
To add fingerings to tablatures, use a combination of @code{\\markup}
and @code{\\finger}.
"
  doctitle = "Adding fingerings to tablatures"
} % begin verbatim


one = \markup { \finger 1 }
two = \markup { \finger 2 }
threeTwo = \markup {
  \override #'(baseline-skip . 2)
  \column {
    \finger 3
    \finger 2
  }
}
threeFour = \markup {
  \override #'(baseline-skip . 2)
  \column {
    \finger 3
    \finger 4
  }
}

\score {
  \new TabStaff {
    \tabFullNotation
    \stemUp
    e8\4^\one b\2 <g\3 e'\1>^>[ b\2 e\4]
    <a\3 fis'\1>^>^\threeTwo[ b\2 e\4]
  }
}

