#include <string>

#include "plugin.h"

#include "envoy/registry/registry.h"
#include "envoy/server/filter_config.h"

namespace Envoy {
namespace Server {
namespace Configuration {

/**
 * Config registration for the egressPolicyFilter filter. @see NamedNetworkFilterConfigFactory.
 */
class EgressPolicyFilterConfigFactory : public NamedNetworkFilterConfigFactory {
public:
  Network::FilterFactoryCb createFilterFactoryFromProto(const Protobuf::Message&,
                                                        FactoryContext&) override {
    return [](Network::FilterManager& filter_manager) -> void {
      filter_manager.addReadFilter(Network::ReadFilterSharedPtr{new Filter::EgressPolicyFilter()});
    };
  }

  ProtobufTypes::MessagePtr createEmptyConfigProto() override {
    return ProtobufTypes::MessagePtr{new Envoy::ProtobufWkt::Struct()};
  }

  std::string name() const override { return "egressPolicyFilter"; }

  bool isTerminalFilter() override { return false; }
};

/**
 * Static registration for the egressPolicyFilter filter. @see RegisterFactory.
 */
static Registry::RegisterFactory<EgressPolicyFilterConfigFactory, NamedNetworkFilterConfigFactory> registered_;

} // namespace Configuration
} // namespace Server
} // namespace Envoy
