#include "plugin.h"

#include "envoy/buffer/buffer.h"
#include "envoy/network/connection.h"

#include "common/common/assert.h"

namespace Envoy {
namespace Filter {

const absl::string_view EgressPolicyFilter::HTTP2_CONNECTION_PREFACE = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";

http_parser_settings EgressPolicyFilter::settings_{
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
};

Network::FilterStatus EgressPolicyFilter::onData(Buffer::Instance& data, bool) {
  http_parser_init(&parser_, HTTP_REQUEST);
  ENVOY_CONN_LOG(error, "***egress policy filter: got {} bytes\n", read_callbacks_->connection(), data.length());
  //read_callbacks_->connection().write(data, false);
  std::cerr << "********egress policy filter onData***********\n";
  
   std::cerr << "********data ***********" << data.toString() << "\n";
      parseHttpHeader(data.toString());
  return Network::FilterStatus::Continue;
}

ParseState EgressPolicyFilter::parseHttpHeader(absl::string_view data) {
   std::cerr << "********data ***********" << data << "\n";
  const size_t len = std::min(data.length(), EgressPolicyFilter::HTTP2_CONNECTION_PREFACE.length());
  if (EgressPolicyFilter::HTTP2_CONNECTION_PREFACE.compare(0, len, data, 0, len) == 0) {
    if (data.length() < EgressPolicyFilter::HTTP2_CONNECTION_PREFACE.length()) {
      return ParseState::Continue;
    }
    ENVOY_LOG(trace, "http inspector: http2 connection preface found");
    protocol_ = "HTTP/2";
    std::cerr << "********egress policy filter HTTP/2 ***********\n";
    return ParseState::Done;
  } else {
    absl::string_view new_data = data.substr(parser_.nread);
    const size_t pos = new_data.find_first_of("\r\n");

    if (pos != absl::string_view::npos) {
      // Include \r or \n
      new_data = new_data.substr(0, pos + 1);
      ssize_t rc = http_parser_execute(&parser_, &settings_, new_data.data(), new_data.length());
      ENVOY_LOG(trace, "http inspector: http_parser parsed {} chars, error code: {}", rc,
                HTTP_PARSER_ERRNO(&parser_));

      // Errors in parsing HTTP.
      if (HTTP_PARSER_ERRNO(&parser_) != HPE_OK && HTTP_PARSER_ERRNO(&parser_) != HPE_PAUSED) {
        return ParseState::Error;
      }

      if (parser_.http_major == 1 && parser_.http_minor == 1) {
        protocol_ = "http1";//Http::Headers::get().ProtocolStrings.Http11String;
      } else {
        // Set other HTTP protocols to HTTP/1.0
        protocol_ = "http0";//Http::Headers::get().ProtocolStrings.Http10String;
        std::cerr << "********egress policy filter HTTP1 ***********\n";
      }
      return ParseState::Done;
    } else {
      ssize_t rc = http_parser_execute(&parser_, &settings_, new_data.data(), new_data.length());
      ENVOY_LOG(trace, "http inspector: http_parser parsed {} chars, error code: {}", rc,
                HTTP_PARSER_ERRNO(&parser_));

      // Errors in parsing HTTP.
      if (HTTP_PARSER_ERRNO(&parser_) != HPE_OK && HTTP_PARSER_ERRNO(&parser_) != HPE_PAUSED) {
        return ParseState::Error;
      } else {
        return ParseState::Continue;
      }
    }
  }
}

} // namespace Filter
} // namespace Envoy
