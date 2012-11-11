#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "client.h"
#include "internal_libreport_gtk.h"
#include "workflow.h"
//#include "wizard.h"

#define DEFAULT_WIDTH   800
#define DEFAULT_HEIGHT  500

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);
    GtkWindow *window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    //show_events_list_dialog(window);

    //workflow_t *w = new_workflow();

    //load_workflow_description_from_file(w, "../plugins/workflow_Fedora.xml");
    show_workflow_list_dialog(window);
    //g_signal_connect(window, "destroy", gtk_main_quit, NULL);
    //gtk_main();

}
