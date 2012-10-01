/*
 * Copyright (C) 2010-2011 Canonical Ltd
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

#include "bamf-application.h"
#include "bamf-window.h"
#include "bamf-matcher.h"
#include "bamf-indicator.h"
#include "bamf-legacy-window.h"
#include "bamf-legacy-screen.h"
#include "bamf-tab.h"
#include <string.h>
#include <gio/gdesktopappinfo.h>

#define BAMF_APPLICATION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
BAMF_TYPE_APPLICATION, BamfApplicationPrivate))

static void bamf_application_dbus_application_iface_init (BamfDBusItemApplicationIface *iface);
G_DEFINE_TYPE_WITH_CODE (BamfApplication, bamf_application, BAMF_TYPE_VIEW,
                         G_IMPLEMENT_INTERFACE (BAMF_DBUS_ITEM_TYPE_APPLICATION,
                                                bamf_application_dbus_application_iface_init));

struct _BamfApplicationPrivate
{
  BamfDBusItemApplication *dbus_iface;
  char * desktop_file;
  GList * desktop_file_list;
  char * app_type;
  char * icon;
  char * wmclass;
  char ** mimes;
  gboolean is_tab_container;
  gboolean show_stubs;
};

enum {
  SUPPORTED_MIMES_CHANGED,
  LAST_SIGNAL
};

static guint application_signals[LAST_SIGNAL] = { 0 };

#define STUB_KEY  "X-Ayatana-Appmenu-Show-Stubs"

static const char *
bamf_application_get_icon (BamfView *view)
{
  g_return_val_if_fail (BAMF_IS_APPLICATION (view), NULL);

  return BAMF_APPLICATION (view)->priv->icon;
}

void
bamf_application_supported_mime_types_changed (BamfApplication *application,
                                               const gchar **new_mimes)
{
  gchar **mimes = (gchar **) new_mimes;

  if (!new_mimes)
    {
      gchar *empty[] = {NULL};
      mimes = g_strdupv (empty);
    }

  g_signal_emit_by_name (application->priv->dbus_iface, "supported-mime-types-changed", mimes);

  if (!new_mimes)
  {
    g_strfreev (mimes);
    mimes = NULL;
  }

  if (application->priv->mimes)
    g_strfreev (application->priv->mimes);

  application->priv->mimes = mimes;
}

static gboolean
bamf_application_default_get_close_when_empty (BamfApplication *application)
{
  return TRUE;
}

static gchar **
bamf_application_default_get_supported_mime_types (BamfApplication *application)
{
  const char *desktop_file = bamf_application_get_desktop_file (application);

  if (!desktop_file)
    return NULL;

  GKeyFile* key_file = g_key_file_new ();
  GError *error = NULL;

  g_key_file_load_from_file (key_file, desktop_file, (GKeyFileFlags) 0, &error);

  if (error)
    {
      g_key_file_free (key_file);
      g_error_free (error);
      return NULL;
    }

  char** mimes = g_key_file_get_string_list (key_file, "Desktop Entry", "MimeType", NULL, NULL);
  g_signal_emit (application, application_signals[SUPPORTED_MIMES_CHANGED], 0, mimes);

  g_key_file_free (key_file);

  return mimes;
}

char **
bamf_application_get_supported_mime_types (BamfApplication *application)
{
  g_return_val_if_fail (BAMF_IS_APPLICATION (application), NULL);

  if (application->priv->mimes)
    return g_strdupv (application->priv->mimes);

  gchar **mimes = BAMF_APPLICATION_GET_CLASS (application)->get_supported_mime_types (application);
  application->priv->mimes = mimes;

  return g_strdupv (mimes);
}

char *
bamf_application_get_application_type (BamfApplication *application)
{
  g_return_val_if_fail (BAMF_IS_APPLICATION (application), NULL);

  return g_strdup (application->priv->app_type);
}

void
bamf_application_set_application_type (BamfApplication *application, const gchar *type)
{
  g_return_if_fail (BAMF_IS_APPLICATION (application));
  
  if (application->priv->app_type)
    g_free (application->priv->app_type);
  
  application->priv->app_type = g_strdup (type);
}

const char *
bamf_application_get_desktop_file (BamfApplication *application)
{
  BamfApplicationPrivate *priv;

  g_return_val_if_fail (BAMF_IS_APPLICATION (application), NULL);
  priv = application->priv;

  return priv->desktop_file;
}

const char *
bamf_application_get_wmclass (BamfApplication *application)
{
  BamfApplicationPrivate *priv;

  g_return_val_if_fail (BAMF_IS_APPLICATION (application), NULL);
  priv = application->priv;

  return priv->wmclass;
}

static gboolean
icon_name_is_valid (char *name)
{
  GtkIconTheme *icon_theme;

  if (name == NULL)
    return FALSE;

  icon_theme = gtk_icon_theme_get_default ();
  return gtk_icon_theme_has_icon (icon_theme, name);
}

static void
bamf_application_setup_icon_and_name (BamfApplication *self)
{
  BamfView *view;
  BamfWindow *window = NULL;
  GDesktopAppInfo *desktop;
  GKeyFile * keyfile;
  GIcon *gicon;
  GList *children, *l;
  const char *class;
  char *icon = NULL, *name = NULL;
  GError *error;

  g_return_if_fail (BAMF_IS_APPLICATION (self));

  if (self->priv->icon && bamf_view_get_name (BAMF_VIEW (self)))
    return;

  if (self->priv->desktop_file)
    {
      keyfile = g_key_file_new();

      if (!g_key_file_load_from_file(keyfile, self->priv->desktop_file, G_KEY_FILE_NONE, NULL))
        {
          g_key_file_free(keyfile);
          return;
        }

      desktop = g_desktop_app_info_new_from_keyfile (keyfile);

      if (!G_IS_APP_INFO (desktop))
        {
          g_key_file_free(keyfile);
          return;
        }

      gicon = g_app_info_get_icon (G_APP_INFO (desktop));

      name = g_strdup (g_app_info_get_display_name (G_APP_INFO (desktop)));

      if (gicon)
        {
          icon = g_icon_to_string (gicon);
        }
      else
        {
          icon = g_strdup ("application-default-icon");
        }

      if (g_key_file_has_key(keyfile, G_KEY_FILE_DESKTOP_GROUP, STUB_KEY, NULL))
        {
          /* This will error to return false, which is okay as it seems
             unlikely anyone will want to set this flag except to turn
             off the stub menus. */
          self->priv->show_stubs = g_key_file_get_boolean (keyfile,
                                                           G_KEY_FILE_DESKTOP_GROUP,
                                                           STUB_KEY, NULL);
        }

      if (g_key_file_has_key (keyfile, G_KEY_FILE_DESKTOP_GROUP, "X-GNOME-FullName", NULL))
        {
          /* Grab the better name if its available */
          gchar *fullname = NULL;
          error = NULL;
          fullname = g_key_file_get_locale_string (keyfile,
                                                   G_KEY_FILE_DESKTOP_GROUP,
                                                   "X-GNOME-FullName", NULL,
                                                   &error);
          if (error != NULL)
            {
              g_error_free (error);
              if (fullname)
                g_free (fullname);
            }
          else
            {
              g_free (name);
              name = fullname;
            }
        }

      g_object_unref (desktop);
      g_key_file_free(keyfile);
    }
  else if ((children = bamf_view_get_children (BAMF_VIEW (self))) != NULL)
    {
      for (l = children; l && !icon; l = l->next)
        {
          view = l->data;
          if (!BAMF_IS_WINDOW (view))
            continue;

          window = BAMF_WINDOW (view);

          do
            {
              class = bamf_legacy_window_get_class_name (bamf_window_get_window (window));
              
              if (class == NULL)
                break;

              icon = g_utf8_strdown (class, -1);

              if (icon_name_is_valid (icon))
                break;

              g_free (icon);
              icon = bamf_legacy_window_get_exec_string (bamf_window_get_window (window));

              if (icon_name_is_valid (icon))
                break;

              g_free (icon);
              icon = NULL;
            }
          while (FALSE);

          name = g_strdup (bamf_legacy_window_get_name (bamf_window_get_window (window)));
        }

      if (!icon)
        {
          if (window)
            {
              icon = g_strdup (bamf_legacy_window_save_mini_icon (bamf_window_get_window (window)));
            }

          if (!icon)
            {
              icon = g_strdup ("application-default-icon");
            }
        }
    }
  else
    {
      /* we do nothing as we have nothing to go on */
    }

  if (icon)
    {
      if (self->priv->icon)
        g_free (self->priv->icon);

      self->priv->icon = icon;
    }

  if (name)
    bamf_view_set_name (BAMF_VIEW (self), name);

  g_free (name);
}

