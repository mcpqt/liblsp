// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSPROTOCOL_H
#define LSPROTOCOL_H
#include "json.h"

#include <QString>

#include <cstdint>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace lsp {

using jsonrpc::as_object;
using jsonrpc::from_json;
using jsonrpc::to_json;

enum class TraceLevels { Off, Messages, Verbose };

inline QJsonValue to_json(TraceLevels Value) { return static_cast<int>(Value); }

inline void from_json(const QJsonValue &Data, TraceLevels &Value) {
  Value = static_cast<TraceLevels>(Data.toInt());
}

enum class ProtocolErrorCode {
  serverErrorStart = -32099,
  serverErrorEnd = -32000,
  ServerNotInitialized = -32002,
  UnknownErrorCode = -32001,

  // Defined by the protocol.
  RequestCancelled = -32800
};

struct EmptyResponse {
  QJsonValue dump() const { return QJsonValue(); }
};

struct Position {
  uint32_t Line;
  uint32_t Character;

  friend void to_json(QJsonValue &Data, const Position &P) {
    QJsonObject Object;
    Object["line"] = static_cast<qint64>(P.Line);
    Object["character"] = static_cast<qint64>(P.Character);
    Data = Object;
  }

  friend void from_json(const QJsonValue &Data, Position &P) {
    const QJsonObject Object = as_object(Data);
    P.Line = static_cast<uint32_t>(Object.value("line").toInteger());
    P.Character = static_cast<uint32_t>(Object.value("character").toInteger());
  }

  void parse(QJsonObject *Position);
  QJsonValue dump();
};

struct Range {
  Position Start;
  Position End;

  friend void to_json(QJsonValue &Data, const Range &R) {
    QJsonObject Object;
    Object["start"] = to_json(R.Start);
    Object["end"] = to_json(R.End);
    Data = Object;
  }

  friend void from_json(const QJsonValue &Data, Range &R) {
    const QJsonObject Object = as_object(Data);
    R.Start = from_json<Position>(Object.value("start"));
    R.End = from_json<Position>(Object.value("end"));
  }

  void parse(QJsonObject *Position);
  QJsonValue dump();
};

struct Location {
  Location(const QString &Uri, const lsp::Range &Range)
      : Uri(Uri), Range(Range) {}

  QString Uri;
  lsp::Range Range;

  friend void to_json(QJsonValue &Data, const Location &L) {
    QJsonObject Object;
    Object["uri"] = L.Uri;
    Object["range"] = to_json(L.Range);
    Data = Object;
  }

  QJsonValue dump();
};

enum class DiagnosticSeverity {
  Error = 1,
  Warning = 2,
  Information = 3,
  Hint = 4
};

inline QJsonValue to_json(DiagnosticSeverity Value) { return static_cast<int>(Value); }

inline void from_json(const QJsonValue &Data, DiagnosticSeverity &Value) {
  Value = static_cast<DiagnosticSeverity>(Data.toInt());
}

struct Diagnostic {
  lsp::Range Range;
  std::optional<DiagnosticSeverity> Severity = {};
  QString Code = {};
  QString Source = {};
  QString Message;

  Diagnostic() {}
  Diagnostic(const lsp::Range &Range, const QString &Message)
      : Range(Range), Message(Message) {}
  Diagnostic(const lsp::Range &Range, const QString &Message,
             lsp::DiagnosticSeverity Severity)
      : Range(Range), Message(Message), Severity(Severity) {}

  friend void to_json(QJsonValue &Data, const Diagnostic &Diag) {
    QJsonObject Object;
    Object["range"] = to_json(Diag.Range);
    Object["message"] = Diag.Message;
    if (Diag.Severity)
      Object["severity"] = to_json(*Diag.Severity);
    if (!Diag.Code.isNull())
      Object["code"] = Diag.Code;
    if (!Diag.Source.isNull())
      Object["source"] = Diag.Source;
    Data = Object;
  }

  friend void from_json(const QJsonValue &Data, Diagnostic &Diag) {
    const QJsonObject Object = as_object(Data);
    Diag.Range = from_json<lsp::Range>(Object.value("range"));
    Diag.Message = Object.value("message").toString();
    Diag.Severity.reset();
    if (Object.contains("severity") && !Object.value("severity").isNull())
      Diag.Severity = from_json<DiagnosticSeverity>(Object.value("severity"));
    Diag.Code = {};
    if (Object.contains("code") && !Object.value("code").isNull())
      Diag.Code = Object.value("code").toString();
    Diag.Source = {};
    if (Object.contains("source") && !Object.value("source").isNull())
      Diag.Source = Object.value("source").toString();
  }
};

struct PublishDiagnosticsParams {
  QString Uri;
  std::vector<Diagnostic> Diagnostics;

