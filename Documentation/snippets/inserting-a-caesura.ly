%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.20"

\header {
  lsrtags = "expressive-marks, tweaks-and-overrides"

%% Translation of GIT committish: 0b55335aeca1de539bf1125b717e0c21bb6fa31b
  texidoces = "
Las marcas de cesura se pueden crear sobreescribiendo la propiedad
@code{'text} del objeto @code{BreathingSign}.  También está disponible
una marca de cesura curva.

"
  doctitlees = "Insertar una cesura"

%% Translation of GIT committish: 0a868be38a775ecb1ef935b079000cebbc64de40
 texidocde = "
Zäsurzeichen können erstellt werden, indem die @code{'text}-Eigenschaft
des @code{BreathingSign}-Objektes verändert wird.  Ein gekrümmtes
Zäsurzeichen ist auch möglich.
"
  doctitlede = "Eine Zäsur einfügen"
%% Translation of GIT committish: 217cd2b9de6e783f2a5c8a42be9c70a82195ad20
  texidocfr = "
Une surcharge de la propriété @code{'text} de l'objet
@code{BreathingSign} permet de créer une marque de césure.  LilyPond
dispose également d'une variante courbée.

"
  doctitlefr = "Insertion d'une césure"


  texidoc = "
Caesura marks can be created by overriding the @code{'text} property of
the @code{BreathingSign} object. A curved caesura mark is also
available.

"
  doctitle = "Inserting a caesura"
} % begin verbatim

\relative c'' {
  \override BreathingSign #'text = \markup {
    \musicglyph #"scripts.caesura.straight"
  }
  c8 e4. \breathe g8. e16 c4

  \override BreathingSign #'text = \markup {
    \musicglyph #"scripts.caesura.curved"
  }
  g8 e'4. \breathe g8. e16 c4
}
