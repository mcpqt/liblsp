// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_LSP_H
#define LSP_LSP_H

#include "session.h"

#include <QString>
#include <memory>

namespace lsp {

using Session = jsonrpc::Session;

std::unique_ptr<Session> createStdIoConnection();
std::unique_ptr<Session> createTCPConnection(const QString &Hostname, quint16 Port);

} // namespace lsp
#endif
