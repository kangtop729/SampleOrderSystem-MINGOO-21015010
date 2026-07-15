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

과제 지침상 **PoC로 핵심 기술 요소를 개별 검증한 뒤, 그 결과를 조합하여 Main 프로젝트를 구현**하는 순서를 따른다
(PDF p.25 "PoC를 작성하여 기능을 검증 후 프로젝트 구현을 시작").

## 2. 공통 기술 스택

- 언어/런타임: C++ (Visual Studio, MSBuild `.vcxproj`)
- 테스트: GoogleTest / GoogleMock (`gmock` NuGet 패키지, 기존 `packages.config`에 이미 등록됨)
- 데이터 영속성 방식: **JSON 파일 기반** (팀 선택 사항, nlohmann/json 사용)
- 아키텍처: MVC (Model / View / Controller 패키지 분리)
- 문서: 각 저장소에 `CLAUDE.md`(에이전트 작업 규칙), `README.md`(사용법), Main 프로젝트는 `PRD.md` 추가

## 3. 저장소 간 의존/재사용 관계

```
ConsoleMVC (MVC 골격)
        └─▶ SampleOrderSystem 의 Model/View/Controller 패키지 구조로 이식

DataPersistence (JSON 영속성 + CRUD)
        └─▶ SampleOrderSystem 의 Repository 계층으로 이식

DataMonitor (콘솔 실시간 조회 도구)
        └─▶ SampleOrderSystem 의 "모니터링" 메뉴 설계에 참고

DummyDataGenerator (더미 데이터 생성)
        └─▶ SampleOrderSystem 개발/테스트 단계에서 초기 시료·주문 데이터 시딩에 사용
```

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

## 5. 제출 체크리스트 (PDF p.27 기준)

- [ ] 5개 저장소 모두 Public 생성
- [ ] Repository 이름 규칙 준수 (`항목명-MINGOO-21015010`)
- [ ] Main 프로젝트에 `CLAUDE.md`, `PRD.md` 포함
- [ ] Harness(에이전틱 워크플로우) 사용 흔적 확보 (Sonnet / Effort: Medium 준수)
- [ ] 각 저장소 테스트 코드 포함
- [ ] Clean Code 원칙 준수 (역할 분리, 짧은 함수, 의미있는 네이밍)
- [ ] 의미 있는 단위의 커밋 이력
- [ ] 과제 종료 후 Claude Code 로그아웃(`/logout`)
