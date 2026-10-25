// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_REMOTE_H
#define LSP_REMOTE_H

#include "jsonrpc/MessageConnection.h"
#include "protocol/LSProtocol.h"

namespace lsp {
class Remote {
public:
  void attach(MessageConnectionPtr MsgConnection) {
    Connection = MsgConnection;
  }
  
  virtual void initialize(const ClientCapabilities &CC) = 0;

protected:
  MessageConnectionPtr Connection;

};

class Remote
} // namespace lsp

#endif // DEBUG