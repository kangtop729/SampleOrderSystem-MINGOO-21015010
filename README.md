# 반도체 시료 생산주문관리 시스템 (SampleOrderSystem)

가상의 반도체 회사 "S-Semi"의 시료(Sample) 주문 접수부터 생산, 출고까지의 흐름을 관리하는
콘솔 기반 애플리케이션이다. 엑셀/메모장으로 관리하던 주문·재고 현황을 체계적으로 추적하기 위해
개발되었다.

> 본 프로젝트는 Claude Code의 오케스트레이터-서브에이전트(도메인 모델링/영속성/TDD 테스트/Clean Code
> 리뷰) 팀 구성으로 구현했으며, 모든 기능은 TDD(test-driven-development) 스킬의 Red-Green-Refactor
> 사이클을 따라 개발했다.

## 배경

시료는 주문이 들어오면 웨이퍼 공정 설비를 통해 제작되고, 검수를 거쳐 고객에게 출고된다.
이 시스템은 주문 담당자와 생산 담당자가 콘솔 명령을 통해 주문을 접수·승인·거절하고,
재고와 생산 현황을 한눈에 파악할 수 있도록 지원한다.

## 주요 기능

| 메뉴 | 설명 |
|------|------|
| 시료 관리 | 새로운 시료 등록, 목록 조회, 이름 검색 |
| 시료 주문 | 고객 요청에 따른 주문 예약(RESERVED) 생성 |
| 주문 승인/거절 | 재고 상황에 따라 자동으로 CONFIRMED 또는 PRODUCING 처리, 또는 REJECTED 처리 |
| 모니터링 | 상태별 주문 수, 시료별 재고 현황(여유/부족/고갈) 확인 |
| 생산 라인 | 생산 중인 시료와 FIFO 생산 큐 확인 |
| 출고 처리 | CONFIRMED 상태 주문에 대한 출고 실행 |

## 주문 상태 흐름

```
RESERVED --(거절)--------------------------> REJECTED (모니터링 제외)
RESERVED --(승인, 재고 충분)---------------> CONFIRMED --(출고)--> RELEASE
RESERVED --(승인, 재고 부족)---------------> PRODUCING --(생산 완료)--> CONFIRMED --(출고)--> RELEASE
```

- 실 생산량 = `ceil(부족분 / 수율)`
- 총 생산 시간 = `평균 생산시간 × 실 생산량`
- 생산 라인은 단일 라인이며 FIFO(선입선출)로 처리한다.

## 기술 스택

- 언어: C++20
- 빌드: Visual Studio / MSBuild
- 테스트: GoogleTest / GoogleMock
- 데이터 영속성: JSON 파일
- 아키텍처: MVC + Repository 패턴

## 빌드 및 실행 방법

단일 `.vcxproj`에서 Configuration으로 빌드 대상을 구분한다.
- **Debug 빌드**: `test/*.cpp`를 포함해 컴파일 → 실행 파일 자체가 gmock 테스트 러너
- **Release 빌드**: `test/*.cpp`는 제외되고 `src/main.cpp` 기반 실제 콘솔 앱이 빌드됨

```
# 테스트 빌드 및 실행
msbuild SampleOrderSystem-MINGOO-21015010.vcxproj /p:Configuration=Debug /p:Platform=x64
x64\Debug\SampleOrderSystem-MINGOO-21015010.exe

# 실제 콘솔 앱 빌드 및 실행
msbuild SampleOrderSystem-MINGOO-21015010.vcxproj /p:Configuration=Release /p:Platform=x64
x64\Release\SampleOrderSystem-MINGOO-21015010.exe
```

실행 위치에 `samples.json`, `orders.json`이 생성/누적되며(JSON 파일 기반 영속성), 앱을 재시작해도
데이터와 생산 진행 상태가 유지된다. 콘솔은 UTF-8(코드페이지 65001)로 자동 전환되어 한글이 정상
출력된다.

