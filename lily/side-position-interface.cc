/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 1998--2012 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#include "side-position-interface.hh"

#include <cmath>                // ceil.
#include <algorithm>
#include <set>
#include <map>

using namespace std;

#include "accidental-interface.hh"
#include "accidental-placement.hh"
#include "axis-group-interface.hh"
#include "directional-element-interface.hh"
#include "grob.hh"
#include "grob-array.hh"
#include "international.hh"
#include "item.hh"
#include "main.hh"
#include "misc.hh"
#include "note-head.hh"
#include "note-column.hh"
#include "pointer-group-interface.hh"
#include "skyline-pair.hh"
#include "staff-symbol-referencer.hh"
#include "staff-symbol.hh"
#include "stem.hh"
#include "string-convert.hh"
#include "system.hh"
#include "warn.hh"
#include "unpure-pure-container.hh"

void
Side_position_interface::add_support (Grob *me, Grob *e)
{
  Pointer_group_interface::add_unordered_grob (me, ly_symbol2scm ("side-support-elements"), e);
}

void
Side_position_interface::recursive_add_support (Grob *me, Grob *e)
{
  Pointer_group_interface::add_unordered_grob (me, ly_symbol2scm ("side-support-elements"), e);
  extract_grob_set (e, "side-support-elements", sse);
  for (vsize i = 0; i < sse.size (); i++)
    recursive_add_support (me, sse[i]);
}

set<Grob *>
get_support_set (Grob *me)
{
  // Only slightly kludgy heuristic...
  // We want to make sure that all AccidentalPlacements'
  // accidentals make it into the side support
  extract_grob_set (me, "side-support-elements", proto_support);
  set<Grob *> support;

  for (vsize i = 0; i < proto_support.size (); i++)
    {
      if (Accidental_placement::has_interface (proto_support[i]))
        {
          Grob *accs = proto_support[i];
          for (SCM acs = accs->get_object ("accidental-grobs"); scm_is_pair (acs);
               acs = scm_cdr (acs))
            for (SCM s = scm_cdar (acs); scm_is_pair (s); s = scm_cdr (s))
              {
                Grob *a = unsmob_grob (scm_car (s));
                support.insert (a);
              }
        }
      else
        support.insert (proto_support[i]);
    }
  return support;
}

/*
  Position next to support, taking into account my own dimensions and padding.
*/
SCM
axis_aligned_side_helper (SCM smob, Axis a, bool pure, int start, int end, SCM current_off_scm)
{
  Real r;
  Real *current_off_ptr = 0;
  if (scm_is_number (current_off_scm))
    {
      r = scm_to_double (current_off_scm);
      current_off_ptr = &r;
    }

  Grob *me = unsmob_grob (smob);
  // We will only ever want widths of spanners after line breaking
  // so we can set pure to false
  if (dynamic_cast<Spanner *> (me) && a == X_AXIS)
    pure = false;

  return Side_position_interface::aligned_side (me, a, pure, start, end, current_off_ptr);
}

MAKE_SCHEME_CALLBACK_WITH_OPTARGS (Side_position_interface, x_aligned_side, 2, 1, "");
SCM
Side_position_interface::x_aligned_side (SCM smob, SCM current_off)
{
  // Because horizontal skylines need vertical heights, we'd trigger
  // unpure calculations too soon if this were called before line breaking.
  // So, we always use pure heights.  Given that horizontal skylines are
  // almost always used before line breaking anyway, this doesn't cause
  // problems.
  return axis_aligned_side_helper (smob, X_AXIS, true, 0, 0, current_off);
}

MAKE_SCHEME_CALLBACK_WITH_OPTARGS (Side_position_interface, y_aligned_side, 2, 1, "");
SCM
Side_position_interface::y_aligned_side (SCM smob, SCM current_off)
{
  return axis_aligned_side_helper (smob, Y_AXIS, false, 0, 0, current_off);
}

MAKE_SCHEME_CALLBACK_WITH_OPTARGS (Side_position_interface, pure_y_aligned_side, 4, 1, "");
SCM
Side_position_interface::pure_y_aligned_side (SCM smob, SCM start, SCM end, SCM cur_off)
{
  return axis_aligned_side_helper (smob, Y_AXIS, true,
                                   scm_to_int (start),
                                   scm_to_int (end),
                                   cur_off);
}

