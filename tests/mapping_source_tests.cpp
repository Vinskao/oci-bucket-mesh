#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../libs/mapping/mapping_source.hpp"

int main() {
    bucketmesh::mapping::MappingSourceConfig config{
        .base_url = "https://mesh.internal/v1",
        .timeout_ms = 1500,
        .use_mesh = true,
        .fallback_to_oci = true,
        .bucket = "peoplesystem-research-zone",
        .object_name = "company-product-mapping.json"
    };

    bucketmesh::mapping::MeshMappingSource mesh(config);
    bucketmesh::mapping::OciSdkMappingSource oci(config);

    auto mesh_payload = mesh.read_payload();
    assert(mesh_payload == "{\"source\":\"mesh\",\"status\":\"mock\"}");

    auto oci_payload = oci.read_payload();
    assert(oci_payload == "{\"source\":\"oci-sdk\",\"status\":\"mock\"}");

    auto available = bucketmesh::mapping::MappingSource::available_sources();
    assert(available.size() == 2);

    std::cout << "mapping_source_tests: OK" << std::endl;
    return 0;
}
