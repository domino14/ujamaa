#include <glib.h>
#include <gio/gio.h>

/**
 * [network_read description]
 * @param  source The source channel.
 * @param  cond   A condition.
 * @param  data   The connection.
 * @return        Boolean - remove event source if FALSE, keep if TRUE.
 */
gboolean network_read(GIOChannel *source, GIOCondition cond, gpointer data) {
    GString *s = g_string_new(NULL);
    GError *error = NULL;
    GIOStatus ret = g_io_channel_read_line_string(source, s, NULL, &error);
    GOutputStream *ostream = g_io_stream_get_output_stream (
        G_IO_STREAM (data));

    if (ret == G_IO_STATUS_ERROR) {
        g_warning ("Error reading: %s\n", error->message);
        // Drop last reference on connection
        g_object_unref (data);
        // Remove the event source
        return FALSE;
    } else {
        g_print("Got: %s\n", s->str);
        g_output_stream_write(ostream,
                          "Hello client!", /* your message goes here */
                          13, /* length of your message */
                          NULL,
                          &error);
        if (error != NULL) {
            g_error("Error writing: %s\n", error->message);
        }
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
    g_io_add_watch(channel, G_IO_IN, (GIOFunc) network_read, connection);
    return TRUE;
}

int main(int argc, char **argv) {
    GSocketService *service = g_socket_service_new();
    GInetAddress *address = g_inet_address_new_from_string("127.0.0.1");
    GSocketAddress *socket_address = g_inet_socket_address_new(address, 4000);
    g_socket_listener_add_address(
        G_SOCKET_LISTENER(service), socket_address, G_SOCKET_TYPE_STREAM,
        G_SOCKET_PROTOCOL_TCP, NULL, NULL, NULL);

    g_object_unref(socket_address);
    g_object_unref(address);
    g_socket_service_start(service);

    g_signal_connect(service, "incoming", G_CALLBACK(new_connection), NULL);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
}
