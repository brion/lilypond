/*
  tie-formatting-problem.cc -- implement Tie_formatting_problem

  source file of the GNU LilyPond music typesetter

  (c) 2005--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#include "tie-formatting-problem.hh"

#include "bezier.hh" 
#include "directional-element-interface.hh"
#include "item.hh"
#include "libc-extension.hh"
#include "note-head.hh"
#include "rhythmic-head.hh"
#include "spanner.hh" 
#include "staff-symbol-referencer.hh"
#include "stem.hh"
#include "tie-configuration.hh"
#include "tie.hh"
#include "warn.hh"

/*
   0 at threshold,  1 at 0, with 1/x falloff.
 */
Real peak_around (Real epsilon,  Real threshold, Real x)
{
  if (x < 0)
    return 1.0;
  return max (- epsilon * (x - threshold) / ((x + epsilon)  * threshold), 0.0);
}

Interval
Tie_formatting_problem::get_attachment (Real y) const
{
  Interval attachments;
  Direction d = LEFT;
  do
    {
      attachments[d] = skyline_height (chord_outlines_[d], y, -d);
    }
  while (flip (&d) != LEFT);
  
  return attachments;
}

Tie_formatting_problem::Tie_formatting_problem()
{
  x_refpoint_ = 0;
}

Tie_formatting_problem::~Tie_formatting_problem ()
{
  for (Tie_configuration_map::const_iterator i (possibilities_.begin ());
       i != possibilities_.end (); i++)
    delete (*i).second;
}

void
Tie_formatting_problem::set_chord_outline (Link_array<Item> bounds,
					   Direction d)
{
  Real staff_space = Staff_symbol_referencer::staff_space (bounds[0]);

  std::vector<Box> boxes;

  Grob *stem = 0;
  for (vsize i = 0; i < bounds.size (); i++)
    {
      Grob *head = bounds[i];
      if (!Note_head::has_interface (head))
	continue;
      
      if (!stem)
	stem = unsmob_grob (head->get_object ("stem"));
	  
      Real p = Staff_symbol_referencer::get_position (head);
      Interval y ((p-1) * 0.5 * staff_space,
		  (p+1) * 0.5 * staff_space);

      Interval x = head->extent (x_refpoint_, X_AXIS);
      boxes.push_back (Box (x, y));

      Grob *dots = Rhythmic_head::get_dots (head);
      if (d == LEFT && dots)
	{
	  Interval x = dots->extent (x_refpoint_, X_AXIS);
	  Interval y (-0.5, 0.5);
	  int p = int (Staff_symbol_referencer::get_position (dots));
	  y.translate (p);

	  dot_positions_.insert (p);
	  dot_x_.unite (x);
	  
	  y *= staff_space * 0.5;
	  // boxes.push_back (Box (x, y));
	}
    }

  chord_outlines_[d] = empty_skyline (-d);

  if (bounds[0]->break_status_dir ())
    {
      Real x = robust_relative_extent (bounds[0],  x_refpoint_, X_AXIS)[-d];
      chord_outlines_[d].at (0).height_ = x; 
    }
	  
  for (vsize i = 0; i < boxes.size (); i++)
    insert_extent_into_skyline (&chord_outlines_[d]  ,
				boxes[i], Y_AXIS, -d);

  if (stem
      && !Stem::is_invisible (stem))
    {
      Interval x;
      x.add_point (stem->relative_coordinate (x_refpoint_, X_AXIS));
      x.widen (staff_space / 20); // ugh.
      Interval y;
      y.add_point (Stem::stem_end_position (stem) * staff_space * .5);

      Direction stemdir = get_grob_direction (stem);
      y.add_point (Stem::head_positions (stem)[-stemdir]
		   * staff_space * .5);
	  
      insert_extent_into_skyline (&chord_outlines_[d], Box (x,y), Y_AXIS, -d);

      stem_extents_[d].unite (Box (x,y));

      if (d == LEFT)
	{
	  Box flag_box = Stem::get_translated_flag (stem).extent_box ();
	  flag_box.translate( Offset (x[RIGHT], X_AXIS));
	  insert_extent_into_skyline (&chord_outlines_[d], flag_box,
				      Y_AXIS, -d);
	}
    }
  
  Direction updowndir = DOWN;
  do
    {
      Interval x;
      Interval y;
      if (boxes.size())
	{
	  Box b = boxes.boundary (updowndir, 0);
	  x = b[X_AXIS];
	  x[-d] =  b[X_AXIS].linear_combination (-d / 2);
	  y[-updowndir] = b[Y_AXIS][updowndir];
	  y[updowndir] = updowndir * infinity_f;
	}

      if (!x.is_empty ())
	insert_extent_into_skyline (&chord_outlines_[d],
				    Box (x,y),
				    Y_AXIS, -d);
    }
  while (flip (&updowndir) != DOWN);
}


