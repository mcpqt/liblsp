// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_STDIOMESSAGEHANDLER_H
#define LSP_STDIOMESSAGEHANDLER_H

#include "jsonrpc/Message.h"
#include "jsonrpc/MessageReader.h"
#include "jsonrpc/MessageWriter.h"

#include <QByteArray>

#include <QString>

#include <iostream>

namespace lsp {
class StdinMessageReader : public MessageReader {
public:
  virtual void listen(CallbackFunction Callback) {
    MessageHandler = Callback;
    State = MessageReaderState::Connected;
    read();
  }

private:
  void read() {
    while (true) {
      QString Header;
      std::size_t Length = 0;

      while (true) {
        Header.clear();
        int Ch = std::cin.get();
        bool GotLine = false;
        while (Ch != EOF) {
          if (Ch == '\n') {
            GotLine = true;
            break;
          }
          Header.append(QChar(static_cast<char>(Ch)));
          Ch = std::cin.get();
        }

        if (!GotLine) {
          if (std::cin.eof())
            CloseEmitter.emitEvent();
          else
            ErrorEmitter.emitEvent(Error(ErrorCode::ParseError,
                                    "Failed to receive header."));
          return;
        }

        if (!Header.isEmpty() && Header.back() == QChar('\r'))
          Header.chop(1);
        if (Header.isEmpty())
          break;

        const std::size_t Parsed = getLength(Header);
        if (Parsed == 0) {
          ErrorEmitter.emitEvent(
              Error(ErrorCode::ParseError, "Invalid header received: " + Header));
          return;
        }
        Length = Parsed;
      }

      QByteArray Body(static_cast<int>(Length), Qt::Uninitialized);
      std::cin.read(Body.data(), static_cast<std::streamsize>(Length));
      if (std::cin.gcount() != static_cast<std::streamsize>(Length)) {
        ErrorEmitter.emitEvent(
            Error(ErrorCode::ParseError, "Failed to receive body."));
        CloseEmitter.emitEvent();
        return;
      }

      try {
        MessageHandler(Message::parse(Body));
      } catch (const Error &Err) {
        ErrorEmitter.emitEvent(Err);
      } catch (const std::exception &Err) {
        ErrorEmitter.emitEvent(Error(ErrorCode::ParseError, QString::fromUtf8(Err.what())));
      }
    }
  }
};

class StdoutMessageWriter : public MessageWriter {
public:
  virtual void write(MessagePtr Msg) {
    const QByteArray Body = Msg->dump();
    std::cout << "Content-Length: " << Body.size() << "\r\n\r\n";
    std::cout.write(Body.constData(), Body.size());
    std::cout.flush();
  }
};
} // namespace lsp

#endif
