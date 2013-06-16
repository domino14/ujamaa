#include "../gaddag/gaddag.h"
#include "anagrammer.h"
#include "../server.h"

struct Answers answers;

/*int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Arguments: gaddag_path anagram_str mode\n");
        printf("Modes: anagram build\n");
        return 1;
    }
    printf("Loading gaddag...\n");
    NODE* node = load_gaddag(argv[1]);
    anagram(node, argv[2], argv[3], &answers, 1, 0);
    print_answers(&answers);
    // Clean up
    cleanup_answers(&answers);

    return 0;
} */



/**
 * Processes an incoming socket message.
 * @param message The message string.
 * @param ostream The output stream for the socket.
 */
void processMessage(gchar* message, GOutputStream* ostream) {
    GError* error = NULL;
    g_print("Got: %s\n", message);
    g_output_stream_write(ostream,
                      "Hello client!", /* your message goes here */
                      13, /* length of your message */
                      NULL,
                      &error);
    if (error != NULL) {
        g_error("Error writing: %s\n", error->message);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        g_print("Arguments: gaddag_path\n");
        return 1;
    }
    GSocketService *service = g_socket_service_new();
    GInetAddress *address = g_inet_address_new_from_string("127.0.0.1");
    GSocketAddress *socket_address = g_inet_socket_address_new(address, 4000);
    struct ConnectionData cdata;
    g_socket_listener_add_address(
        G_SOCKET_LISTENER(service), socket_address, G_SOCKET_TYPE_STREAM,
        G_SOCKET_PROTOCOL_TCP, NULL, NULL, NULL);

    g_object_unref(socket_address);
    g_object_unref(address);
    g_socket_service_start(service);

    cdata.processMessage = &processMessage;

    g_signal_connect(service, "incoming", G_CALLBACK(new_connection), &cdata);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    g_print("Loading gaddag...");
    NODE* node = load_gaddag(argv[1]);

    g_main_loop_run(loop);
    return 1;
}
