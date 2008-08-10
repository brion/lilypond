%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.55"

\header {
  lsrtags = "expressive-marks, tweaks-and-overrides"

  texidoc = "
Some dynamic expressions involve additional text, like \"sempre pp\".
Since lilypond aligns all dynamics centered on the note, the \\pp would
be displayed way after the note it applies to.

To correctly align the \"sempre \\pp\" horizontally, so that it is
aligned as if it were only the \\pp, there are several approaches:

* Simply use @code{\\once\\override DynamicText #'X-offset = #-9.2}
before the note with the dynamics to manually shift it to the correct
position. Drawback: This has to be done manually each time you use that
dynamic markup... * Add some padding (@code{#:hspace 7.1}) into the
definition of your custom dynamic mark, so that after lilypond
center-aligns it, it is already correctly aligned. Drawback: The
padding really takes up that space and does not allow any other markup
or dynamics to be shown in that position.

* Shift the dynamic script @code{\\once\\override ... #'X-offset = ..}.
Drawback: @code{\\once\\override} is needed for every invocation!

* Set the dimensions of the additional text to 0 (using
@code{#:with-dimensions '(0 . 0) '(0 . 0)}). Drawback: To lilypond
\"sempre\" has no extent, so it might put other stuff there and create
collisions (which are not detected by the collision dection!). Also,
there seems to be some spacing, so it's not exactly the same alignment
as without the additional text

* Add an explicit shifting directly inside the scheme function for the
dynamic-script.

* Set an explicit alignment inside the dynamic-script. By default, this
won't have any effect, only if one sets X-offset! Drawback: One needs
to set @code{DynamicText #'X-offset}, which will apply to all dynamic
texts! Also, it is aligned at the right edge of the additional text,
not at the center of pp.




"
  doctitle = "Horizontally aligning custom dynamics (e.g. \"sempre pp\", \"piu f\", \"subito p\")"
} % begin verbatim
\header { title = "Horizontally aligning custom dynamics" }
\layout { ragged-right = ##t }

% Solution 1: Using a simple markup with a particular halign value
% Drawback: It's a markup, not a dynamic command, so \dynamicDown etc. will have no effect
semppMarkup = \markup { \halign #1.4 \italic "sempre" \dynamic "pp" }

% Solution 2: Using a dynamic script and shifting with \once\override ... #'X-offset = ..
% Drawback: \once\override needed for every invocation
semppK = #(make-dynamic-script (markup #:line( #:normal-text #:italic "sempre" #:dynamic "pp")))

% Solution 3: Padding the dynamic script so the center-alignment puts it to the correct position
% Drawback: the padding really reserves the space, nothing else can be there
semppT = #(
  make-dynamic-script (
    markup #:line (
      #:normal-text #:italic "sempre" #:dynamic "pp" #:hspace 7.1
    )
  )
)

% Solution 4: Dynamic, setting the dimensions of the additional text to 0
% Drawback: To lilypond "sempre" has no extent, so it might put other stuff there => collisions
% Drawback: Also, there seems to be some spacing, so it's not exactly the 
%           same alignment as without the additional text
semppM = #(make-dynamic-script (markup #:line( #:with-dimensions '(0 . 0) '(0 . 0) #:right-align #:normal-text #:italic "sempre" #:dynamic "pp")))

% Solution 5: Dynamic with explicit shifting inside the scheme function
semppG = #(make-dynamic-script
  (markup
    #:hspace 0 #:translate (cons -18.85 0 )
    #:line( #:normal-text #:italic "sempre" #:dynamic "pp"))
)

% Solution 6: Dynamic with explicit alignment. This has only effect, if one sets X-offset!
% Drawback: One needs to set DynamicText #'X-offset!
% Drawback: Aligned at the right edge of the additional text, not at the center of pp
semppMII = #(make-dynamic-script (markup #:line(#:right-align #:normal-text #:italic "sempre" #:dynamic "pp")))


\context StaffGroup <<
  \context Staff="s" << \set Staff.instrumentName = "Normal"
       \relative c'' { \key es \major c4\pp c\p c c | c\ff c c\pp c } 
  >>
  \context Staff="sMarkup" << \set Staff.instrumentName = \markup\column{"Normal" "Markup"}
       \relative c'' { \key es \major c4-\semppMarkup c\p c c | c\ff c c-\semppMarkup c} 
  >>
  \context Staff="sK" << \set Staff.instrumentName = \markup\column{"Explicit" "shifting"}
       \relative c'' { \key es \major 
           \once \override DynamicText #'X-offset = #-9.2 c4\semppK c\p c c | 
           c\ff c \once \override DynamicText #'X-offset = #-9.2  c\semppK c } 
  >>
  \context Staff="sT" << \set Staff.instrumentName = \markup\column{"Right" "padding"}
       \relative c'' { \key es \major c4\semppT c\p c c | c\ff c c\semppT c } 
  >>
  \context Staff="sM" << \set Staff.instrumentName = \markup\column{"Setting" "dimension" "to zero"}
       \relative c'' { \key es \major c4\semppM c\p c c | c\ff c c\semppM c } 
  >>
  \context Staff="sG" << \set Staff.instrumentName = \markup\column{"Shifting" "inside" "dynamics"}
       \relative c'' { \key es \major c4\semppG c\p c c | c\ff c c\semppG c} 
  >>
  \context Staff="sMII" << \set Staff.instrumentName = \markup\column{"Alignment" "inside" "dynamics"}
    \relative c'' { \key es \major 
      \override DynamicText #'X-offset = #0  % Setting to ##f (false) gives the same resul
      c4\semppMII c\p c c | c\ff c c\semppMII c } 
    >>
>>

