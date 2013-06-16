#include "../gaddag/gaddag.h"
#include "anagrammer.h"
#include "../server.h"
#include <string.h>
struct Answers answers;
NODE* node;

/**
 * Writes a message to socket.
 * @param ostream The output stream for the socket.
 * @param message The message.
 */
void writeToSocket(GOutputStream* ostream, char* message) {
    GError* error = NULL;
    g_output_stream_write(ostream, message, strlen(message), NULL, &error);
    if (error != NULL) {
        g_error("Error writing: %s\n", error->message);
    }
}

/**
 * Processes an incoming socket message.
 * @param message The message string.
 * @param ostream The output stream for the socket.
 */
void processMessage(gchar* message, GOutputStream* ostream) {
    int i;
    char param_error[] = "Wrong format; use <anagram|build> <string>\n";
    g_print("Got: %s\n", message);

    gchar** command = g_strsplit(message, " ", 2);
    // Figure out how many tokens it actually generated.
    // g_strsplit returns a null-terminated list.
    for (i = 0; command[i] != NULL; i++) {
    }
    if (i != 2 || strlen(command[0]) > 7 || strlen(command[1]) > 15 ||
        !(g_strcmp0(command[0], "anagram") == 0 ||
            g_strcmp0(command[0], "build") == 0)) {
        writeToSocket(ostream, param_error);
    } else {
        GString* str = g_string_new("");
        anagram(node, command[1], command[0], &answers, 1, 0);
        for (i = 0; i < answers.num_answers; i++) {
            g_string_append(str, answers.answers[i]);
            g_string_append(str, "\n");
        }
        cleanup_answers(&answers);
        writeToSocket(ostream, str->str);
        g_string_free(str, TRUE);
    }
    g_strfreev(command);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        g_print("Arguments: gaddag_path\n");
        return 1;
    }
    g_type_init();
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
    node = load_gaddag(argv[1]);

    g_main_loop_run(loop);
    return 1;
}
