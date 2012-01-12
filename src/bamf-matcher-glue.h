/* Generated by dbus-binding-tool; do not edit! */


#ifndef __dbus_glib_marshal_bamf_matcher_MARSHAL_H__
#define __dbus_glib_marshal_bamf_matcher_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

#ifdef G_ENABLE_DEBUG
#define g_marshal_value_peek_boolean(v)  g_value_get_boolean (v)
#define g_marshal_value_peek_char(v)     g_value_get_schar (v)
#define g_marshal_value_peek_uchar(v)    g_value_get_uchar (v)
#define g_marshal_value_peek_int(v)      g_value_get_int (v)
#define g_marshal_value_peek_uint(v)     g_value_get_uint (v)
#define g_marshal_value_peek_long(v)     g_value_get_long (v)
#define g_marshal_value_peek_ulong(v)    g_value_get_ulong (v)
#define g_marshal_value_peek_int64(v)    g_value_get_int64 (v)
#define g_marshal_value_peek_uint64(v)   g_value_get_uint64 (v)
#define g_marshal_value_peek_enum(v)     g_value_get_enum (v)
#define g_marshal_value_peek_flags(v)    g_value_get_flags (v)
#define g_marshal_value_peek_float(v)    g_value_get_float (v)
#define g_marshal_value_peek_double(v)   g_value_get_double (v)
#define g_marshal_value_peek_string(v)   (char*) g_value_get_string (v)
#define g_marshal_value_peek_param(v)    g_value_get_param (v)
#define g_marshal_value_peek_boxed(v)    g_value_get_boxed (v)
#define g_marshal_value_peek_pointer(v)  g_value_get_pointer (v)
#define g_marshal_value_peek_object(v)   g_value_get_object (v)
#define g_marshal_value_peek_variant(v)  g_value_get_variant (v)
#else /* !G_ENABLE_DEBUG */
/* WARNING: This code accesses GValues directly, which is UNSUPPORTED API.
 *          Do not access GValues directly in your code. Instead, use the
 *          g_value_get_*() functions
 */
#define g_marshal_value_peek_boolean(v)  (v)->data[0].v_int
#define g_marshal_value_peek_char(v)     (v)->data[0].v_int
#define g_marshal_value_peek_uchar(v)    (v)->data[0].v_uint
#define g_marshal_value_peek_int(v)      (v)->data[0].v_int
#define g_marshal_value_peek_uint(v)     (v)->data[0].v_uint
#define g_marshal_value_peek_long(v)     (v)->data[0].v_long
#define g_marshal_value_peek_ulong(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_int64(v)    (v)->data[0].v_int64
#define g_marshal_value_peek_uint64(v)   (v)->data[0].v_uint64
#define g_marshal_value_peek_enum(v)     (v)->data[0].v_long
#define g_marshal_value_peek_flags(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_float(v)    (v)->data[0].v_float
#define g_marshal_value_peek_double(v)   (v)->data[0].v_double
#define g_marshal_value_peek_string(v)   (v)->data[0].v_pointer
#define g_marshal_value_peek_param(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_boxed(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_pointer(v)  (v)->data[0].v_pointer
#define g_marshal_value_peek_object(v)   (v)->data[0].v_pointer
#define g_marshal_value_peek_variant(v)  (v)->data[0].v_pointer
#endif /* !G_ENABLE_DEBUG */


/* BOXED:STRING */
extern void dbus_glib_marshal_bamf_matcher_BOXED__STRING (GClosure     *closure,
                                                          GValue       *return_value,
                                                          guint         n_param_values,
                                                          const GValue *param_values,
                                                          gpointer      invocation_hint,
                                                          gpointer      marshal_data);
void
dbus_glib_marshal_bamf_matcher_BOXED__STRING (GClosure     *closure,
                                              GValue       *return_value G_GNUC_UNUSED,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint G_GNUC_UNUSED,
                                              gpointer      marshal_data)
{
  typedef gpointer (*GMarshalFunc_BOXED__STRING) (gpointer     data1,
                                                  gpointer     arg_1,
                                                  gpointer     data2);
  register GMarshalFunc_BOXED__STRING callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gpointer v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 2);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOXED__STRING) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       data2);

  g_value_take_boxed (return_value, v_return);
}

