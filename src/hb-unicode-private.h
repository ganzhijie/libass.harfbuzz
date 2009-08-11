/*
 * Copyright (C) 2009  Red Hat, Inc.
 *
 *  This is part of HarfBuzz, an OpenType Layout engine library.
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
 * Red Hat Author(s): Behdad Esfahbod
 */

#ifndef HB_UNICODE_PRIVATE_H
#define HB_UNICODE_PRIVATE_H

#include "hb-private.h"

#include "hb-unicode.h"

HB_BEGIN_DECLS

/*
 * hb_unicode_funcs_t
 */

struct _hb_unicode_funcs_t {
  hb_reference_count_t ref_count;

  /*
  hb_unicode_get_general_category_func_t general_category_func);
  hb_unicode_get_combining_class_func_t combining_class_func);
  hb_unicode_get_mirroring_char_func_t mirroring_char_func);
  hb_unicode_get_script_func_t script_func);
  hb_unicode_get_eastasian_width_func_t eastasian_width_func);
  */
};

HB_END_DECLS

#endif /* HB_UNICODE_PRIVATE_H */