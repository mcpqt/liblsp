// SPDX-License-Identifier: AGPL-3.0-only
#include "lsp.h"

#include "tcp_transport.h"
#include "session.h"
#include "stdio_transport.h"

#include <QTcpSocket>

#include <stdexcept>

namespace lsp {

std::unique_ptr<lsp::Session> createStdIoConnection() {
  auto transport = std::make_unique<jsonrpc::StdioTransport>();
  return std::make_unique<Session>(std::move(transport));
}

std::unique_ptr<lsp::Session> createTCPConnection(const QString &Hostname,
                                                    quint16 Port) {
  QTcpSocket *Socket = new QTcpSocket();
  Socket->connectToHost(Hostname, Port);
  if (!Socket->waitForConnected(5000)) {
    const QString ErrorString = Socket->errorString();
    delete Socket;
    throw std::runtime_error(ErrorString.toUtf8().constData());
  }

  auto transport = std::make_unique<jsonrpc::TcpTransport>(Socket);
  return std::make_unique<Session>(std::move(transport));
}

} // namespace lsp
