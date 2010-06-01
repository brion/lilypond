\version "2.13.25"

#(set-default-paper-size "a6")

\header {
  texidoc = "system-count and \pageBreak are compatible."
}

\book {
  \relative c'' {
    \repeat "unfold" 4 { c4 c c c | }
    \pageBreak
    \repeat "unfold" 4 { c4 c c c | }
  }
  \paper {
    system-count = #4
  }
}

