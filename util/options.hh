/*
 * Copyright © 2011  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Behdad Esfahbod
 */

#ifndef OPTIONS_HH
#define OPTIONS_HH


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <errno.h>
#include <fcntl.h>
#ifdef HAVE_IO_H
#include <io.h> /* for _setmode() under Windows */
#endif

#include <hb.h>
#include <glib.h>
#include <glib/gprintf.h>

void fail (hb_bool_t suggest_help, const char *format, ...) G_GNUC_NORETURN;


extern hb_bool_t debug;

struct option_group_t
{
  virtual void add_options (struct option_parser_t *parser) = 0;

  virtual void pre_parse (GError **error G_GNUC_UNUSED) {};
  virtual void post_parse (GError **error G_GNUC_UNUSED) {};
};


struct option_parser_t
{
  option_parser_t (const char *usage) {
    memset (this, 0, sizeof (*this));
    usage_str = usage;
    context = g_option_context_new (usage);

    add_main_options ();
  }
  ~option_parser_t (void) {
    g_option_context_free (context);
  }

  void add_main_options (void);

  void add_group (GOptionEntry   *entries,
		  const gchar    *name,
		  const gchar    *description,
		  const gchar    *help_description,
		  option_group_t *option_group);

  void parse (int *argc, char ***argv);

  G_GNUC_NORETURN void usage (void) {
    g_printerr ("Usage: %s [OPTION...] %s\n", g_get_prgname (), usage_str);
    exit (1);
  }

  const char *usage_str;
  GOptionContext *context;
};


#define DEFAULT_MARGIN 18
#define DEFAULT_FORE "#000000"
#define DEFAULT_BACK "#FFFFFF"
#define DEFAULT_FONT_SIZE 36

struct view_options_t : option_group_t
{
  view_options_t (option_parser_t *parser) {
    annotate = false;
    fore = DEFAULT_FORE;
    back = DEFAULT_BACK;
    line_space = 0;
    margin.t = margin.r = margin.b = margin.l = DEFAULT_MARGIN;
    font_size = DEFAULT_FONT_SIZE;

    add_options (parser);
  }

  void add_options (option_parser_t *parser);

  hb_bool_t annotate;
  const char *fore;
  const char *back;
  double line_space;
  struct margin_t {
    double t, r, b, l;
  } margin;
  double font_size;
};


struct shape_options_t : option_group_t
{
  shape_options_t (option_parser_t *parser) {
    direction = language = script = NULL;
    features = NULL;
    num_features = 0;
    shapers = NULL;

    add_options (parser);
  }
  ~shape_options_t (void) {
    free (features);
    g_free (shapers);
  }

  void add_options (option_parser_t *parser);

  void setup_buffer (hb_buffer_t *buffer) {
    hb_buffer_set_direction (buffer, hb_direction_from_string (direction, -1));
    hb_buffer_set_script (buffer, hb_script_from_string (script, -1));
    hb_buffer_set_language (buffer, hb_language_from_string (language, -1));
  }

  hb_bool_t shape (const char *text, int text_len,
		   hb_font_t *font, hb_buffer_t *buffer) {
    hb_buffer_reset (buffer);
    hb_buffer_add_utf8 (buffer, text, text_len, 0, text_len);
    setup_buffer (buffer);
    return hb_shape_full (font, buffer, features, num_features, NULL, shapers);
  }

  const char *direction;
  const char *language;
  const char *script;
  hb_feature_t *features;
  unsigned int num_features;
  char **shapers;
};


struct font_options_t : option_group_t
{
  font_options_t (option_parser_t *parser) {
    font_file = NULL;
    face_index = 0;

    font = NULL;

    add_options (parser);
  }
  ~font_options_t (void) {
    hb_font_destroy (font);
  }

  void add_options (option_parser_t *parser);

  hb_font_t *get_font (void) const;

  const char *font_file;
  int face_index;

  private:
  mutable hb_font_t *font;
};


struct text_options_t : option_group_t
{
  text_options_t (option_parser_t *parser) {
    text = NULL;
    text_file = NULL;

    fp = NULL;
    gs = NULL;
    text_len = (unsigned int) -1;

    add_options (parser);
  }
  ~text_options_t (void) {
    if (gs)
      g_string_free (gs, TRUE);
    if (fp)
      fclose (fp);
  }

  void add_options (option_parser_t *parser);

  void post_parse (GError **error G_GNUC_UNUSED) {
    if (text && text_file)
      g_set_error (error,
		   G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
		   "Only one of text and text-file must be set");

  };

  const char *get_line (unsigned int *len);

  const char *text;
  const char *text_file;

  private:
  FILE *fp;
  GString *gs;
  unsigned int text_len;
};

struct output_options_t : option_group_t
{
  output_options_t (option_parser_t *parser) {
    output_file = NULL;
    output_format = NULL;

    fp = NULL;

    add_options (parser);
  }
  ~output_options_t (void) {
    if (fp)
      fclose (fp);
  }

  void add_options (option_parser_t *parser);

  void post_parse (GError **error G_GNUC_UNUSED)
  {
    if (output_file && !output_format) {
      output_format = strrchr (output_file, '.');
      if (output_format)
	  output_format++; /* skip the dot */
    }

    if (output_file && 0 == strcmp (output_file, "-"))
      output_file = NULL; /* STDOUT */
  }

  FILE *get_file_handle (void);

  virtual void init (const font_options_t *font_opts) = 0;
  virtual void consume_line (hb_buffer_t  *buffer,
			     const char   *text,
			     unsigned int  text_len) = 0;
  virtual void finish (const font_options_t *font_opts) = 0;

  const char *output_file;
  const char *output_format;

  protected:

  mutable FILE *fp;
};

struct format_options_t : option_group_t
{
  format_options_t (option_parser_t *parser) {
    show_glyph_names = true;
    show_positions = true;
    show_clusters = true;

    add_options (parser);
  }
  ~format_options_t (void) {
  }

  void add_options (option_parser_t *parser);

  void serialize (hb_buffer_t *buffer,
		  hb_font_t   *font,
		  GString     *gs);

  protected:
  hb_bool_t show_glyph_names;
  hb_bool_t show_positions;
  hb_bool_t show_clusters;
};


#endif