void
bamf_application_set_desktop_file (BamfApplication *application,
                                   const char * desktop_file)
{
  g_return_if_fail (BAMF_IS_APPLICATION (application));

  if (application->priv->desktop_file)
    g_free (application->priv->desktop_file);

  if (desktop_file && desktop_file[0] != '\0')
    application->priv->desktop_file = g_strdup (desktop_file);
  else
    application->priv->desktop_file = NULL;

  bamf_application_setup_icon_and_name (application);
}

gboolean
bamf_application_set_desktop_file_from_id (BamfApplication *application,
                                           const char *desktop_id)
{
  GDesktopAppInfo *info;
  const char *filename;
  
  info = g_desktop_app_info_new (desktop_id);
  
  if (info == NULL)
    {
      g_warning ("Failed to load desktop file from desktop ID: %s", desktop_id);
      return FALSE;
    }
  filename = g_desktop_app_info_get_filename (info);
  bamf_application_set_desktop_file (application, filename);
  
  g_object_unref (G_OBJECT (info));
  
  return TRUE;
}

void
bamf_application_set_wmclass (BamfApplication *application,
                              const char *wmclass)
{
  g_return_if_fail (BAMF_IS_APPLICATION (application));

  if (application->priv->wmclass)
    g_free (application->priv->wmclass);

  if (wmclass && wmclass[0] != '\0')
    application->priv->wmclass = g_strdup (wmclass);
  else
    application->priv->wmclass = NULL;
}

