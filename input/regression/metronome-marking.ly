

\header {

  
  texidoc = "Here @code{\\tempo} directives are printed as metronome markings.



The marking is left aligned with the time signature, if there is one.
"
  
}

\layout {  ragged-right = ##t }

\version "2.17.17"

\relative c'' {
  \tempo \breve = 100 c1 c1 \tempo "Allegro" 8.. = 50 c1 \tempo "Adagio" c2 c'
}




