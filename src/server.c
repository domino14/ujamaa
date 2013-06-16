#include "server.h"

/**
 * Reads from the socket and processes message with passed in external function
 * in 'data' pointer. 'data' is a ConnectionData instance.
 * @param  source The source channel.
 * @param  cond   A condition.
 * @param  data   The connection.
 * @return        Boolean - remove event source if FALSE, keep if TRUE.
 */
gboolean network_read(GIOChannel *source, GIOCondition cond, gpointer data) {
    GString *s = g_string_new(NULL);
    GError *error = NULL;
    GIOStatus ret = g_io_channel_read_line_string(source, s, NULL, &error);
    struct ConnectionData *cdata = data;
    GOutputStream *ostream = g_io_stream_get_output_stream (
        G_IO_STREAM (cdata->connection));
    if (ret == G_IO_STATUS_ERROR) {
        g_warning ("Error reading: %s\n", error->message);
        // Drop last reference on connection
        g_object_unref (cdata->connection);
        // Remove the event source
        return FALSE;
    } else {
        cdata->processMessage(s->str, ostream);
    }
    if (ret == G_IO_STATUS_EOF) {
        return FALSE;
    }
    return TRUE;
}

gboolean new_connection(GSocketService *service, GSocketConnection *connection,
    GObject *source_object, gpointer user_data) {

    GSocketAddress *sockaddr = g_socket_connection_get_remote_address(
        connection, NULL);
    GInetAddress *addr = g_inet_socket_address_get_address(
        G_INET_SOCKET_ADDRESS(sockaddr));
    guint16 port = g_inet_socket_address_get_port(G_INET_SOCKET_ADDRESS(
        sockaddr));

    g_print("New Connection from %s:%d\n",
            g_inet_address_to_string(addr), port);
    g_object_ref (connection); // Keep connection alive.
    GSocket *socket = g_socket_connection_get_socket(connection);

    gint fd = g_socket_get_fd(socket);
    GIOChannel *channel = g_io_channel_unix_new(fd);
    struct ConnectionData *cdata = user_data;
    cdata->connection = connection;
    g_io_add_watch(channel, G_IO_IN, (GIOFunc) network_read, cdata);
    return TRUE;
}
