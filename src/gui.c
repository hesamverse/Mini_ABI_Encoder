#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Mini ABI Encoder");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_widget_show_all(window);
}

int launch_gui(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.hesamverse.abi_encoder", G_APPLICATION_DEFAULT_FLAGS);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}