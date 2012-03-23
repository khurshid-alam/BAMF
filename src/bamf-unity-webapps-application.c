/*
 * Copyright (C) 2010-2011 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANAPPLICATIONILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: 
 *              Robert Carr <racarr@canonical.com>
 *
 */

#include <stdlib.h>


#include "bamf-unity-webapps-application.h"
#include "bamf-unity-webapps-tab.h"
#include "bamf-matcher.h"

#include <unity-webapps-context.h>

#define BAMF_UNITY_WEBAPPS_APPLICATION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
BAMF_TYPE_UNITY_WEBAPPS_APPLICATION, BamfUnityWebappsApplicationPrivate))

G_DEFINE_TYPE(BamfUnityWebappsApplication, bamf_unity_webapps_application, BAMF_TYPE_APPLICATION);
	
enum
{
  PROP_0,
  PROP_CONTEXT,
};

/*enum
{
  VANISHED,
  LAST_SIGNAL
};

static guint unity_webapps_application_signals[LAST_SIGNAL] = { 0 };*/

struct _BamfUnityWebappsApplicationPrivate
{
  UnityWebappsContext *context;
};


static void
bamf_unity_webapps_application_get_application_menu (BamfApplication *application,
						     gchar **name, 
						     gchar **path)
{
  BamfUnityWebappsApplication *self;
  
  self = (BamfUnityWebappsApplication *)application;
  
  *name = g_strdup (unity_webapps_context_get_context_name (self->priv->context));
  *path = g_strdup ("/com/canonical/Unity/Webapps/Context/ApplicationActions");
}