GVariant *
bamf_application_get_xids (BamfApplication *application)
{
  GList *l;
  GVariantBuilder b;
  BamfView *view;
  guint32 xid;

  g_return_val_if_fail (BAMF_IS_APPLICATION (application), NULL);

  g_variant_builder_init (&b, G_VARIANT_TYPE ("(au)"));
  g_variant_builder_open (&b, G_VARIANT_TYPE ("au"));

  for (l = bamf_view_get_children (BAMF_VIEW (application)); l; l = l->next)
    {
      view = l->data;

      if (BAMF_IS_WINDOW (view))
        xid = bamf_window_get_xid (BAMF_WINDOW (view));
      else if (BAMF_IS_TAB (view))
        xid = bamf_tab_get_xid (BAMF_TAB (view));
      else
        continue;
      g_variant_builder_add (&b, "u", xid);
    }

  g_variant_builder_close (&b);

  return g_variant_builder_end (&b);
}

gboolean
bamf_application_contains_similar_to_window (BamfApplication *self,
                                             BamfWindow *bamf_window)
{
  GList *children, *l;
  BamfView *child;

  g_return_val_if_fail (BAMF_IS_APPLICATION (self), FALSE);
  g_return_val_if_fail (BAMF_IS_WINDOW (bamf_window), FALSE);

  BamfLegacyWindow *window = bamf_window_get_window (bamf_window);
  const char *window_class = bamf_legacy_window_get_class_name (window);
  const char *instance_name = bamf_legacy_window_get_class_instance_name (window);

  if (!window_class && !instance_name)
    return FALSE;

  children = bamf_view_get_children (BAMF_VIEW (self));
  for (l = children; l; l = l->next)
    {
      child = l->data;

      if (!BAMF_IS_WINDOW (child))
        continue;

      window = bamf_window_get_window (BAMF_WINDOW (child));
      const char *owned_win_class = bamf_legacy_window_get_class_name (window);
      const char *owned_instance = bamf_legacy_window_get_class_instance_name (window);

      if (g_strcmp0 (window_class, owned_win_class) == 0 &&
          g_strcmp0 (instance_name, owned_instance) == 0)
        {
          return TRUE;
        }
    }

  return FALSE;
}

