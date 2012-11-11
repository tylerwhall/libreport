/*
    Copyright (C) 2011  ABRT team
    Copyright (C) 2010  RedHat Inc

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

#include "event_config.h"
#include "workflow.h"
#include "internal_libreport.h"

GHashTable *g_workflow_list;

workflow_t *new_workflow(void)
{
    workflow_t *w = xzalloc(sizeof(event_config_t));
    return w;
}

void free_workflow(workflow_t *w)
{
    if (!w)
        return;

    free(w->screen_name);
    free(w->description);
    GList *event;
    for (event = w->events; event; event = g_list_next(event))
        free_event_config((event_config_t *)event);

    g_list_free(w->events);
    free(w);
}

void free_workflow_list()
{
    g_print("NOT IMPLEMENTED!!");
}

workflow_t *get_workflow(const char *name)
{
    if (!g_workflow_list)
        return NULL;
    /* @@ FIXME: SYMLINKS@!!! we canuse file_
    if (g_event_config_symlinks)
    {
        char *link = g_hash_table_lookup(g_event_config_symlinks, name);
        if (link)
            name = link;
    }
    */
    return g_hash_table_lookup(g_workflow_list, name);
}

void load_workflow_config_data(const char* path)
{
    if (g_workflow_list == NULL)
    {
        g_workflow_list = g_hash_table_new_full(
                                        g_str_hash,
                                        g_str_equal,
                                        g_free,
                                        (GDestroyNotify) free_workflow
        );
    }

    GList *workflow_files = get_file_list(path, "xml");
    while(workflow_files)
    {
        file_obj_t *file = (file_obj_t *)workflow_files->data;

        workflow_t *workflow = get_workflow(file->filename);
        bool nw_workflow = (!workflow);
        if (nw_workflow)
            workflow = new_workflow();

        load_workflow_description_from_file(workflow, file->fullpath);

        if (nw_workflow)
            g_hash_table_replace(g_workflow_list, xstrdup(file->filename), workflow);

        workflow_files = g_list_next(workflow_files);
    }
    free_file_list(workflow_files);
}
