# API and event boundaries

## Ownership

| Interface | Owner | Consumers | Purpose |
| --- | --- | --- | --- |
| Public REST | Gateway | Frontend | Authentication, public route and coarse rate limit. |
| Mesh REST | OciBucketMesh | Gateway | Job creation/status and small control operations. |
| Mesh gRPC | OciBucketMesh | Backend/Consumer, future internal services | Typed internal control and streaming contracts. |
| `bucket.transfer.completed.v1` | OciBucketMesh | Consumer | Notify a committed, verified object ready for business processing. |
| `bucket.transfer.failed.v1` | OciBucketMesh | Consumer/operations | Notify terminal failures with safe error classification. |

## Contract rules

- Every mutating request requires an idempotency key.
- Object keys are server-authorized prefixes, never arbitrary user-supplied bucket/key pairs.
- The event payload contains object metadata, checksum/ETag, job ID and correlation ID—not object content and not credentials.
- The Mesh API does not expose bucket deletion, bucket IAM management, or arbitrary lifecycle-policy changes in v1.
- API and event schemas are versioned from their first release.

## First endpoints to specify, not implement

```text
POST   /v1/transfers                 create transfer job
GET    /v1/transfers/{jobId}         read job status
POST   /v1/transfers/{jobId}/cancel  request cancellation
POST   /v1/objects/{objectId}/download-url
GET    /healthz
GET    /readyz
GET    /metrics
```
