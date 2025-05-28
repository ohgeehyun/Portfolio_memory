# 🧠 메모리 관리 

이 프로젝트는 게임 서버 또는 고성능 애플리케이션에서 사용할 수 있는 **커스텀 메모리 할당**입니다.  
`BaseAllocator`, `StompAllocator`, `PoolAllocator`, `Memory`, `MemoryPool` 등의 계층으로 구성되어 있습니다.

---

## 📊 전체 흐름도

```mermaid
flowchart TD
    A[사용자 요청] --> B{할당 크기 확인}
    B -- 4096 이하 --> C[PoolAllocator::Alloc]
    C --> D[Memory::Allocate]
    D --> E[MemoryPool::Pop or _aligned_malloc]
    B -- 4096 초과 --> F[_aligned_malloc]
    F --> G[Header 처리 - AttachHeader]
    E --> G
    G --> H[사용자에게 포인터 반환]
--------------------------------------------
🔍 흐름 설명
1. 사용자 요청
사용자가 xnew<T>(), PoolAllocator::Alloc(), Make_Shared() 등을 통해 메모리 할당 요청을 보냅니다.

2. 할당 크기 확인
Memory::Allocate(size)에서 요청한 크기를 기준으로 분기합니다.

기준은 MAX_ALLOC_SIZE = 4096 바이트입니다.

✅ 크기 ≤ 4096바이트: PoolAllocator 경로
_poolTable[size]을 통해 적절한 MemoryPool을 O(1)로 즉시 선택합니다.

선택된 MemoryPool에서 Pop()을 호출하여 메모리를 가져옵니다.

풀에 여유가 없다면 _aligned_malloc을 통해 새 블록을 할당합니다.

메모리 앞부분에 MemoryHeader를 붙인 후, 사용자에게 데이터 포인터를 반환합니다.

✅ 크기 > 4096바이트: 일반 할당 경로
MemoryPool에서 관리하기엔 크기 초과이므로 _aligned_malloc을 사용해 직접 메모리를 요청합니다.

이 경우에도 MemoryHeader를 붙여서 반환합니다.

3. 공통 처리
모든 할당된 메모리에는 MemoryHeader가 함께 붙습니다.

해제 시 DetacchHeader()를 통해 헤더를 분리합니다.

크기에 따라 다시 메모리 풀로 반납하거나 _aligned_free로 해제합니다.

📌 핵심 컴포넌트 요약
컴포넌트	역할
MemoryPool	크기별로 미리 할당된 메모리 블록 재사용 (lock-free push/pop)
Memory	요청된 크기에 따라 적절한 풀을 선택하고 할당 관리
PoolAllocator	커스텀 메모리 시스템의 진입점
MemoryHeader	SLIST 호환 메타정보. 데이터 앞에 붙어 추적 및 해제 용도
_aligned_malloc	풀에 여유가 없거나 4096바이트 초과 시 직접 할당

✅ 예제 사용 코드
cpp
복사
편집
struct MyStruct {
    int x;
    float y;
};

// 메모리 할당 및 해제
MyStruct* data = xnew<MyStruct>();
xdelete(data);

// shared_ptr도 커스텀 풀 기반으로 생성
auto shared = Make_Shared<MyStruct>();
