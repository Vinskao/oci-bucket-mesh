#pragma once

#include <string>
#include <optional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "transfer_job.hpp"

namespace bucketmesh::domain {

class JobRepository {
public:
    virtual ~JobRepository() = default;
    virtual void save(const TransferJob& job) = 0;
    virtual std::optional<TransferJob> find_by_id(const std::string& job_id) = 0;
    virtual std::optional<TransferJob> find_by_idempotency_key(const std::string& key) = 0;
    virtual bool update_status(const std::string& job_id, TransferStatus status,
                               const std::string& error_code = "",
                               const std::string& error_message = "") = 0;
};

class InMemoryJobRepository : public JobRepository {
public:
    void save(const TransferJob& job) override {
        std::lock_guard<std::mutex> lock(mutex_);
        jobs_[job.job_id] = job;
        if (!job.idempotency_key.empty()) {
            idempotency_index_[job.idempotency_key] = job.job_id;
        }
    }

    std::optional<TransferJob> find_by_id(const std::string& job_id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = jobs_.find(job_id);
        if (it != jobs_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<TransferJob> find_by_idempotency_key(const std::string& key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it_idx = idempotency_index_.find(key);
        if (it_idx != idempotency_index_.end()) {
            auto it_job = jobs_.find(it_idx->second);
            if (it_job != jobs_.end()) {
                return it_job->second;
            }
        }
        return std::nullopt;
    }

    bool update_status(const std::string& job_id, TransferStatus status,
                       const std::string& error_code = "",
                       const std::string& error_message = "") override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = jobs_.find(job_id);
        if (it == jobs_.end()) {
            return false;
        }
        it->second.status = status;
        it->second.error_code = error_code;
        it->second.error_message = error_message;
        it->second.updated_at = std::chrono::system_clock::now();
        return true;
    }

private:
    std::mutex mutex_;
    std::unordered_map<std::string, TransferJob> jobs_;
    std::unordered_map<std::string, std::string> idempotency_index_;
};

} // namespace bucketmesh::domain