void
Tie_formatting_problem::from_tie (Grob *tie)
{
  Link_array<Grob> ties;
  ties.push_back (tie);
  from_ties (ties);

  details_.from_grob (tie);
}

Grob *
Tie_formatting_problem::common_x_refpoint () const
{
  return x_refpoint_;
}

void
Tie_formatting_problem::from_ties (Link_array<Grob> const &ties)
{
  if (ties.empty ())
    return;
  
  x_refpoint_ = ties[0];
  for (vsize i = 0; i < ties.size (); i++)
    {
      x_refpoint_ = dynamic_cast<Spanner*> (ties[i])->get_bound (LEFT)->common_refpoint (x_refpoint_, X_AXIS); 
      x_refpoint_ = dynamic_cast<Spanner*> (ties[i])->get_bound (RIGHT)->common_refpoint (x_refpoint_, X_AXIS); 
    }

  details_.from_grob (ties[0]);
  
  Direction d = LEFT;
  do
    {
      Link_array<Item> bounds;
      
      for (vsize i = 0; i < ties.size (); i++)
	{
	  Item *it = dynamic_cast<Spanner*> (ties[i])->get_bound (d);
					     
	  bounds.push_back (it);
	}
      
      set_chord_outline (bounds, d);
    }
  while (flip (&d) != LEFT);


  for (vsize i = 0; i < ties.size (); i++)
    {
      Tie_specification spec;
      
      if (scm_is_number (ties[i]->get_property_data (ly_symbol2scm ("direction"))))
	{
	  spec.manual_dir_ = to_dir (ties[i]->get_property ("direction"));
	  spec.has_manual_dir_ = true;
	}
	  
      spec.position_ = Tie::get_position (ties[i]);

      do
	{
	  spec.note_head_drul_[d] = Tie::head (ties[i], d);
	}
      while (flip (&d) != LEFT);
      
      specifications_.push_back (spec);
    }
}

void
Tie_formatting_problem::from_lv_ties (Link_array<Grob> const &lv_ties)
{
  if (lv_ties.empty ())
    return;
  
  details_.from_grob (lv_ties[0]);
  Link_array<Item> heads;
  
  for (vsize i = 0; i < lv_ties.size (); i++)
    {
      Tie_specification spec;
      Item *head = unsmob_item (lv_ties[i]->get_object ("note-head"));
       
      if (!head)
	programming_error ("LV tie without head?!");

      if (head)
	{
	  spec.position_ = int (Staff_symbol_referencer::get_position (head));
	}

      spec.note_head_drul_[LEFT] = head;
      heads.push_back (head);
      specifications_.push_back (spec);
    }

  x_refpoint_ = lv_ties [0];
  for (vsize i = 0; i < lv_ties.size (); i++)
    {
      x_refpoint_ = lv_ties[i]->common_refpoint (x_refpoint_, X_AXIS); 
    }

  set_chord_outline (heads, LEFT);

  Real right_most = - infinity_f;   

  for (vsize i = 0; i < chord_outlines_[LEFT].size (); i++)
    {
      right_most = max (right_most, chord_outlines_[LEFT][i].height_);
    }

  Skyline_entry right_entry;
  right_entry.width_.set_full ();
  right_entry.height_ = right_most + 1.5;
  
  chord_outlines_[RIGHT].push_back (right_entry);
}


Tie_specification
Tie_formatting_problem::get_tie_specification (int i) const
{
  return specifications_[i];
}


