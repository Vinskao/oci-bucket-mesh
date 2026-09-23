# OciBucketMesh

> Status: architecture scaffold only. No OCI request, queue consumer, HTTP server, container image, or Kubernetes resource is implemented yet.

OciBucketMesh is TY Multiverse's internal **Object Storage data-plane gateway and transfer scheduler**. It is not an Object Storage replacement and it is not exposed to the internet directly.

## Placement

```text
ty-multiverse-frontend
  -> ty-multiverse-gateway (JWT, public rate limit)
  -> oci-bucket-mesh (transfer admission, streaming, jobs)
  -> OCI Object Storage
       |
       +-> RabbitMQ -> ty-multiverse-consumer (business processing)
```

The Gateway remains the external entry point. OciBucketMesh is a `ClusterIP`-only service; it will eventually run as its own Kubernetes Deployment, initially with one replica and `emptyDir` staging storage.

## Repository layout

```text
apps/          Process entry points; intentionally empty until implementation begins
libs/          Reusable C++ domains and adapters; intentionally empty
proto/         Versioned gRPC contracts
k8s/           Deployment design and future manifests; no live manifests yet
docs/           Architecture, API ownership, and technology decisions
```

## Build status

The top-level CMake project is intentionally target-free. It verifies the C++20/CMake baseline without downloading or compiling selected dependencies. `vcpkg.json` records the approved dependency set for the implementation phase.

```bash
cmake --preset dev
cmake --build --preset dev
```

Read [technology selection](docs/TECH_STACK.md) before adding a target or dependency.
# oci-bucket-mesh
