// SPDX-License-Identifier: AGPL-3.0-only
#ifndef LSP_LSP_H
#define LSP_LSP_H

#include "rpc/session.h"

#include <QString>
#include <memory>

namespace lsp {

std::unique_ptr<Session> createStdIoConnection();
std::unique_ptr<Session> createQtTCPConnection(const QString &Hostname, quint16 Port);

} // namespace lsp
#endif