Tie_configuration*
Tie_formatting_problem::get_configuration (int pos, Direction dir) 
{
  pair<int,int> key (pos, dir);
  Tie_configuration_map::const_iterator f = possibilities_.find (key);
							      
  if (f != possibilities_.end ())
    {
      return (*f).second;
    }

  
  Tie_configuration *conf = generate_configuration (pos,dir);
  possibilities_[key] = conf;
  return conf;
}

Tie_configuration*
Tie_formatting_problem::generate_configuration (int pos, Direction dir) const
{
  Tie_configuration *conf = new Tie_configuration;
  conf->position_ = pos;
  conf->dir_ = dir;
  Real y = conf->position_ * 0.5 * details_.staff_space_;

  if (dot_positions_.find (pos) != dot_positions_.end ())
    {
      conf->delta_y_ += 0.25 * details_.staff_space_;
    }
  
  conf->attachment_x_ = get_attachment (y + conf->delta_y_);

  Real h =  conf->height (details_);
  if (!conf->delta_y_)
    {
      /*
	TODO:

	- should make sliding criterion, should flatten ties if

	- they're just the wrong (ie. touching line at top & bottom)
	size.
	
       */
      if (h < details_.intra_space_threshold_ * 0.5 * details_.staff_space_)
	{
	  if (!Staff_symbol_referencer::on_line (details_.staff_symbol_referencer_, pos)
	      && abs (pos) < 2 * Staff_symbol_referencer::staff_radius (details_.staff_symbol_referencer_))
	    {
	      conf->center_tie_vertically (details_);
	    }
	  else if (Staff_symbol_referencer::on_line (details_.staff_symbol_referencer_, pos))
	    {
	      conf->delta_y_ += dir *
		details_.tip_staff_line_clearance_ * 0.5 *  details_.staff_space_;
	    }
	}
      else 
	{
	  Real top_y = y + conf->delta_y_ + conf->dir_ * h;
	  Real top_pos = top_y / (0.5*details_.staff_space_);
	  int round_pos = int (my_round (top_pos));

	  /* TODO: should use other variable? */
	  Real clearance = details_.center_staff_line_clearance_;
	  if (fabs (top_pos - round_pos) < clearance
	      && Staff_symbol_referencer::on_staff_line (details_.staff_symbol_referencer_,
							 round_pos))
	    {
	      Real new_y = (round_pos + clearance * conf->dir_) * 0.5 * details_.staff_space_;
	      conf->delta_y_ = (new_y - top_y);
	    }
	}
    }
  
  /*
    we don't recompute attachment_x_ to take changed Y (through
    delta_Y) into account. Doing would make ties go into small holes between heads, which
    means we get collisions with neighboring heads.
   */
  conf->attachment_x_.widen ( - details_.x_gap_);

  Direction d = LEFT;
  do
    {
      Real y = conf->position_ * details_.staff_space_ * 0.5 + conf->delta_y_;
      if (stem_extents_[d][X_AXIS].is_empty ()
	  || !stem_extents_[d][Y_AXIS].contains (y))
	continue;

      conf->attachment_x_[d] =
	d* min (d * conf->attachment_x_[d],
		d * (stem_extents_[d][X_AXIS][-d] - d * details_.stem_gap_));
    }
  while (flip (&d) != LEFT);
  return conf;
}

Real
Tie_formatting_problem::score_aptitude (Tie_configuration const &conf,
					Tie_specification const &spec) const
{
  Real penalty = 0.0;
  Real curve_y = conf.position_ * details_.staff_space_ * 0.5 + conf.delta_y_;
  Real tie_y = spec.position_ * details_.staff_space_ * 0.5;
  if (sign (curve_y - tie_y) != conf.dir_)
    penalty += details_.wrong_direction_offset_penalty_;

  penalty += details_.vertical_distance_penalty_factor_ * fabs (curve_y - tie_y);


  Direction d = LEFT;
  do
    {
      if (!spec.note_head_drul_[d])
	continue;
      
      Interval head_x = spec.note_head_drul_[d]->extent (x_refpoint_, X_AXIS);
      Real dist = head_x.distance (conf.attachment_x_[d]);
      penalty += details_.horizontal_distance_penalty_factor_ * dist;
    }
  while  (flip (&d) != LEFT);

  return penalty;
}

