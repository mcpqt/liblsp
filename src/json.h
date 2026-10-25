// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_JSON_H
#define LSP_JSON_H

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace lsp {

inline QJsonObject as_object(const QJsonValue &Value) {
  if (Value.isNull() || Value.isUndefined())
    return QJsonObject();
  if (!Value.isObject())
    throw std::runtime_error("expected a JSON object");
  return Value.toObject();
}

inline QJsonArray as_array(const QJsonValue &Value) {
  if (!Value.isArray())
    throw std::runtime_error("expected a JSON array");
  return Value.toArray();
}

inline QJsonValue to_json(std::nullptr_t) { return QJsonValue(QJsonValue::Null); }
inline QJsonValue to_json(const QJsonValue &Value) { return Value; }
inline QJsonValue to_json(const QJsonObject &Value) { return Value; }
inline QJsonValue to_json(const QJsonArray &Value) { return Value; }
inline QJsonValue to_json(bool Value) { return QJsonValue(Value); }
inline QJsonValue to_json(int Value) { return QJsonValue(Value); }
inline QJsonValue to_json(unsigned Value) {
  return QJsonValue(static_cast<qint64>(Value));
}
inline QJsonValue to_json(qint64 Value) { return QJsonValue(Value); }
inline QJsonValue to_json(double Value) { return QJsonValue(Value); }
inline QJsonValue to_json(const QString &Value) { return QJsonValue(Value); }
inline QJsonValue to_json(const char *Value) {
  return QJsonValue(QString::fromUtf8(Value));
}

template <class T> QJsonValue to_json(const std::optional<T> &Value) {
  if (!Value.has_value())
    return QJsonValue(QJsonValue::Null);
  return to_json(*Value);
}

template <class T> QJsonValue to_json(const std::vector<T> &Value) {
  QJsonArray Array;
  for (const auto &Element : Value)
    Array.append(to_json(Element));
  return Array;
}

template <class K, class V> QJsonValue to_json(const std::map<K, V> &Value) {
  QJsonObject Object;
  for (const auto &Entry : Value)
    Object.insert(Entry.first, to_json(Entry.second));
  return Object;
}

template <class T> QJsonValue to_json(const T &Value) {
  QJsonValue Result;
  to_json(Result, Value);
  return Result;
}

inline void from_json(const QJsonValue &Value, QJsonValue &Out) { Out = Value; }

inline void from_json(const QJsonValue &Value, bool &Out) {
  if (!Value.isBool())
    throw std::runtime_error("expected a boolean");
  Out = Value.toBool();
}

inline void from_json(const QJsonValue &Value, int &Out) {
  if (!Value.isDouble())
    throw std::runtime_error("expected an integer");
  Out = static_cast<int>(Value.toInteger());
}

inline void from_json(const QJsonValue &Value, uint32_t &Out) {
  if (!Value.isDouble())
    throw std::runtime_error("expected an integer");
  Out = static_cast<uint32_t>(Value.toInteger());
}

inline void from_json(const QJsonValue &Value, qint64 &Out) {
  if (!Value.isDouble())
    throw std::runtime_error("expected an integer");
  Out = Value.toInteger();
}

inline void from_json(const QJsonValue &Value, double &Out) {
  if (!Value.isDouble())
    throw std::runtime_error("expected a number");
  Out = Value.toDouble();
}

inline void from_json(const QJsonValue &Value, QString &Out) {
  if (!Value.isString())
    throw std::runtime_error("expected a string");
  Out = Value.toString();
}

inline void from_json(const QJsonValue &Value, QJsonObject &Out) {
  Out = as_object(Value);
}

inline void from_json(const QJsonValue &Value, QJsonArray &Out) {
  Out = as_array(Value);
}

template <class T> void from_json(const QJsonValue &Value, std::optional<T> &Out) {
  if (Value.isNull() || Value.isUndefined()) {
    Out.reset();
    return;
  }
  T Decoded{};
  from_json(Value, Decoded);
  Out = std::move(Decoded);
}

template <class T> void from_json(const QJsonValue &Value, std::vector<T> &Out) {
  const QJsonArray Array = as_array(Value);
  Out.clear();
  Out.reserve(static_cast<size_t>(Array.size()));
  for (const auto &Element : Array) {
    T Decoded{};
    from_json(Element, Decoded);
    Out.push_back(std::move(Decoded));
  }
}

template <class K, class V>
void from_json(const QJsonValue &Value, std::map<K, V> &Out) {
  const QJsonObject Object = as_object(Value);
  Out.clear();
  for (auto It = Object.begin(); It != Object.end(); ++It) {
    V Decoded{};
    from_json(It.value(), Decoded);
    Out.emplace(It.key(), std::move(Decoded));
  }
}

template <class T> T from_json(const QJsonValue &Value) {
  T Out{};
  from_json(Value, Out);
  return Out;
}

inline QJsonValue parse_json(const QByteArray &Body) {
  QJsonParseError Error;
  const QJsonDocument Document = QJsonDocument::fromJson(Body, &Error);
  if (Error.error != QJsonParseError::NoError)
    throw std::runtime_error(Error.errorString().toUtf8().constData());
  if (Document.isObject())
    return Document.object();
  if (Document.isArray())
    return Document.array();
  throw std::runtime_error("JSON document is neither an object nor an array");
}

inline QByteArray dump_json(const QJsonValue &Value) {
  QJsonDocument Document;
  if (Value.isObject())
    Document = QJsonDocument(Value.toObject());
  else if (Value.isArray())
    Document = QJsonDocument(Value.toArray());
  else {
    QJsonObject Wrapper;
    Wrapper.insert("value", Value);
    Document = QJsonDocument(Wrapper);
  }
  return Document.toJson(QJsonDocument::Compact);
}

} // namespace lsp

#endif