  friend void to_json(QJsonValue &Data, const PublishDiagnosticsParams &PDP) {
    QJsonObject Object;
    Object["uri"] = PDP.Uri;
    Object["diagnostics"] = to_json(PDP.Diagnostics);
    Data = Object;
  }
};

struct Command {
  QString Title;
  QString Cmd;
  std::optional<std::vector<QString>> Arguments = {};

  friend void to_json(QJsonValue &Data, const Command &C) {
    QJsonObject Object;
    Object["title"] = C.Title;
    Object["command"] = C.Cmd;
    if (C.Arguments)
      Object["arguments"] = to_json(*C.Arguments);
    Data = Object;
  }
};

struct TextEdit {
  lsp::Range Range;
  QString NewText;

  friend void to_json(QJsonValue &Data, const TextEdit &TE) {
    QJsonObject Object;
    Object["range"] = to_json(TE.Range);
    Object["newText"] = TE.NewText;
    Data = Object;
  }
};

struct TextDocumentIdentifier {
  QString Uri;

  friend void to_json(QJsonValue &Data, const TextDocumentIdentifier &I) {
    QJsonObject Object;
    Object["uri"] = I.Uri;
    Data = Object;
  }

  friend void from_json(const QJsonValue &Data, TextDocumentIdentifier &I) {
    const QJsonObject Object = as_object(Data);
    I.Uri = Object.value("uri").toString();
  }
};

struct VersionedTextDocumentIdentifier : public TextDocumentIdentifier {
  uint32_t Version;

  friend void to_json(QJsonValue &Data, const VersionedTextDocumentIdentifier &V) {
    QJsonObject Object;
    Object["uri"] = V.Uri;
    Object["version"] = static_cast<qint64>(V.Version);
    Data = Object;
  }

  friend void from_json(const QJsonValue &Data, VersionedTextDocumentIdentifier &V) {
    const QJsonObject Object = as_object(Data);
    V.Uri = Object.value("uri").toString();
    V.Version = static_cast<uint32_t>(Object.value("version").toInteger());
  }
};

struct TextDocumentEdit {
  lsp::VersionedTextDocumentIdentifier TextDocument;
  std::vector<lsp::TextEdit> Edits;

  friend void to_json(QJsonValue &Data, const TextDocumentEdit &Edit) {
    QJsonObject Object;
    Object["textDocument"] = to_json(Edit.TextDocument);
    Object["edits"] = to_json(Edit.Edits);
    Data = Object;
  }
};

struct WorkspaceEdit {
  std::optional<std::map<QString, std::vector<lsp::TextEdit>>> Changes = {};
  std::optional<std::vector<lsp::TextDocumentEdit>> DocumentChanges = {};

  friend void to_json(QJsonValue &Data, const WorkspaceEdit &WE) {
    QJsonObject Object;
    if (WE.Changes)
      Object["changes"] = to_json(*WE.Changes);
    if (WE.DocumentChanges)
      Object["documentChanges"] = to_json(*WE.DocumentChanges);
    Data = Object;
  }
};

struct TextDocumentItem {
  QString Uri;
  QString LanguageId;
  uint32_t Version;
  QString Text;

  friend void from_json(const QJsonValue &Data, TextDocumentItem &Item);
  friend void to_json(QJsonValue &Data, const TextDocumentItem &Item) {
    QJsonObject Object;
    Object["uri"] = Item.Uri;
    Object["languageId"] = Item.LanguageId;
    Object["version"] = static_cast<qint64>(Item.Version);
    Object["text"] = Item.Text;
    Data = Object;
  }
};
void from_json(const QJsonValue &Data, TextDocumentItem &Item);

struct DocumentFilter {
  QString Language = {};
  QString Scheme = {};
  QString Pattern = {};

  friend void to_json(QJsonValue &Data, const DocumentFilter &Filter) {
    QJsonObject Object;
    if (!Filter.Language.isNull())
      Object["language"] = Filter.Language;
    if (!Filter.Scheme.isNull())
      Object["scheme"] = Filter.Scheme;
    if (!Filter.Pattern.isNull())
      Object["pattern"] = Filter.Pattern;
    Data = Object;
  }
};

struct ClientCapabilities {

  friend void from_json(const QJsonValue &Data, ClientCapabilities &CC) {}
};
struct EmptyInitializationOptions {
  friend void from_json(const QJsonValue &Data,
                        EmptyInitializationOptions &Options){};
};

