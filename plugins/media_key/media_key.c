/*
    DeaDBeeF - ultimate music player for GNU/Linux systems with X11
    Copyright (C) 2011 Thynson <lanxingcan@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <glib.h>
#include "marshal.h"
#include "../../deadbeef.h"
#include <string.h>

#define MEDIA_KEY_OBJECT_NAME "org.gnome.SettingsDaemon"
#define MEDIA_KEY_OBJECT_PATH "/org/gnome/SettingsDaemon/MediaKeys"
#define MEDIA_KEY_OBJECT_IFACE "org.gnome.SettingsDaemon.MediaKeys"

static DB_functions_t *deadbeef = NULL;
static uintptr_t mk_mutex;
static uintptr_t mk_cond;
static int mk_state;


static void
media_key_pressed (DBusGProxy *proxy, const char *value1, const char *value2) {
    fprintf (stderr, "%s\n", value2);
    if (strcmp (value2, "Play") == 0) {
        deadbeef->sendmessage(DB_EV_PLAY_CURRENT, 0, 0, 0);
    }
    else if (strcmp (value2, "Stop") == 0) {
        deadbeef->sendmessage(DB_EV_STOP, 0, 0, 0);
    }
    else if (strcmp (value2, "Next") == 0) {
        deadbeef->sendmessage(DB_EV_NEXT, 0, 0, 0);
    }
    else if (strcmp (value2, "Previous") == 0) {
        deadbeef->sendmessage(DB_EV_PREV, 0, 0, 0);
    }
}

static void
media_key_thread (void *ctx) {
    GError *error;
    GMainLoop *loop;
    DBusGConnection *conn;
    DBusGProxy *proxy;

    g_type_init ();

    //deadbeef->mutex_lock(mk_mutex);
    loop = g_main_loop_new (NULL, FALSE);
    conn = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
    proxy = dbus_g_proxy_new_for_name (conn, MEDIA_KEY_OBJECT_NAME, MEDIA_KEY_OBJECT_PATH, MEDIA_KEY_OBJECT_IFACE);

    if(!proxy) {
        g_printerr("Could not create proxy object\n");
    }

    error = NULL;
    if(!dbus_g_proxy_call(proxy, "GrabMediaPlayerKeys", &error, G_TYPE_STRING, "deadbeef", G_TYPE_UINT, 0, G_TYPE_INVALID, G_TYPE_INVALID)) {
        g_printerr("Could not grab media player keys: %s\n", error->message);
    }

    dbus_g_object_register_marshaller (marshal_VOID__STRING_STRING, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);

    dbus_g_proxy_add_signal(proxy, "MediaPlayerKeyPressed", G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);

    dbus_g_proxy_connect_signal(proxy, "MediaPlayerKeyPressed", G_CALLBACK(media_key_pressed), NULL, NULL);

    g_print("Starting media key listener\n");
    mk_state = 0;
    //deadbeef->cond_signal(mk_cond);
    //deadbeef->mutex_unlock(mk_mutex);
    g_main_loop_run (loop);



on_failed:
    mk_state = -1;
    deadbeef->cond_signal(mk_cond);
    deadbeef->mutex_unlock(mk_mutex);
    return;

}

static int
media_key_start (void) {
    mk_mutex = deadbeef->mutex_create ();
    mk_cond = deadbeef->cond_create ();

    if (mk_mutex == 0 || mk_cond == 0)
        goto failed_cleanup;

    //deadbeef->mutex_lock (mk_mutex);
    int tid = deadbeef->thread_start(media_key_thread, NULL);
    if (tid == 0) {
        deadbeef->mutex_unlock (mk_mutex);
        goto failed_cleanup;
    }

    //deadbeef->cond_wait (mk_cond, mk_mutex);
    //deadbeef->mutex_unlock (mk_mutex);

    if (mk_state == 0)
        return 0;
    /* else failed */

failed_cleanup:

    if (mk_mutex)
        deadbeef->mutex_free(mk_mutex);

    if (mk_cond)
        deadbeef->cond_free(mk_cond);

    if (tid)
        deadbeef->thread_join(tid);

    return -1;
}

static int
media_key_stop (void) {
    return 0;
}

static DB_misc_t plugin = {
    .plugin.api_vmajor = 1,
    .plugin.api_vminor = 0,
    .plugin.type = DB_PLUGIN_MISC,
    .plugin.version_major = 1,
    .plugin.version_minor = 0,
    .plugin.id = "media_key",
    .plugin.name = "Media keys",
    .plugin.descr = "Handling DBus media key events",
    .plugin.copyright =
        "Copyright (C) 2011 Thynson <lanxingcan@gmail.com>\n"
        "\n"
        "This program is free software; you can redistribute it and/or\n"
        "modify it under the terms of the GNU General Public License\n"
        "as published by the Free Software Foundation; either version 2\n"
        "of the License, or (at your option) any later version.\n"
        "\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "GNU General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program; if not, write to the Free Software\n"
        "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\n"
    ,
    .plugin.website = "http://github.com/thynson/deadbeef",
    .plugin.start = media_key_start,
    .plugin.stop = media_key_stop,
};

DB_plugin_t *
media_key_load (DB_functions_t *ddb) {
    deadbeef = ddb;
    return &plugin.plugin;
}