MAKE_SCHEME_CALLBACK (Side_position_interface, calc_cross_staff, 1)
SCM
Side_position_interface::calc_cross_staff (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  extract_grob_set (me, "side-support-elements", elts);
// Commented out because of cross staff issues
// Direction for cross staff stems depends on the spacing of staves,
// which depends on the inclusion of cross-staff side position grobs,
// which need the direction for positioning.  So the get_grob_direction call
// may lead to circular dependencies.
// #if 0
  Direction my_dir = get_grob_direction (me) ;

  // if a cross-staff grob is pointing in a different direction than
  // that of an aligning element, we assume that the alignment
  // of said element will not be influenced the cross-staffitude
  // of the grob and thus we do not mark the aligning element
  // as cross-staff
  for (vsize i = 0; i < elts.size (); i++)
    if (to_boolean (elts[i]->get_property ("cross-staff"))
         && my_dir == get_grob_direction (elts[i]))
      return SCM_BOOL_T;
//#endif
#if 0
  for (vsize i = 0; i < elts.size (); i++)
    if (to_boolean (elts[i]->get_property ("cross-staff")))
      return SCM_BOOL_T;
#endif
  Grob *myvag = Grob::get_vertical_axis_group (me);
  for (vsize i = 0; i < elts.size (); i++)
    if (myvag != Grob::get_vertical_axis_group (elts[i]))
      return SCM_BOOL_T;

  return SCM_BOOL_F;
}

// long function - each stage is clearly marked

