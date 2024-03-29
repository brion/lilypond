/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 1997--2012 Han-Wen Nienhuys <hanwen@xs4all.nl>

  LilyPond is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LilyPond is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LilyPond.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "note-column.hh"

#include <cmath>                // ceil
using namespace std;

#include "accidental-placement.hh"
#include "axis-group-interface.hh"
#include "directional-element-interface.hh"
#include "international.hh"
#include "item.hh"
#include "note-head.hh"
#include "output-def.hh"
#include "pointer-group-interface.hh"
#include "rest.hh"
#include "staff-symbol-referencer.hh"
#include "stem.hh"
#include "warn.hh"

/*
  TODO: figure out if we can prune this class. This is just an
  annoying layer between (rest)collision & (note-head + stem)
*/

Interval
Note_column::accidental_width (Grob *me)
{
  extract_grob_set (me, "note-heads", nhs);
  vector<Grob *> accs;
  for (vsize i = 0; i < nhs.size (); i++)
    if (Grob *acc = unsmob_grob (nhs[i]->get_object ("accidental-grob")))
      accs.push_back (acc);

  Grob *common = common_refpoint_of_array (accs, me, X_AXIS);
  common = common_refpoint_of_array (nhs, common, X_AXIS);

  Interval nhs_ex = Axis_group_interface::relative_group_extent (nhs, common, X_AXIS);
  Interval accs_ex = Axis_group_interface::relative_group_extent (accs, common, X_AXIS);

  if (nhs_ex.is_empty ())
    return accs_ex;

  // want an empty interval here
  if (accs_ex.is_empty ())
    return Interval ();

  return Interval (accs_ex[LEFT], nhs_ex[LEFT]);
}

bool
Note_column::has_rests (Grob *me)
{
  return unsmob_grob (me->get_object ("rest"));
}

bool
Note_column::shift_less (Grob *const &p1, Grob *const &p2)
{
  SCM s1 = p1->get_property ("horizontal-shift");
  SCM s2 = p2->get_property ("horizontal-shift");

  int h1 = (scm_is_number (s1)) ? scm_to_int (s1) : 0;
  int h2 = (scm_is_number (s2)) ? scm_to_int (s2) : 0;
  return h1 < h2;
}

Item *
Note_column::get_stem (Grob *me)
{
  SCM s = me->get_object ("stem");
  return unsmob_item (s);
}

Item *
Note_column::get_flag (Grob *me)
{
  Item *stem = get_stem (me);
  if (stem)
    {
      SCM s = stem->get_object ("flag");
      return unsmob_item (s);
    }
  return 0;
}

Slice
Note_column::head_positions_interval (Grob *me)
{
  Slice iv;

  iv.set_empty ();

  extract_grob_set (me, "note-heads", heads);
  for (vsize i = 0; i < heads.size (); i++)
    {
      Grob *se = heads[i];

      int j = Staff_symbol_referencer::get_rounded_position (se);
      iv.unite (Slice (j, j));
    }
  return iv;
}

Direction
Note_column::dir (Grob *me)
{
  Grob *stem = unsmob_grob (me->get_object ("stem"));
  if (stem && Stem::has_interface (stem))
    return get_grob_direction (stem);
  else
    {
      extract_grob_set (me, "note-heads", heads);
      if (heads.size ())
        return (Direction)sign (head_positions_interval (me).center ());
    }

  programming_error ("note column without heads and stem");
  return CENTER;
}

void
Note_column::set_stem (Grob *me, Grob *stem)
{
  me->set_object ("stem", stem->self_scm ());
  Axis_group_interface::add_element (me, stem);
}

Grob *
Note_column::get_rest (Grob *me)
{
  return unsmob_grob (me->get_object ("rest"));
}

void
Note_column::add_head (Grob *me, Grob *h)
{
  bool both = false;
  if (Rest::has_interface (h))
    {
      extract_grob_set (me, "note-heads", heads);
      if (heads.size ())
        both = true;
      else
        me->set_object ("rest", h->self_scm ());
    }
  else if (Note_head::has_interface (h))
    {
      if (unsmob_grob (me->get_object ("rest")))
        both = true;
      Pointer_group_interface::add_grob (me, ly_symbol2scm ("note-heads"), h);
    }

  if (both)
    me->warning (_ ("cannot have note heads and rests together on a stem"));
  else
    Axis_group_interface::add_element (me, h);
}

Grob *
Note_column::first_head (Grob *me)
{
  Grob *st = get_stem (me);
  return st ? Stem::first_head (st) : 0;
}

/*
  Return the first AccidentalPlacement grob that we find in a note-head.
*/
Grob *
Note_column::accidentals (Grob *me)
{
  extract_grob_set (me, "note-heads", heads);
  Grob *acc = 0;
  for (vsize i = 0; i < heads.size (); i++)
    {
      Grob *h = heads[i];
      acc = h ? unsmob_grob (h->get_object ("accidental-grob")) : 0;
      if (acc)
        break;
    }

  if (!acc)
    return 0;

  if (Accidental_placement::has_interface (acc->get_parent (X_AXIS)))
    return acc->get_parent (X_AXIS);

  /* compatibility. */
  return acc;
}

Grob *
Note_column::dot_column (Grob *me)
{
  extract_grob_set (me, "note-heads", heads);
  for (vsize i = 0; i < heads.size (); i++)
    {
      Grob *dots = unsmob_grob (heads[i]->get_object ("dot"));
      if (dots)
        return dots->get_parent (X_AXIS);
    }

  return 0;
}

/* If a note-column contains a cross-staff stem then
   nc->extent (Y_AXIS, refp) will not consider the extent of the stem.
   If you want the extent of the stem to be included (and you are safe
   from any cross-staff issues) then call this function instead. */
Interval
Note_column::cross_staff_extent (Grob *me, Grob *refp)
{
  Interval iv = me->extent (refp, Y_AXIS);
  if (Grob *s = get_stem (me))
    iv.unite (s->extent (refp, Y_AXIS));

  return iv;
}

ADD_INTERFACE (Note_column,
               "Stem and noteheads combined.",

               /* properties */
               "force-hshift "
               "horizontal-shift "
               "ignore-collision "
               "note-heads "
               "rest "
               "rest-collision "
               "stem "
              );
