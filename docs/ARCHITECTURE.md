# Architecture

## Pod responsibility

One OciBucketMesh Pod is a **controlled Object Storage data plane**:

1. validates the caller identity and request limits;
2. admits or rejects a transfer before allocating buffers or disk;
3. streams bytes between a client/internal caller and OCI Object Storage;
4. persists job metadata and publishes completed-work events; and
5. exposes health, readiness, metrics, and an operator-safe job view.

It does not own business transformations, AI inference, the canonical catalogue of application data, or Object Storage lifecycle policies.

## Execution paths

### Synchronous read

`Gateway -> Mesh -> OCI GetObject (range capable) -> Mesh -> caller`

The response is streamed. A full object must not be accumulated in RAM.

### Asynchronous write

`Gateway -> Mesh create job -> staged multipart transfer -> OCI commit -> RabbitMQ event -> Consumer`

The job record is durable before an event can be emitted. Consumer-side processing must be idempotent because delivery is at-least-once.

### Browser direct transfer, later phase

For very large client uploads/downloads, Mesh may issue narrowly-scoped, short-lived pre-authenticated/signed requests. It still owns authorization, audit records, object-name policy, and completion verification; it does not proxy the bytes.

## State ownership

| State | Owner | Initial location |
| --- | --- | --- |
| Transfer/job status, idempotency key, rate counters | Mesh | Redis; move durable job ledger to PostgreSQL before replicas > 1 |
| In-flight chunks | Mesh | `emptyDir`, bounded by an ephemeral-storage limit |
| Object bytes | OCI Object Storage | Private buckets |
| Business processing result | Consumer/domain service | Existing service stores |

## Kubernetes posture

- namespace: create a dedicated `bucket-mesh` namespace at deployment time;
- workload: one-replica Deployment initially, ClusterIP Service only, no Ingress;
- authentication: Gateway remains the public JWT boundary; Mesh verifies a service credential and later may also validate JWT audience/claims;
- authorization: dedicated OCI workload/service identity, least-privilege policies per bucket and operation; do not reuse the current node-wide Instance Principal permission;
- staging: `emptyDir` with `sizeLimit`, `ephemeral-storage` requests/limits, and automatic cleanup; do not attach the existing shared RWO PVC;
- scale-out prerequisite: durable, shared job state plus resumable multipart state and explicit shard/lease ownership.

## Initial operating envelope

Start with conservative per-pod limits: two to four concurrent transfers, a bounded byte queue, and no in-memory whole-object cache. Tune only after metrics-server and workload measurements are available in OKE.
