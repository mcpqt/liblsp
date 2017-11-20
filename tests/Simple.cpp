// SPDX-License-Identifier: AGPL-3.0-only
#include "ServerConnection.h"


int main(int argc, char **argv) {
    int a = 1;
    while(a);
    lsp::StdioServerConnection Con;
    Con.run();
}