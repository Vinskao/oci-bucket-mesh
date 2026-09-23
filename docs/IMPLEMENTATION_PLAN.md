# OciBucketMesh implementation plan

## Goal

Keep the public API and frontend contract unchanged while replacing the backend data-source implementation behind a single abstraction layer.

## Scope

This document captures the implementation path for the research-zone mapping flow and the bucket mesh integration approach discussed in the project.

## Current issue

The existing research-zone flow is effectively:

- frontend bundled JSON fallback
- GET /tymb/resources/company-product-mapping
- Backend CompanyProductMappingService
- OCI Java SDK direct object access
- Redis cache

The real coupling is in the backend service layer, not in the public HTTP contract.

## Decision

Do not change the public REST contract.

Do not make consumer code or frontend logic aware of the storage implementation.

Only abstract the backend data-source dependency:

- ResourceController
  -> CompanyProductMappingService
    -> CompanyProductMappingSource (new interface)
      -> OciSdkMappingSource (current direct OCI implementation)
      -> MeshMappingSource (new Mesh-backed implementation)

This keeps:

- frontend URL unchanged
- response JSON unchanged
- Gateway path unchanged
- consumer contract unchanged
- backend callers unchanged

## Why this is the right boundary

The bucket operation is a data-access concern, not a business API concern.

CompanyProductMappingService already owns the business logic for mapping resolution, caching, and fallback behavior. It should depend on an abstract source interface instead of a concrete OCI SDK implementation.

That creates a stable seam for:

- feature-flag-based rollout
- fallback-to-OCI behavior when Mesh is unavailable
- later direct file upload/download or AI processing flows without forcing every service to know about OCI details

## Recommended first implementation

### 1. Add the source abstraction

Create a new Java interface in the backend, for example:

```java
public interface CompanyProductMappingSource {
    String loadJsonContent() throws IOException;
}
```

This interface should represent the contract for "retrieve the mapping payload" without exposing bucket internals or OCI-specific APIs.

### 2. Move current direct OCI logic into a compatibility adapter

Existing implementation:

- direct OCI ObjectStorage client
- GetObjectRequest / object key selection
- Redis caching behavior

This becomes `OciSdkMappingSource`.

It preserves current behavior exactly, including the current fallback path and cache TTL logic.

### 3. Add the Mesh implementation

Create `MeshMappingSource` as the new implementation behind the same interface.

It should:

- call the Mesh control API or gRPC endpoint
- request the object payload or metadata
- parse the response into the same JSON contract expected by the backend
- surface meaningful errors that allow fallback to `OciSdkMappingSource`

This implementation should be used only when the feature flag is enabled.

### 4. Add a feature flag / selector

At the service layer, choose the implementation like:

```java
private final CompanyProductMappingSource source;
```

The provider can be selected from configuration, for example:

- `mapping.source=oci-sdk`
- `mapping.source=mesh`
- `mapping.source=mesh-with-fallback`

The default should remain the current OCI SDK path until production verification is complete.

### 5. Preserve fallback behavior

Mesh failures must not break the app.

Recommended policy:

- try Mesh first when feature enabled
- if failure, timeout, or malformed payload, retry via existing OCI SDK source
- log structured warning and keep current frontend response contract intact

This prevents an unstable new data plane from impacting the Research Zone.

## Operational boundary

### In scope for Mesh v1

- transfer admission
- streaming object reads/writes
- job lifecycle / status
- idempotency and rate controls
- completion/failure event publication
- health and metrics endpoints

### Out of scope for Mesh v1

- bucket lifecycle management
- bucket IAM policy management
- arbitrary lifecycle rules
- DB backup handling
- business transformation logic
- direct public internet exposure

## DB backup exception

Daily PostgreSQL backup is not part of this abstraction.

It is an existing operational workload using:

- pg_dump
- emptyDir staging
- OCI CLI upload

This should remain independent for now because it is an active recovery path and should not become dependent on a newly introduced data plane that is still under validation.

## IAM / permission guidance

The current OCI policy includes write-new-version and is attached to worker nodes.

Once Mesh comes online, the permission should be narrowed to a dedicated Mesh identity and only the exact bucket/prefixes required for the new service.

Backend services should no longer retain broad OCI write scope unless a specific new requirement justifies it.

## Recommended rollout order

1. Add `CompanyProductMappingSource` abstraction in backend.
2. Move current OCI code into `OciSdkMappingSource`.
3. Add failing-safe `MeshMappingSource` implementation behind the same interface.
4. Add feature flag and fallback path.
5. Enable Mesh for staging only.
6. Verify mapping payload, cache, latency, and observability.
7. Expand only when the transfer and event model is stable.

## Non-goals

This design intentionally does not require:

- frontend API changes
- gateway route changes
- consumer contract changes
- full replacement of all bucket calls in the monolith
- immediate DB backup migration

## Summary

The correct pattern is to localize the change to the backend data-access layer, not to rewrite the entire service boundary.

The result is a low-risk modernization path that preserves the current user-facing behavior while creating a clean path toward the Mesh architecture.
