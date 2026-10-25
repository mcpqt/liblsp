// SPDX-License-Identifier: AGPL-3.0-only
#include "jsonrpc/QtMessageHandler.h"
#include "lsp.h"

#include <QTcpSocket>

#include <stdexcept>

using namespace lsp;

MessageConnection *lsp::createQtTCPConnection(const QString &Hostname,
                                              quint16 Port) {
  QTcpSocket *Socket = new QTcpSocket();
  Socket->connectToHost(Hostname, Port);
  if (!Socket->waitForConnected(5000)) {
    const QString ErrorString = Socket->errorString();
    delete Socket;
    throw std::runtime_error(ErrorString.toUtf8().constData());
  }

  QtMessageReader *Reader = new QtMessageReader(Socket);
  QtMessageWriter *Writer = new QtMessageWriter(Socket);
  return new MessageConnection(Reader, Writer);
}
