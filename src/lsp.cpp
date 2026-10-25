// SPDX-License-Identifier: AGPL-3.0-only
#include "lsp.h"

#include "rpc/qt_tcp_transport.h"
#include "rpc/session.h"
#include "rpc/stdio_transport.h"

#include <QTcpSocket>

#include <stdexcept>

namespace lsp {

std::unique_ptr<lsp::Session> createStdIoConnection() {
  auto transport = std::make_unique<lsp::StdioTransport>();
  return std::make_unique<lsp::Session>(std::move(transport));
}

std::unique_ptr<lsp::Session> createQtTCPConnection(const QString &Hostname,
                                                    quint16 Port) {
  QTcpSocket *Socket = new QTcpSocket();
  Socket->connectToHost(Hostname, Port);
  if (!Socket->waitForConnected(5000)) {
    const QString ErrorString = Socket->errorString();
    delete Socket;
    throw std::runtime_error(ErrorString.toUtf8().constData());
  }

  auto transport = std::make_unique<lsp::rpc::QtTcpTransport>(Socket);
  return std::make_unique<lsp::Session>(std::move(transport));
}

} // namespace lsp
