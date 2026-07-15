# 반도체 시료 생산주문관리 시스템 (SampleOrderSystem)

가상의 반도체 회사 "S-Semi"의 시료(Sample) 주문 접수부터 생산, 출고까지의 흐름을 관리하는
콘솔 기반 애플리케이션이다. 엑셀/메모장으로 관리하던 주문·재고 현황을 체계적으로 추적하기 위해
개발되었다.

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

## 빌드 방법

1. Visual Studio에서 `SampleOrderSystem-MINGOO-21015010.slnx` 열기
2. NuGet 패키지 복원 (gmock 자동 복원)
3. Debug/Release, x64/Win32 원하는 구성으로 빌드

또는 명령줄에서:

```
msbuild SampleOrderSystem-MINGOO-21015010.vcxproj /p:Configuration=Debug /p:Platform=x64
```

## 프로젝트 구조

```
SampleOrderSystem-MINGOO-21015010/
├── src/               # Model / View / Controller / Service / Repository
├── test/              # gmock 기반 단위 테스트
├── docs_implement/     # Phase별 구현 계획 문서 (전체/PoC 4종/본 프로젝트)
├── CLAUDE.md           # 에이전트 작업 규칙
└── README.md
```

`src/`, `test/`는 구현 진행에 따라 채워진다. 각 서브 프로젝트의 상세 구현 계획은
[docs_implement/](docs_implement/) 디렉토리를 참고한다.

## 관련 저장소 (PoC)

- `ConsoleMVC-MINGOO-21015010` — MVC 스켈레톤 코드
- `DataPersistence-MINGOO-21015010` — JSON 기반 데이터 영속성 및 CRUD
- `DataMonitor-MINGOO-21015010` — 콘솔 기반 데이터 모니터링 도구
- `DummyDataGenerator-MINGOO-21015010` — 테스트용 더미 데이터 생성 도구

PoC와의 관계(코드 공유 여부, 일치해야 할 기술적 선택)는
[docs_implement/Overall/PLAN.md](docs_implement/Overall/PLAN.md) 3절 참고.
