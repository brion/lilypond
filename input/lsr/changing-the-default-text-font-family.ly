%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.46"

\header {
  lsrtags = "text, tweaks-and-overrides"

  texidoc = "
The default font families for text can be overridden with
@code{make-pango-font-tree}. 

"
  doctitle = "Changing the default text font family"
} % begin verbatim
\paper {
  % change for other default global staff size. 
  myStaffSize = #20
  %{
     run
         lilypond -dshow-available-fonts blabla
     to show all fonts available in the process log.  
  %}

  #(define fonts
    (make-pango-font-tree "Times New Roman"
                          "Nimbus Sans"
                          "Luxi Mono"
;;                        "Helvetica"
;;                        "Courier"
     (/ myStaffSize 20)))
}

\relative {
  c'^\markup {
    roman: foo \bold bla \italic bar \italic \bold baz 
  }
  c'_\markup {
    \override #'(font-family . sans)
    {
      sans: foo \bold bla \italic bar \italic \bold baz
    }
  }
  c'2^\markup {
    \override #'(font-family . typewriter)
    {
      mono: foo \bold bla \italic bar \italic \bold baz
    }
  }
}  