/* STRING:UINT */
extern void dbus_glib_marshal_bamf_matcher_STRING__UINT (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);
void
dbus_glib_marshal_bamf_matcher_STRING__UINT (GClosure     *closure,
                                             GValue       *return_value G_GNUC_UNUSED,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint G_GNUC_UNUSED,
                                             gpointer      marshal_data)
{
  typedef gchar* (*GMarshalFunc_STRING__UINT) (gpointer     data1,
                                               guint        arg_1,
                                               gpointer     data2);
  register GMarshalFunc_STRING__UINT callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gchar* v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 2);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_STRING__UINT) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_uint (param_values + 1),
                       data2);

  g_value_take_string (return_value, v_return);
}

/* BOOLEAN:STRING */
extern void dbus_glib_marshal_bamf_matcher_BOOLEAN__STRING (GClosure     *closure,
                                                            GValue       *return_value,
                                                            guint         n_param_values,
                                                            const GValue *param_values,
                                                            gpointer      invocation_hint,
                                                            gpointer      marshal_data);
void
dbus_glib_marshal_bamf_matcher_BOOLEAN__STRING (GClosure     *closure,
                                                GValue       *return_value G_GNUC_UNUSED,
                                                guint         n_param_values,
                                                const GValue *param_values,
                                                gpointer      invocation_hint G_GNUC_UNUSED,
                                                gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING) (gpointer     data1,
                                                    gpointer     arg_1,
                                                    gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 2);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* STRING:NONE */
extern void dbus_glib_marshal_bamf_matcher_STRING__VOID (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);
void
dbus_glib_marshal_bamf_matcher_STRING__VOID (GClosure     *closure,
                                             GValue       *return_value G_GNUC_UNUSED,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint G_GNUC_UNUSED,
                                             gpointer      marshal_data)
{
  typedef gchar* (*GMarshalFunc_STRING__VOID) (gpointer     data1,
                                               gpointer     data2);
  register GMarshalFunc_STRING__VOID callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gchar* v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 1);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_STRING__VOID) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       data2);

  g_value_take_string (return_value, v_return);
}
#define dbus_glib_marshal_bamf_matcher_STRING__NONE	dbus_glib_marshal_bamf_matcher_STRING__VOID

/* BOXED:NONE */
extern void dbus_glib_marshal_bamf_matcher_BOXED__VOID (GClosure     *closure,
                                                        GValue       *return_value,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint,
                                                        gpointer      marshal_data);
void
dbus_glib_marshal_bamf_matcher_BOXED__VOID (GClosure     *closure,
                                            GValue       *return_value G_GNUC_UNUSED,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint G_GNUC_UNUSED,
                                            gpointer      marshal_data)
{
  typedef gpointer (*GMarshalFunc_BOXED__VOID) (gpointer     data1,
                                                gpointer     data2);
  register GMarshalFunc_BOXED__VOID callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gpointer v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 1);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOXED__VOID) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       data2);

  g_value_take_boxed (return_value, v_return);
}
#define dbus_glib_marshal_bamf_matcher_BOXED__NONE	dbus_glib_marshal_bamf_matcher_BOXED__VOID

/* STRING:STRING */
extern void dbus_glib_marshal_bamf_matcher_STRING__STRING (GClosure     *closure,
                                                           GValue       *return_value,
                                                           guint         n_param_values,
                                                           const GValue *param_values,
                                                           gpointer      invocation_hint,
                                                           gpointer      marshal_data);
void
dbus_glib_marshal_bamf_matcher_STRING__STRING (GClosure     *closure,
                                               GValue       *return_value G_GNUC_UNUSED,
                                               guint         n_param_values,
                                               const GValue *param_values,
                                               gpointer      invocation_hint G_GNUC_UNUSED,
                                               gpointer      marshal_data)
{
  typedef gchar* (*GMarshalFunc_STRING__STRING) (gpointer     data1,
                                                 gpointer     arg_1,
                                                 gpointer     data2);
  register GMarshalFunc_STRING__STRING callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gchar* v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 2);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_STRING__STRING) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       data2);

  g_value_take_string (return_value, v_return);
}

/* BOOLEAN:BOXED,POINTER */
extern void dbus_glib_marshal_bamf_matcher_BOOLEAN__BOXED_POINTER (GClosure     *closure,
                                                                   GValue       *return_value,
                                                                   guint         n_param_values,
                                                                   const GValue *param_values,
                                                                   gpointer      invocation_hint,
                                                                   gpointer      marshal_data);
