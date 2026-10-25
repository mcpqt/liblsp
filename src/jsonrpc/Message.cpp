// SPDX-License-Identifier: AGPL-3.0-only
#include "jsonrpc/Message.h"

using namespace lsp;

static bool checkRequest(const QJsonObject &Data) {
  return Data.contains("method") && Data.contains("id");
}

static bool checkResponse(const QJsonObject &Data) {
  return (Data.contains("result") || Data.contains("error")) &&
         Data.contains("id");
}

static bool checkNotification(const QJsonObject &Data) {
  return Data.contains("method") && !Data.contains("id");
}

MessagePtr Message::parse(const QByteArray &Body) {
  MessagePtr Msg;
  QJsonObject Data;

  try {
    const QJsonValue Parsed = parse_json(Body);
    Data = as_object(Parsed);
  } catch (const std::exception &e) {
    throw Error(ErrorCode::ParseError, QString::fromUtf8(e.what()));
  }

  if (checkRequest(Data)) {
    Msg = std::make_unique<RequestMessage>(Data);
  } else if (checkNotification(Data)) {
    Msg = std::make_unique<NotificationMessage>(Data);
  } else if (checkResponse(Data)) {
    Msg = std::make_unique<ResponseMessage>(Data);
  } else {
    Msg = std::make_unique<Message>(Data);
  }

  return Msg;
}

MessagePtr ResponseMessage::fromRequest(RequestMessage &Msg) {
  auto Response = std::make_unique<ResponseMessage>(Msg.getId());
  Response->data()["id"] = Msg.getIdValue();
  return Response;
}

MessagePtr NotificationMessage::createNotification(const QString &Method) {
  return std::make_unique<NotificationMessage>(Method);
}