gboolean
bamf_application_manages_xid (BamfApplication *application,
                              guint32 xid)
{
  GList *l;
  gboolean result = FALSE;

  g_return_val_if_fail (BAMF_IS_APPLICATION (application), FALSE);

  for (l = bamf_view_get_children (BAMF_VIEW (application)); l; l = l->next)
    {
      BamfView *view = l->data;

      if (!BAMF_IS_WINDOW (view))
        continue;

      if (bamf_window_get_xid (BAMF_WINDOW (view)) == xid)
        {
          result = TRUE;
          break;
        }
    }

  return result;
}

static const char *
bamf_application_get_view_type (BamfView *view)
{
  return "application";
}

static char *
bamf_application_get_stable_bus_name (BamfView *view)
{
  BamfApplication *self;
  GList *children, *l;
  BamfView *child;

  g_return_val_if_fail (BAMF_IS_APPLICATION (view), NULL);
  self = BAMF_APPLICATION (view);

  if (self->priv->desktop_file)
    return g_strdup_printf ("application%i", abs (g_str_hash (self->priv->desktop_file)));

  children = bamf_view_get_children (BAMF_VIEW (self));
  for (l = children; l; l = l->next)
    {
      child = l->data;

      if (!BAMF_IS_WINDOW (child))
        continue;

      return g_strdup_printf ("application%s",
                              bamf_legacy_window_get_class_name (bamf_window_get_window (BAMF_WINDOW (child))));
    }

  return g_strdup_printf ("application%p", view);
}

static void
bamf_application_ensure_flags (BamfApplication *self)
{
  gboolean urgent = FALSE, visible = FALSE, running = FALSE, active = FALSE, close_when_empty;
  GList *l;
  BamfView *view;

  for (l = bamf_view_get_children (BAMF_VIEW (self)); l; l = l->next)
    {
      view = l->data;

      if (!BAMF_IS_VIEW (view))
        continue;

      running = TRUE;

      if (BAMF_IS_INDICATOR (view))
        visible = TRUE;

      if (!BAMF_IS_WINDOW (view) && !BAMF_IS_TAB (view))
        continue;

      if (bamf_view_is_urgent (view))
        urgent = TRUE;
      if (bamf_view_user_visible (view))
        visible = TRUE;
      if (bamf_view_is_active (view))
        active = TRUE;

      if (urgent && visible && active)
        break;
    }

  close_when_empty = bamf_application_get_close_when_empty (self);
  bamf_view_set_urgent       (BAMF_VIEW (self), urgent);
  if (close_when_empty == TRUE || running == TRUE)
    bamf_view_set_user_visible (BAMF_VIEW (self), visible);
  if ((running == TRUE) || close_when_empty)
    bamf_view_set_running      (BAMF_VIEW (self), running);
  bamf_view_set_active       (BAMF_VIEW (self), active);
}

static void
view_active_changed (BamfView *view, gboolean active, BamfApplication *self)
{
  bamf_application_ensure_flags (self);
}

static void
view_urgent_changed (BamfView *view, gboolean urgent, BamfApplication *self)
{
  bamf_application_ensure_flags (self);
}

static void
view_visible_changed (BamfView *view, gboolean visible, BamfApplication *self)
{
  bamf_application_ensure_flags (self);
}

static void
view_xid_changed (GObject *object, GParamSpec *pspec, gpointer user_data)
{
  BamfApplication *self;
  
  self = (BamfApplication *)user_data;
  bamf_application_ensure_flags (self);
}

