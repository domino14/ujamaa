#ifndef _UJAMAA_SERVER_H_
#define _UJAMAA_SERVER_H_

#include <glib.h>
#include <gio/gio.h>

struct ConnectionData {
    GSocketConnection *connection;
    void (*processMessage)(gchar*, GOutputStream*);
};

gboolean network_read(GIOChannel *source, GIOCondition cond, gpointer data);
gboolean new_connection(GSocketService *service, GSocketConnection *connection,
    GObject *source_object, gpointer user_data);

#endif