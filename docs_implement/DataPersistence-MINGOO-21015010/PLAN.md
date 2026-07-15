# PoC 2: DataPersistence 구현 계획

## 1. 목적

**JSON 파일 기반 데이터 영속성**(저장/불러오기) 구조와 CRUD를 직접 구현해보며 검증하는 참고용 PoC다.
애플리케이션을 재실행해도 데이터가 유지되어야 한다.

> PoC와 Main의 관계(코드 재사용 여부, 리뷰/TDD 기대 수준)는
> [Overall/PLAN.md](../Overall/PLAN.md) 3절 참고. 단, **영속성 방식(JSON 파일)은 Main과 반드시
> 일치**해야 하는 항목이므로 엑셀/DB 등 다른 방식으로 구현하지 않는다.

## 2. 저장소 정보

- Repository: `DataPersistence-MINGOO-21015010` (Public, 형제 디렉토리에 신규 생성)
- 기술 스택: C++ / Visual Studio, GoogleTest/GoogleMock
- JSON 라이브러리: nlohmann/json (NuGet 또는 vcpkg로 연동)

## 3. 패키지 구조

```
DataPersistence-MINGOO-21015010/
├── src/
│   ├── Model/            # 저장 대상 엔티티 (예: Item)
│   ├── Repository/       # IRepository<T> 인터페이스 + JsonFileRepository 구현체
│   ├── Serialization/     # to_json/from_json 변환 로직
│   └── main.cpp            # CRUD 데모 실행
├── test/                   # Repository CRUD 단위 테스트
├── CLAUDE.md
└── README.md
```

## 4. Phase 계획

### Phase 1 — 라이브러리 연동 및 데이터 모델 정의
- nlohmann/json 패키지 연동 확인 (빌드 통과)
- 저장 대상 최소 엔티티(Item) 정의: id, name, quantity 등

### Phase 2 — Repository 인터페이스 설계
- `IRepository<T>`: `save`, `findAll`, `findById`, `update`, `remove` 시그니처 정의
- 인터페이스 분리로 향후 저장 방식(파일→DB 등) 교체 가능하도록 설계

### Phase 3 — JSON 파일 Read/Write 구현
- 파일이 없을 때 빈 배열로 초기화
- 파일 → JSON 파싱 → 엔티티 리스트 역직렬화
- 엔티티 리스트 → JSON 직렬화 → 파일 저장 (매 CRUD 연산 후 즉시 flush)

### Phase 4 — CRUD 및 예외 처리
- Create/Read/Update/Delete 전체 구현
- 예외 상황 처리: 파일 손상(JSON 파싱 실패), 존재하지 않는 id 조회/수정/삭제
- 동시성은 고려하지 않음 (단일 프로세스, 콘솔 앱 가정)

### Phase 5 — (가벼운) 테스트
- 핵심 시나리오 1~2개(예: 저장 후 재로드 시 데이터 일치)에 대해 실패하는 테스트를 먼저 작성 →
  최소 구현으로 통과시키는 간단한 TDD 사이클 수행
- 임시 테스트 파일 경로를 사용해 실제 파일 시스템에 저장/재로드하는 통합 테스트 최소 1개 포함
- (선택) 파일 손상 시나리오에 대한 예외 테스트

## 5. 검증 기준

- 프로세스를 재시작해도 이전에 저장한 데이터가 그대로 조회될 것 (영속성)
- Repository 인터페이스만으로 CRUD 호출이 가능하며 JSON 세부 구현이 노출되지 않을 것
- 영속성 방식이 JSON 파일이라는 점에서 Main 프로젝트와 "내용"이 일치할 것 (코드 재사용 여부는 무관)
