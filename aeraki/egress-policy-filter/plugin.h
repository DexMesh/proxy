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
  Network::FilterStatus onNewConnection() override;
  Network::FilterStatus onNewConnection() { 
    std::cerr << "********egress policy filter onNewConnection***********";
    return Network::FilterStatus::Continue; 
  }
  void initializeReadFilterCallbacks(Network::ReadFilterCallbacks& callbacks) override {
    std::cerr << "********egress policy filter initializeReadFilterCallbacks***********";
  }

private:
  static const absl::string_view HTTP2_CONNECTION_PREFACE = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";

  ParseState parseHttpHeader(absl::string_view data);

  http_parser parser_;
  static http_parser_settings settings_;
};

} // namespace Filter
} // namespace Envoy