template <typename T> struct InitializeParams {
  uint32_t ProcessId;
  QString RootUri = "";
  TraceLevels TraceLevel = TraceLevels::Off;
  std::optional<T> InitializationOptions = {};
  ClientCapabilities Capabilities;

  friend void from_json(const QJsonValue &Data, InitializeParams<T> &Params) {
    const QJsonObject Object = as_object(Data);
    Params.ProcessId =
        static_cast<uint32_t>(Object.value("processId").toInteger());

    const QJsonValue Uri = Object.value("rootUri");
    if (!Uri.isUndefined() && !Uri.isNull()) {
      Params.RootUri = Uri.toString();
    } else {
      const QJsonValue Path = Object.value("rootPath");
      if (!Path.isUndefined() && !Path.isNull())
        Params.RootUri = Path.toString();
      else
        Params.RootUri = "";
    }

    const QJsonValue InitOptions = Object.value("initializationOptions");
    if (!InitOptions.isUndefined())
      Params.InitializationOptions = from_json<T>(InitOptions);

    Params.Capabilities = from_json<ClientCapabilities>(
        Object.value("capabilities"));

    const QJsonValue Trace = Object.value("traceLevel");
    if (!Trace.isUndefined())
      Params.TraceLevel = from_json<TraceLevels>(Trace);
  }
};

struct InitializedParams {
  friend void from_json(const QJsonValue &Data, InitializedParams &Params){};
};

enum class TextDocumentSyncKind { None = 0, Full = 1, Incremental = 2 };

inline QJsonValue to_json(TextDocumentSyncKind Value) {
  return static_cast<int>(Value);
}

inline void from_json(const QJsonValue &Data, TextDocumentSyncKind &Value) {
  Value = static_cast<TextDocumentSyncKind>(Data.toInt());
}

struct SaveOptions {
  bool IncluedeText = false;

  friend void to_json(QJsonValue &Data, const SaveOptions &O);
};
void to_json(QJsonValue &Data, const SaveOptions &O);

struct TextDocumentSyncOptions {
  bool OpenClose;
  TextDocumentSyncKind Change;
  bool WillSave;
  bool WillSaveWaitUntil;
  SaveOptions Save;

  friend void to_json(QJsonValue &Data, const TextDocumentSyncOptions &O);
};
void to_json(QJsonValue &Data, const TextDocumentSyncOptions &O);

struct CompletionOptions {
  bool ResolveProvider = false;
  std::vector<QString> TriggerCharacters;

  friend void to_json(QJsonValue &Data, const CompletionOptions &O);
};
void to_json(QJsonValue &Data, const CompletionOptions &O);

struct ServerCapabilities {
  TextDocumentSyncOptions TextDocumentSync;
  bool HoverProvider;
  CompletionOptions CompletionProvider;

  bool DocumentFormattingProvider;

  friend void to_json(QJsonValue &Data, const ServerCapabilities &SC) {
    QJsonObject Object;
    Object["textDocumentSync"] = to_json(SC.TextDocumentSync);
    Object["hoverProvider"] = SC.HoverProvider;
    Object["completionProvider"] = to_json(SC.CompletionProvider);
    Object["documentFormattingProvider"] = SC.DocumentFormattingProvider;
    Data = Object;
  }
};

struct InitializeResult {
  ServerCapabilities Capabilities;

  friend void to_json(QJsonValue &Data, const InitializeResult &IR) {
    QJsonObject Object;
    Object["capabilities"] = to_json(IR.Capabilities);
    Data = Object;
  }
};

struct LogMessageParams {
  enum class MessageType { Error = 1, Warning = 2, Info = 3, Log = 4 };

  MessageType type;
  QString message;

  friend void to_json(QJsonValue &Data, const LogMessageParams &Params);
  friend void from_json(const QJsonValue &Data, LogMessageParams &Params);
};

void to_json(QJsonValue &Data, const LogMessageParams &Params);
void from_json(const QJsonValue &Data, LogMessageParams &Params);

inline const QString ShowMessage = "window/showMessage";

struct TextDocumentPositionParams {
  lsp::TextDocumentIdentifier TextDocument;
  lsp::Position Position;

  friend void from_json(const QJsonValue &Params, TextDocumentPositionParams &TDP);
};
void from_json(const QJsonValue &Params, TextDocumentPositionParams &TDP);

struct TextDocumentDidOpenParams {
  TextDocumentItem TextDocument;

  friend void from_json(const QJsonValue &Params, TextDocumentDidOpenParams &TDP);
};
void from_json(const QJsonValue &Params, TextDocumentDidOpenParams &TDP);

inline const QString TextDocumentDidOpen = "textDocument/didOpen";

/*
 * Workspace changes
 */
struct TextDocumentContentChangeEvent {
  std::optional<lsp::Range> Range = {};
  std::optional<uint32_t> RangeLength = {};
  QString Text;

  friend void from_json(const QJsonValue &Params,
                        TextDocumentContentChangeEvent &DCP);
};
void from_json(const QJsonValue &Params, TextDocumentContentChangeEvent &DCP);

