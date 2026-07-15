# 전체 프로젝트 구현 계획 (Overall)

## 1. 배경 및 목표

"[CRA_AI] Day3_개인과제_반도체시료관리" 과제는 아래 5개의 산출물을 요구한다.

| # | 산출물 | Repository 이름 | 성격 |
|---|--------|------------------|------|
| 1 | MVC 스켈레톤 코드 | `ConsoleMVC-MINGOO-21015010` | PoC |
| 2 | 데이터 영속성 처리 | `DataPersistence-MINGOO-21015010` | PoC |
| 3 | 데이터 모니터링 Tool | `DataMonitor-MINGOO-21015010` | PoC |
| 4 | Dummy 데이터 생성 Tool | `DummyDataGenerator-MINGOO-21015010` | PoC |
| 5 | 반도체 시료 생산주문관리 시스템 | `SampleOrderSystem-MINGOO-21015010` | Main |

과제 지침상 **PoC로 핵심 기술 요소를 개별 검증한 뒤, Main 프로젝트를 구현**하는 순서를 따른다
(PDF p.25 "PoC를 작성하여 기능을 검증 후 프로젝트 구현을 시작"). PoC와 Main의 정확한 관계(코드 재사용
여부, TDD/리뷰 기대 수준)는 3절을 참고.

## 2. 공통 기술 스택

- 언어/런타임: C++ (Visual Studio, MSBuild `.vcxproj`)
- 테스트: GoogleTest / GoogleMock (`gmock` NuGet 패키지, 기존 `packages.config`에 이미 등록됨)
- 데이터 영속성 방식: **JSON 파일 기반** (팀 선택 사항, nlohmann/json 사용)
- 아키텍처: MVC (Model / View / Controller 패키지 분리)
- 문서: 각 저장소에 `CLAUDE.md`(에이전트 작업 규칙), `README.md`(사용법), Main 프로젝트는 `PRD.md` 추가

## 3. PoC와 Main의 관계 (기준 문서)

> **다른 문서에서 PoC-Main 관계를 언급할 때는 이 절을 요약/링크만 하고, 아래 내용을 다시 풀어 쓰지
> 않는다.** 원칙이 바뀌면 이 절만 고치면 되도록 하기 위함이다.

PoC는 AI를 활용해 직접 구현해보고 코드를 읽어보며 학습하는 참고용 산출물이다. Main 프로젝트가
PoC의 코드를 그대로 가져다 쓰거나 구조를 문자 그대로 복제할 필요는 없다 — 각 저장소는 독립적으로
구현한다. 다만 **"내용"(기술적 선택)은 아래 표만큼은 반드시 일치**해야 한다.

| PoC | 검증하는 기술 요소 | Main과 반드시 일치해야 할 것 |
|-----|--------------------|------------------------------|
| ConsoleMVC | MVC 역할 분리 패턴 | Model/View/Controller로 역할이 분리된 구조라는 점 (세부 클래스 설계는 자유) |
| DataPersistence | 영속성 방식(JSON) + CRUD | **JSON 파일 기반** 영속성이라는 점 (JSON이 아닌 다른 방식으로 구현하면 안 됨) |
| DataMonitor | 콘솔 실시간 조회 UX | 없음 — Main "모니터링" 메뉴 설계에 아이디어 차원에서만 참고 가능 |
| DummyDataGenerator | 더미 데이터 생성 방식 | 없음 — Main 개발/테스트 시 초기 데이터 시딩 아이디어로만 참고 가능 |

PoC도 **엄연한 제출 대상**이므로, 각 PoC 저장소 역시 Public 저장소로 생성하고 `README.md`,
`PLAN.md`(또는 `CLAUDE.md`)를 갖추며, 가벼운 수준의 TDD(핵심 동작에 대한 실패→통과 테스트 1~2개
이상)를 포함해야 한다. 다만 코드 리뷰·리팩토링 등은 Main만큼 세세하게 맞추지 않아도 된다.

## 4. 전체 일정 (Phase)

| Phase | 내용 | 산출물 |
|-------|------|--------|
| Phase 0 | 과제 분석 및 문서화 (본 문서 세트) | `docs_implement/**/PLAN.md` |
| Phase 1 | PoC 1: ConsoleMVC 구현 | `ConsoleMVC-MINGOO-21015010` repo |
| Phase 2 | PoC 2: DataPersistence 구현 | `DataPersistence-MINGOO-21015010` repo |
| Phase 3 | PoC 3: DataMonitor 구현 | `DataMonitor-MINGOO-21015010` repo |
| Phase 4 | PoC 4: DummyDataGenerator 구현 | `DummyDataGenerator-MINGOO-21015010` repo |
| Phase 5 | Main: SampleOrderSystem 설계/구현/테스트 | `SampleOrderSystem-MINGOO-21015010` (본 저장소) |
| Phase 6 | 문서화·커밋 이력 정리·제출 준비 | 5개 Public Repo |

각 Phase의 세부 계획은 해당 서브 디렉토리의 `PLAN.md` 참조:

- [ConsoleMVC PLAN](../ConsoleMVC-MINGOO-21015010/PLAN.md)
- [DataPersistence PLAN](../DataPersistence-MINGOO-21015010/PLAN.md)
- [DataMonitor PLAN](../DataMonitor-MINGOO-21015010/PLAN.md)
- [DummyDataGenerator PLAN](../DummyDataGenerator-MINGOO-21015010/PLAN.md)
- [SampleOrderSystem(Main) PLAN](../SampleOrderSystem-MINGOO-21015010/PLAN.md)

## 5. Agent 팀 구성 가이드 (PoC vs Main)

Main 프로젝트(`SampleOrderSystem-MINGOO-21015010`)에는 `.claude/agents/`에 오케스트레이터 1개 +
서브에이전트 4개(도메인 모델링, 영속성, TDD 테스트, Clean Code 리뷰)로 구성된 풀 팀이 이미 있다.
각 PoC는 참고용 산출물이므로 동일한 5인 팀을 그대로 복제하지 않는다. PoC용 팀을 구성할 때는:

- 오케스트레이터 1개 + 구현 담당 서브에이전트 1개(설계+구현 겸임) + 가벼운 테스트 담당 서브에이전트
  1개 정도로 축소한다.
- `clean-code-reviewer`처럼 세세한 리뷰를 전담하는 에이전트는 PoC에는 두지 않는다.
- 테스트 담당 에이전트에게는 "핵심 동작 1~2개에 대한 실패→통과 TDD 사이클"만 요구하고, Main처럼
  경계값을 촘촘히 요구하지 않는다.

## 6. 제출 체크리스트 (PDF p.27 기준)

- [ ] 5개 저장소 모두 Public 생성
- [ ] Repository 이름 규칙 준수 (`항목명-MINGOO-21015010`)
- [ ] Main 프로젝트에 `CLAUDE.md`, `PRD.md` 포함
- [ ] Harness(에이전틱 워크플로우) 사용 흔적 확보 (Sonnet / Effort: Medium 준수)
- [ ] 각 저장소 테스트 코드 포함 (PoC는 가벼운 TDD 1~2개 이상, Main은 경계값까지 포함한 충분한 커버리지)
- [ ] Clean Code 원칙 준수 (Main 기준. PoC는 세세한 리팩토링/리뷰까지 맞추지 않아도 됨)
- [ ] 의미 있는 단위의 커밋 이력
- [ ] 과제 종료 후 Claude Code 로그아웃(`/logout`)
