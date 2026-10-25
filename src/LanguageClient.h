// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_LANGUAGECLIENT_H
#define LSP_LANGUAGECLIENT_H
#include "protocol/LSProtocol.h"
#include "session.h"

namespace lsp {
class LanguageClient {

public:
  virtual void showMessage(const LogMessageParams &Params) = 0;
  virtual void logMessage(const LogMessageParams &Params) = 0;
  virtual void publishDiagnostics(const PublishDiagnosticsParams &Param) = 0;
};

class ProxyLanguageClient : public LanguageClient {
  jsonrpc::Session &Connection;

public:
  ProxyLanguageClient(jsonrpc::Session &Connection) : Connection(Connection) {}

  virtual void showMessage(const LogMessageParams &Params) {
    Connection.send_notification(ShowMessage, jsonrpc::to_json(Params));
  }

  virtual void logMessage(const LogMessageParams &Params) {
    //Connection.send_notification(LogMessage, jsonrpc::to_json(Params));
  }

  virtual void publishDiagnostics(const PublishDiagnosticsParams &Params) {
      //Connection.send_notification(PublishDiagnostics, jsonrpc::to_json(Params));
  }
};
}; // namespace lsp
#endif
