# CLAUDE.md

이 문서는 Claude(에이전트)가 이 저장소에서 작업할 때 따라야 할 규칙을 정리한다.

## 프로젝트 개요

"반도체 시료 생산주문관리 시스템"을 구현하는 콘솔 애플리케이션이다.
가상의 반도체 회사 S-Semi가 시료(Sample) 주문 접수부터 생산, 출고까지의 흐름을 체계적으로 관리하기 위해
사용한다. 요구사항은 [`PRD.md`](PRD.md), Phase별 구현 계획은
[docs_implement/SampleOrderSystem-MINGOO-21015010/PLAN.md](docs_implement/SampleOrderSystem-MINGOO-21015010/PLAN.md)를 참고한다.

이 저장소는 "[미션2] 프로젝트 개발"에 해당한다. 4개의 PoC(별도 저장소, 학습·참고용)와의 관계 —
코드를 공유하지 않되 영속성 방식(JSON) 등은 일치시켜야 한다는 원칙 — 및 전체 로드맵은
[docs_implement/Overall/PLAN.md](docs_implement/Overall/PLAN.md)를 참고한다.

## 기술 스택

- 언어: C++20
- 빌드: Visual Studio / MSBuild (`SampleOrderSystem-MINGOO-21015010.vcxproj`)
- 플랫폼: Win32, x64 / Debug, Release
- 테스트: GoogleTest / GoogleMock (`gmock` 1.11.0, NuGet 패키지)
- 데이터 영속성: JSON 파일 (nlohmann/json)
- 아키텍처: MVC + Repository 패턴

## 빌드 및 테스트 명령

```
msbuild SampleOrderSystem-MINGOO-21015010.vcxproj /p:Configuration=Debug /p:Platform=x64
```

테스트 프로젝트가 구성되면 별도 테스트 실행 파일을 빌드 후 실행한다. (테스트 프로젝트 추가 시 본 절 갱신)

## 작업 원칙

1. **문서 우선**: 새로운 기능을 구현하기 전에 `docs_implement/SampleOrderSystem-MINGOO-21015010/PLAN.md`의
   해당 Phase 항목을 확인하고, 계획과 다르게 구현해야 한다면 문서를 먼저 갱신한다.
2. **역할 분리(MVC)**: `Model`은 View/Controller를 몰라야 하고, `View`는 콘솔 입출력만 담당하며,
   `Controller`가 흐름을 제어한다. Service 계층이 도메인 로직(재고 판정, 생산 계산 등)을 담당한다.
3. **작은 단위 커밋**: Phase 단위 혹은 그보다 작은, 의미 있는 단위로 커밋한다.
4. **커밋 메시지 규칙**: 제목은 반드시 아래 5개 prefix 중 하나로 시작한다.
   - `[feature]` 새 기능 추가
   - `[refactor]` 동작 변경 없는 구조 개선
   - `[fix]` 버그 수정
   - `[docs]` 문서 변경
   - `[test]` 테스트 추가/수정, 테스트 인프라 변경
5. **TDD**: 새 기능/버그 수정 전에는 [`.claude/skills/test-driven-development/SKILL.md`](.claude/skills/test-driven-development/SKILL.md)의
   Red-Green-Refactor 사이클을 따른다 — 실패하는 테스트를 먼저 작성하고 실패를 확인한 뒤, 최소한의
   구현으로 통과시키고, 통과 상태를 유지하며 리팩토링한다. Service/Repository 계층은 gmock 기반
   단위 테스트를 함께 작성하며, 특히 아래 경계값은 반드시 테스트한다.
   - 재고가 정확히 충분/1개 부족/전량 부족한 경우
   - 수율에 따른 실 생산량 계산 (`ceil(부족분 / 수율)`)
   - 재고 상태 분류 경계값 (여유/부족/고갈, 특히 0)
6. **Clean Code**: 함수는 한 가지 일만 하도록 작게 유지하고, 매직 넘버 대신 의미 있는 이름의 상수를 사용하며,
   불필요한 주석 대신 좋은 이름으로 의도를 드러낸다.
7. **불필요한 추상화 금지**: 명세에 없는 기능(예: 다중 생산 라인, 동시성 제어)을 미리 만들지 않는다.

## 도메인 핵심 규칙 요약

자세한 내용은 PLAN.md 3절을 참고. 요약:

- 주문 상태: `RESERVED → (승인/거절) → CONFIRMED|PRODUCING|REJECTED`, `PRODUCING → CONFIRMED`,
  `CONFIRMED → RELEASE`
- `REJECTED`는 모니터링 대상에서 제외되는 종단 상태
- 실 생산량 = `ceil(부족분 / 수율)`, 총 생산 시간 = `평균 생산시간 × 실 생산량`
- 생산 라인은 단일 라인, FIFO 큐로 처리
