// SPDX-License-Identifier: AGPL-3.0-only
#include <asio.hpp>
#include <lsp/lsp.h>

#include <cstdlib>
#include <iostream>

using asio::ip::tcp;

int main(int argc, char *argv[]) {
  asio::io_service service;
  lsp::MessageConnection *Con = lsp::createAsioTCPConnection(service, "localhost","19191");

  Con->listen();
  
  service.run();
  return 0;
}