Real
Tie_formatting_problem::score_configuration (Tie_configuration const &conf) const
{
  Real penalty = 0.0;
  Real length = conf.attachment_x_.length ();
  if (length < details_.min_length_)
    penalty += details_.length_penalty_factor_ / max (0.01, length);

  Real tip_pos = conf.position_ + conf.delta_y_ / 0.5 * details_.staff_space_;
  Real tip_y = tip_pos * details_.staff_space_ * 0.5;
  Real height =  conf.height (details_);

  Real top_y = tip_y + conf.dir_ * height;
  Real top_pos = 2 * top_y / details_.staff_space_;
  Real round_top_pos = rint (top_pos);
  if (Staff_symbol_referencer::on_line (details_.staff_symbol_referencer_,
						int (round_top_pos))
      && Staff_symbol_referencer::staff_radius (details_.staff_symbol_referencer_) > top_y)
    {
      penalty +=
	details_.staff_line_collision_penalty_
	* peak_around (0.1 * details_.center_staff_line_clearance_,
		     details_.center_staff_line_clearance_,
		     fabs (top_pos - round_top_pos));
    }
  
  if (Staff_symbol_referencer::on_line (details_.staff_symbol_referencer_,
					int (rint (tip_pos))))
    {
      penalty += details_.staff_line_collision_penalty_
	* peak_around (0.1 * details_.tip_staff_line_clearance_,
		       details_.tip_staff_line_clearance_,
		       fabs (tip_pos - rint (tip_pos)));
    }

  if (!dot_x_.is_empty ())
    {
      /* use left edge? */
      Real x = dot_x_.center ();
      
      Bezier b = conf.get_transformed_bezier (details_);
      if (b.control_point_extent (X_AXIS).contains (x))
	{
	  Real y = b.get_other_coordinate (X_AXIS, x);

	  for (set<int>::const_iterator i (dot_positions_.begin ());
	       i != dot_positions_.end (); i ++)
	    {
	      int dot_pos = (*i);
	      penalty +=
		details_.dot_collision_penalty_
		* peak_around (.1 * details_.dot_collision_clearance_,
			       details_.dot_collision_clearance_,
			       fabs (dot_pos * details_.staff_space_ * 0.5 - y)); 
	    }
	}
    }
  
  return penalty;
}

Tie_configuration
Tie_formatting_problem::find_optimal_tie_configuration (Tie_specification const &spec) const
{
  Link_array<Tie_configuration> confs;

  int pos = spec.position_;
  Direction dir = spec.manual_dir_;

  int region_size = 3;
  for (int i = 0; i < region_size; i ++)
    {
      confs.push_back (generate_configuration (pos + i * dir, dir));
    }

  std::vector<Real> scores;

  int best_idx = -1;
  Real best_score = 1e6;
  for (vsize i = 0; i < confs.size (); i ++)
    {
      Real score = 0.0;
      score += score_configuration (*confs[i]);
      score += score_aptitude (*confs[i], spec);

      if (score < best_score)
	{
	  best_score = score;
	  best_idx = i;
	}
    }

  Tie_configuration best = *confs[best_idx];
  for (vsize i = 0; i < confs.size (); i++)
    delete confs[i];

  return best;
}

Tie_specification::Tie_specification ()
{
  has_manual_position_ = false;
  has_manual_dir_ = false;
  position_ = 0;
  manual_position_ = 0;
  manual_dir_ = CENTER;
  note_head_drul_[LEFT] =
    note_head_drul_[RIGHT] = 0;
}


Real
Tie_formatting_problem::score_ties_aptitude (Ties_configuration const &ties) const
{
  Real score = 0.0;
  if  (ties.size () != specifications_.size ())
    {
      programming_error ("Huh? Mismatch between sizes.");
      return infinity_f;
    }

  for (vsize i = 0; i < ties.size (); i++)
    score += score_aptitude (ties[i], specifications_[i]);

  return score;
}

