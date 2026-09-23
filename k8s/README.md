# Kubernetes deployment plan

There are deliberately no apply-ready manifests in this scaffold.

Before adding them, prepare:

1. a dedicated `bucket-mesh` namespace and ServiceAccount;
2. OCI least-privilege workload identity for the exact allowed bucket/prefixes;
3. a `ClusterIP` service, no Ingress, and Gateway route policy;
4. `emptyDir.sizeLimit`, `ephemeral-storage`, CPU and memory limits;
5. startup/readiness/liveness probes and Prometheus scraping;
6. a NetworkPolicy after the cluster's current permissive networking is replaced or explicitly accepted;
7. a RabbitMQ exchange, versioned queues, retry policy and dead-letter queue.

The initial Deployment must use `replicas: 1`. Horizontal scaling is blocked until job state, multipart recovery and ownership leases are durable and shared.
