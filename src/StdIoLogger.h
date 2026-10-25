// SPDX-License-Identifier: AGPL-3.0-only
class StdIoLogger : public Logger {
public:
  virtual void logError(const QString &Message) {
    std::cerr << "Error: " << Message.toUtf8().constData() << std::endl;
  }
  virtual void logInfo(const QString &Message) {
    std::cout << "Info: " << Message.toUtf8().constData() << std::endl;
  }
  virtual void logWarning(const QString &Message) {
    std::cerr << "Warning: " << Message.toUtf8().constData() << std::endl;
  }
};