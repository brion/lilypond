/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2004--2012 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#ifndef CONTEXT_HH
#define CONTEXT_HH

#include "duration.hh"
#include "lily-proto.hh"
#include "listener.hh"
#include "moment.hh"
#include "scm-hash.hh"
#include "std-vector.hh"
#include "virtual-methods.hh"

class Context
{
  Scheme_hash_table *properties_dict () const;
  Context (Context const &src);

  DECLARE_SMOBS (Context);
  DECLARE_CLASSNAME (Context);
  void terminate ();

private:
  friend class Context_handle;
  /* how many Context_handles point to this Context */
  int client_count_;

  /* Used internally by create_context */
  Stream_event *infant_event_;

protected:
  Context *daddy_context_;
  /* The used Context_def */
  SCM definition_;
  /* Additions to the Context_def, given by \with */
  SCM definition_mods_;

  SCM properties_scm_;
  SCM context_list_;
  SCM accepts_list_;
  SCM default_child_;
  SCM aliases_;
  Translator_group *implementation_;
  string id_string_;

  /* Events reported in the context is sent to this dispatcher. */
  Dispatcher *event_source_;

  /* Events reported to this context or recursively in any of its
     children, are sent to this dispatcher. */
  Dispatcher *events_below_;

  // Translator_group is allowed to set implementation_.
  friend class Translator_group;
  // Context_def::instantiate initialises some protected members.
  friend class Context_def;
  // UGH! initialises implementation_
  friend SCM ly_make_global_translator (SCM);

  DECLARE_LISTENER (set_property_from_event);
  DECLARE_LISTENER (unset_property_from_event);

public:
  string id_string () const { return id_string_; }
  SCM children_contexts () const { return context_list_; }
  SCM default_child_context_name () const;

  Dispatcher *event_source () const { return event_source_; }
  Dispatcher *events_below () const { return events_below_; }
  void internal_send_stream_event (SCM type, Input *origin, SCM props[]);

  SCM get_definition () const { return definition_; }
  SCM get_definition_mods () const { return definition_mods_; }

  Translator_group *implementation () const { return implementation_; }
  Context *get_parent_context () const;
  Context ();

  /* properties:  */
  SCM internal_get_property (SCM name_sym) const;
  SCM properties_as_alist () const;
  Context *where_defined (SCM name_sym, SCM *value) const;
  void unset_property (SCM var_sym);

  void instrumented_set_property (SCM, SCM, const char *, int, const char *);
  void internal_set_property (SCM var_sym, SCM value);

  Context *create_context (Context_def *, string, SCM);
  DECLARE_LISTENER (create_context_from_event);
  DECLARE_LISTENER (acknowledge_infant);
  DECLARE_LISTENER (remove_context);
  DECLARE_LISTENER (change_parent);
  void disconnect_from_parent ();
  void check_removal ();
  string context_name () const;
  SCM context_name_symbol () const;
  Global_context *get_global_context () const;

  virtual Context *get_score_context () const;
  virtual Output_def *get_output_def () const;
  virtual Moment now_mom () const;
  virtual Context *get_default_interpreter (string context_id = "");

  // It would make some sense to have the following just available in
  // a global context.  It would be decidedly tricky, however, to have
  // it initialized before the constructor of its Context base class
  // was able to trigger garbage collection.
  SCM ancestor_lookup_;
  SCM make_event_class (SCM);

  bool is_alias (SCM) const;
  void add_alias (SCM);
  void add_context (Context *trans);
  bool is_bottom_context () const;
  bool is_removable () const;

  Context *find_create_context (SCM context_name,
                                string id, SCM ops);
  Context *create_unique_context (SCM context_name, string context_id,
                                  SCM ops);
  vector<Context_def *> path_to_acceptable_context (SCM alias) const;
};

/*
  Context arg?
*/

void apply_property_operations (Context *tg, SCM pre_init_ops);
void execute_revert_property (Context *context,
                              SCM context_property,
                              SCM grob_property_path);
void execute_pushpop_property (Context *trg, SCM prop, SCM eltprop, SCM val);
void sloppy_general_pushpop_property (Context *context,
                                      SCM context_property, SCM grob_property_path, SCM val);
SCM updated_grob_properties (Context *tg, SCM sym);
Context *find_context_below (Context *where,
                             SCM type_sym, string id);
bool melisma_busy (Context *);

Context *get_voice_to_lyrics (Context *lyrics);
Grob *get_current_note_head (Context *voice, bool include_grace_notes);
Grob *get_current_rest (Context *voice);
DECLARE_UNSMOB (Context, context);

Moment measure_position (Context const *context);
Moment measure_position (Context const *context, Duration const *dur);
Rational measure_length (Context const *context);
int measure_number (Context const *context);

bool check_repeat_count_visibility (Context const *context, SCM count);

void set_context_property_on_children (Context *trans, SCM sym, SCM val);

/* Shorthand for creating and broadcasting stream events. */
#define send_stream_event(ctx, type, origin, ...)                               \
{                                                                       \
  SCM props[] = { __VA_ARGS__, 0 };                                     \
  ctx->internal_send_stream_event (ly_symbol2scm (type), origin, props);        \
}

SCM nested_property_alist (SCM alist, SCM prop_path, SCM value);
SCM nested_property_revert_alist (SCM alist, SCM prop_path);
SCM evict_from_alist (SCM, SCM, SCM);

#endif /* CONTEXT_HH */
