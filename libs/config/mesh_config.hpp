#pragma once

#include <string>
#include <utility>

namespace bucketmesh::config {

struct MeshConfig {
    std::string service_name{"oci-bucket-mesh"};
    std::string service_version{"0.1.0"};
    std::string namespace_name{"bucket-mesh"};
    std::string redis_url{"redis://localhost:6379/0"};
    std::string rabbitmq_url{"amqp://guest:guest@localhost:5672"};
    int max_concurrent_transfers{4};
    int64_t max_staging_bytes{268435456};
    bool require_idempotency{true};
    bool ready_for_scaling{false};
};

inline MeshConfig default_config() {
    return MeshConfig{};
}

} // namespace bucketmesh::config
