# 커스텀 메모리 관리 시스템

이 프로젝트는 게임 서버 또는 고성능 애플리케이션에서 사용할 수 있는 **커스텀 메모리 할당기**입니다.  
`BaseAllocator`, `StompAllocator`, `PoolAllocator`, `Memory`, `MemoryPool` 등의 계층으로 구성되어 있습니다.

---

# 커스텀 메모리 관리 시스템

이 프로젝트는 게임 서버 또는 고성능 애플리케이션에서 사용할 수 있는 **커스텀 메모리 할당기**입니다.  
`BaseAllocator`, `StompAllocator`, `PoolAllocator`, `Memory`, `MemoryPool` 등의 계층으로 구성되어 있습니다.

---

## 🧠 전체 흐름도

```mermaid
flowchart TD
    A[사용자 요청] --> B{할당 크기 확인}
    B -- 4096 이하 --> C[PoolAllocator::Alloc]
    C --> D[Memory::Allocate]
    D --> E[MemoryPool::Pop or _aligned_malloc]
    B -- 4096 초과 --> F[_aligned_malloc]
    F --> G[Header 붙이기 (AttachHeader)]
    G --> H[사용자에게 포인터 반환]
