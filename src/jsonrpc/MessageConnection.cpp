// SPDX-License-Identifier: AGPL-3.0-only
#include "jsonrpc/MessageConnection.h"

using namespace lsp;

void MessageConnection::errorHandler(const Error &Message) {}

void MessageConnection::closeHandler() {}

void MessageConnection::messageHandler(MessagePtr Msg) {
  switch (Msg->getType()) {
  case Message::MessageType::Invalid:
    break;
  case Message::MessageType::Notification:
  case Message::MessageType::Request: {
    std::unique_lock<std::mutex> Lock(RequestMutex);
    RequestQueue.push(std::move(Msg));
    RequestCV.notify_one();
    break;
  }
  case Message::MessageType::Response: {
    std::unique_lock<std::mutex> Lock(ResponseMutex);
    ResponseMap[static_cast<ResponseMessage *>(Msg.get())->getId()] =
        std::move(Msg);
    ResponseCV.notify_all();
    break;
  }
  }
}

void MessageConnection::listen() {
  Reader->listen(std::bind(&MessageConnection::messageHandler, this,
                           std::placeholders::_1));
}


void MessageConnection::processMessageQueue() {
  std::unique_lock<std::mutex> Lock(RequestMutex);

  // Wait for the queue to be filled
  if (RequestQueue.size() == 0) {
    RequestCV.wait(Lock, [this]() { return RequestQueue.size() != 0; });
  }

  MessagePtr Msg = std::move(RequestQueue.front());
  RequestQueue.pop();
  switch (Msg->getType()) {
  case lsp::Message::MessageType::Request: {
    RequestMessage &Req = *static_cast<RequestMessage *>(Msg.get());
    handleRequest(Req);
    break;
  }
  case lsp::Message::MessageType::Notification: {
    NotificationMessage &Notification =
        *static_cast<NotificationMessage *>(Msg.get());
    handleNotification(Notification);
    break;
  }
  case lsp::Message::MessageType::Response:
  case lsp::Message::MessageType::Invalid:
    break;
  }
}

void MessageConnection::handleRequest(RequestMessage &Msg) {
  MessagePtr RespMsg = ResponseMessage::fromRequest(Msg);
  ResponseMessage &Resp = *static_cast<ResponseMessage *>(RespMsg.get());

  auto Handler = RequestHandlers.find(Msg.getMethod());
  if (Handler != RequestHandlers.end()) {
    try {
      Resp.setResult(Handler->second(Msg.getParams()));
    } catch (const Error &Err) {
      Resp.setError(Err.object());
    } catch (const std::exception &Err) {
      ErrorObject ErrObj;
      ErrObj.code = error_code::internal_error;
      ErrObj.message = QString::fromUtf8(Err.what());
      Resp.setError(ErrObj);
    } catch (...) {
      ErrorObject ErrObj;
      ErrObj.code = error_code::internal_error;
      ErrObj.message = QStringLiteral("Internal error");
      Resp.setError(ErrObj);
    }
  } else {
    ErrorObject ErrObj;
    ErrObj.code = error_code::method_not_found;
    ErrObj.message = QStringLiteral("Method not found");
    Resp.setError(ErrObj);
  }
  Writer->write(std::move(RespMsg));
}
void MessageConnection::handleNotification(NotificationMessage &Msg) {
  auto Handler = NotificationHandlers.find(Msg.getMethod());
  if (Handler != NotificationHandlers.end()) {
    try {
      Handler->second(Msg.getParams());
    } catch (...) {
      // TODO:
    }
  }
}
