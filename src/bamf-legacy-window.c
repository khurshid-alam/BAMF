/*
 * Copyright (C) 2010-2012 Canonical Ltd
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
 * Authored by: Jason Smith <jason.smith@canonical.com>
 *              Marco Trevisan (Treviño) <3v1n0@ubuntu.com>
 *
 */

#include "config.h"

#include "bamf-legacy-window.h"
#include "bamf-legacy-screen.h"
#include "bamf-xutils.h"
#include <libgtop-2.0/glibtop.h>
#include <glibtop/procargs.h>
#include <glibtop/procuid.h>
#include <stdio.h>

G_DEFINE_TYPE (BamfLegacyWindow, bamf_legacy_window, G_TYPE_OBJECT);
#define BAMF_LEGACY_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
BAMF_TYPE_LEGACY_WINDOW, BamfLegacyWindowPrivate))

enum
{
  NAME_CHANGED,
  STATE_CHANGED,
  GEOMETRY_CHANGED,
  CLOSED,

  LAST_SIGNAL,
};

static guint legacy_window_signals[LAST_SIGNAL] = { 0 };

struct _BamfLegacyWindowPrivate
{
  WnckWindow * legacy_window;
  char       * mini_icon_path;
  gboolean     is_closed;
};

gboolean
bamf_legacy_window_is_active (BamfLegacyWindow *self)
{
  WnckWindow *active;

  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), FALSE);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->is_active)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->is_active (self);

  active = wnck_screen_get_active_window (wnck_screen_get_default ());

  return active == self->priv->legacy_window;
}

BamfWindowType
bamf_legacy_window_get_window_type (BamfLegacyWindow *self)
{
  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), 0);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_window_type)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_window_type (self);

  g_return_val_if_fail (self->priv->legacy_window, 0);

  return (BamfWindowType) wnck_window_get_window_type (self->priv->legacy_window);
}

gboolean
bamf_legacy_window_needs_attention (BamfLegacyWindow *self)
{
  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), FALSE);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->needs_attention)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->needs_attention (self);

  if (!self->priv->legacy_window)
    return FALSE;
  return wnck_window_needs_attention (self->priv->legacy_window);
}

gboolean
bamf_legacy_window_is_skip_tasklist (BamfLegacyWindow *self)
{
  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), FALSE);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->is_skip_tasklist)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->is_skip_tasklist (self);

  if (!self->priv->legacy_window)
    return FALSE;
  return wnck_window_is_skip_tasklist (self->priv->legacy_window);
}

const char *
bamf_legacy_window_get_class_instance_name (BamfLegacyWindow *self)
{
  WnckWindow *window;

  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), NULL);

  window = self->priv->legacy_window;

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_class_instance_name)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_class_instance_name (self);

  if (!window)
    return NULL;

  return wnck_window_get_class_instance_name (window);
}

const char *
bamf_legacy_window_get_class_name (BamfLegacyWindow *self)
{
  WnckWindow *window;

  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), NULL);

  window = self->priv->legacy_window;

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_class_name)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_class_name (self);

  if (!window)
    return NULL;

  return wnck_window_get_class_group_name (window);
}

const char *
bamf_legacy_window_get_name (BamfLegacyWindow *self)
{
  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), NULL);


  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_name)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_name (self);

  if (!self->priv->legacy_window)
    return NULL;
  return wnck_window_get_name (self->priv->legacy_window);
}

char *
bamf_legacy_window_get_process_name (BamfLegacyWindow *self)
{
  gchar *stat_path;
  gchar *contents;
  gchar **lines;
  gchar **sections;
  gchar *result = NULL;
  guint pid;

  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), NULL);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_process_name)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_process_name (self);

  pid = bamf_legacy_window_get_pid (self);

  if (pid <= 0)
    return NULL;

  stat_path = g_strdup_printf ("/proc/%i/status", pid);

  if (g_file_get_contents (stat_path, &contents, NULL, NULL))
    {
      lines = g_strsplit (contents, "\n", 2);

      if (lines && g_strv_length (lines) > 0)
        {
          sections = g_strsplit (lines[0], "\t", 0);
          if (sections)
            {
              if (g_strv_length (sections) > 1)
                result = g_strdup (sections[1]);

              g_strfreev (sections);
            }
          g_strfreev (lines);
        }
      g_free (contents);
    }
  g_free (stat_path);

  return result;
}