static void
view_exported (BamfView *view, BamfApplication *self)
{
  g_signal_emit_by_name (self, "window-added", bamf_view_get_path (view));
}

static void
bamf_application_child_added (BamfView *view, BamfView *child)
{
  BamfApplication *application;

  application = BAMF_APPLICATION (view);

  if (BAMF_IS_WINDOW (child))
    {
      if (bamf_view_is_on_bus (child))
        g_signal_emit_by_name (BAMF_APPLICATION (view), "window-added", bamf_view_get_path (child));
      else
        g_signal_connect (G_OBJECT (child), "exported",
                          (GCallback) view_exported, view);
    }

  g_signal_connect (G_OBJECT (child), "active-changed",
                    (GCallback) view_active_changed, view);
  g_signal_connect (G_OBJECT (child), "urgent-changed",
                    (GCallback) view_urgent_changed, view);
  g_signal_connect (G_OBJECT (child), "user-visible-changed",
                    (GCallback) view_visible_changed, view);

  if (BAMF_IS_TAB (child))
    {
      g_signal_connect (G_OBJECT (child), "notify::xid",
                        (GCallback) view_xid_changed, view);
    }

  bamf_application_ensure_flags (BAMF_APPLICATION (view));

  bamf_application_setup_icon_and_name (application);
}

static char *
bamf_application_favorite_from_list (BamfApplication *self, GList *desktop_list)
{
  BamfMatcher *matcher;
  GList *favs, *l;
  char *result = NULL;
  const char *desktop_class;

  g_return_val_if_fail (BAMF_IS_APPLICATION (self), NULL);

  matcher = bamf_matcher_get_default ();
  favs = bamf_matcher_get_favorites (matcher);

  if (favs)
    {
      for (l = favs; l; l = l->next)
        {
          if (g_list_find_custom (desktop_list, l->data, (GCompareFunc) g_strcmp0))
            {
              desktop_class = bamf_matcher_get_desktop_file_class (matcher, l->data);

              if (!desktop_class || g_strcmp0 (self->priv->wmclass, desktop_class) == 0)
                {
                  result = l->data;
                  break;
                }
            }
        }
    }

  return result;
}

static void
bamf_application_set_desktop_file_from_list (BamfApplication *self, GList *list)
{
  BamfApplicationPrivate *priv;
  GList *l;
  char *desktop_file;

  g_return_if_fail (BAMF_IS_APPLICATION (self));
  g_return_if_fail (list);

  priv = self->priv;

  if (priv->desktop_file_list)
    {
      g_list_free_full (priv->desktop_file_list, g_free);
      priv->desktop_file_list = NULL;
    }

  for (l = list; l; l = l->next)
    priv->desktop_file_list = g_list_prepend (priv->desktop_file_list, g_strdup (l->data));

  priv->desktop_file_list = g_list_reverse (priv->desktop_file_list);

  desktop_file = bamf_application_favorite_from_list (self, priv->desktop_file_list);

  /* items, after reversing them, are in priority order */
  if (!desktop_file)
    desktop_file = list->data;

  bamf_application_set_desktop_file (self, desktop_file);
}

static void
bamf_application_child_removed (BamfView *view, BamfView *child)
{
  BamfApplication *self = BAMF_APPLICATION (view);
  if (BAMF_IS_WINDOW (child))
    {
      if (bamf_view_is_on_bus (child))
        g_signal_emit_by_name (BAMF_APPLICATION (view), "window-removed",
                               bamf_view_get_path (child));
    }

  g_signal_handlers_disconnect_by_func (G_OBJECT (child), view_active_changed, view);
  g_signal_handlers_disconnect_by_func (G_OBJECT (child), view_urgent_changed, view);
  g_signal_handlers_disconnect_by_func (G_OBJECT (child), view_visible_changed, view);

  bamf_application_ensure_flags (self);

  if ((bamf_view_get_children (view) == NULL)  && (bamf_application_get_close_when_empty (self)))
    {
      bamf_view_close (view);
    }
}