현재 239개의 gmock/gtest 단위·통합 테스트가 Model/Repository/Service/View/Controller 전 계층의
경계값(재고 정확/1개 부족/전량 부족, 수율에 따른 ceil 계산, 재고 상태 여유/부족/고갈 등)을 검증한다.

## 메인 메뉴 구조

```
[1] 시료 관리        [2] 시료 주문
[3] 주문 승인/거절    [4] 모니터링
[5] 생산라인 조회     [6] 출고 처리
[0] 종료
```

| 메뉴 | 세부 흐름 |
|------|----------|
| 시료 관리 | 시료 등록 / 목록 조회 / 이름 검색 |
| 시료 주문 | 시료ID·고객명·수량 입력 → 입력 내용 확인 → [Y] 예약 접수 / [N] 취소 |
| 주문 승인/거절 | RESERVED 목록에서 번호 선택 → 재고 확인(부족 시 부족분/실생산량/생산시간 안내) → [Y] 승인 / [N] 거절 |
| 모니터링 | 상태별 주문 건수, 시료별 재고와 색상 배지·잔여율 막대 |
| 생산라인 조회 | 현재 생산 중인 작업의 진행률·완료 예정 시각, FIFO 대기열 |
| 출고 처리 | CONFIRMED 목록에서 번호 선택 → 출고(RELEASE) 처리 |

## 아키텍처

MVC + Repository 패턴으로 계층을 분리했다.

```
src/
├── Model/       # Sample, Order(상태 머신), OrderStatus, ProductionJob — 다른 계층을 모름
├── Repository/  # IRepository<T> → ISampleRepository/IOrderRepository → Json*Repository (JSON 영속성)
├── Serialization/ # nlohmann/json 기반 Sample/Order 직렬화
├── Service/     # SampleService, OrderService, ProductionLineService, MonitoringService, ReleaseService
│                # (재고 판정·생산량 계산 등 도메인 로직, Repository 인터페이스에만 의존)
├── View/        # ConsoleFormat(색상/표/배지 등 순수 포맷 함수) + 화면별 View 클래스 (콘솔 출력 전용)
├── Controller/  # 화면별 Controller + MainController (Service 호출 결과를 View에 위임)
└── main.cpp     # Repository→Service→View→Controller 와이어링, Release 진입점

test/            # gmock 기반 단위/통합 테스트 (Debug 빌드에 포함)
```

의존 방향은 `Controller → Service → Repository`, `Controller → View` 이며 `Model`은 어떤 계층도
참조하지 않는다. 생산 라인은 별도 큐 자료구조 없이 `Order` 상태(PRODUCING)로 표현되며, FIFO 순서는
승인 시각 기반으로 정렬해 도출한다. 생산 시작 시각은 `Order`에 영속화되어 프로그램을 재시작해도
실제 경과 시간 기준으로 이어서 진행된다.

## 프로젝트 구조

```
SampleOrderSystem-MINGOO-21015010/
├── src/                # Model / View / Controller / Service / Repository / Serialization
├── test/               # gmock 기반 단위/통합 테스트 (239건)
├── docs_implement/      # Phase별 구현 계획 문서 (전체/PoC 4종/본 프로젝트)
├── CLAUDE.md            # 에이전트 작업 규칙
├── PRD.md               # 요구사항 명세
└── README.md
```

각 서브 프로젝트의 상세 구현 계획은 [docs_implement/](docs_implement/) 디렉토리를 참고한다.

## 관련 저장소 (PoC)

- `ConsoleMVC-MINGOO-21015010` — MVC 스켈레톤 코드
- `DataPersistence-MINGOO-21015010` — JSON 기반 데이터 영속성 및 CRUD
- `DataMonitor-MINGOO-21015010` — 콘솔 기반 데이터 모니터링 도구
- `DummyDataGenerator-MINGOO-21015010` — 테스트용 더미 데이터 생성 도구

PoC와의 관계(코드 공유 여부, 일치해야 할 기술적 선택)는
[docs_implement/Overall/PLAN.md](docs_implement/Overall/PLAN.md) 3절 참고.