char *
bamf_legacy_window_get_exec_string (BamfLegacyWindow *self)
{
  gchar *result = NULL;
  gint pid = 0, i = 0;
  gchar **argv = NULL;
  GString *exec = NULL;
  glibtop_proc_args buffer;

  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), NULL);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_exec_string)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_exec_string (self);

  pid = bamf_legacy_window_get_pid (self);

  if (pid == 0)
    return NULL;

  argv = glibtop_get_proc_argv (&buffer, pid, 0);
  exec = g_string_new ("");

  while (argv[i] != NULL)
    {
      g_string_append (exec, argv[i]);
      if (argv[i + 1] != NULL)
        g_string_append (exec, " ");
      g_free (argv[i]);
      i++;
    }

  g_free (argv);

  result = g_strdup (exec->str);
  g_string_free (exec, TRUE);
  return result;
}



const char *
bamf_legacy_window_save_mini_icon (BamfLegacyWindow *self)
{
  WnckWindow *window;
  GdkPixbuf *pbuf;
  char *tmp;

  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), NULL);

  if (self->priv->mini_icon_path)
    {
      if (g_file_test (self->priv->mini_icon_path, G_FILE_TEST_EXISTS))
        return self->priv->mini_icon_path;
      else
        g_free (self->priv->mini_icon_path);
    }

  window = self->priv->legacy_window;

  if (!window)
    return NULL;

  if (wnck_window_get_icon_is_fallback (window))
    return NULL;

  tmp = tmpnam (NULL);
  if (!tmp)
    return NULL;

  pbuf = wnck_window_get_icon (window);
  if (!gdk_pixbuf_save (pbuf, tmp, "png", NULL, NULL))
    return NULL;

  self->priv->mini_icon_path = g_strdup (tmp);
  return self->priv->mini_icon_path;
}

guint
bamf_legacy_window_get_pid (BamfLegacyWindow *self)
{
  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), 0);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_pid)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_pid (self);

  if (!self->priv->legacy_window)
    return 0;

  int pid = wnck_window_get_pid (self->priv->legacy_window);
  return G_LIKELY (pid >= 0) ? pid : 0;
}

guint32
bamf_legacy_window_get_xid (BamfLegacyWindow *self)
{
  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), 0);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_xid)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_xid (self);

  if (!self->priv->legacy_window)
    return 0;

  return (guint32) wnck_window_get_xid (self->priv->legacy_window);
}

BamfLegacyWindow *
bamf_legacy_window_get_transient (BamfLegacyWindow *self)
{
  BamfLegacyScreen *screen;
  BamfLegacyWindow *other;
  GList *windows, *l;
  WnckWindow *transient_legacy;

  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), NULL);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_transient)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_transient (self);

  g_return_val_if_fail (self->priv->legacy_window, NULL);

  transient_legacy = wnck_window_get_transient (self->priv->legacy_window);
  screen = bamf_legacy_screen_get_default ();
  g_return_val_if_fail (BAMF_IS_LEGACY_SCREEN (screen), NULL);

  windows = bamf_legacy_screen_get_windows (screen);
  for (l = windows; l; l = l->next)
    {
      other = l->data;

      if (!BAMF_IS_LEGACY_WINDOW (other))
        continue;

      if (other->priv->legacy_window == transient_legacy)
        return other;
    }

  return NULL;
}

gint
bamf_legacy_window_get_stacking_position (BamfLegacyWindow *self)
{
  BamfLegacyScreen *screen;

  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), -1);

  screen = bamf_legacy_screen_get_default ();
  g_return_val_if_fail (BAMF_IS_LEGACY_SCREEN (screen), -1);

  return g_list_index (bamf_legacy_screen_get_windows (screen), self);
}

static void
handle_name_changed (WnckWindow *window, BamfLegacyWindow *self)
{
  g_return_if_fail (BAMF_IS_LEGACY_WINDOW (self));

  g_signal_emit (self, legacy_window_signals[NAME_CHANGED], 0);
}