static void
matcher_favorites_changed (BamfMatcher *matcher, BamfApplication *self)
{
  char *new_desktop_file = NULL;

  g_return_if_fail (BAMF_IS_APPLICATION (self));
  g_return_if_fail (BAMF_IS_MATCHER (matcher));

  new_desktop_file = bamf_application_favorite_from_list (self, self->priv->desktop_file_list);

  if (new_desktop_file)
    {
      bamf_application_set_desktop_file (self, new_desktop_file);
    }
}

static void
on_window_added (BamfApplication *self, const gchar *win_path, gpointer _not_used)
{
  g_return_if_fail (BAMF_IS_APPLICATION (self));
  g_signal_emit_by_name (self->priv->dbus_iface, "window-added", win_path);
}

static void
on_window_removed (BamfApplication *self, const gchar *win_path, gpointer _not_used)
{
  g_return_if_fail (BAMF_IS_APPLICATION (self));
  g_signal_emit_by_name (self->priv->dbus_iface, "window-removed", win_path);
}

static gboolean
on_dbus_handle_show_stubs (BamfDBusItemApplication *interface,
                           GDBusMethodInvocation *invocation,
                           BamfApplication *self)
{
  gboolean show_stubs = bamf_application_get_show_stubs (self);
  g_dbus_method_invocation_return_value (invocation,
                                         g_variant_new ("(b)", show_stubs));

  return TRUE;
}

static gboolean
on_dbus_handle_xids (BamfDBusItemApplication *interface,
                     GDBusMethodInvocation *invocation,
                     BamfApplication *self)
{
  GVariant *xids = bamf_application_get_xids (self);
  g_dbus_method_invocation_return_value (invocation, xids);

  return TRUE;
}

static gboolean
on_dbus_handle_focusable_child (BamfDBusItemApplication *interface,
                           GDBusMethodInvocation *invocation,
                           BamfApplication *self)
{
  GVariant *out_variant;
  BamfView *focusable_child;

  out_variant = NULL;

  focusable_child = bamf_application_get_focusable_child (self);

  if (focusable_child == NULL)
    {
      out_variant = g_variant_new("(s)", "");
    }
  else
    {
      const gchar *path;

      path = bamf_view_get_path (BAMF_VIEW (focusable_child));

      out_variant = g_variant_new("(s)", path);
    }

  g_dbus_method_invocation_return_value (invocation, out_variant);

  return TRUE;
}

static gboolean
on_dbus_handle_desktop_file (BamfDBusItemApplication *interface,
                             GDBusMethodInvocation *invocation,
                             BamfApplication *self)
{
  const char *desktop_file = self->priv->desktop_file ? self->priv->desktop_file : "";
  g_dbus_method_invocation_return_value (invocation,
                                         g_variant_new ("(s)", desktop_file));

  return TRUE;
}

static gboolean
on_dbus_handle_supported_mime_types (BamfDBusItemApplication *interface,
                                     GDBusMethodInvocation *invocation,
                                     BamfApplication *self)
{
  GVariant *list;
  GVariant *value;

  gchar **mimes = bamf_application_get_supported_mime_types (self);

  if (mimes)
    {
      list = g_variant_new_strv ((const gchar**) mimes, -1);
      g_strfreev (mimes);
    }
  else
    {
      const gchar **empty = { NULL };
      list = g_variant_new_strv (empty, 0);
    }

  value = g_variant_new ("(@as)", list);
  g_dbus_method_invocation_return_value (invocation, value);

  return TRUE;
}

static gboolean
on_dbus_handle_application_menu (BamfDBusItemApplication *interface,
                                 GDBusMethodInvocation *invocation,
                                 BamfApplication *self)
{
  gchar *name, *path;

  bamf_application_get_application_menu (self, &name, &path);

  name = name ? name : "";
  path = path ? path : "";

  g_dbus_method_invocation_return_value (invocation,
                                         g_variant_new ("(ss)", name, path));

  return TRUE;
}

