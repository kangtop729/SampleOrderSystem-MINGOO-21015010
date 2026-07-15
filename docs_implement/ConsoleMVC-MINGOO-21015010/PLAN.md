# PoC 1: ConsoleMVC 구현 계획

## 1. 목적

콘솔 애플리케이션에서 **Model / View / Controller 역할을 명확히 분리**한 최소 골격(Skeleton)을 만들어,
이후 Main 프로젝트(SampleOrderSystem)에 이식할 수 있는 구조적 패턴을 검증한다.
비즈니스 로직(반도체 시료 도메인)은 최소화하고, "메뉴 입력 → 처리 → 화면 출력" 흐름 자체의 구조에 집중한다.

## 2. 저장소 정보

- Repository: `ConsoleMVC-MINGOO-21015010` (Public, 형제 디렉토리에 신규 생성)
- 기술 스택: C++ / Visual Studio (`.vcxproj`), GoogleTest/GoogleMock

## 3. 패키지 구조

```
ConsoleMVC-MINGOO-21015010/
├── src/
│   ├── Model/         # 데이터 상태를 표현하는 순수 클래스 (예: MenuItem, AppState)
│   ├── View/          # 콘솔 입출력만 담당 (표시 포맷팅, 입력 프롬프트)
│   ├── Controller/    # Model과 View를 연결, 사용자 입력에 따른 흐름 제어
│   └── main.cpp        # 진입점: Controller 생성 및 루프 실행
├── test/               # gmock 기반 단위 테스트
├── CLAUDE.md
└── README.md
```

## 4. Phase 계획

### Phase 1 — 프로젝트 셋업
- vcxproj/솔루션 생성, gmock NuGet 패키지 연동
- 디렉토리 구조(Model/View/Controller/test) 생성
- CLAUDE.md 작성 (에이전트 작업 규칙, 빌드/테스트 명령)

### Phase 2 — Model 계층
- 화면 전환에 필요한 최소 상태를 표현하는 클래스 정의 (예: 현재 메뉴 상태, 종료 플래그)
- Model은 View/Controller를 몰라야 함 (단방향 의존)

### Phase 3 — View 계층
- 메뉴 출력, 사용자 입력 수신을 전담하는 클래스 작성
- 입출력은 `std::cin`/`std::cout` 래핑 — 추후 테스트 시 입출력 스트림 주입 가능하도록 인터페이스화

### Phase 4 — Controller 계층
- 사용자 입력값 → 처리 로직 라우팅
- View에 결과 렌더링을 위임, Model 상태 갱신
- 종료 조건(0 입력 등) 처리

### Phase 5 — 통합 및 테스트
- Controller 단위 테스트: 가짜(Mock) View를 주입하여 입력값별 분기 검증
- 메뉴 순환(왕복) 시나리오 테스트
- README.md에 실행 방법, 구조 설명 정리

## 5. 검증 기준

- Model/View/Controller 간 순환 의존이 없을 것
- View를 Mock으로 교체해도 Controller 테스트가 가능할 것 (DI 가능한 구조)
- 신규 메뉴 항목 추가 시 Controller의 분기만 늘어나고 View/Model 수정이 최소화될 것