static void
handle_state_changed (WnckWindow *window,
                      WnckWindowState change_mask,
                      WnckWindowState new_state,
                      BamfLegacyWindow *self)
{
  g_return_if_fail (BAMF_IS_LEGACY_WINDOW (self));

  g_signal_emit (self, legacy_window_signals[STATE_CHANGED], 0);
}

static void
handle_geometry_changed (WnckWindow *window, BamfLegacyWindow *self)
{
  g_return_if_fail (BAMF_IS_LEGACY_WINDOW (self));

  g_signal_emit (self, legacy_window_signals[GEOMETRY_CHANGED], 0);
}

gboolean
bamf_legacy_window_is_closed (BamfLegacyWindow *self)
{
  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), TRUE);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->is_closed)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->is_closed (self);

  return self->priv->is_closed;
}

void
bamf_legacy_window_get_geometry (BamfLegacyWindow *self, gint *x, gint *y,
                                 gint *width, gint *height)
{
  if (x) *x = 0;
  if (y) *y = 0;
  if (width) *width = 0;
  if (height) *height = 0;

  g_return_if_fail (BAMF_IS_LEGACY_WINDOW (self));

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_app_id)
    BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_app_id (self);

  if (!self->priv->legacy_window)
    return;

  wnck_window_get_geometry (self->priv->legacy_window, x, y, width, height);
}

BamfWindowMaximizationType
bamf_legacy_window_maximized (BamfLegacyWindow *self)
{
  WnckWindowState window_state;
  BamfWindowMaximizationType maximization_type;
  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), BAMF_WINDOW_FLOATING);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->maximized)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->maximized (self);

  if (!self->priv->legacy_window)
    return BAMF_WINDOW_FLOATING;

  window_state = wnck_window_get_state (self->priv->legacy_window);

  gboolean vertical = (window_state & WNCK_WINDOW_STATE_MAXIMIZED_VERTICALLY);
  gboolean horizontal = (window_state & WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY);

  if (vertical && horizontal)
    {
      maximization_type = BAMF_WINDOW_MAXIMIZED;
    }
  else if (horizontal)
    {
      maximization_type = BAMF_WINDOW_HORIZONTAL_MAXIMIZED;
    }
  else if (vertical)
    {
      maximization_type = BAMF_WINDOW_VERTICAL_MAXIMIZED;
    }
  else
    {
      maximization_type = BAMF_WINDOW_FLOATING;
    }

  return maximization_type;
}

char *
bamf_legacy_window_get_hint (BamfLegacyWindow *self, const char *name)
{
  g_return_val_if_fail (BAMF_IS_LEGACY_WINDOW (self), NULL);
  g_return_val_if_fail (name, NULL);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_hint)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->get_hint (self, name);

  g_return_val_if_fail (WNCK_IS_WINDOW (self->priv->legacy_window), NULL);

  guint xid = bamf_legacy_window_get_xid (self);

  return bamf_xutils_get_string_window_hint (xid, name);
}

void
bamf_legacy_window_set_hint (BamfLegacyWindow *self, const char *name, const char *value)
{
  g_return_if_fail (BAMF_IS_LEGACY_WINDOW (self));
  g_return_if_fail (name);

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->set_hint)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->set_hint (self, name, value);

  g_return_if_fail (WNCK_IS_WINDOW (self->priv->legacy_window));

  guint xid = bamf_legacy_window_get_xid (self);

  bamf_xutils_set_string_window_hint (xid, name, value);
}

static void
handle_window_closed (WnckScreen *screen,
                      WnckWindow *window,
                      BamfLegacyWindow *self)
{
  g_return_if_fail (BAMF_IS_LEGACY_WINDOW (self));
  g_return_if_fail (WNCK_IS_WINDOW (window));

  if (self->priv->legacy_window == window)
    {
      self->priv->is_closed = TRUE;
      g_signal_emit (self, legacy_window_signals[CLOSED], 0);
    }
}

static void
handle_destroy_notify (gpointer *data, BamfLegacyWindow *self_was_here)
{
  BamfLegacyScreen *screen = bamf_legacy_screen_get_default ();
  bamf_legacy_screen_inject_window (screen, GPOINTER_TO_UINT (data));
}