static gboolean
on_dbus_handle_application_type (BamfDBusItemApplication *interface,
                                 GDBusMethodInvocation *invocation,
                                 BamfApplication *self)
{
  const char *type = self->priv->app_type ? self->priv->app_type : "";
  g_dbus_method_invocation_return_value (invocation,
                                         g_variant_new ("(s)", type));

  return TRUE;
}

static void
bamf_application_dispose (GObject *object)
{
  BamfApplication *app;
  BamfApplicationPrivate *priv;

  app = BAMF_APPLICATION (object);
  priv = app->priv;

  if (priv->desktop_file)
    {
      g_free (priv->desktop_file);
      priv->desktop_file = NULL;
    }

  if (priv->desktop_file_list)
    {
      g_list_free_full (priv->desktop_file_list, g_free);
      priv->desktop_file_list = NULL;
    }

  if (priv->app_type)
    {
      g_free (priv->app_type);
      priv->app_type = NULL;
    }

  if (priv->icon)
    {
      g_free (priv->icon);
      priv->icon = NULL;
    }

  if (priv->wmclass)
    {
      g_free (priv->wmclass);
      priv->wmclass = NULL;
    }

  g_strfreev (priv->mimes);
  priv->mimes = NULL;

  g_signal_handlers_disconnect_by_func (G_OBJECT (bamf_matcher_get_default ()),
                                        matcher_favorites_changed, object);

  G_OBJECT_CLASS (bamf_application_parent_class)->dispose (object);
}

static void
bamf_application_finalize (GObject *object)
{
  BamfApplication *self;
  self = BAMF_APPLICATION (object);

  g_object_unref (self->priv->dbus_iface);

  G_OBJECT_CLASS (bamf_application_parent_class)->finalize (object);
}

static void
bamf_application_init (BamfApplication * self)
{
  BamfApplicationPrivate *priv;
  priv = self->priv = BAMF_APPLICATION_GET_PRIVATE (self);

  priv->is_tab_container = FALSE;
  priv->app_type = g_strdup ("system");
  priv->show_stubs = TRUE;
  priv->wmclass = NULL;

  /* Initializing the dbus interface */
  priv->dbus_iface = bamf_dbus_item_application_skeleton_new ();

  /* We need to connect to the object own signals to redirect them to the dbus
   * interface                                                                */
  g_signal_connect (self, "window-added", G_CALLBACK (on_window_added), NULL);
  g_signal_connect (self, "window-removed", G_CALLBACK (on_window_removed), NULL);

  /* Registering signal callbacks to reply to dbus method calls */
  g_signal_connect (priv->dbus_iface, "handle-show-stubs",
                    G_CALLBACK (on_dbus_handle_show_stubs), self);

  g_signal_connect (priv->dbus_iface, "handle-xids",
                    G_CALLBACK (on_dbus_handle_xids), self);

  g_signal_connect (priv->dbus_iface, "handle-focusable-child",
                    G_CALLBACK (on_dbus_handle_focusable_child), self);

  g_signal_connect (priv->dbus_iface, "handle-desktop-file",
                    G_CALLBACK (on_dbus_handle_desktop_file), self);

  g_signal_connect (priv->dbus_iface, "handle-supported-mime-types",
                    G_CALLBACK (on_dbus_handle_supported_mime_types), self);

  g_signal_connect (priv->dbus_iface, "handle-application-menu",
                    G_CALLBACK (on_dbus_handle_application_menu), self);

  g_signal_connect (priv->dbus_iface, "handle-application-type",
                    G_CALLBACK (on_dbus_handle_application_type), self);

  /* Setting the interface for the dbus object */
  bamf_dbus_item_object_skeleton_set_application (BAMF_DBUS_ITEM_OBJECT_SKELETON (self),
                                                  priv->dbus_iface);

  g_signal_connect (G_OBJECT (bamf_matcher_get_default ()), "favorites-changed",
                    (GCallback) matcher_favorites_changed, self);
}

static void
bamf_application_dbus_application_iface_init (BamfDBusItemApplicationIface *iface)
{
}