SCM
Side_position_interface::aligned_side (Grob *me, Axis a, bool pure, int start, int end,
                                       Real *current_off)
{
  Direction dir = get_grob_direction (me);

  set<Grob *> support = get_support_set (me);

  Grob *common[2];
  for (Axis ax = X_AXIS; ax < NO_AXES; incr (ax))
    common[ax] = common_refpoint_of_array (support,
                                           (ax == a
                                           ? me->get_parent (ax)
                                           : me),
                                           ax);

  Grob *staff_symbol = Staff_symbol_referencer::get_staff_symbol (me);
  bool quantize_position = to_boolean (me->get_maybe_pure_property ("quantize-position", pure, start, end));

  bool include_staff
    = staff_symbol
      && a == Y_AXIS
      && scm_is_number (me->get_maybe_pure_property ("staff-padding", pure, start, end))
      && !quantize_position;

  if (include_staff)
    common[Y_AXIS] = staff_symbol->common_refpoint (common[Y_AXIS], Y_AXIS);

  Skyline my_dim;
  Skyline_pair *skyp = Skyline_pair::unsmob (
                         me->get_maybe_pure_property (a == X_AXIS
                                                      ? "horizontal-skylines"
                                                      : "vertical-skylines",
                                                      pure,
                                                      start,
                                                      end));
  if (skyp)
    {
      // for spanner pure heights, we don't know horizontal spacing,
      // so a spanner can never have a meaningful x coordiante
      // we just give it the parents' coordinate because its
      // skyline will likely be of infinite width anyway
      // and we don't want to prematurely trigger H spacing
      Real xc = a == X_AXIS || (pure && dynamic_cast<Spanner *> (me))
                ? me->get_parent (X_AXIS)->relative_coordinate (common[X_AXIS], X_AXIS)
                : me->relative_coordinate (common[X_AXIS], X_AXIS);
      // same here, for X_AXIS spacing, if it's happening, it should only be
      // before line breaking.  because there is no thing as "pure" x spacing,
      // we assume that it is all pure
      Real yc = a == X_AXIS
                ? me->pure_relative_y_coordinate (common[Y_AXIS], start, end)
                : me->get_parent (Y_AXIS)->maybe_pure_coordinate (common[Y_AXIS], Y_AXIS, pure, start, end);
      Skyline_pair copy = Skyline_pair (*skyp);
      copy.shift (a == X_AXIS ? yc : xc);
      copy.raise (a == X_AXIS ? xc : yc);
      my_dim = copy[-dir];
    }
  else
    me->warning ("cannot find skylines - strange alignment will follow");


  vector<Box> boxes;
  vector<Skyline_pair> skyps;
  set<Grob *>::iterator it;
  Real max_raise = -dir * infinity_f;
  bool aligns_to_cross_staff = false;

  for (it = support.begin (); it != support.end (); it++)
    {
      Grob *e = *it;

      // In the case of a stem, we will find a note head as well
      // ignoring the stem solves cyclic dependencies if the stem is
      // attached to a cross-staff beam.
      if (a == Y_AXIS
          && Stem::has_interface (e)
          && dir == - get_grob_direction (e))
        continue;

      if (e)
        {

           bool cross_staff = to_boolean (e->get_property ("cross-staff"));

           Skyline_pair *sp = Skyline_pair::unsmob
             (e->get_maybe_pure_property (a == X_AXIS
                                          ? "horizontal-skylines"
                                          : "vertical-skylines",
                                          pure || cross_staff,
                                          start,
                                          end));

           aligns_to_cross_staff |= cross_staff;
           if (sp)
             {
               Real xc = pure && dynamic_cast<Spanner *> (e)
                         ? e->get_parent (X_AXIS)->relative_coordinate (common[X_AXIS], X_AXIS)
                         : e->relative_coordinate (common[X_AXIS], X_AXIS);
               // same logic as above
               // we assume horizontal spacing is always pure
               Real yc = a == X_AXIS
                         ? e->pure_relative_y_coordinate (common[Y_AXIS], start, end)
                         : e->maybe_pure_coordinate (common[Y_AXIS], Y_AXIS, pure, start, end);
               Skyline_pair copy = Skyline_pair (*sp);
               if (a == Y_AXIS
                   && Stem::has_interface (e)
                   && to_boolean (me->get_maybe_pure_property ("add-stem-support", pure, start, end)))
                 copy[dir].set_minimum_height (copy[dir].max_height ());
               copy.shift (a == X_AXIS ? yc : xc);
               copy.raise (a == X_AXIS ? xc : yc);
               max_raise = minmax (dir, max_raise, a == X_AXIS ? xc : yc);
               skyps.push_back (copy);
             }
           else { /* no warning*/ }
        }
    }

  Skyline dim (boxes, other_axis (a), dir);
  if (skyps.size ())
    {
      Skyline_pair merged (skyps);
      dim.merge (merged[dir]);
    }

  if (include_staff)
    {
      Interval staff_extents;
      common[Y_AXIS] = staff_symbol->common_refpoint (common[Y_AXIS], Y_AXIS);
      staff_extents = staff_symbol->maybe_pure_extent (common[Y_AXIS], Y_AXIS, pure, start, end);
      dim.set_minimum_height (staff_extents[dir]);
    }

  // Sometimes, we want to side position for grobs but they
  // don't position against anything.  Some cases where this is true:
  //   - StanzaNumber if the supporting lyrics are hara-kiri'd
  //     SystemStartBracket
  //     InstrumentName
  // In all these cases, we set the height of the support to 0.
  // This becomes then like the self-alignment-interface with the
  // caveat that there is padding added.
  // TODO: if there is a grob that never has side-support-elements
  // (like InstrumentName), why are we using this function? Isn't it
  // overkill? A function like self-alignment-interface with padding
  // works just fine.
  // One could even imagine the two interfaces merged, as the only
  // difference is that in self-alignment-interface we align on the parent
  // where as here we align on a group of grobs.
  if (dim.is_empty ())
    {
      dim = Skyline (dim.direction ());
      dim.set_minimum_height (0.0);
    }

  // Many cross-staff grobs do not have good height estimations.
  // We give the grob the best chance of not colliding by shifting
  // it to the maximum height in the case of cross-staff alignment.
  // This means, in other words, that the old way things were done
  // (using boxes instead of skylines) is just reactivated for
  // alignment to cross-staff grobs.
  if (aligns_to_cross_staff)
    dim.set_minimum_height (dim.max_height ());

  Real ss = Staff_symbol_referencer::staff_space (me);
  Real dist = dim.distance (my_dim, robust_scm2double (me->get_maybe_pure_property ("horizon-padding", pure, start, end), 0.0));
  Real total_off = !isinf (dist) ? dir * dist : 0.0;

  total_off += dir * ss * robust_scm2double (me->get_maybe_pure_property ("padding", pure, start, end), 0.0);

  Real minimum_space = ss * robust_scm2double (me->get_maybe_pure_property ("minimum-space", pure, start, end), -1);

  if (minimum_space >= 0
      && dir
      && total_off * dir < minimum_space)
    total_off = minimum_space * dir;

  if (current_off)
    total_off = dir * max (dir * total_off,
                           dir * (*current_off));

  /* FIXME: 1000 should relate to paper size.  */
  if (fabs (total_off) > 1000)
    {
      string msg
        = String_convert::form_string ("Improbable offset for grob %s: %f",
                                       me->name ().c_str (), total_off);

      programming_error (msg);
      if (strict_infinity_checking)
        scm_misc_error (__FUNCTION__, "Improbable offset.", SCM_EOL);
    }

  /*
    Maintain a minimum distance to the staff. This is similar to side
    position with padding, but it will put adjoining objects on a row if
    stuff sticks out of the staff a little.
  */
  Grob *staff = Staff_symbol_referencer::get_staff_symbol (me);
  if (staff && a == Y_AXIS)
    {
      if (quantize_position)
        {
          Grob *common = me->common_refpoint (staff, Y_AXIS);
          Real my_off = me->get_parent (Y_AXIS)->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
          Real staff_off = staff->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
          Real ss = Staff_symbol::staff_space (staff);
          Real position = 2 * (my_off + total_off - staff_off) / ss;
          Real rounded = directed_round (position, dir);
          Grob *head = me->get_parent (X_AXIS);

          Interval staff_span = Staff_symbol::line_span (staff);
          staff_span.widen (1);
          if (staff_span.contains (position)
              /* In case of a ledger lines, quantize even if we're outside the staff. */
              || (Note_head::has_interface (head)

                  && abs (Staff_symbol_referencer::get_position (head)) > abs (position)))
            {
              total_off += (rounded - position) * 0.5 * ss;
              if (Staff_symbol_referencer::on_line (me, int (rounded)))
                total_off += dir * 0.5 * ss;
            }
        }
      else if (scm_is_number (me->get_maybe_pure_property ("staff-padding", pure, start, end)) && dir)
        {
          Interval iv = me->maybe_pure_extent (me, a, pure, start, end);

          Real staff_padding
            = Staff_symbol_referencer::staff_space (me)
              * scm_to_double (me->get_maybe_pure_property ("staff-padding", pure, start, end));

          Grob *parent = me->get_parent (Y_AXIS);
          Grob *common = me->common_refpoint (staff, Y_AXIS);
          Real parent_position = parent->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
          Real staff_position = staff->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
          Interval staff_extent = staff->maybe_pure_extent (staff, a, pure, start, end);
          Real diff = (dir * staff_extent[dir] + staff_padding
                       - dir * (total_off + iv[-dir])
                       + dir * (staff_position - parent_position));
          total_off += dir * max (diff, 0.0);
        }
    }
  return scm_from_double (total_off);
}

