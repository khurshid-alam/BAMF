//  
//  Copyright (C) 2009 Canonical Ltd.
// 
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 

#include "bamfdbus.h"
#include "windowmatcher.h"
#include "bamfdbus-glue.h"

G_DEFINE_TYPE (BamfDBus, bamf_dbus, G_TYPE_OBJECT);

BamfMatcher *matcher;

static void
bamf_dbus_init (BamfDBus * self)
{
  /* initialize all public and private members to reasonable default values. */

  matcher = bamf_matcher_new ();
}

static void
bamf_dbus_class_init (BamfDBusClass * klass)
{
  dbus_g_object_type_install_info (BAMF_TYPE_DBUS,
				   &dbus_glib_bamf_dbus_object_info);
}

/**
 * bamf_dbus_new:
 *
 * Returns: a new BamfDBus GObject.
 **/
BamfDBus *
bamf_dbus_new (void)
{
  DBusGConnection *bus;
  DBusGProxy *bus_proxy;
  GError *error = NULL;
  guint request_name_result;
  BamfDBus *obj;
  
  obj = (BamfDBus *) g_object_new (BAMF_TYPE_DBUS, NULL);

  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  g_return_val_if_fail (bus, NULL);

  bus_proxy =
    dbus_g_proxy_new_for_name (bus, "org.freedesktop.DBus",
			       "/org/freedesktop/DBus",
			       "org.freedesktop.DBus");

  if (!dbus_g_proxy_call (bus_proxy, "RequestName", &error,
			  G_TYPE_STRING, BAMF_DBUS_SERVICE,
			  G_TYPE_UINT, 0,
			  G_TYPE_INVALID,
			  G_TYPE_UINT, &request_name_result, G_TYPE_INVALID))
    return NULL;

  dbus_g_connection_register_g_object (bus, BAMF_DBUS_PATH,
				       G_OBJECT (obj));

  return obj;
}

gboolean
bamf_dbus_window_match_is_ready (BamfDBus * dbus, guint32 xid)
{
  WnckWindow *window;
  
  window = wnck_window_get (xid);
  
  if (!window)
    return FALSE;

  return bamf_matcher_window_is_match_ready (matcher, window);
}

gboolean
bamf_dbus_desktop_file_for_xid (BamfDBus * dbus, guint32 xid,
				    gchar ** filename, GError ** error)
{
  WnckWindow *window;
  GString *desktopFile;
  
  window = wnck_window_get (xid);

  if (window != NULL)
    {
      desktopFile = bamf_matcher_desktop_file_for_window (matcher, window);
      if (desktopFile)
	{
	  *filename = g_strdup (desktopFile->str);
	  g_string_free (desktopFile, TRUE);
	}
      else
	{
	  *filename = g_strdup ("");
	}
    }
  else
    {
      // this should basically never happen, but lets deal with it.
      *filename = g_strdup ("");
    }
  return TRUE;
}

gboolean
bamf_dbus_xids_for_desktop_file (BamfDBus * dbus, gchar * filename,
				     GArray ** xids, GError ** error)
{
  GString *desktopFile;
  GArray *arr;
  WnckWindow *window;
  guint32 xid;
  
  desktopFile = g_string_new (filename);
  arr = bamf_matcher_window_list_for_desktop_file (matcher, desktopFile);

  *xids = g_array_new (FALSE, TRUE, sizeof (guint32));

  int i;
  for (i = 0; i < arr->len; i++)
    {
      window = g_array_index (arr, WnckWindow *, i);
      xid = (guint32) wnck_window_get_xid (window);
      g_array_append_val (*xids, xid);
    }

  g_array_free (arr, TRUE);
  g_string_free (desktopFile, TRUE);
  
  return TRUE;
}

gboolean
bamf_dbus_register_desktop_file_for_pid (BamfDBus * dbus,
					     gchar * filename, gint pid,
					     GError ** error)
{
  GString *file;
  
  file = g_string_new (filename);
  bamf_matcher_register_desktop_file_for_pid (matcher, file, pid);
  g_string_free (file, TRUE);

  return TRUE;
}