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
#include "../../deadbeef.h"

static int
media_key_start (void) {
    return 0;
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
    return &plugin.plugin;
}
