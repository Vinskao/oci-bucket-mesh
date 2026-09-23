#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../../libs/config/mesh_config.hpp"
#include "../../libs/domain/job_repository.hpp"
#include "../../libs/domain/transfer_job.hpp"
#include "../../libs/oci_adapter/oci_client.hpp"

namespace {

void print_banner(const bucketmesh::config::MeshConfig& config) {
    std::cout << "[OciBucketMesh] service=" << config.service_name
              << " version=" << config.service_version
              << " namespace=" << config.namespace_name << '\n';
    std::cout << "[OciBucketMesh] initial posture: single replica, ClusterIP only, emptyDir staging" << '\n';
}

bool verify_transfer_lifecycle() {
    bucketmesh::domain::InMemoryJobRepository repository;

    bucketmesh::domain::TransferJob job{
        .job_id = "job-sample-001",
        .idempotency_key = "idem-sample-001",
        .direction = bucketmesh::domain::TransferDirection::Download,
        .status = bucketmesh::domain::TransferStatus::Pending,
        .bucket = "peoplesystem-research-zone",
        .object_name = "company-product-mapping.json",
        .total_bytes = 1024,
        .transferred_bytes = 0,
        .created_at = std::chrono::system_clock::now(),
        .updated_at = std::chrono::system_clock::now()
    };

    repository.save(job);
    auto found = repository.find_by_id(job.job_id);
    if (!found || found->bucket != job.bucket) {
        return false;
    }

    auto idem = repository.find_by_idempotency_key(job.idempotency_key);
    if (!idem || idem->job_id != job.job_id) {
        return false;
    }

    if (!repository.update_status(job.job_id, bucketmesh::domain::TransferStatus::Completed,
                                  "OK", "verified")) {
        return false;
    }

    auto final_job = repository.find_by_id(job.job_id);
    return final_job.has_value() &&
           final_job->status == bucketmesh::domain::TransferStatus::Completed &&
           final_job->error_code == "OK";
}

} // namespace

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    const auto config = bucketmesh::config::default_config();
    print_banner(config);

    const bool ok = verify_transfer_lifecycle();
    if (!ok) {
        std::cerr << "[OciBucketMesh] transfer lifecycle verification failed" << std::endl;
        return 1;
    }

    const bucketmesh::oci::OciSigner signer{};
    const std::string signing_input = signer.build_signing_string(
        "Tue, 23 Sep 2026 00:00:00 GMT",
        "GET",
        "/n/objectstorage/ap-tokyo-1/buckets/peoplesystem-research-zone/objects/company-product-mapping.json",
        "objectstorage.ap-tokyo-1.oraclecloud.com");

    std::cout << "[OciBucketMesh] OCI signing input preview: " << signing_input.substr(0, 120) << "..." << std::endl;
    std::cout << "[OciBucketMesh] healthz/readyz/metrics endpoints are defined for the next service implementation pass." << std::endl;
    std::cout << "[OciBucketMesh] runtime is ready for job admission and transfer coordination." << std::endl;
    return 0;
}
