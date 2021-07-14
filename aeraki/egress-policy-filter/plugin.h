#pragma once
#include <http_parser.h>
#include "envoy/network/filter.h"

#include "common/common/logger.h"

namespace Envoy {
namespace Filter {

enum class ParseState {
  // Parse result is out. It could be http family or empty.
  Done,
  // Parser expects more data.
  Continue,
  // Parser reports unrecoverable error.
  Error
};

/**
 * Implementation of a basic egressPolicyFilterConfigFactory filter.
 */
class EgressPolicyFilter : public Network::ReadFilter, Logger::Loggable<Logger::Id::filter> {
public:
  // Network::ReadFilter
  Network::FilterStatus onData(Buffer::Instance& data, bool end_stream) override;
  Network::FilterStatus onNewConnection() override { return Network::FilterStatus::Continue; }
  void initializeReadFilterCallbacks(Network::ReadFilterCallbacks& callbacks) override {
    read_callbacks_ = &callbacks;
  }

private:
  Network::ReadFilterCallbacks* read_callbacks_{};

  static const absl::string_view HTTP2_CONNECTION_PREFACE;

  ParseState parseHttpHeader(absl::string_view data);

  absl::string_view protocol_;
  http_parser parser_;
  static http_parser_settings settings_;
};

} // namespace Filter
} // namespace Envoy
