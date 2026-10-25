// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_LANGUAGECLIENT_H
#define LSP_LANGUAGECLIENT_H
#include "protocol/LSProtocol.h"
#include "jsonrpc/MessageConnection.h"

namespace lsp {
class LanguageClient {

public:
  virtual void showMessage(const LogMessageParams &Params) = 0;
  virtual void logMessage(const LogMessageParams &Params) = 0;
  virtual void publishDiagnostics(const PublishDiagnosticsParams &Param) = 0;
};

class ProxyLanguageClient : public LanguageClient {
  MessageConnection &Connection;

public:
  ProxyLanguageClient(MessageConnection &Connection) : Connection(Connection) {}

  virtual void showMessage(const LogMessageParams &Params) {
    Connection.sendNotification(ShowMessage, Params);
  }

  virtual void logMessage(const LogMessageParams &Params) {
    //Connection.sendNotification(LogMessage, Params);
  }

  virtual void publishDiagnostics(const PublishDiagnosticsParams &Params) {
      //Connection.sendNotification(PublishDiagnostics, Params);
  }
};
}; // namespace lsp
#endif