static void
bamf_application_class_init (BamfApplicationClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  BamfViewClass *view_class = BAMF_VIEW_CLASS (klass);

  object_class->dispose = bamf_application_dispose;
  object_class->finalize = bamf_application_finalize;

  view_class->view_type = bamf_application_get_view_type;
  view_class->child_added = bamf_application_child_added;
  view_class->child_removed = bamf_application_child_removed;
  view_class->get_icon = bamf_application_get_icon;
  view_class->stable_bus_name = bamf_application_get_stable_bus_name;

  klass->get_supported_mime_types = bamf_application_default_get_supported_mime_types;
  klass->get_close_when_empty = bamf_application_default_get_close_when_empty;
  klass->supported_mimes_changed = bamf_application_supported_mime_types_changed;

  g_type_class_add_private (klass, sizeof (BamfApplicationPrivate));

  application_signals[SUPPORTED_MIMES_CHANGED] =
    g_signal_new ("supported-mimes-changed",
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (BamfApplicationClass, supported_mimes_changed),
                  NULL, NULL,
                  g_cclosure_marshal_generic,
                  G_TYPE_NONE, 1,
                  G_TYPE_STRV);
}

BamfApplication *
bamf_application_new (void)
{
  BamfApplication *application;
  application = (BamfApplication *) g_object_new (BAMF_TYPE_APPLICATION, NULL);

  return application;
}

BamfApplication *
bamf_application_new_from_desktop_file (const char * desktop_file)
{
  BamfApplication *application;
  application = (BamfApplication *) g_object_new (BAMF_TYPE_APPLICATION, NULL);

  bamf_application_set_desktop_file (application, desktop_file);

  return application;
}

BamfApplication *
bamf_application_new_from_desktop_files (GList *desktop_files)
{
  BamfApplication *application;
  application = (BamfApplication *) g_object_new (BAMF_TYPE_APPLICATION, NULL);

  bamf_application_set_desktop_file_from_list (application, desktop_files);

  return application;
}

BamfApplication *
bamf_application_new_with_wmclass (const char *wmclass)
{
  BamfApplication *application;
  application = (BamfApplication *) g_object_new (BAMF_TYPE_APPLICATION, NULL);

  bamf_application_set_wmclass (application, wmclass);

  return application;
}

/**
    bamf_application_get_show_stubs:
    @application: Application to check for menu stubs

    Checks to see if the application should show menu stubs or not.
    This is specified with the "X-Ayatana-Appmenu-Show-Stubs" desktop
    file key.

    Return Value: Defaults to TRUE, else FALSE if specified in
      .desktop file.
*/
gboolean
bamf_application_get_show_stubs (BamfApplication *application)
{
    g_return_val_if_fail(BAMF_IS_APPLICATION(application), TRUE);
    return application->priv->show_stubs;
}


gboolean
bamf_application_get_close_when_empty (BamfApplication *application)
{
  g_return_val_if_fail (BAMF_IS_APPLICATION(application), FALSE);
  
  if (BAMF_APPLICATION_GET_CLASS (application)->get_close_when_empty)
    {
      return BAMF_APPLICATION_GET_CLASS (application)->get_close_when_empty(application);
    }
  return TRUE;
}

void
bamf_application_get_application_menu (BamfApplication *application, gchar **name, gchar **object_path)
{
  g_return_if_fail (BAMF_IS_APPLICATION (application));
  
  if (BAMF_APPLICATION_GET_CLASS (application)->get_application_menu)
    {
      BAMF_APPLICATION_GET_CLASS (application)->get_application_menu (application, name, object_path);
    }
  else
    {
      *name = NULL;
      *object_path = NULL;
    }
}

BamfView *
bamf_application_get_focusable_child (BamfApplication *application)
{
  g_return_val_if_fail (BAMF_IS_APPLICATION (application), NULL);
  
  if (BAMF_APPLICATION_GET_CLASS (application)->get_focusable_child)
    {
      return BAMF_APPLICATION_GET_CLASS (application)->get_focusable_child (application);
    }
  
  return NULL;
}