struct TextDocumentDidChangeParams {
  lsp::VersionedTextDocumentIdentifier TextDocument;
  std::vector<lsp::TextDocumentContentChangeEvent> ContentChanges;

  friend void from_json(const QJsonValue &Params, TextDocumentDidChangeParams &DCP);
};
void from_json(const QJsonValue &Params, TextDocumentDidChangeParams &DCP);
inline const QString TextDocumentDidChange = "textDocument/didChange";

enum class MarkupKind { Plaintext, Markdown };

inline QJsonValue to_json(MarkupKind Value) { return static_cast<int>(Value); }

inline void from_json(const QJsonValue &Data, MarkupKind &Value) {
  Value = static_cast<MarkupKind>(Data.toInt());
}

struct MarkupContent {
  MarkupContent() {}
  MarkupContent(const QString &Value) : Value(Value){};
  MarkupContent(const QString &Value, const QString &Language)
      : Value(Value), Language(Language){};

  QString Value = "";
  QString Language = "";
  lsp::MarkupKind Kind = lsp::MarkupKind::Plaintext;

  friend void to_json(QJsonValue &Data, const MarkupContent &MC) {
    QJsonObject Object;
    Object["value"] = MC.Value;
    Object["language"] = MC.Language;
    Object["kind"] = to_json(MC.Kind);
    Data = Object;
  }
};

struct Hover {
  lsp::MarkupContent Contents;
  std::optional<lsp::Range> Range = {};

  Hover() {}

  friend void to_json(QJsonValue &Data, const Hover &H) {
    QJsonObject Object;
    Object["contents"] = to_json(H.Contents);
    if (H.Range.has_value())
      Object["range"] = to_json(H.Range.value());
    Data = Object;
  }
};

enum class CompletionTriggerKind { Invoked = 1, TriggerCharacter = 2 };

enum class InsertTextFormat { PlainText = 1, Snippet = 2 };

inline QJsonValue to_json(CompletionTriggerKind Value) {
  return static_cast<int>(Value);
}
inline void from_json(const QJsonValue &Data, CompletionTriggerKind &Value) {
  Value = static_cast<CompletionTriggerKind>(Data.toInt());
}
inline QJsonValue to_json(InsertTextFormat Value) { return static_cast<int>(Value); }
inline void from_json(const QJsonValue &Data, InsertTextFormat &Value) {
  Value = static_cast<InsertTextFormat>(Data.toInt());
}

struct CompletionContext {
  CompletionTriggerKind TriggerKind;
  QString TriggerCharacter;

  friend void from_json(const QJsonValue &Data, CompletionContext &CC);
};
void from_json(const QJsonValue &Data, CompletionContext &CC);

struct CompletionParams : public TextDocumentPositionParams {
  std::optional<CompletionContext> Context = {};

  friend void from_json(const QJsonValue &Data, CompletionParams &CP);
};
void from_json(const QJsonValue &Data, CompletionParams &CP);

struct EmptyCompletionData {};

template <typename T> struct CompletionItem {
  QString Label;
  uint32_t Kind = 1;

  QString Detail = {};
  std::optional<lsp::MarkupContent> Documentation = {};
  QString SortText = {};
  QString FilterText = {};
  QString InsertText = {};
  lsp::InsertTextFormat InsertTextFormat;
  std::optional<lsp::TextEdit> TextEdit = {};
  std::vector<lsp::TextEdit> AdditionalTextEdits;
  std::vector<QString> CommitCharacters;
  lsp::Command Command;
  T Data;

  friend void to_json(QJsonValue &Data,
                      const CompletionItem<EmptyCompletionData> &Item);
};
void to_json(QJsonValue &Data, const CompletionItem<EmptyCompletionData> &Item);

inline const QString InitializeRequest = "initialize";

/*
 * Language feautures
 */

// TextDocument formatting
struct FormattingOptions {
  unsigned TabSize;
  bool InsertSpaces;

  friend void to_json(QJsonValue &Data, const FormattingOptions &Item);
  friend void from_json(const QJsonValue &Data, FormattingOptions &Item);
};
void to_json(QJsonValue &Data, const FormattingOptions &Item);
void from_json(const QJsonValue &Data, FormattingOptions &Item);

struct DocumentFormattingParams {
  TextDocumentIdentifier TextDocument;
  FormattingOptions Options;

  friend void to_json(QJsonValue &Data, const DocumentFormattingParams &Item);
  friend void from_json(const QJsonValue &Data, DocumentFormattingParams &Item);
};
void to_json(QJsonValue &Data, const DocumentFormattingParams &Item);
void from_json(const QJsonValue &Data, DocumentFormattingParams &Item);

inline const QString TextDocumentFormatting = "textDocument/formatting";

} // namespace lsp

#endif // !LSPROTOCOL_H
