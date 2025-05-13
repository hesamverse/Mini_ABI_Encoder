// src/gui.c
#include <gtk/gtk.h>

/*
 * Callback: Triggered when the GTK application is launched
 */
static void activate(GtkApplication *app, gpointer user_data) {
    (void)user_data; // Suppress unused parameter warning

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Mini ABI Encoder");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // Future: Add layout containers, input fields, buttons here

    gtk_widget_show_all(window);
}

/*
 * Launch GTK GUI interface
 */
int launch_gui(int argc, char **argv) {
    GtkApplication *app = gtk_application_new(
        "org.hesamverse.abi_encoder",
        G_APPLICATION_DEFAULT_FLAGS  // Replaces deprecated G_APPLICATION_FLAGS_NONE
    );

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}