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
    COLUMN_WORKFLOW_UINAME,
    COLUMN_WORKFLOW_NAME,
    NUM_COLUMNS
};

static GtkWindow *g_parent_window;
static GHashTable *g_events_options = NULL;

static void create_event_config_dialog_content_cb(event_config_t *ec, gpointer content)
{
    if (ec->options)
    {
        GtkWidget *ev_lbl = gtk_label_new(ec_get_screen_name(ec));
        gtk_box_pack_start(GTK_BOX(content), ev_lbl, false, false, 0);
    }

    config_dialog_t *cdialog = create_event_config_dialog_content(ec, (GtkWidget *)content);
    if (g_events_options == NULL)
    {
        g_events_options = g_hash_table_new_full(
                    /*hash_func*/ g_str_hash,
                    /*key_equal_func:*/ g_str_equal,
                    /*key_destroy_func:*/ g_free,
                    /*value_destroy_func:*/ NULL);
    }
    g_hash_table_insert(g_events_options, ec, cdialog);

}

static void save_event_config_data_foreach(event_config_t *ec,
                                          config_dialog_t *cdialog,
                                          gpointer user_data)
{
    save_data_from_event_config_dialog(cdialog_get_data(cdialog), ec);
}

void save_data_from_worfklow_dialog(gpointer data, /* not needed */ const char *name)
{
    g_hash_table_foreach((GHashTable *)data, (GHFunc)save_event_config_data_foreach, NULL);
}

config_dialog_t *create_workflow_config_dialog(const char *workflow_name, GtkWindow *parent)
{
    INITIALIZE_LIBREPORT();

    workflow_t *workflow = get_workflow(workflow_name);
    GList *events = wf_get_event_list(workflow);

    GtkWindow *parent_window = parent ? parent : g_parent_window;

    char *window_title = xasprintf("%s - Reporting Configuration",
            wf_get_screen_name(workflow) ? wf_get_screen_name(workflow) : workflow_name);

    GtkWidget *dialog = gtk_dialog_new_with_buttons(
                        window_title,
                        parent_window,
                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                        _("_Cancel"),
                        GTK_RESPONSE_CANCEL,
                        _("_OK"),
                        GTK_RESPONSE_APPLY,
                        NULL);

    free(window_title);

    gtk_window_set_resizable(GTK_WINDOW(dialog), true);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 450, 450);

    if (parent_window != NULL)
    {
        gtk_window_set_icon_name(GTK_WINDOW(dialog),
        gtk_window_get_icon_name(parent_window));
    }

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

#if ((GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION < 7) || (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 7 && GTK_MICRO_VERSION < 8))
    /* http://developer.gnome.org/gtk3/unstable/GtkScrolledWindow.html#gtk-scrolled-window-add-with-viewport */
    /* gtk_scrolled_window_add_with_viewport has been deprecated since version 3.8 and should not be used in newly-written code. */
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), content);
#else
    /* gtk_container_add() will now automatically add a GtkViewport if the child doesn't implement GtkScrollable. */
    gtk_container_add(GTK_CONTAINER(scrolled), content);
#endif

    GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_box_pack_start(GTK_BOX(dialog_box), scrolled, false, true, 0);

    g_events_options = NULL;
    g_list_foreach(events, (GFunc)create_event_config_dialog_content_cb, content);

    gtk_widget_show_all(GTK_WIDGET(scrolled));

    config_dialog_t *cdialog = new_config_dialog(dialog,
                                g_events_options,
                                (config_save_fun_t)save_data_from_worfklow_dialog);
    return cdialog;
}

static void add_workflow_to_liststore(gpointer key, gpointer value, gpointer user_data)
{
    config_item_info_t *info = workflow_get_config_info((workflow_t *)value);
    config_dialog_t *cdialog = create_workflow_config_dialog(key, g_parent_window);
    add_item_to_config_liststore(cdialog, info, user_data);
}

GtkListStore *add_workflows_to_liststore(GHashTable *workflows)
{
    GtkListStore *list_store = new_conf_liststore();
    g_hash_table_foreach(workflows, (GHFunc)add_workflow_to_liststore, list_store);

    return list_store;
}

static void load_single_event_config_foreach(event_config_t *ec, gpointer user_data)
{
    load_single_event_config_data_from_user_storage(ec);
}

static void load_events_foreach_workflow(const char *name, workflow_t *workflow, gpointer user_data)
{
    g_list_foreach(wf_get_event_list(workflow), (GFunc)load_single_event_config_foreach, NULL);
}

void load_workflow_config_data_from_user_storage(GHashTable *workflows)
{
    g_hash_table_foreach(workflows, (GHFunc)load_events_foreach_workflow, NULL);
}
