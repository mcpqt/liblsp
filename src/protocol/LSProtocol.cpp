// SPDX-License-Identifier: AGPL-3.0-only
#include "jsonrpc/Message.h"
#include "protocol/LSProtocol.h"

using namespace lsp;

RequestType<InitializeParams<QString>, InitializeResult>
    lsp::InitializeRequest("initialize");

void Position::parse(QJsonObject *Object) {
  if (Object->contains("line"))
    Line = static_cast<uint32_t>(Object->value("line").toInteger());
  if (Object->contains("character"))
    Character = static_cast<uint32_t>(Object->value("character").toInteger());
}

QJsonValue Position::dump() { return to_json(*this); }

void Range::parse(QJsonObject *Object) {
  if (Object->contains("start") && Object->value("start").isObject()) {
    QJsonObject StartObject = Object->value("start").toObject();
    Start.parse(&StartObject);
  }
  if (Object->contains("end") && Object->value("end").isObject()) {
    QJsonObject EndObject = Object->value("end").toObject();
    End.parse(&EndObject);
  }
}

QJsonValue Range::dump() { return to_json(*this); }

QJsonValue Location::dump() { return to_json(*this); }

void lsp::to_json(QJsonValue &Data, const LogMessageParams &Params) {
  QJsonObject Object;
  Object["message"] = Params.message;
  Object["type"] = static_cast<int>(Params.type);
  Data = Object;
}

void lsp::from_json(const QJsonValue &Data, LogMessageParams &Params) {
  const QJsonObject Object = as_object(Data);
  Params.message = Object.value("message").toString();
  Params.type = static_cast<LogMessageParams::MessageType>(
      Object.value("type").toInt());
}

NotificationType<LogMessageParams> ShowMessage("window/showMessage");

void lsp::from_json(const QJsonValue &Data, TextDocumentItem &Item) {
  const QJsonObject Object = as_object(Data);
  Item.Uri = Object.value("uri").toString();
  Item.LanguageId = Object.value("languageId").toString();
  Item.Version = static_cast<uint32_t>(Object.value("version").toInteger());
  Item.Text = Object.value("text").toString();
}

void lsp::from_json(const QJsonValue &Params, TextDocumentPositionParams &TDP) {
  const QJsonObject Object = as_object(Params);
  TDP.TextDocument =
      from_json<TextDocumentIdentifier>(Object.value("textDocument"));
  TDP.Position = from_json<Position>(Object.value("position"));
}

void lsp::to_json(QJsonValue &Data, const SaveOptions &O) {
  QJsonObject Object;
  Object["includeText"] = O.IncluedeText;
  Data = Object;
}

void lsp::to_json(QJsonValue &Data, const TextDocumentSyncOptions &O) {
  QJsonObject Object;
  Object["openClose"] = O.OpenClose;
  Object["change"] = to_json(O.Change);
  Object["willSave"] = O.WillSave;
  Object["willSaveWaitUntil"] = O.WillSaveWaitUntil;
  Object["save"] = to_json(O.Save);
  Data = Object;
}

/*
 * Workspace sync
 */

void lsp::from_json(const QJsonValue &Params, TextDocumentDidOpenParams &TDO) {
  const QJsonObject Object = as_object(Params);
  TDO.TextDocument =
      from_json<TextDocumentItem>(Object.value("textDocument"));
}
NotificationType<TextDocumentDidOpenParams>
    lsp::TextDocumentDidOpen("textDocument/didOpen");

void lsp::from_json(const QJsonValue &Data, TextDocumentContentChangeEvent &CCE) {
  const QJsonObject Object = as_object(Data);
  if (Object.contains("range") && !Object.value("range").isNull())
    CCE.Range = from_json<lsp::Range>(Object.value("range"));
  if (Object.contains("rangeLength") && !Object.value("rangeLength").isNull())
    CCE.RangeLength =
        static_cast<uint32_t>(Object.value("rangeLength").toInteger());
  CCE.Text = Object.value("text").toString();
}

void lsp::from_json(const QJsonValue &Params, TextDocumentDidChangeParams &DCP) {
  const QJsonObject Object = as_object(Params);
  DCP.TextDocument =
      from_json<VersionedTextDocumentIdentifier>(Object.value("textDocument"));
  DCP.ContentChanges = from_json<std::vector<TextDocumentContentChangeEvent>>(
      Object.value("contentChanges"));
}
NotificationType<TextDocumentDidChangeParams>
    lsp::TextDocumentDidChange("textDocument/didChange");

void lsp::to_json(QJsonValue &Data, const CompletionOptions &O) {
  QJsonObject Object;
  Object["resolveProvider"] = O.ResolveProvider;
  Object["triggerCharacters"] = to_json(O.TriggerCharacters);
  Data = Object;
}

void lsp::from_json(const QJsonValue &Data, CompletionContext &CC) {
  const QJsonObject Object = as_object(Data);
  CC.TriggerKind =
      static_cast<CompletionTriggerKind>(Object.value("triggerKind").toInt());
  if (Object.contains("triggerCharacter"))
    CC.TriggerCharacter =
        Object.value("triggerCharacter").toString();
}
void lsp::from_json(const QJsonValue &Data, CompletionParams &CP) {
  const QJsonObject Object = as_object(Data);
  CP.TextDocument =
      from_json<TextDocumentIdentifier>(Object.value("textDocument"));
  CP.Position = from_json<Position>(Object.value("position"));
  if (Object.contains("context") && !Object.value("context").isNull())
    CP.Context = from_json<CompletionContext>(Object.value("context"));
}

void lsp::to_json(QJsonValue &Data, const CompletionItem<EmptyCompletionData> &Item) {
  QJsonObject Object;
  Object["label"] = Item.Label;
  Object["kind"] = static_cast<qint64>(Item.Kind);
  Data = Object;
}

/*
 * Language Features
 */
void lsp::to_json(QJsonValue &Data, const FormattingOptions &Item) {
  QJsonObject Object;
  Object["tabSize"] = static_cast<qint64>(Item.TabSize);
  Object["insertSpaces"] = Item.InsertSpaces;
  Data = Object;
}

void lsp::from_json(const QJsonValue &Data, FormattingOptions &Item) {
  const QJsonObject Object = as_object(Data);
  Item.TabSize = static_cast<unsigned>(Object.value("tabSize").toInteger());
  Item.InsertSpaces = Object.value("insertSpaces").toBool();
}

void lsp::to_json(QJsonValue &Data, const DocumentFormattingParams &Item) {
  QJsonObject Object;
  Object["textDocument"] = to_json(Item.TextDocument);
  Object["options"] = to_json(Item.Options);
  Data = Object;
}

void lsp::from_json(const QJsonValue &Data, DocumentFormattingParams &Item) {
  const QJsonObject Object = as_object(Data);
  Item.TextDocument =
      from_json<TextDocumentIdentifier>(Object.value("textDocument"));
  Item.Options = from_json<FormattingOptions>(Object.value("options"));
}
RequestType<DocumentFormattingParams, std::vector<TextEdit>>
    lsp::TextDocumentFormatting("textDocument/formatting");