void
dbus_glib_marshal_bamf_matcher_BOOLEAN__BOXED_POINTER (GClosure     *closure,
                                                       GValue       *return_value G_GNUC_UNUSED,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint G_GNUC_UNUSED,
                                                       gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__BOXED_POINTER) (gpointer     data1,
                                                           gpointer     arg_1,
                                                           gpointer     arg_2,
                                                           gpointer     data2);
  register GMarshalFunc_BOOLEAN__BOXED_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__BOXED_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_boxed (param_values + 1),
                       g_marshal_value_peek_pointer (param_values + 2),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

G_END_DECLS

#endif /* __dbus_glib_marshal_bamf_matcher_MARSHAL_H__ */

#include <dbus/dbus-glib.h>
static const DBusGMethodInfo dbus_glib_bamf_matcher_methods[] = {
  { (GCallback) bamf_matcher_get_active_application, dbus_glib_marshal_bamf_matcher_STRING__NONE, 0 },
  { (GCallback) bamf_matcher_get_active_window, dbus_glib_marshal_bamf_matcher_STRING__NONE, 66 },
  { (GCallback) bamf_matcher_application_for_xid, dbus_glib_marshal_bamf_matcher_STRING__UINT, 122 },
  { (GCallback) bamf_matcher_application_is_running, dbus_glib_marshal_bamf_matcher_BOOLEAN__STRING, 196 },
  { (GCallback) bamf_matcher_application_dbus_paths, dbus_glib_marshal_bamf_matcher_BOXED__NONE, 277 },
  { (GCallback) bamf_matcher_window_dbus_paths, dbus_glib_marshal_bamf_matcher_BOXED__NONE, 337 },
  { (GCallback) bamf_matcher_dbus_path_for_application, dbus_glib_marshal_bamf_matcher_STRING__STRING, 392 },
  { (GCallback) bamf_matcher_register_favorites, dbus_glib_marshal_bamf_matcher_BOOLEAN__BOXED_POINTER, 468 },
  { (GCallback) bamf_matcher_running_application_paths, dbus_glib_marshal_bamf_matcher_BOXED__NONE, 529 },
  { (GCallback) bamf_matcher_tab_dbus_paths, dbus_glib_marshal_bamf_matcher_BOXED__NONE, 592 },
  { (GCallback) bamf_matcher_xids_for_application, dbus_glib_marshal_bamf_matcher_BOXED__STRING, 644 },
};

const DBusGObjectInfo dbus_glib_bamf_matcher_object_info = {  1,
  dbus_glib_bamf_matcher_methods,
  11,
"org.ayatana.bamf.matcher\0ActiveApplication\0S\0application\0O\0F\0R\0s\0\0org.ayatana.bamf.matcher\0ActiveWindow\0S\0window\0O\0F\0R\0s\0\0org.ayatana.bamf.matcher\0ApplicationForXid\0S\0xid\0I\0u\0application\0O\0F\0R\0s\0\0org.ayatana.bamf.matcher\0ApplicationIsRunning\0S\0application\0I\0s\0running\0O\0F\0R\0b\0\0org.ayatana.bamf.matcher\0ApplicationPaths\0S\0paths\0O\0F\0R\0as\0\0org.ayatana.bamf.matcher\0WindowPaths\0S\0paths\0O\0F\0R\0as\0\0org.ayatana.bamf.matcher\0PathForApplication\0S\0applicaiton\0I\0s\0path\0O\0F\0R\0s\0\0org.ayatana.bamf.matcher\0RegisterFavorites\0S\0favorites\0I\0as\0\0org.ayatana.bamf.matcher\0RunningApplications\0S\0paths\0O\0F\0R\0as\0\0org.ayatana.bamf.matcher\0TabPaths\0S\0paths\0O\0F\0R\0as\0\0org.ayatana.bamf.matcher\0XidsForApplication\0S\0application\0I\0s\0xids\0O\0F\0R\0au\0\0\0",
"org.ayatana.bamf.matcher\0ViewOpened\0org.ayatana.bamf.matcher\0ViewClosed\0org.ayatana.bamf.matcher\0ActiveWindowChanged\0org.ayatana.bamf.matcher\0ActiveApplicationChanged\0\0",
"\0"
};