static void
bamf_unity_webapps_application_get_property (GObject *object, guint property_id, GValue *gvalue, GParamSpec *pspec)
{
  BamfUnityWebappsApplication *self;
  
  self = BAMF_UNITY_WEBAPPS_APPLICATION (object);
  
  switch (property_id)
    {
    case PROP_CONTEXT:
      g_value_set_object (gvalue, self->priv->context);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static BamfUnityWebappsTab *
bamf_unity_webapps_application_find_child_by_interest (BamfUnityWebappsApplication *application,
						       gint interest_id)
{
  GList *children, *walk;
  BamfUnityWebappsTab *child;
  
  children = bamf_view_get_children (BAMF_VIEW (application));
  
  for (walk = children; walk != NULL; walk = walk->next)
    {
      child = BAMF_UNITY_WEBAPPS_TAB (walk->data);
      
      if (interest_id == bamf_unity_webapps_tab_get_interest_id (child))
	{
	  return child;
	}
    }
  
  return NULL;
}

static BamfView *
bamf_unity_webapps_application_get_focus_child (BamfApplication *application)
{
  BamfUnityWebappsApplication *self;
  gint focus_interest;
  
  self = BAMF_UNITY_WEBAPPS_APPLICATION (application);
  
  focus_interest = unity_webapps_context_get_focus_interest (self->priv->context);
  
  if (focus_interest == -1)
    return NULL;

  return (BamfView *)bamf_unity_webapps_application_find_child_by_interest (self, focus_interest);
}


static void
bamf_unity_webapps_application_interest_appeared (UnityWebappsContext *context,
						  gint interest_id,
						  gpointer user_data)
{
  BamfUnityWebappsApplication *self;
  BamfView *interest_view;
  BamfUnityWebappsTab *child;
  
  self = (BamfUnityWebappsApplication *)user_data;

  child = bamf_unity_webapps_application_find_child_by_interest (self, interest_id);
  
  if (child != NULL)
    {
      return;
    }
  
  interest_view = (BamfView *)bamf_unity_webapps_tab_new (context, interest_id);
  
  bamf_view_add_child (BAMF_VIEW (self), interest_view);
  
  // It's possible that the context had become lonely (i.e. no children) but not yet shut down.
  // however, if we gain an interest we are always running and "mapped".
  
  bamf_view_set_running (BAMF_VIEW (self), TRUE);
  bamf_view_set_user_visible (BAMF_VIEW (self), TRUE);
}

static void
bamf_unity_webapps_application_interest_vanished (UnityWebappsContext *context,
						  gint interest_id,
						  gpointer user_data)
{
  BamfUnityWebappsApplication *self;
  BamfUnityWebappsTab *child;
  
  self = (BamfUnityWebappsApplication *)user_data;
  
  child = bamf_unity_webapps_application_find_child_by_interest (self, interest_id);
  
  if (child == NULL)
    {
      return;
    }
  
  bamf_view_remove_child (BAMF_VIEW (self), BAMF_VIEW (child));
}

/* It doesn't make any sense for a BamfUnityWebappsTab to live without it's assosciated context.
 * so when our children are removed, dispose of them. */
static void
bamf_unity_webapps_application_child_removed (BamfView *view, BamfView *child)
{
  // Chain up first before we destroy the object.
  BAMF_VIEW_CLASS (bamf_unity_webapps_application_parent_class)->child_removed (view, child);

  bamf_view_set_running (child, FALSE);
  bamf_view_close (BAMF_VIEW (child));
  g_object_unref (BAMF_VIEW (child));
}

/* 
 * As soon as we have a tab, we wan't to export it on the bus. We want to make sure tabs are registered with 
 * the matcher, after the application is added, so we do the registration here rather than
 * inside BamfUnityWebappsTab 
 */
static void
bamf_unity_webapps_application_child_added (BamfView *view, BamfView *child)
{
  bamf_matcher_register_view_stealing_ref (bamf_matcher_get_default (), child);
  
  BAMF_VIEW_CLASS (bamf_unity_webapps_application_parent_class)->child_added (view, child);
}

static void
bamf_unity_webapps_application_add_existing_interests (BamfUnityWebappsApplication *self)
{
  GVariant *interests, *interest_variant;
  GVariantIter *variant_iter;
  
  interests = unity_webapps_context_list_interests (self->priv->context);
  
  if (interests == NULL)
    {
      return;
    }
  
  variant_iter = g_variant_iter_new (interests);
  
  while ((interest_variant = g_variant_iter_next_value (variant_iter)))
    {
      gint interest_id;
      
      interest_id = g_variant_get_int32 (interest_variant);
      
      bamf_unity_webapps_application_interest_appeared (self->priv->context, interest_id, self);
    }
}

static void
bamf_unity_webapps_application_context_set (BamfUnityWebappsApplication *self)
{
  gchar *desktop_file = g_strdup_printf("%s/.local/share/applications/%s", g_get_home_dir (), unity_webapps_context_get_desktop_name (self->priv->context));
  gchar *wmclass = g_strdup_printf("unity-webapps-%p", self);
  
  bamf_application_set_desktop_file (BAMF_APPLICATION (self), desktop_file);

  // TODO: Currently we just put something unique here so the matcher wont get confused within the span of a single run. It's a little
  // meaningless however...is there something useful we can put here? If not, how can we prevent the matcher from becoming confused
  // when nothing is here.
  bamf_application_set_wmclass (BAMF_APPLICATION (self), wmclass);

  // Sometimes we might have no children for a short period (for example, the page is reloading), in the case
  // Unity Webapps will keep the context alive for a while. Allowing for new children to appear...before eventually
  // shutting it down. So we use this flag to ensure BAMF will not shut us down prematurely.
  bamf_application_set_close_when_empty (BAMF_APPLICATION (self), FALSE);
  
  bamf_matcher_register_view_stealing_ref (bamf_matcher_get_default (), BAMF_VIEW (self));
  
  bamf_unity_webapps_application_add_existing_interests (self);
  
  unity_webapps_context_on_interest_appeared (self->priv->context, bamf_unity_webapps_application_interest_appeared, self);
  unity_webapps_context_on_interest_vanished (self->priv->context, bamf_unity_webapps_application_interest_vanished, self);
  
  g_free (wmclass);  
  g_free (desktop_file);
}

static void
bamf_unity_webapps_application_set_property (GObject *object, guint property_id, const GValue *gvalue, GParamSpec *pspec)
{
  BamfUnityWebappsApplication *self;
  
  self = BAMF_UNITY_WEBAPPS_APPLICATION (object);
  
  switch (property_id)
    {
    case PROP_CONTEXT:
      g_assert (self->priv->context == NULL);
      self->priv->context = g_value_get_object (gvalue);
      
      bamf_unity_webapps_application_context_set (self);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static gchar *
bamf_unity_webapps_application_get_stable_bus_name (BamfView *view)
{
  return g_strdup_printf ("application%i", abs (g_str_hash (bamf_application_get_desktop_file (BAMF_APPLICATION (view)))));
}


static void
bamf_unity_webapps_application_dispose (GObject *object)
{
  //  BamfUnityWebappsApplication *self = BAMF_UNITY_WEBAPPS_APPLICATION (object);
  
    
  G_OBJECT_CLASS (bamf_unity_webapps_application_parent_class)->dispose (object);
}

static void
bamf_unity_webapps_application_finalize (GObject *object)
{
  BamfUnityWebappsApplication *self = BAMF_UNITY_WEBAPPS_APPLICATION (object);
  
  g_object_unref (self->priv->context);
  
  G_OBJECT_CLASS (bamf_unity_webapps_application_parent_class)->finalize (object);
}

static void
bamf_unity_webapps_application_init (BamfUnityWebappsApplication *self)
{
  self->priv = BAMF_UNITY_WEBAPPS_APPLICATION_GET_PRIVATE (self);
  
  bamf_application_set_application_type (BAMF_APPLICATION (self), "webapp");
  
}

static void
bamf_unity_webapps_application_class_init (BamfUnityWebappsApplicationClass * klass)
{
  GParamSpec *pspec;
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  BamfApplicationClass *bamf_application_class = BAMF_APPLICATION_CLASS (klass);
  BamfViewClass *bamf_view_class = BAMF_VIEW_CLASS (klass);
  
  object_class->get_property = bamf_unity_webapps_application_get_property;
  object_class->set_property = bamf_unity_webapps_application_set_property;
  object_class->dispose = bamf_unity_webapps_application_dispose;
  object_class->finalize = bamf_unity_webapps_application_finalize;
  
  bamf_view_class->stable_bus_name = bamf_unity_webapps_application_get_stable_bus_name;
  bamf_view_class->child_removed = bamf_unity_webapps_application_child_removed;
  bamf_view_class->child_added = bamf_unity_webapps_application_child_added;
  
  bamf_application_class->get_application_menu = bamf_unity_webapps_application_get_application_menu;
  bamf_application_class->get_focus_child = bamf_unity_webapps_application_get_focus_child;
  
  pspec = g_param_spec_object("context", "Context", "The Unity Webapps Context assosciated with the Application",
			      UNITY_WEBAPPS_TYPE_CONTEXT, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
  g_object_class_install_property (object_class, PROP_CONTEXT, pspec);
  
  
  g_type_class_add_private (klass, sizeof (BamfUnityWebappsApplicationPrivate));
}


BamfApplication *
bamf_unity_webapps_application_new (UnityWebappsContext *context)
{
  return (BamfApplication *)g_object_new (BAMF_TYPE_UNITY_WEBAPPS_APPLICATION, "context", context, NULL);
}

UnityWebappsContext *
bamf_unity_webapps_application_get_context (BamfUnityWebappsApplication *application)
{
  return application->priv->context;
}