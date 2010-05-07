/*
 * Copyright (C) 2009 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as 
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by Jason Smith <jason.smith@canonical.com>
 *
 */

#ifndef __BAMFMOCKWINDOW_H__
#define __BAMFMOCKWINDOW_H__

#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>
#include "bamf-application.h"
#include "bamf-window.h"
#include "bamf-legacy-window.h"

#define BAMF_TYPE_LEGACY_WINDOW_TEST (bamf_legacy_window_test_get_type ())

#define BAMF_LEGACY_WINDOW_TEST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
	BAMF_TYPE_LEGACY_WINDOW_TEST, BamfLegacyWindowTest))

#define BAMF_LEGACY_WINDOW_TEST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass),\
	BAMF_TYPE_LEGACY_WINDOW_TEST, BamfLegacyWindowTestClass))

#define BAMF_IS_LEGACY_WINDOW_TEST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
	BAMF_TYPE_LEGACY_WINDOW_TEST))

#define BAMF_IS_LEGACY_WINDOW_TEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),\
	BAMF_TYPE_LEGACY_WINDOW_TEST))

#define BAMF_LEGACY_WINDOW_TEST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),\
	BAMF_TYPE_LEGACY_WINDOW_TEST, BamfLegacyWindowTestClass))

typedef struct _BamfLegacyWindowTest        BamfLegacyWindowTest;
typedef struct _BamfLegacyWindowTestClass   BamfLegacyWindowTestClass;

struct _BamfLegacyWindowTest
{
  BamfLegacyWindow parent;
  guint32 xid;
  gint    pid;
  char * name;
  gboolean needs_attention;
  gboolean is_desktop;
  gboolean is_skip;
};

struct _BamfLegacyWindowTestClass
{
  /*< private >*/
  BamfLegacyWindowClass parent_class;
  
  void (*_test_padding1) (void);
  void (*_test_padding2) (void);
  void (*_test_padding3) (void);
  void (*_test_padding4) (void);
  void (*_test_padding5) (void);
  void (*_test_padding6) (void);
};

GType       bamf_legacy_window_test_get_type (void) G_GNUC_CONST;

gint
bamf_legacy_window_test_get_pid (BamfLegacyWindow *legacy_window);

guint32
bamf_legacy_window_test_get_xid (BamfLegacyWindow *legacy_window);

void
bamf_legacy_window_test_set_attention (BamfLegacyWindowTest *self, gboolean val);

gboolean
bamf_legacy_window_test_needs_attention (BamfLegacyWindow *legacy_window);

void
bamf_legacy_window_test_set_desktop (BamfLegacyWindowTest *self, gboolean val);

gboolean
bamf_legacy_window_test_is_desktop (BamfLegacyWindow *legacy_window);

void
bamf_legacy_window_test_set_skip (BamfLegacyWindowTest *self, gboolean val);

gboolean
bamf_legacy_window_test_is_skip_tasklist (BamfLegacyWindow *legacy_window);

void
bamf_legacy_window_test_set_name (BamfLegacyWindowTest *self, char *val);

char *
bamf_legacy_window_test_get_name (BamfLegacyWindow *legacy_window);

BamfLegacyWindowTest *
bamf_legacy_window_test_new (guint32 xid, gchar *name);

#endif
