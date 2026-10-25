// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_QTMESSAGEHANDLER_H
#define LSP_QTMESSAGEHANDLER_H

#include "jsonrpc/Message.h"
#include "jsonrpc/MessageReader.h"
#include "jsonrpc/MessageWriter.h"

#include <QByteArray>
#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QTcpSocket>

#include <algorithm>
#include <mutex>

namespace lsp {

namespace detail {

inline qint64 headerLength(const QByteArray &Header) {
  const QList<QByteArray> Lines = Header.split('\n');
  for (const QByteArray &RawLine : Lines) {
    const QByteArray Line = RawLine.trimmed();
    const int Colon = Line.indexOf(':');
    if (Colon < 0)
      continue;
    const QByteArray Key = Line.left(Colon).trimmed().toLower();
    if (Key != "content-length")
      continue;
    bool Ok = false;
    const qint64 Length = Line.mid(Colon + 1).trimmed().toLongLong(&Ok);
    return Ok ? Length : -1;
  }
  return -1;
}

} // namespace detail

class QtMessageReader : public QObject, public MessageReader {
  QPointer<QTcpSocket> Socket;
  QByteArray Buffer;
  bool Closed = false;

public:
  explicit QtMessageReader(QTcpSocket *Socket) : Socket(Socket) {
    if (!Socket)
      return;
    connect(Socket, &QTcpSocket::readyRead, this,
            &QtMessageReader::onReadyRead);
    connect(Socket, &QTcpSocket::disconnected, this,
            &QtMessageReader::onDisconnected);
    connect(Socket, &QTcpSocket::errorOccurred, this,
            &QtMessageReader::onErrorOccurred);
  }

  virtual void listen(CallbackFunction Callback) {
    MessageHandler = Callback;
    State = MessageReaderState::Connected;
    if (Socket && Socket->bytesAvailable() > 0)
      Buffer.append(Socket->readAll());
    processBuffer();
  }

private:
  void onReadyRead() {
    if (!Socket)
      return;
    Buffer.append(Socket->readAll());
    processBuffer();
  }

  void onDisconnected() {
    if (Closed)
      return;
    Closed = true;
    State = MessageReaderState::Closed;
    CloseEmitter.emitEvent();
  }

  void onErrorOccurred(QAbstractSocket::SocketError ErrorCode) {
    if (!Socket || ErrorCode == QAbstractSocket::RemoteHostClosedError)
      return;
    ErrorEmitter.emitEvent(
        Error(lsp::ErrorCode::InternalError, Socket->errorString()));
  }

  void processBuffer() {
    while (true) {
      int HeaderEnd = Buffer.indexOf("\r\n\r\n");
      int SeparatorLength = 4;
      if (HeaderEnd < 0) {
        HeaderEnd = Buffer.indexOf("\n\n");
        SeparatorLength = 2;
      }
      if (HeaderEnd < 0)
        return;

      const QByteArray Header = Buffer.left(HeaderEnd);
      const qint64 Length = detail::headerLength(Header);
      if (Length < 0) {
        ErrorEmitter.emitEvent(
            Error(lsp::ErrorCode::ParseError, "Missing or invalid Content-Length"));
        Buffer.remove(0, HeaderEnd + SeparatorLength);
        continue;
      }

      const qint64 Total = HeaderEnd + SeparatorLength + Length;
      if (Buffer.size() < Total)
        return;

      const QByteArray Body =
          Buffer.mid(HeaderEnd + SeparatorLength, static_cast<int>(Length));
      Buffer.remove(0, static_cast<int>(Total));

      try {
        MessagePtr Msg = Message::parse(Body);
        if (MessageHandler)
          MessageHandler(std::move(Msg));
      } catch (const Error &Err) {
        ErrorEmitter.emitEvent(Err);
      } catch (const std::exception &Err) {
        ErrorEmitter.emitEvent(
            Error(lsp::ErrorCode::ParseError, QString::fromUtf8(Err.what())));
      }
    }
  }
};

class QtMessageWriter : public MessageWriter {
  QPointer<QTcpSocket> Socket;
  std::mutex WriteMutex;

public:
  explicit QtMessageWriter(QTcpSocket *Socket) : Socket(Socket) {}

  virtual void write(MessagePtr Msg) {
    const QByteArray Body = Msg->dump();
    const QByteArray Frame = QByteArray("Content-Length: ") +
                             QByteArray::number(Body.size()) + "\r\n\r\n" +
                             Body;

    std::lock_guard<std::mutex> Lock(WriteMutex);
    if (!Socket || Socket->state() != QAbstractSocket::ConnectedState)
      return;

    QMetaObject::invokeMethod(
        Socket.data(),
        [Socket = Socket, Frame]() {
          if (Socket)
            Socket->write(Frame);
        },
        Qt::AutoConnection);
  }
};

} // namespace lsp

#endif
