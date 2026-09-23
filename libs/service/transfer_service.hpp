#pragma once

#include <string>
#include <utility>

#include "../domain/transfer_job.hpp"
#include "../mapping/mapping_source.hpp"

namespace bucketmesh::service {

struct TransferRequest {
    std::string idempotency_key;
    std::string bucket;
    std::string object_name;
    std::string direction{"download"};
    int64_t total_bytes{0};
};

struct TransferOutcome {
    bool ok{false};
    std::string job_id;
    std::string status{ "pending" };
    std::string message;
};

class TransferService {
public:
    explicit TransferService(std::shared_ptr<bucketmesh::mapping::MappingSource> source)
        : source_(std::move(source)) {}

    TransferOutcome create_job(const TransferRequest& request) const {
        const std::string job_id = request.idempotency_key.empty()
            ? "job-generated"
            : request.idempotency_key;

        const bool ok = source_ != nullptr;
        return TransferOutcome{
            .ok = ok,
            .job_id = job_id,
            .status = ok ? "accepted" : "rejected",
            .message = ok ? "job accepted by mesh abstraction" : "no mapping source configured"
        };
    }

    std::string read_mapping_payload() const {
        if (!source_) {
            return "{}";
        }
        return source_->read_payload();
    }

private:
    std::shared_ptr<bucketmesh::mapping::MappingSource> source_;
};

} // namespace bucketmesh::service
