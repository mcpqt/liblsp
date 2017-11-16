// SPDX-License-Identifier: AGPL-3.0-only
#include "ServerConnection.h"


int main(int argc, char **argv) {
    lsp::StdioServerConnection Con;
    Con.run();
}