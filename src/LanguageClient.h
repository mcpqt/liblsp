// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_LANGUAGECLIENT_H
#define LSP_LANGUAGECLIENT_H
#include "protocol/LSProtocol.h"
#include "rpc/session.h"

namespace lsp {
class LanguageClient {

public:
  virtual void showMessage(const LogMessageParams &Params) = 0;
  virtual void logMessage(const LogMessageParams &Params) = 0;
  virtual void publishDiagnostics(const PublishDiagnosticsParams &Param) = 0;
};

class ProxyLanguageClient : public LanguageClient {
  rpc::Session &Connection;

public:
  ProxyLanguageClient(rpc::Session &Connection) : Connection(Connection) {}

  virtual void showMessage(const LogMessageParams &Params) {
    Connection.send_notification(ShowMessage, rpc::to_json(Params));
  }

  virtual void logMessage(const LogMessageParams &Params) {
    //Connection.send_notification(LogMessage, rpc::to_json(Params));
  }

  virtual void publishDiagnostics(const PublishDiagnosticsParams &Params) {
      //Connection.send_notification(PublishDiagnostics, rpc::to_json(Params));
  }
};
}; // namespace lsp
#endif