Real
Tie_formatting_problem::score_ties (Ties_configuration const &ties) const
{
  return score_ties_configuration (ties)
    + score_ties_aptitude (ties);
}

Real
Tie_formatting_problem::score_ties_configuration (Ties_configuration const &ties) const
{
  Real score = 0.0;
  for (vsize i = 0; i < ties.size (); i++)
    {
      score += score_configuration (ties[i]);
    }

  Real last_edge = 0.0;
  Real last_center = 0.0;
  for (vsize i = 0; i < ties.size (); i++)
    {
      Bezier b (ties[i].get_transformed_bezier (details_));
	
      Real center = b.curve_point (0.5)[Y_AXIS];
      Real edge = b.curve_point (0.0)[Y_AXIS];
      
      if (i)
	{
	  if (edge <= last_edge)
	    score += details_.tie_column_monotonicity_penalty_;
	  if (center <= last_center)
	    score += details_.tie_column_monotonicity_penalty_;

	  score +=
	    details_.tie_tie_collision_penalty_ *
	    peak_around (0.1 * details_.tie_tie_collision_distance_,
			 details_.tie_tie_collision_distance_,
			 fabs (center - last_center));
	  score +=
	    details_.tie_tie_collision_penalty_ *
	    peak_around (0.1 * details_.tie_tie_collision_distance_,
			 details_.tie_tie_collision_distance_,
			 fabs (edge - last_edge));
	}

      last_edge = edge;
      last_center = center;
    }


  score +=
    details_.outer_tie_length_symmetry_penalty_factor_
    * fabs (ties[0].attachment_x_.length () - ties.back ().attachment_x_.length ());
  
  score +=
    details_.outer_tie_vertical_distance_symmetry_penalty_factor_
    * (fabs (specifications_[0].position_
	     - (ties[0].position_ * 0.5 * details_.staff_space_ + ties[0].delta_y_))
       -
       fabs (specifications_.back ().position_
	     - (ties.back ().position_ * 0.5 * details_.staff_space_ + ties.back ().delta_y_)));
  
  return score;
}

/*
  Generate with correct X-attachments and beziers, copying delta_y_
  from TIES_CONFIG if necessary.
*/
Ties_configuration
Tie_formatting_problem::generate_ties_configuration (Ties_configuration const &ties_config)
{
  Ties_configuration copy;
  for (vsize i = 0; i < ties_config.size (); i++)
    {
      Tie_configuration * ptr = get_configuration (ties_config[i].position_, ties_config[i].dir_);
      if (specifications_[i].has_manual_position_)
	{
	  ptr->delta_y_
	    = (specifications_[i].manual_position_ - ties_config[i].position_)
	    * 0.5 * details_.staff_space_;
	}
      copy.push_back (*ptr);
    }
  
  return copy;
}

Ties_configuration
Tie_formatting_problem::generate_base_chord_configuration () 
{
  Ties_configuration ties_config;
  for (vsize i = 0;  i < specifications_.size (); i ++)
    {
      Tie_configuration conf;
      if (specifications_[i].has_manual_dir_)
	conf.dir_ = specifications_[i].manual_dir_;
      if (specifications_[i].has_manual_position_)
	{
	  conf.position_ = (int) my_round (specifications_[i].manual_position_);
	  conf.delta_y_ = (specifications_[i].manual_position_ - conf.position_)
	    * 0.5 * details_.staff_space_;
	}
      else
	{
	  conf.position_ = specifications_[i].position_;
	}
      ties_config.push_back (conf);
    }

  set_ties_config_standard_directions (&ties_config);
  for (vsize i = 0; i < ties_config.size (); i++)
    if (!specifications_[i].manual_position_)
      ties_config[i].position_ += ties_config[i].dir_;

  ties_config = generate_ties_configuration (ties_config);
  
  return ties_config;
}

Ties_configuration
Tie_formatting_problem::generate_optimal_chord_configuration ()
{
  Ties_configuration base = generate_base_chord_configuration ();
  std::vector<Tie_configuration_variation> vars = get_variations (base);

  Ties_configuration best = base;
  Real best_score = score_ties (best);

  /*
    This simply is 1-opt: we have K substitions, and we try applying
    exactly every one for each.
  */
  for (vsize i = 0; i < vars.size (); i++)
    {
      Ties_configuration variant = base;
      variant[vars[i].index_] = *vars[i].suggestion_;

      Real score = score_ties (variant);
      if (score < best_score)
	{
	  best = variant;
	  best_score = score;
	}
    }

  return best;
}

