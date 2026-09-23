# Technology selection

## Baseline

| Area | Selection | Reason |
| --- | --- | --- |
| Language | C++20 | `std::jthread`, `stop_token`, ranges and coroutines are mature enough while retaining broad ARM Linux support. |
| Build | CMake >= 3.25 + vcpkg manifest mode | Reproducible dependency declaration without vendoring libraries. |
| Platform | Linux/ARM64 OCI worker image | Matches the current two OKE ARM nodes. |
| Public edge | Existing Spring Cloud Gateway | Preserves existing JWT, CORS, ingress and external rate-limit ownership. |

## Libraries approved for the implementation phase

| Concern | Library | Boundary / decision |
| --- | --- | --- |
| REST control API and streaming server | Drogon | Gateway-to-Mesh HTTP API; do not use it as the public ingress. |
| Internal typed API | gRPC + Protocol Buffers | Mesh-to-service contracts and future streaming control calls. |
| OCI native REST transport | libcurl + OpenSSL | OCI has no official C++ Object Storage SDK in this project; implement OCI request signing behind one adapter. Do not use the S3 compatibility API unless the credential model is explicitly approved. |
| Transfer scheduling | Boost.Asio plus `std::jthread` / `stop_token` | Bounded executor and cancellation. A custom lock-free queue is explicitly deferred. |
| RabbitMQ | rabbitmq-c | Publish versioned completion/failure events only after durable job state is updated. |
| Redis | redis-plus-plus + hiredis | Rate counters, idempotency and short-lived job state; not a large-object cache. |
| Authentication | jwt-cpp + OpenSSL | Validate service/JWT assertions where required; never pass trust solely through an arbitrary header. |
| Serialization/configuration | nlohmann-json + yaml-cpp | JSON transport and configuration parsing. |
| Logs/metrics | spdlog + prometheus-cpp | Structured logs, Prometheus metrics and transfer-level observability. |

## Explicit non-selections

- No sidecar: caches and admission limits must be shared, not copied per caller Pod.
- No custom memory pool in v1: bounded buffers and streaming are the first correctness requirement.
- No `std::async`: lifecycle, cancellation, overload behavior and resource limits are too implicit for a long-running service.
- No `mmap` as the network transfer strategy: it is only relevant after data is deliberately staged to a local file.
- No full-object memory cache: cache metadata and small hot control artifacts only after a measured need.
- No direct OCI credentials in Kubernetes Secrets: use a dedicated OCI workload identity or a reviewed equivalent.

## Dependency adoption gate

Before the first implementation target, pin vcpkg baseline/versions, build a multi-architecture container for `linux/arm64`, and run a minimal OCI-authentication proof against a non-production bucket.
