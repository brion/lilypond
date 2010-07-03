%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.20"

\header {
  lsrtags = "rhythms"

%% Translation of GIT committish: 0b55335aeca1de539bf1125b717e0c21bb6fa31b
  texidoces = "
Son posibles tanto los corchetes rectos sobre notas sueltas como
extremos de barra sueltos en figuras unidas, con una combinación de
@code{stemLeftBeamCount}, @code{stemRightBeamCount} e indicadores de
barra @code{[]} emparejados.

Para corchetes rectos que apunten a la derecha sobre notas sueltas,
use indicadores de barra emparejados @code{[]} y establezca
@code{stemLeftBeamCount} a cero (véase el ejemplo 1).

Para corchetes rectos que apunten a la izquierda, establezca en su
lugar @code{stemRightBeamCount} (ejemplo 2).

Para extremos sueltos que apunten a la derecha al final de un conjunto
de notas unidas, establezca @code{stemRightBeamCount} a un valor
positivo.  Y para extremos sueltos que apunten a la izquierda al
principio de un conjunto de notas unidas, establezca
@code{stemLeftBeamCount} en su lugar (ejemplo 3).

A veces, para una nota suelta rodeada de silencios tiene sentido que
lleve los dos extremos sueltos del corchete plano, apuntando a derecha
e izquierda.  Hágalo solamente con indicadores de barra emparejados
@code{[ ]} (ejemplo 4).

(Observe que @code{\\set stemLeftBeamCount} siempre equivale a
@code{\\once \\set}.  En otras palabras, los ajustes de la cantidad de
barras no se recuerdan, y por ello el par de corchetes planos
aplicados a la nota Do semicorchea @code{c'16[]} del último ejemplo no
tiene nada que ver con el @code{\\set} de dos notas por detrás.)

"
  doctitlees = "Corchetes rectos y extremos de barra sueltos"


%% Translation of GIT committish: 0a868be38a775ecb1ef935b079000cebbc64de40
  texidocde = "
Gerade Fähnchen an einzelnen Noten und überstehende Balkenenden bei
bebalkten Notengruppen sind möglich mit einer Kombination aus
@code{stemLeftBeamCount}, @code{stemRightBeamCount} und Paaren von
@code{[]}-Balkenbegrenzungen.

Für gerade Fähnchen, die nach rechts zeigen, kann @code{[]} eingesetzt
werden und @code{stemLeftBeamCount} auf Null gesetzt werden (wie
Bsp. 1).

Für gerade Fähnchen, die nach links zeigen, muss @code{stemRightBeamCount}
eingesetzt werden (Bsp. 2).

Für überstehende Balkenenden nach rechts muss @code{stemRightBeamCount}
auf einen positiven Wert gesetzt werden, für Balkenenden, die nach links
zeigen benutzt man @code{stemLeftBeamCount} (Bsp. 3).

Manchmal können einzelne Noten, die von Pausen umgeben sind, auch Balkenenden
in beide Richtungen tragen.  Das geschieht mit @code{[]}-Klammern (Bsp. 4).

(@code{\\set stemLeftBeamCount} entspricht immer dem Befehl
@code{\\once \\set}.  Anders gesagt müssen die Einstellungen immer wieder
wiederholt werden und die Fähnchen des letzten Sechszehntels im letzten
Beispiel haben nichts mit dem @code{\\set}-Befehl zwei Noten vorher zu tun.)

"
  doctitlede = "Gerade Fähnchen und überstehende Balkenenden"


%% Translation of GIT committish: 374d57cf9b68ddf32a95409ce08ba75816900f6b
  texidocfr = "
En combinant @code{stemLeftBeamCount}, @code{stemRightBeamCount} et des
paires de @code{[]}, vous pourrez obtenir des crochets rectilignes et
des ligatures qui débordent à leurs extrémités.

Pour des crochets rectilignes à droite sur des notes isolées, il suffit
d'ajouter une paire d'indicateurs de ligature @code{[]} et de déterminer
@code{stemLeftBeamCount} à zéro, comme dans l'exemple 1.

Pour des crochets rectiligne à gauche, c'est @code{stemRightBeamCount}
qu'il faudra déterminer (exemple 2).

Pour que les barres de ligature débordent sur la droite,
@code{stemRightBeamCount} doit avoir une valeur positive ; pour un
débrodement à gauche, c'est sur @code{stemLeftBeamCount} qu'il faut
jouer.  Tout ceci est illustré par l'exemple 3.

Il est parfois judicieux, lorsqu'une note est encadrée de silences, de
l'affubler de crochets rectilignes de part et d'autre. L'exemple 4
montre qu'il suffit d'adjoindre à cette note un @code{[]}.

(Notez bien que @code{\\set stemLeftBeamCount} sera toujours synonyme de
@code{\\once \\set}.  Autrement dit, la détermination des ligatures
n'est pas @qq{permanente} ; c'est la raison pour laquelle les crochets
du @code{c'16[]} isolé du dernier exemple n'ont rien à voir avec le
@code{\\set} indiqué deux notes auparavant.)

"
  doctitlefr = "Crochet rectiligne et débordement de ligature"

  texidoc = "
 Flat flags on lone notes and beam nibs at the ends of beamed figures
are both possible with a combination of @code{stemLeftBeamCount},
@code{stemRightBeamCount} and paired @code{[]} beam indicators.




For right-pointing flat flags on lone notes, use paired @code{[]} beam
indicators and set @code{stemLeftBeamCount} to zero (see Example 1).




For left-pointing flat flags, set @code{stemRightBeamCount} instead
(Example 2).




For right-pointing nibs at the end of a run of beamed notes, set
@code{stemRightBeamCount} to a positive value. And for left-pointing
nibs at the start of a run of beamed notes, set
@code{stemLeftBeamCount} instead (Example 3).




Sometimes it may make sense for a lone note surrounded by rests to
carry both a left- and right-pointing flat flag. Do this with paired
@code{[]} beam indicators alone (Example 4).




(Note that @code{\\set stemLeftBeamCount} is always equivalent to
@code{\\once \\set}.  In other words, the beam count settings are not
@qq{sticky}, so the pair of flat flags attached to the lone
@code{c'16[]} in the last example have nothing to do with the
@code{\\set} two notes prior.)




"
  doctitle = "Flat flags and beam nibs"
} % begin verbatim

\score {
  <<
    % Example 1
    \new RhythmicStaff {
      \set stemLeftBeamCount = #0
      c16[]
      r8.
    }
    % Example 2
    \new RhythmicStaff {
      r8.
      \set stemRightBeamCount = #0
      c16[]
    }
    % Example 3
    \new RhythmicStaff {
      c16 c
      \set stemRightBeamCount = #2
      c16 r r
      \set stemLeftBeamCount = #2
      c16 c c
    }
    % Example 4
    \new RhythmicStaff {
      c16 c
      \set stemRightBeamCount = #2
      c16 r
      c16[]
      r16
      \set stemLeftBeamCount = #2
      c16 c
    }
  >>
}

