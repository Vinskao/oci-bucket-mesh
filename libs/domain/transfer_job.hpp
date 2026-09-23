#pragma once

#include <string>
#include <chrono>
#include <cstdint>

namespace bucketmesh::domain {

enum class TransferDirection {
    Upload,
    Download
};

enum class TransferStatus {
    Pending,
    InProgress,
    Completed,
    Failed,
    Cancelled
};

inline std::string direction_to_string(TransferDirection direction) {
    switch (direction) {
        case TransferDirection::Upload: return "upload";
        case TransferDirection::Download: return "download";
    }
    return "unknown";
}

inline std::string status_to_string(TransferStatus status) {
    switch (status) {
        case TransferStatus::Pending: return "pending";
        case TransferStatus::InProgress: return "in_progress";
        case TransferStatus::Completed: return "completed";
        case TransferStatus::Failed: return "failed";
        case TransferStatus::Cancelled: return "cancelled";
    }
    return "unknown";
}

struct TransferJob {
    std::string job_id;
    std::string idempotency_key;
    TransferDirection direction;
    TransferStatus status{TransferStatus::Pending};
    std::string bucket;
    std::string object_name;
    int64_t total_bytes{0};
    int64_t transferred_bytes{0};
    std::string etag;
    std::string error_code;
    std::string error_message;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
};

struct ObjectMetadata {
    std::string bucket;
    std::string object_name;
    int64_t content_length{0};
    std::string content_type;
    std::string etag;
    std::string md5;
    std::chrono::system_clock::time_point last_modified;
};

} // namespace bucketmesh::domain