void
Side_position_interface::set_axis (Grob *me, Axis a)
{
  if (!scm_is_number (me->get_property ("side-axis")))
    {
      me->set_property ("side-axis", scm_from_int (a));
      chain_offset_callback (me,
                             (a == X_AXIS)
                             ? x_aligned_side_proc
                             : ly_make_unpure_pure_container (y_aligned_side_proc, pure_y_aligned_side_proc),
                             a);
    }
}

Axis
Side_position_interface::get_axis (Grob *me)
{
  if (scm_is_number (me->get_property ("side-axis")))
    return Axis (scm_to_int (me->get_property ("side-axis")));

  string msg = String_convert::form_string ("side-axis not set for grob %s.",
                                            me->name ().c_str ());
  me->programming_error (msg);
  return NO_AXES;
}

MAKE_SCHEME_CALLBACK (Side_position_interface, move_to_extremal_staff, 1);
SCM
Side_position_interface::move_to_extremal_staff (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  System *sys = dynamic_cast<System *> (me->get_system ());
  Direction dir = get_grob_direction (me);
  if (dir != DOWN)
    dir = UP;

  Interval iv = me->extent (sys, X_AXIS);
  iv.widen (1.0);
  Grob *top_staff = sys->get_extremal_staff (dir, iv);

  if (!top_staff)
    return SCM_BOOL_F;

  // Only move this grob if it is a direct child of the system.  We
  // are not interested in moving marks from other staves to the top
  // staff; we only want to move marks from the system to the top
  // staff.
  if (sys != me->get_parent (Y_AXIS))
    return SCM_BOOL_F;

  me->set_parent (top_staff, Y_AXIS);
  me->flush_extent_cache (Y_AXIS);
  Axis_group_interface::add_element (top_staff, me);

  // Remove any cross-staff side-support dependencies
  Grob_array *ga = unsmob_grob_array (me->get_object ("side-support-elements"));
  if (ga)
    {
      vector<Grob *> const &elts = ga->array ();
      vector<Grob *> new_elts;
      for (vsize i = 0; i < elts.size (); ++i)
        {
          if (me->common_refpoint (elts[i], Y_AXIS) == top_staff)
            new_elts.push_back (elts[i]);
        }
      ga->set_array (new_elts);
    }
  return SCM_BOOL_T;
}

ADD_INTERFACE (Side_position_interface,
               "Position a victim object (this one) next to other objects"
               " (the support).  The property @code{direction} signifies where"
               " to put the victim object relative to the support (left or"
               " right, up or down?)\n"
               "\n"
               "The routine also takes the size of the staff into account if"
               " @code{staff-padding} is set.  If undefined, the staff symbol"
               " is ignored.",

               /* properties */
               "add-stem-support "
               "direction "
               "minimum-space "
               "horizon-padding "
               "padding "
               "quantize-position "
               "side-axis "
               "side-support-elements "
               "slur-padding "
               "staff-padding "
               "use-skylines "
              );
