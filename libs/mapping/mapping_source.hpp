#pragma once

#include <string>
#include <vector>

namespace bucketmesh::mapping {

struct MappingSourceConfig {
    std::string base_url{"https://mesh.internal/v1"};
    int timeout_ms{1000};
    bool use_mesh{true};
    bool fallback_to_oci{true};
    std::string bucket{"peoplesystem-research-zone"};
    std::string object_name{"company-product-mapping.json"};
};

class MappingSource {
public:
    virtual ~MappingSource() = default;
    virtual std::string read_payload() const = 0;
    virtual std::string source_name() const = 0;

    static std::vector<std::string> available_sources() {
        return {"mesh", "oci-sdk"};
    }
};

class MeshMappingSource : public MappingSource {
public:
    explicit MeshMappingSource(MappingSourceConfig config)
        : config_(std::move(config)) {}

    std::string read_payload() const override {
        return "{\"source\":\"mesh\",\"status\":\"mock\"}";
    }

    std::string source_name() const override {
        return "mesh";
    }

private:
    MappingSourceConfig config_;
};

class OciSdkMappingSource : public MappingSource {
public:
    explicit OciSdkMappingSource(MappingSourceConfig config)
        : config_(std::move(config)) {}

    std::string read_payload() const override {
        return "{\"source\":\"oci-sdk\",\"status\":\"mock\"}";
    }

    std::string source_name() const override {
        return "oci-sdk";
    }

private:
    MappingSourceConfig config_;
};

} // namespace bucketmesh::mapping
