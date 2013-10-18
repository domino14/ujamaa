#include "../gaddag/gaddag.h"
#include "anagrammer.h"
#include <string.h>
#include <glib.h>
#include <zmq.h>
#include <assert.h>
struct Answers answers;
NODE* node;

/**
 * Processes an incoming socket message and writes the command response
 * to a GString.
 * @param message The message string.
 * @param output The output string to write to.
 */
void processMessage(gchar* message, GString* output) {
    int i;
    char param_error[] = "Wrong format; use <anagram|build> <string>\n";
    g_print("Got: %s\n", message);

    gchar** command = g_strsplit(message, " ", 2);
    // Figure out how many tokens it actually generated.
    // g_strsplit returns a null-terminated list.
    for (i = 0; command[i] != NULL; i++) {}
    if (i != 2 || strlen(command[0]) > 7 || strlen(command[1]) > 15 ||
        !(g_strcmp0(command[0], "anagram") == 0 ||
            g_strcmp0(command[0], "build") == 0)) {
        g_string_append(output, param_error);
    } else {
        anagram(node, command[1], command[0], &answers, 1, 0);
        for (i = 0; i < answers.num_answers; i++) {
            g_string_append(output, answers.answers[i]);
            g_string_append(output, "\n");
        }
        cleanup_answers(&answers);
    }
    g_strfreev(command);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        g_print("Arguments: gaddag_path\n");
        return 1;
    }
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:4000");
    assert (rc == 0);

    g_print("Loading gaddag...");
    node = load_gaddag(argv[1]);

    while (1) {
        char buffer[255];
        GString* str = g_string_new("");

        zmq_recv(responder, buffer, 255, 0);
        g_print("Received message: %s", buffer);
        processMessage(buffer, str);
        zmq_send(responder, str->str, strlen(str->str), 0);
        g_string_free(str, TRUE);

    }
    return 1;
}