/* This utility function allows to set a BamfLegacyWindow as closed, notifying
 * all its owners, and to reopen it once the current window has been destroyed.
 * This allows to remap particular windows to different applications.         */
void
bamf_legacy_window_reopen (BamfLegacyWindow *self)
{
  g_return_if_fail (BAMF_IS_LEGACY_WINDOW (self));

  if (BAMF_LEGACY_WINDOW_GET_CLASS (self)->reopen)
    return BAMF_LEGACY_WINDOW_GET_CLASS (self)->reopen (self);

  g_return_if_fail (WNCK_IS_WINDOW (self->priv->legacy_window));

  guint xid = bamf_legacy_window_get_xid (self);

  /* Adding a weak ref to this object, causes to get notified after the object
   * destruction, so once this BamfLegacyWindow has been closed and destroyed
   * the handle_destroy_notify() function will be called, and that will
   * provide to inject another window like this one to the BamfLegacyScreen  */
  g_object_weak_ref (G_OBJECT (self), (GWeakNotify) handle_destroy_notify,
                                                    GUINT_TO_POINTER (xid));

  self->priv->is_closed = TRUE;
  g_signal_emit (self, legacy_window_signals[CLOSED], 0);
}

static void
bamf_legacy_window_dispose (GObject *object)
{
  BamfLegacyWindow *self;
  GFile *file;

  self = BAMF_LEGACY_WINDOW (object);


  if (self->priv->mini_icon_path)
    {
      file = g_file_new_for_path (self->priv->mini_icon_path);
      g_file_delete (file, NULL, NULL);
      g_object_unref (file);

      g_free (self->priv->mini_icon_path);
      self->priv->mini_icon_path = NULL;
    }

  g_signal_handlers_disconnect_by_data (wnck_screen_get_default (), self);

  if (self->priv->legacy_window)
    {
      g_signal_handlers_disconnect_by_data (self->priv->legacy_window, self);
    }

  G_OBJECT_CLASS (bamf_legacy_window_parent_class)->dispose (object);
}

static void
bamf_legacy_window_init (BamfLegacyWindow * self)
{
  self->priv = BAMF_LEGACY_WINDOW_GET_PRIVATE (self);

  g_signal_connect (wnck_screen_get_default (), "window-closed",
                    (GCallback) handle_window_closed, self);
}

static void
bamf_legacy_window_class_init (BamfLegacyWindowClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose      = bamf_legacy_window_dispose;

  g_type_class_add_private (klass, sizeof (BamfLegacyWindowPrivate));

  legacy_window_signals [NAME_CHANGED] =
    g_signal_new (BAMF_LEGACY_WINDOW_SIGNAL_NAME_CHANGED,
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (BamfLegacyWindowClass, name_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  legacy_window_signals [STATE_CHANGED] =
    g_signal_new (BAMF_LEGACY_WINDOW_SIGNAL_STATE_CHANGED,
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (BamfLegacyWindowClass, state_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  legacy_window_signals [GEOMETRY_CHANGED] =
    g_signal_new (BAMF_LEGACY_WINDOW_SIGNAL_GEOMETRY_CHANGED,
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (BamfLegacyWindowClass, geometry_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  legacy_window_signals [CLOSED] =
    g_signal_new (BAMF_LEGACY_WINDOW_SIGNAL_CLOSED,
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (BamfLegacyWindowClass, closed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

BamfLegacyWindow *
bamf_legacy_window_new (WnckWindow *legacy_window)
{
  BamfLegacyWindow *self;
  self = (BamfLegacyWindow *) g_object_new (BAMF_TYPE_LEGACY_WINDOW, NULL);

  self->priv->legacy_window = legacy_window;

  g_signal_connect (G_OBJECT (legacy_window), "name-changed",
                    (GCallback) handle_name_changed, self);

  g_signal_connect (G_OBJECT (legacy_window), "state-changed",
                    (GCallback) handle_state_changed, self);

  g_signal_connect (G_OBJECT (legacy_window), "geometry-changed",
                    (GCallback) handle_geometry_changed, self);

  return self;
}
