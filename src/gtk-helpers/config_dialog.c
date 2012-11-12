/*
    Copyright (C) 2011  ABRT Team
    Copyright (C) 2011  RedHat inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <gtk/gtk.h>
#include "internal_libreport_gtk.h"

enum
{
    COLUMN_UINAME,
    COLUMN_NAME,
    NUM_COLUMNS
};

void add_item_to_config_liststore(gpointer key, gpointer value, gpointer user_data)
{
    GtkListStore *list_store = (GtkListStore *)user_data;
    config_item_info_t *info = (config_item_info_t *)value;

    g_print("adding %s to workflows\n", info->screen_name);
    char *label;
    if (info->screen_name != NULL && info->description != NULL)
        label = xasprintf("<b>%s</b>\n%s", info->screen_name, info->description);
    else
        //if event has no xml description
        label = xasprintf("<b>%s</b>\nNo description available", key);

    GtkTreeIter iter;
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set(list_store, &iter,
                      COLUMN_UINAME, label,
                      COLUMN_NAME, key,
                      -1);
    free(label);
}
