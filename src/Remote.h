// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_REMOTE_H
#define LSP_REMOTE_H

#include "session.h"
#include "protocol/LSProtocol.h"

#include <memory>

namespace lsp {
class Remote {
public:
  void attach(std::shared_ptr<jsonrpc::Session> MsgConnection) {
    Connection = MsgConnection;
  }
  
  virtual void initialize(const ClientCapabilities &CC) = 0;

protected:
  std::shared_ptr<jsonrpc::Session> Connection;

};

} // namespace lsp

#endif // DEBUG