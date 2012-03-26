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
#include <gio/gio.h>
#include <glib.h>
#include "../../deadbeef.h"
#include <string.h>

#define MEDIA_KEY_OBJECT_NAME "org.gnome.SettingsDaemon"
#define MEDIA_KEY_OBJECT_PATH "/org/gnome/SettingsDaemon/MediaKeys"
#define MEDIA_KEY_OBJECT_IFACE "org.gnome.SettingsDaemon.MediaKeys"

static DB_functions_t *deadbeef = NULL;
static uintptr_t mk_mutex;
static uintptr_t mk_cond;
static int mk_state = DB_EV_STOP;

static int
on_message (uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2) {
    deadbeef->mutex_lock (mk_mutex);
    switch (id) {
    case DB_EV_PLAY_CURRENT:
    case DB_EV_PLAY_NUM:
    case DB_EV_PLAY_RANDOM:
        mk_state = DB_EV_PLAY_CURRENT;
        break;
    case DB_EV_STOP:
        mk_state = DB_EV_STOP;
        break;
    default:
        break;
    }
    deadbeef->mutex_unlock (mk_mutex);
    return 0;
}

static void
on_media_key_pressed (GDBusProxy *proxy, const gchar *sender, const gchar *signal_name, GVariant *var, gpointer data) {

    const gchar *app, *action;
    g_variant_get(var, "(&s&s)", &app, &action);
    deadbeef->mutex_lock (mk_mutex);
    if (strcmp (action, "Play") == 0) {
        if (mk_state == DB_EV_STOP)
            deadbeef->sendmessage(DB_EV_PLAY_CURRENT, 0, 0, 0);
        else
            deadbeef->sendmessage(DB_EV_TOGGLE_PAUSE, 0, 0, 0);
    }
    else if (strcmp (action, "Stop") == 0) {
        deadbeef->sendmessage(DB_EV_STOP, 0, 0, 0);
    }
    else if (strcmp (action, "Next") == 0) {
        deadbeef->sendmessage(DB_EV_NEXT, 0, 0, 0);
    }
    else if (strcmp (action, "Previous") == 0) {
        deadbeef->sendmessage(DB_EV_PREV, 0, 0, 0);
    }
    deadbeef->mutex_unlock (mk_mutex);
}

static void
gnome_media_key_thread (void *ctx) {
    GError *error;
    GMainLoop *loop;
    GMainContext *context;
    GDBusProxy *proxy;
    GVariant *var = g_variant_new ("(su)", "deadbeef", 0);

    g_type_init ();

    g_thread_init (NULL);

    proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE, NULL, MEDIA_KEY_OBJECT_NAME, MEDIA_KEY_OBJECT_PATH, MEDIA_KEY_OBJECT_IFACE, NULL, &error);

    if(!proxy) {
        g_printerr("Could not create proxy object\n");
    }

    error = NULL;

    g_dbus_proxy_call_sync(proxy, "GrabMediaPlayerKeys", var,
                G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

    g_signal_connect(proxy, "g-signal", G_CALLBACK(on_media_key_pressed), NULL);

    g_printerr("Starting media key listener\n");

    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);
    g_main_loop_unref (loop);
}

static int
gnome_media_key_start (void) {
    mk_mutex = deadbeef->mutex_create_nonrecursive ();
    mk_cond = deadbeef->cond_create ();

    if (mk_mutex == 0 || mk_cond == 0)
        goto failed_cleanup;

    int tid = deadbeef->thread_start(gnome_media_key_thread, NULL);
    if (tid == 0) {
        goto failed_cleanup;
    }

    return 0;

failed_cleanup:

    return -1;
}

static int
gnome_media_key_stop (void) {
    return 0;
}

static DB_misc_t plugin = {
    .plugin.api_vmajor = 1,
    .plugin.api_vminor = 0,
    .plugin.type = DB_PLUGIN_MISC,
    .plugin.version_major = 1,
    .plugin.version_minor = 0,
    .plugin.id = "gnome_media_key",
    .plugin.name = "GNOME Media Keys",
    .plugin.descr = "Handling GNOME media key events",
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
    .plugin.start = gnome_media_key_start,
    .plugin.stop = gnome_media_key_stop,
    .plugin.message = on_message,
};

DB_plugin_t *
gnome_media_key_load (DB_functions_t *ddb) {
    deadbeef = ddb;
    return &plugin.plugin;
}
