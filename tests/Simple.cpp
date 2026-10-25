// SPDX-License-Identifier: AGPL-3.0-only
#include <QCoreApplication>
#include "lsp.h"
#include "protocol/LSProtocol.h"

#include <cstdlib>
#include <iostream>
#include <thread>

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
  lsp::MessageConnection *Con = lsp::createQtTCPConnection(
      QStringLiteral("localhost"), static_cast<quint16>(19191));
  std::function<lsp::InitializeResult(
      const lsp::InitializeParams<QString> &)>
      Func(&asd);
  Con->registerRequestHandler(lsp::InitializeRequest, Func);
  Con->registerRequestHandler(lsp::TextDocumentFormatting,
                              std::function(&format));
  Con->registerNotificationHandler(lsp::TextDocumentDidOpen,
                                   std::function(&TDopen));
  Con->registerNotificationHandler(lsp::TextDocumentDidChange,
                                   std::function(&TDchange));
  Con->listen();

  try {
    std::thread Worker([Con] {
      try {
        while (true)
          Con->processMessageQueue();
      } catch (...) {
        return;
      }
    });
    Worker.detach();
    return App.exec();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}