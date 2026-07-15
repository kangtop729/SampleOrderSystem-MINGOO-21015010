# PoC 4: DummyDataGenerator 구현 계획

## 1. 목적

테스트를 위한 **더미 데이터를 생성하여 JSON 저장소에 추가**하는 도구를 직접 구현해보는 참고용 PoC다.

> `DataPersistence` PoC와는 별도 Git 저장소이므로 코드를 공유하지 않고, 엔티티/JSON 스키마를 이
> 저장소 안에서 자체적으로 정의한다. PoC 전반의 리뷰/TDD 기대 수준은 [Overall/PLAN.md](../Overall/PLAN.md)
> 3절 참고.

## 2. 저장소 정보

- Repository: `DummyDataGenerator-MINGOO-21015010` (Public, 형제 디렉토리에 신규 생성)
- 기술 스택: C++ / Visual Studio, GoogleTest/GoogleMock
- 영속성: JSON 파일에 append (이 저장소 자체에서 스키마 정의)

## 3. 패키지 구조

```
DummyDataGenerator-MINGOO-21015010/
├── src/
│   ├── Model/            # 이 저장소 자체에서 정의하는 최소 엔티티
│   ├── Repository/        # JSON 저장소에 append 하는 writer
│   ├── Generator/          # 랜덤 값 생성 로직 (이름, 수량, ID 시퀀스 등)
│   └── main.cpp             # CLI 실행 (생성 개수 등 옵션 입력)
├── test/
├── CLAUDE.md
└── README.md
```

## 4. Phase 계획

### Phase 1 — 데이터 모델 정의
- 더미 데이터로 채울 최소 엔티티/JSON 스키마를 이 저장소 안에서 자체적으로 정의

### Phase 2 — 랜덤 생성 로직
- 이름/수치 등 속성별 랜덤 생성 규칙 정의 (예: 이름은 사전 정의된 접두어+번호 조합, 수량은 범위 내 난수)
- 재현성을 위한 시드(seed) 옵션 고려

### Phase 3 — 저장소 연동(append)
- 기존 JSON 파일을 읽어 마지막 ID 이후 번호로 신규 데이터 추가
- 기존 데이터를 덮어쓰지 않고 append 하는 것을 보장

### Phase 4 — CLI 옵션
- 생성할 데이터 개수를 인자/입력으로 받기
- 실행 결과 요약(생성된 개수, 총 데이터 개수) 출력

### Phase 5 — (가벼운) 테스트
- 핵심 시나리오 1~2개(생성 개수만큼 정확히 추가되는지, 기존 데이터가 보존되는지)에 대해 실패하는
  테스트를 먼저 작성 → 통과시키는 간단한 TDD 사이클 수행

## 5. 검증 기준

- 실행할 때마다 기존 데이터를 유지한 채 지정한 개수만큼 데이터가 추가될 것
- 생성된 더미 데이터가 상식적인 도메인 제약(수량은 양수 등)을 위반하지 않을 것
- 영속성 방식이 JSON 파일이라는 점에서 Main 프로젝트와 "내용"이 일치할 것 (코드 재사용 여부는 무관)
