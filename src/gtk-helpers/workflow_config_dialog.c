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

static void add_item_to_config_liststore_wrap(gpointer key, gpointer value, gpointer user_data)
{
    config_item_info_t *info = workflow_get_config_info((workflow_t *)value);
    add_item_to_config_liststore(key, info, user_data);
}

GtkWidget *create_workflow_config_dialog(const char *workflow_name)
{
    workflow_t *workflow = get_workflow(workflow_name);
    GList *events = workflow->events;

    GtkWidget *dialog = gtk_dialog_new_with_buttons(
                        /*title:*/ workflow->info->screen_name ? workflow->info->screen_name : workflow_name,
                        NULL,
                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_STOCK_CANCEL,
                        GTK_RESPONSE_CANCEL,
                        GTK_STOCK_OK,
                        GTK_RESPONSE_APPLY,
                        NULL);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    //GtkWidget *event_conf_widg = NULL;
    while(events)
    {

        event_config_t *ec = (event_config_t *)events->data;
        create_event_config_dialog_content(ec, content);
        //gtk_box_pack_start(GTK_BOX(content), event_conf_widg, true, true, 0);
        events = g_list_next(events);
    }

    return dialog;
}

GtkWidget *create_workflow_list_dialog()
{
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *workflows_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(workflows_scroll),
                                    GTK_POLICY_NEVER,
                                    GTK_POLICY_AUTOMATIC);
    /* workflow list treeview */
    GtkWidget *workflows_tv = gtk_tree_view_new();
    /* column with workflow name and description */
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    /* add column to tree view */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Workflow"),
                                                 renderer,
                                                 "markup",
                                                 COLUMN_WORKFLOW_UINAME,
                                                 NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    g_object_set(G_OBJECT(renderer), "wrap-mode", PANGO_WRAP_WORD, NULL);
    g_object_set(G_OBJECT(renderer), "wrap-width", 440, NULL);
    gtk_tree_view_column_set_sort_column_id(column, COLUMN_WORKFLOW_NAME);
    gtk_tree_view_append_column(GTK_TREE_VIEW(workflows_tv), column);
    /* "Please draw rows in alternating colors": */
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(workflows_tv), TRUE);
    // TODO: gtk_tree_view_set_headers_visible(FALSE)? We have only one column anyway...

    /* Create data store for the list and attach it
     * COLUMN_WORKFLOW_UINAME -> name+description
     * COLUMN_WORKFLOW_NAME -> workflow name so we can retrieve it from the row
     */
    GtkListStore *workflows_list_store = gtk_list_store_new(NUM_COLUMNS,
                                                G_TYPE_STRING, /* Workflow name + description */
                                                G_TYPE_STRING  /* workflow name */
    );
    gtk_tree_view_set_model(GTK_TREE_VIEW(workflows_tv), GTK_TREE_MODEL(workflows_list_store));

    g_hash_table_foreach(g_workflow_list,
                        &add_item_to_config_liststore_wrap,
                        workflows_list_store);
//TODO: can unref workflows_list_store? treeview holds one ref.

    /* Double click/Enter handler */
    //g_signal_connect(workflows_tv, "row-activated", G_CALLBACK(on_workflow_row_activated_cb), NULL);

    gtk_container_add(GTK_CONTAINER(workflows_scroll), workflows_tv);

    GtkWidget *configure_workflow_btn = gtk_button_new_with_mnemonic(_("Configure E_vent"));
    gtk_widget_set_sensitive(configure_workflow_btn, false);
    //g_signal_connect(configure_workflow_btn, "clicked", G_CALLBACK(on_configure_workflow_cb), workflows_tv);
    //g_signal_connect(workflows_tv, "cursor-changed", G_CALLBACK(on_workflow_row_changed_cb), configure_workflow_btn);

    GtkWidget *close_btn = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
    //g_signal_connect(close_btn, "clicked", G_CALLBACK(on_close_workflow_list_cb), g_workflow_list_window);

    GtkWidget *btnbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_end(GTK_BOX(btnbox), close_btn, false, false, 0);
    gtk_box_pack_end(GTK_BOX(btnbox), configure_workflow_btn, false, false, 0);

    gtk_box_pack_start(GTK_BOX(main_vbox), workflows_scroll, true, true, 10);
    gtk_box_pack_start(GTK_BOX(main_vbox), btnbox, false, false, 0);

    return main_vbox;
}

void show_workflow_list_dialog(GtkWindow *parent)
{
    //g_verbose = 3;
    g_print(__func__);
    if (g_workflow_list == NULL)
    {
        g_print("g_workflow_list is '%p': reloading\n", g_workflow_list);
        load_workflow_config_data("/etc/libreport/workflows");
    }

    GtkWindow *workflow_list_window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(workflow_list_window, _("Workflow Configuration"));
    gtk_window_set_default_size(workflow_list_window, 450, 400);
    gtk_window_set_position(workflow_list_window, parent ? GTK_WIN_POS_CENTER_ON_PARENT : GTK_WIN_POS_CENTER);
    if (parent != NULL)
    {
        gtk_window_set_transient_for(workflow_list_window, parent);
        // modal = parent window can't steal focus
        gtk_window_set_modal(workflow_list_window, true);
        gtk_window_set_icon_name(workflow_list_window,
            gtk_window_get_icon_name(parent));
    }

    GtkWidget *main_vbox = create_workflow_list_dialog();

    gtk_container_add(GTK_CONTAINER(workflow_list_window), main_vbox);

    gtk_widget_show_all(GTK_WIDGET(workflow_list_window));

}
