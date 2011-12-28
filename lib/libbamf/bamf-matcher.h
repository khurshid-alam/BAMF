/*
 * Copyright 2010 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of either or both of the following licenses:
 *
 * 1) the GNU Lesser General Public License version 3, as published by the
 * Free Software Foundation; and/or
 * 2) the GNU Lesser General Public License version 2.1, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 and version 2.1 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jason Smith <jason.smith@canonical.com>
 *              Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */

#ifndef _BAMF_MATCHER_H_
#define _BAMF_MATCHER_H_

#include <glib-object.h>
#include <libbamf/bamf-window.h>
#include <libbamf/bamf-application.h>

G_BEGIN_DECLS

#define BAMF_TYPE_MATCHER (bamf_matcher_get_type ())

#define BAMF_MATCHER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
        BAMF_TYPE_MATCHER, BamfMatcher))

#define BAMF_MATCHER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass),\
        BAMF_TYPE_MATCHER, BamfMatcherClass))

#define BAMF_IS_MATCHER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
        BAMF_TYPE_MATCHER))

#define BAMF_IS_MATCHER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),\
        BAMF_TYPE_MATCHER))

#define BAMF_MATCHER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),\
        BAMF_TYPE_MATCHER, BamfMatcherClass))

#define BAMF_MATCHER_SIGNAL_VIEW_OPENED                "view-opened"
#define BAMF_MATCHER_SIGNAL_VIEW_CLOSED                "view-closed"
#define BAMF_MATCHER_SIGNAL_ACTIVE_APPLICATION_CHANGED "active-application-changed"
#define BAMF_MATCHER_SIGNAL_ACTIVE_WINDOW_CHANGED      "active-window-changed"

typedef struct _BamfMatcher        BamfMatcher;
typedef struct _BamfMatcherClass   BamfMatcherClass;
typedef struct _BamfMatcherPrivate BamfMatcherPrivate;

struct _BamfMatcher
{
  GObject           parent;

  BamfMatcherPrivate *priv;
};

struct _BamfMatcherClass
{
  GObjectClass parent_class;

  /*< private >*/
  void (*_matcher_padding1) (void);
  void (*_matcher_padding2) (void);
  void (*_matcher_padding3) (void);
  void (*_matcher_padding4) (void);
  void (*_matcher_padding5) (void);
  void (*_matcher_padding6) (void);
};

GType             bamf_matcher_get_type                 (void) G_GNUC_CONST;

/**
 * bamf_matcher_get_default:
 * @matcher: a #BamfMatcher
 *
 * Returns the default matcher. This matcher is owned by bamf and shared between other callers.
 *
 * Returns: (transfer none): A new #BamfMatcher
 */
BamfMatcher     * bamf_matcher_get_default              (void);

/**
 * bamf_matcher_get_active_application:
 * @matcher: a #BamfMatcher
 *
 * Used to fetch the active #BamfApplication.
 *
 * Returns: (transfer none): The active #BamfApplication.
 */
BamfApplication * bamf_matcher_get_active_application   (BamfMatcher *matcher);

/**
 * bamf_matcher_get_active_window:
 * @matcher: a #BamfMatcher
 *
 * Used to fetch the active #BamfWindow.
 *
 * Returns: (transfer none): The active #BamfWindow.
 */
BamfWindow      * bamf_matcher_get_active_window        (BamfMatcher *matcher);

/**
 * bamf_matcher_get_application_for_xid:
 * @matcher: a #BamfMatcher
 * @xid: The XID to search for
 *
 * Used to fetch the #BamfApplication containing the passed xid.
 *
 * Returns: (transfer container): The #BamfApplication representing the xid passed, or NULL if none exists.
 */
BamfApplication * bamf_matcher_get_application_for_xid  (BamfMatcher *matcher,
                                                         guint32      xid);
/**
 * bamf_matcher_get_application_for_window:
 * @matcher: a #BamfMatcher
 * @window: The window to look for
 *
 * Used to fetch the #BamfApplication containing the passed window.
 *
 * Returns: (transfer container): The #BamfApplication representing the xid passed, or NULL if none exists.
 */
BamfApplication * bamf_matcher_get_application_for_window  (BamfMatcher *matcher,
                                                            BamfWindow *window);

gboolean          bamf_matcher_application_is_running   (BamfMatcher *matcher,
                                                         const gchar *desktop_file);

/**
 * bamf_matcher_get_applications:
 * @matcher: a #BamfMatcher
 *
 * Used to fetch all #BamfApplication's running or not. Application authors who wish to only 
 * see running applications should use bamf_matcher_get_running_applications instead. The reason
 * this method is needed is bamf will occasionally track applications which are not currently
 * running for nefarious purposes.
 *
 * Returns: (element-type Bamf.Application) (transfer container): A list of #BamfApplication's.
 */
GList *           bamf_matcher_get_applications         (BamfMatcher *matcher);

/**
 * bamf_matcher_register_favorites:
 * @matcher: a #BamfMatcher
 * @favorites: an array of strings, each containing an absolute path to a .desktop file
 *
 * Used to effect how bamf performs matching. Desktop files passed to this method will
 * be prefered by bamf to system desktop files.
 */
void              bamf_matcher_register_favorites       (BamfMatcher *matcher,
                                                         const gchar **favorites);

/**
 * bamf_matcher_get_running_applications:
 * @matcher: a #BamfMatcher
 *
 * Used to fetch all #BamfApplication's which are running.
 *
 * Returns: (element-type Bamf.Application) (transfer container): A list of #BamfApplication's.
 */
GList *           bamf_matcher_get_running_applications (BamfMatcher *matcher);

/**
 * bamf_matcher_get_tabs:
 * @matcher: a #BamfMatcher
 *
 * Used to fetch all #BamfView's representing tabs. Currently unused.
 *
 * Returns: (element-type Bamf.View) (transfer container): A list of #BamfViews's.
 */
GList *           bamf_matcher_get_tabs                 (BamfMatcher *matcher);

/**
 * bamf_matcher_get_applications:
 * @matcher: a #BamfMatcher
 *
 * Used to fetch all xid's associated with an application.
 * 
 *
 * Returns: (array zero-terminated=1) (element-type guint32) (transfer full): An array of xids.
 */
GArray *          bamf_matcher_get_xids_for_application (BamfMatcher *matcher,
                                                         const gchar *desktop_file);

/**
 * bamf_matcher_get_windows:
 * @matcher: a #BamfMatcher
 *
 * Used to fetch all windows that BAMF knows about.
 *
 * Returns: (element-type Bamf.View) (transfer container): A list of windows.
 */
GList *           bamf_matcher_get_windows (BamfMatcher *matcher);

BamfApplication * bamf_matcher_get_application_for_desktop_file (BamfMatcher *matcher,
                                                                 const gchar *desktop_file_path,
                                                                 gboolean create_if_not_found);

G_END_DECLS

#endif
