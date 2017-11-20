// SPDX-License-Identifier: AGPL-3.0-only
#include <iostream>
#include <string>

#include "MessageWriter.h"

using namespace lsp;

void StdoutMessageWriter::write(json &Data) {
    std::string Buf = Data.dump();

    std::cout.write(Buf.c_str(),Buf.size());
}