// SPDX-License-Identifier: AGPL-3.0-only
#include <QCoreApplication>
#include "lsp.h"
#include "protocol/LSProtocol.h"
#include "json.h"

#include <cstdlib>
#include <iostream>

lsp::InitializeResult asd(const lsp::InitializeParams<QString> &Params) {
  lsp::InitializeResult res;
  res.Capabilities.DocumentFormattingProvider = true;
  res.Capabilities.TextDocumentSync.OpenClose = true;
  res.Capabilities.TextDocumentSync.Change = lsp::TextDocumentSyncKind::Full;
  return res;
}
std::vector<lsp::TextEdit> format(const lsp::DocumentFormattingParams &Params) {
  std::vector<lsp::TextEdit> Edits;
  lsp::TextEdit Edit;
  Edit.NewText = "Hello";
  Edit.Range = {{1, 1}, {1, 1}};
  Edits.push_back(std::move(Edit));
  return Edits;
}

void TDopen(const lsp::TextDocumentDidOpenParams &Params) { return; }

void TDchange(const lsp::TextDocumentDidChangeParams &Params) { return; }

int main(int argc, char *argv[]) {
  QCoreApplication App(argc, argv);
  auto Con = lsp::createQtTCPConnection(
      QStringLiteral("localhost"), static_cast<quint16>(19191));

  Con->set_request_handler(
      lsp::InitializeRequest, [](const QJsonValue &params) -> QJsonValue {
        auto p = lsp::from_json<lsp::InitializeParams<QString>>(params);
        return lsp::to_json(asd(p));
      });
  Con->set_request_handler(
      lsp::TextDocumentFormatting, [](const QJsonValue &params) -> QJsonValue {
        auto p = lsp::from_json<lsp::DocumentFormattingParams>(params);
        return lsp::to_json(format(p));
      });
  Con->set_notification_handler(
      lsp::TextDocumentDidOpen, [](const QJsonValue &params) {
        auto p = lsp::from_json<lsp::TextDocumentDidOpenParams>(params);
        TDopen(p);
      });
  Con->set_notification_handler(
      lsp::TextDocumentDidChange, [](const QJsonValue &params) {
        auto p = lsp::from_json<lsp::TextDocumentDidChangeParams>(params);
        TDchange(p);
      });
  Con->start();

  try {
    return App.exec();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
