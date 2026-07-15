# Main 프로젝트: 반도체 시료 생산주문관리 시스템 구현 계획

## 1. 배경 요약

가상의 반도체 회사 "S-Semi"는 시료(Sample) 주문을 엑셀/메모장으로 관리하다 실수가 잦아,
콘솔 기반 "반도체 시료 생산주문관리 시스템"을 도입한다. 역할은 고객(요청) → 주문 담당자(주문서 관리)
→ 생산 담당자(승인/거절, 생산)로 이어진다.

## 2. 저장소 정보

- Repository: `SampleOrderSystem-MINGOO-21015010` (본 저장소, 이미 vcxproj/gmock 세팅됨)
- 기술 스택: C++ / Visual Studio, GoogleTest/GoogleMock, nlohmann/json
- 아키텍처: MVC (ConsoleMVC PoC 이식) + Repository 패턴 (DataPersistence PoC 이식)

## 3. 핵심 도메인 규칙 정리

### 3.1 엔티티

| 엔티티 | 속성 |
|--------|------|
| Sample(시료) | 시료 ID, 이름, 평균 생산시간, 수율, 현재 재고 |
| Order(주문) | 주문번호, 시료 ID, 고객명, 주문 수량, 상태, 생성 시각 |

### 3.2 주문 상태 머신

```
RESERVED --(거절)--> REJECTED (모니터링 제외, 종단 상태)
RESERVED --(승인, 재고 충분)--> CONFIRMED --(출고)--> RELEASE
RESERVED --(승인, 재고 부족)--> PRODUCING --(생산 완료)--> CONFIRMED --(출고)--> RELEASE
```

### 3.3 생산 계산 공식

- 부족분 = 주문 수량 − 현재 재고 (음수면 0)
- 실 생산량 = `ceil(부족분 / 수율)`
- 총 생산 시간 = 평균 생산시간 × 실 생산량
- 생산 라인은 단일 라인, FIFO 큐로 스케줄링
- 생산 완료 시 재고에 실 생산량 반영 + 주문 상태 PRODUCING → CONFIRMED

### 3.4 모니터링 재고 상태 분류

- 여유: 주문 대비 재고 충분
- 부족: 주문 대비 재고 수량 부족 (0 초과)
- 고갈: 재고 수량 0

## 4. 패키지 구조

```
SampleOrderSystem-MINGOO-21015010/
├── src/
│   ├── Model/
│   │   ├── Sample.h/.cpp
│   │   ├── Order.h/.cpp          # 상태(enum class OrderStatus) 포함
│   │   └── ProductionJob.h/.cpp   # 생산 큐에 들어가는 작업 단위
│   ├── Repository/
│   │   ├── ISampleRepository.h
│   │   ├── IOrderRepository.h
│   │   └── JsonSampleRepository / JsonOrderRepository (DataPersistence PoC 이식)
│   ├── Service/
│   │   ├── SampleService.h/.cpp        # 시료 등록/조회/검색
│   │   ├── OrderService.h/.cpp         # 주문 예약/승인/거절, 재고 판정
│   │   ├── ProductionLineService.h/.cpp # FIFO 큐, 생산 진행/완료 처리
│   │   ├── MonitoringService.h/.cpp     # 상태별 집계, 재고 현황 산출
│   │   └── ReleaseService.h/.cpp        # 출고 처리
│   ├── View/                # 콘솔 출력 포맷 (메뉴, 표, 진행률 등)
│   ├── Controller/           # 메뉴 라우팅 (MainMenu, SampleMenu, OrderMenu, ...)
│   └── main.cpp
├── test/                     # gmock 기반 Service/Repository 단위 테스트
├── docs_implement/           # 본 구현 계획 문서 모음
├── CLAUDE.md
├── PRD.md
└── README.md
```

## 5. Phase 계획

### Phase 1 — 프로젝트 기반 다지기
- `CLAUDE.md`(에이전트 작업 규칙, 빌드/테스트 명령, 코딩 컨벤션), `PRD.md`(기능 명세를 요구사항 문서로 정리) 작성
- ConsoleMVC PoC의 Model/View/Controller 골격을 본 프로젝트 구조로 이식
- gmock 기반 테스트 하네스 셋업 확인 (`packages.config`의 gmock 활용)

