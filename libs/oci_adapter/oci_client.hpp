#pragma once

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <cstdint>
#include "../domain/transfer_job.hpp"

namespace bucketmesh::oci {

struct OciConfig {
    std::string tenancy_id;
    std::string user_id;
    std::string fingerprint;
    std::string private_key_pem;
    std::string region{"ap-tokyo-1"};
    std::string namespace_name;
    bool use_instance_principal{false};
};

struct PutObjectRequest {
    std::string bucket;
    std::string object_name;
    std::string content_type{"application/octet-stream"};
    std::vector<uint8_t> body;
};

struct PutObjectResponse {
    std::string etag;
    std::string opc_request_id;
};

struct GetObjectRequest {
    std::string bucket;
    std::string object_name;
    std::optional<std::string> byte_range; // e.g., "bytes=0-1024"
};

struct GetObjectResponse {
    int status_code{200};
    std::string content_type;
    int64_t content_length{0};
    std::string etag;
    std::vector<uint8_t> body;
};

class OciClient {
public:
    virtual ~OciClient() = default;
    virtual PutObjectResponse put_object(const PutObjectRequest& req) = 0;
    virtual GetObjectResponse get_object(const GetObjectRequest& req) = 0;
    virtual domain::ObjectMetadata get_object_metadata(const std::string& bucket, const std::string& object_name) = 0;
};

/**
 * Helper to generate an OCI REST API Authorization header for HTTP request signing.
 * format: Signature version="1",keyId="...",algorithm="rsa-sha256",headers="...",signature="..."
 */
class OciSigner {
public:
    static std::string build_key_id(const std::string& tenancy_id,
                                    const std::string& user_id,
                                    const std::string& fingerprint) {
        return tenancy_id + "/" + user_id + "/" + fingerprint;
    }

    static std::string build_signing_string(const std::string& date,
                                            const std::string& method,
                                            const std::string& target_path,
                                            const std::string& host) {
        std::string result;
        result += "(request-target): ";
        result += method;
        result += " ";
        result += target_path;
        result += "\n";
        result += "date: ";
        result += date;
        result += "\n";
        result += "host: ";
        result += host;
        return result;
    }
};

} // namespace bucketmesh::oci
