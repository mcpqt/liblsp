// SPDX-License-Identifier: AGPL-3.0-only
#include "LanguageServer.h"

using namespace lsp;

template <typename T1, typename T2, typename T3>
static typename std::enable_if<!std::is_void<T2>::value,
                               std::function<QJsonValue(const QJsonValue &)>>::type
createWrapperFunction(T1 *Target, T2 (T1::*TargetFunction)(const T3 &)) {
  std::function<T2(T3)> Func =
      std::bind(TargetFunction, Target, std::placeholders::_1);
  return [Func](const QJsonValue &JsonParams) -> QJsonValue {
    T3 Params = jsonrpc::from_json<T3>(JsonParams);
    T2 Result = Func(Params);
    return jsonrpc::to_json(Result);
  };
}

template <typename T1, typename T2, typename T3>
static typename std::enable_if<std::is_void<T2>::value,
                               std::function<QJsonValue(const QJsonValue &)>>::type
createWrapperFunction(T1 *Target, T2 (T1::*TargetFunction)(const T3 &)) {
  std::function<T2(T3)> Func =
      std::bind(TargetFunction, Target, std::placeholders::_1);
  return [Func](const QJsonValue &JsonParams) -> QJsonValue {
    T3 Params = jsonrpc::from_json<T3>(JsonParams);
    Func(Params);
    return QJsonValue(QJsonValue::Null);
  };
}

template <typename T1, typename T2>
static typename std::enable_if<std::is_void<T2>::value,
                               std::function<QJsonValue(const QJsonValue &)>>::type
createWrapperFunction(T1 *Target, T2 (T1::*TargetFunction)()) {
  std::function<T2()> Func = std::bind(TargetFunction, Target);
  return [Func](const QJsonValue &JsonParams) -> QJsonValue {
    (void)JsonParams;
    Func();
    return QJsonValue(QJsonValue::Null);
  };
}

void LanguageServer::connect(jsonrpc::Session &Connection) {
  Connection.set_request_handler(
      "initialize", createWrapperFunction(this, &LanguageServer::initialize));
  Connection.set_notification_handler(
      "initialized", createWrapperFunction(this, &LanguageServer::initialized));
  Connection.set_request_handler(
      "shutdown", createWrapperFunction(this, &LanguageServer::shutdown));
  Connection.set_notification_handler(
      "exit", createWrapperFunction(this, &LanguageServer::exit));

  Connection.set_notification_handler(
      "textDocument/didChange",
      createWrapperFunction(this, &LanguageServer::textDocumentDidChange));

  Connection.set_request_handler(
      "textDocument/completion",
      createWrapperFunction(this, &LanguageServer::completion));
  Connection.set_request_handler(
      "textDocument/hover",
      createWrapperFunction(this, &LanguageServer::hover));
}

InitializeResult LanguageServer::initialize(
    const InitializeParams<EmptyInitializationOptions> &Params) {
  return InitializeResult();
}

void LanguageServer::initialized(const InitializedParams &Params) {}

void LanguageServer::shutdown() {}

void LanguageServer::exit() { std::exit(0); }

void LanguageServer::textDocumentDidChange(
    const TextDocumentDidChangeParams &Params) {}

std::vector<CompletionItem<EmptyCompletionData>>
LanguageServer::completion(const CompletionParams &Params) {
  return std::vector<CompletionItem<EmptyCompletionData>>();
}
Hover LanguageServer::hover(const TextDocumentPositionParams &Params) {
  return Hover();
}