### Phase 2 — 도메인 모델 구현
- `Sample`, `Order`, `OrderStatus` 정의
- 상태 전이 규칙을 상태 머신 형태로 캡슐화 (불법 전이 방지)
- 단위 테스트: 상태 전이 유효성 검증

### Phase 3 — 영속성 계층 통합
- DataPersistence PoC의 `IRepository<T>` 패턴을 `ISampleRepository`, `IOrderRepository`로 특화
- JSON 파일 스키마 확정 (samples.json, orders.json)
- 단위 테스트: 저장/재로드 시 데이터 일치 검증

### Phase 4 — 시료 관리 기능
- 시료 등록 (ID, 이름, 평균 생산시간, 수율 입력 검증)
- 시료 조회 (재고 수량 포함 목록, 페이지네이션 여부 결정)
- 시료 검색 (이름 등 속성 기반)
- 단위/통합 테스트

### Phase 5 — 주문 기능 (예약/승인/거절)
- 주문 예약: 시료 ID·고객명·수량 입력 → RESERVED 주문 생성
- 주문 승인: 재고 확인 로직 → 충분하면 CONFIRMED, 부족하면 생산 큐 등록 후 PRODUCING
- 주문 거절: 즉시 REJECTED 전환
- 단위 테스트: 재고 경계값(정확히 충분/1개 부족/전량 부족) 시나리오

### Phase 6 — 생산 라인
- FIFO 생산 큐 구현
- 실 생산량/총 생산 시간 계산 로직 (수율 반영, ceil 처리)
- 생산 진행 표시(진행률 등) 및 완료 시 재고 반영 + 상태 전환(PRODUCING → CONFIRMED)
- 대기 중인 생산 큐 목록 출력
- 단위 테스트: 수율에 따른 실 생산량 계산 정확성, FIFO 순서 보장

### Phase 7 — 모니터링 기능
- 상태별(RESERVED/CONFIRMED/PRODUCING/RELEASE) 주문 수 집계 (REJECTED 제외)
- 시료별 재고 현황과 여유/부족/고갈 상태 표기
- DataMonitor PoC의 조회·집계 패턴 재사용
- 단위 테스트: 경계값(재고 0, 정확히 소진 등) 분류 검증

### Phase 8 — 출고 처리
- CONFIRMED 상태 주문 목록 표시 → 특정 주문 출고 실행 → RELEASE 전환
- 단위 테스트: CONFIRMED가 아닌 주문에 대한 출고 시도 방지

### Phase 9 — 메인 메뉴 통합 및 UX
- 메인 메뉴에서 등록 시료 수, 총 재고, 전체 주문 수, 생산라인 대기 건수 등 요약 정보 표시
- 각 하위 메뉴(1~6) 라우팅 통합
- 콘솔 UI 포맷 다듬기 (표, 상태 배지 등은 자유 형식)

### Phase 10 — 테스트 강화 및 더미 데이터 연동
- DummyDataGenerator PoC 로직을 활용해 초기 시료/주문 데이터 시딩 스크립트 또는 메뉴 항목 추가(선택)
- 전체 시나리오 통합 테스트: 주문 접수 → 승인(생산 필요) → 생산 완료 → 출고까지 end-to-end 검증
- 테스트 커버리지 점검 (Service 계층 중심)

### Phase 11 — 문서화 및 제출 준비
- README.md에 빌드/실행 방법, 메뉴 구조, 아키텍처 설명 정리
- 커밋 이력 정리 (Phase 단위로 의미 있는 커밋 유지되었는지 점검)
- 최종 Clean Code 리뷰 (역할 분리, 중복 제거, 네이밍)
- Public 저장소 전환 및 제출

## 6. 리스크 및 확인 필요 사항

- 생산 라인은 명세상 "단일 라인"으로 해석되나, 여러 시료의 생산 큐가 하나의 라인에서 FIFO로 처리되는지
  재확인 필요 (Phase 6 착수 전 예시 화면 재검토로 확정)
- 수율에 따른 반올림(ceil) 처리 시 실제 생산량이 부족분보다 많아지는 경우의 재고 잉여 처리 방식은
  명세에 없으므로, "잉여분은 재고로 축적"으로 가정하고 Phase 6에서 구현 시 결정 사항으로 문서화
