// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_MESSAGE_H
#define LSP_MESSAGE_H

#include "../error.h"
#include "json.h"

#include <QByteArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include <functional>
#include <memory>
#include <type_traits>

namespace lsp {
class Message;
typedef std::unique_ptr<Message> MessagePtr;
class Message {
public:
  enum MessageType { Invalid, Request, Response, Notification };

protected:
  QJsonObject Data;
  MessageType Type;

  Message(MessageType Type) : Type(Type) { Data["jsonrpc"] = "2.0"; }
  Message(MessageType Type, const QJsonObject &Data)
      : Type(Type), Data(Data) {}

public:
  Message() : Type(Invalid) {}
  Message(const QJsonObject &Data) : Type(Invalid), Data(Data) {}
  virtual ~Message() {}

  MessageType getType() const { return Type; }

  bool isInvalid() const { return Type == Invalid; }
  bool isRequest() const { return Type == Request; }
  bool isResponse() const { return Type == Response; }
  bool isNotification() const { return Type == Notification; }

  const QJsonObject &data() const { return Data; }
  QJsonObject &data() { return Data; }
  QByteArray dump() const { return dump_json(QJsonValue(Data)); }

  static MessagePtr parse(const QByteArray &Body);
};

class RequestMessage : public Message {
public:
  RequestMessage(const QString &Method, const QString &Id)
      : Message(Message::MessageType::Request) {
    Data["id"] = Id;
    Data["method"] = Method;
  }
  RequestMessage(const QJsonObject &Data)
      : Message(Message::MessageType::Request, Data) {}

  QJsonValue getIdValue() const { return Data.value("id"); }
  QString getId() const {
    const QJsonValue Id = Data.value("id");
    if (Id.isString())
      return Id.toString();
    if (Id.isDouble())
      return QString::number(static_cast<qint64>(Id.toDouble()));
    return QString();
  }
  QString getMethod() const {
    return Data.value("method").toString();
  }

  QJsonValue getParams() const { return Data.value("params"); }
};

class ResponseMessage : public Message {
public:
  ResponseMessage(const QString &Id) : Message(Response) {
    Data["id"] = Id;
  }
  ResponseMessage(const QJsonObject &Data)
      : Message(Message::MessageType::Response, Data) {}

  bool isError() const { return Data.contains("error"); }
  QString getId() const {
    const QJsonValue Id = Data.value("id");
    if (Id.isString())
      return Id.toString();
    if (Id.isDouble())
      return QString::number(static_cast<qint64>(Id.toDouble()));
    return QString();
  }
  QJsonValue getResult() const { return Data.value("result"); }
  QJsonValue getError() const { return Data.value("error"); }
  void setResult(const QJsonValue &Result) { Data["result"] = Result; }
  void setError(const QJsonValue &ErrorValue) { Data["error"] = ErrorValue; }

  static MessagePtr fromRequest(RequestMessage &Msg);
};

class NotificationMessage : public Message {
public:
  NotificationMessage(const QString &Method)
      : Message(Message::MessageType::Notification) {
    Data["method"] = Method;
  }
  NotificationMessage(const QJsonObject &Data)
      : Message(Message::MessageType::Notification, Data) {}

  QString getMethod() const {
    return Data.value("method").toString();
  }
  QJsonValue getParams() const { return Data.value("params"); }

  static MessagePtr createNotification(const QString &Method);
};

enum ErrorCode {
  ParseError = -32700,
  InvalidRequest = -32600,
  MethodNotFound = -32601,
  InvalidParams = -32602,
  InternalError = -32603
};

class MessageType {
  QString Method;

public:
  MessageType(const QString &Method) : Method(Method) {}

  QString method() const { return Method; }
};

template <typename ParamType, typename ReturnType>
class RequestType : public MessageType {
public:
  RequestType(const QString &Method) : MessageType(Method) {}

  std::function<QJsonValue(const QJsonValue &)>
  getRequestFunction(std::function<ReturnType(const ParamType &)> Func) {
    return [Func](const QJsonValue &JsonParams) -> QJsonValue {
      ParamType Params = from_json<ParamType>(JsonParams);
      if constexpr (std::is_void_v<ReturnType>) {
        Func(Params);
        return QJsonValue(QJsonValue::Null);
      } else {
        ReturnType Result = Func(Params);
        return to_json(Result);
      }
    };
  }
};

template <typename ParamType> class NotificationType : public MessageType {
public:
  NotificationType(const QString &Method) : MessageType(Method) {}

  std::function<void(const QJsonValue &)>
  getNotificationFunc(std::function<void(const ParamType &)> Func) {
    return [Func](const QJsonValue &JsonParams) {
      ParamType Params = from_json<ParamType>(JsonParams);
      Func(Params);
    };
  }
};

} // namespace lsp
#endif // !LSP_MESSAGE_H
