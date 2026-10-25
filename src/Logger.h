// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_LOGGER_H
#define LSP_LOGGER_H

#include <QString>

namespace lsp {

/// Generic logger
/// The class is handling the logging of messages with different types
class Logger {
public:
  /// Log an error message
  virtual void error(const QString &Message) const = 0;

  /// Log an info message
  virtual void info(const QString &Message) const = 0;

  /// Log a warning message
  virtual void warning(const QString &Message) const = 0;

  /// Log a generic message
  virtual void log(const QString &Message) const = 0;
};

class NullLogger : public Logger {
public:
  /// Log an error message
  virtual void error(const QString &Message) const {}

  /// Log an info message
  virtual void info(const QString &Message) const  {}

  /// Log a warning message
  virtual void warning(const QString &Message) const  {}

  /// Log a generic message
  virtual void log(const QString &Message) const  {}
};
}
#endif