\header { texidoc = "Deeply nested system braces, brackets, etc., may be
  created with the @code{systemStartDelimiterHierarchy} property."
}

\version "2.17.15"

\paper {
  ragged-right = ##t
}

\new StaffGroup
\relative c' <<
  \set StaffGroup.systemStartDelimiterHierarchy
    = #'(SystemStartSquare (SystemStartBracket a (SystemStartSquare b)) d)
  \new Staff { c1 }
  \new Staff { c1 }
  \new Staff { c1 }
  \new Staff { c1 }
  \new Staff { c1 }
  >>
