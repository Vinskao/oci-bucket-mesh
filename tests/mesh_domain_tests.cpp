#include <cassert>
#include <chrono>
#include <iostream>

#include "../libs/domain/job_repository.hpp"
#include "../libs/domain/transfer_job.hpp"

int main() {
    bucketmesh::domain::InMemoryJobRepository repository;

    bucketmesh::domain::TransferJob job{
        .job_id = "job-001",
        .idempotency_key = "idem-001",
        .direction = bucketmesh::domain::TransferDirection::Download,
        .status = bucketmesh::domain::TransferStatus::Pending,
        .bucket = "peoplesystem-research-zone",
        .object_name = "company-product-mapping.json",
        .total_bytes = 2048,
        .transferred_bytes = 0,
        .created_at = std::chrono::system_clock::now(),
        .updated_at = std::chrono::system_clock::now()
    };

    repository.save(job);

    auto found = repository.find_by_id("job-001");
    assert(found.has_value());
    assert(found->job_id == "job-001");
    assert(found->bucket == "peoplesystem-research-zone");

    auto idem = repository.find_by_idempotency_key("idem-001");
    assert(idem.has_value());
    assert(idem->job_id == "job-001");

    bool updated = repository.update_status("job-001", bucketmesh::domain::TransferStatus::InProgress);
    assert(updated);

    auto in_progress = repository.find_by_id("job-001");
    assert(in_progress.has_value());
    assert(in_progress->status == bucketmesh::domain::TransferStatus::InProgress);

    assert(bucketmesh::domain::status_to_string(bucketmesh::domain::TransferStatus::Completed) == "completed");
    assert(bucketmesh::domain::direction_to_string(bucketmesh::domain::TransferDirection::Upload) == "upload");

    std::cout << "bucketmesh_domain_tests: OK" << std::endl;
    return 0;
}