void
Tie_formatting_problem::set_ties_config_standard_directions (Ties_configuration *tie_configs)
{
  if (tie_configs->empty ())
    return ;
  
  if (!tie_configs->at (0).dir_)
    tie_configs->at (0).dir_ = DOWN;
  if (!tie_configs->back ().dir_)
    tie_configs->back ().dir_ = UP;

  /*
    Seconds
   */
  for (vsize i = 1; i < tie_configs->size (); i++)
    {
      Real diff = (tie_configs->at (i-1).position_
		   - tie_configs->at (i).position_);

      if (fabs (diff) <= 1)
	{
	  if (!tie_configs->at (i-1).dir_)
	    tie_configs->at (i-1).dir_ = DOWN;
	  if (!tie_configs->at (i).dir_)
	    tie_configs->at (i).dir_ = UP;
	}
    }

  for (vsize i = 1; i < tie_configs->size() - 1; i++)
    {
      Tie_configuration &conf = tie_configs->at (i);
      if (conf.dir_)
	continue;

      Direction position_dir =
	Direction (sign (conf.position_));
      if (!position_dir)
	position_dir = DOWN;

      conf.dir_ = position_dir;
    }
}

Tie_configuration_variation::Tie_configuration_variation ()
{
  index_ = 0;
  suggestion_ = 0;
}

std::vector<Tie_configuration_variation>
Tie_formatting_problem::get_variations (Ties_configuration const &ties) 
{
  Real center_distance_tolerance = 0.25;
  
  std::vector<Tie_configuration_variation> vars;
  Real last_center = 0.0;
  for (vsize i = 0; i < ties.size (); i++)
    {
      Bezier b (ties[i].get_transformed_bezier (details_));
	
      Real center = b.curve_point (0.5)[Y_AXIS];
      
      if (i)
	{
	  if (center <= last_center + center_distance_tolerance)
	    {
	      if (!specifications_[i].has_manual_dir_)
		{
		  Tie_configuration_variation var;
		  var.index_ = i;
		  var.suggestion_ = get_configuration (specifications_[i].position_
						       - ties[i].dir_,
						       -ties[i].dir_);

		  vars.push_back (var);
		}

	      if (!specifications_[i-1].has_manual_dir_)
		{
		  Tie_configuration_variation var;
		  var.index_ = i-1;
		  var.suggestion_ = get_configuration (specifications_[i-1].position_
						       - ties[i-1].dir_,
						       - ties[i-1].dir_);

		  vars.push_back (var);
		}
	    }
	}

      last_center = center;
    }

  /*  TODO: switch off? */
  Direction d = DOWN;
  do
    {
      if (ties.boundary (d, 0).dir_ == d)
	{
	  Tie_configuration_variation var;
	  var.index_ = (d == DOWN) ? 0 : ties.size () - 1;
	  var.suggestion_ = get_configuration (ties.boundary (d, 0).position_ + d,
					       d);
	  vars.push_back (var);
	}
    }
  while (flip (&d) !=  DOWN);

  return vars;
}

void
Tie_formatting_problem::set_manual_tie_configuration (SCM manual_configs)
{
  vsize k = 0;
  for (SCM s = manual_configs;
       scm_is_pair (s) && k < specifications_.size (); s = scm_cdr (s))
    {
      SCM entry = scm_car (s);
      if (scm_is_pair (entry))
	{
	  Tie_specification &spec = specifications_[k];

	  if (scm_is_number (scm_cdr (entry)))
	    {
	      spec.has_manual_dir_ = true;
	      spec.manual_dir_ = Direction (scm_to_int (scm_cdr (entry)));
	    }
	  if (scm_is_number (scm_car (entry)))
	    {
	      spec.has_manual_position_ = true;
	      spec.manual_position_ = scm_to_double (scm_car (entry));
	    }
	}	  
      k ++;
    }
}

