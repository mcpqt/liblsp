// SPDX-License-Identifier: AGPL-3.0-only
#include <functional>
#include <iostream>
#include <istream>
#include <iterator>
#include <QString>

#include "jsonrpc/MessageReader.h"

using namespace lsp;

const char *LENGTH_HEADER = "Content-Length";

std::size_t MessageReader::getLength(const QString &Header) {
  const int Colon = Header.indexOf(':');
  const int Key = Header.indexOf("Content-Length");

  if (Colon == -1 || Key == -1 || Key + 14 != Colon) {
    return 0;
  }

  bool Ok = false;
  const ulong Length = Header.mid(Colon + 1).trimmed().toULong(&Ok);
  return Ok ? static_cast<std::size_t>(Length) : 0;
}