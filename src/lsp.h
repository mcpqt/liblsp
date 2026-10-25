// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_LSP_H
#define LSP_LSP_H

#include "jsonrpc/MessageConnection.h"

#include <QString>

namespace lsp {

MessageConnection *createStdIoConnection();
MessageConnection *createQtTCPConnection(const QString &Hostname, quint16 Port);

} // namespace lsp
#endif
