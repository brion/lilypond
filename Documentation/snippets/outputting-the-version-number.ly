%% DO NOT EDIT this file manually; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% Make any changes in LSR itself, or in Documentation/snippets/new/ ,
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
\version "2.17.15"

\header {
  lsrtags = "really-simple, scheme-language, text, titles"

  texidoc = "
By putting the output of
    @code{lilypond-version} into a lyric, it is possible to print the
    version number of LilyPond in a score, or in a document generated
    with @code{lilypond-book}.  Another possibility is to append the
    version number to the doc-string, in this manner:

"
  doctitle = "Outputting the version number"
} % begin verbatim


\score {
  \new Lyrics {
    \override Score.RehearsalMark.self-alignment-X = #LEFT
    \mark #(string-append "Processed with LilyPond version " (lilypond-version))
    s2
  }
}
