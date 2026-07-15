# PoC 4: DummyDataGenerator 구현 계획

## 1. 목적

테스트를 위한 **더미 데이터를 생성하여 연결된 저장소(JSON 파일)에 추가**하는 도구를 검증한다.
Main 프로젝트 개발 초기에 시료·주문 데이터를 대량으로 채워 모니터링/생산라인 기능을
실감나게 테스트할 수 있도록 지원하는 것이 목표다.

## 2. 저장소 정보

- Repository: `DummyDataGenerator-MINGOO-21015010` (Public, 형제 디렉토리에 신규 생성)
- 기술 스택: C++ / Visual Studio, GoogleTest/GoogleMock
- 영속성: `DataPersistence` PoC와 동일한 JSON 저장 포맷에 append

## 3. 패키지 구조

```
DummyDataGenerator-MINGOO-21015010/
├── src/
│   ├── Model/            # DataPersistence PoC와 동일한 엔티티 정의
│   ├── Repository/        # 기존 JSON 저장소에 append 하는 writer
│   ├── Generator/          # 랜덤 값 생성 로직 (이름, 수량, ID 시퀀스 등)
│   └── main.cpp             # CLI 실행 (생성 개수 등 옵션 입력)
├── test/
├── CLAUDE.md
└── README.md
```

## 4. Phase 계획

### Phase 1 — 데이터 모델 재사용 확인
- DataPersistence PoC의 엔티티/스키마 재사용 (필드 불일치 없도록 정렬)

### Phase 2 — 랜덤 생성 로직
- 이름/수치 등 속성별 랜덤 생성 규칙 정의 (예: 이름은 사전 정의된 접두어+번호 조합, 수량은 범위 내 난수)
- 재현성을 위한 시드(seed) 옵션 고려

### Phase 3 — 저장소 연동(append)
- 기존 JSON 파일을 읽어 마지막 ID 이후 번호로 신규 데이터 추가
- 기존 데이터를 덮어쓰지 않고 append 하는 것을 보장

### Phase 4 — CLI 옵션
- 생성할 데이터 개수를 인자/입력으로 받기
- 실행 결과 요약(생성된 개수, 총 데이터 개수) 출력

### Phase 5 — 테스트
- 생성 개수만큼 정확히 추가되는지 검증
- 기존 데이터가 보존되는지(비파괴적 append) 검증
- 랜덤 값이 유효 범위 내에 있는지 검증

## 5. 검증 기준

- 실행할 때마다 기존 데이터를 유지한 채 지정한 개수만큼 데이터가 추가될 것
- 생성된 더미 데이터가 Main 프로젝트의 도메인 제약(수량은 양수, 이름 중복 방지 등)을 위반하지 않을 것
- Main 프로젝트에서 초기 시료/주문 데이터 시딩 스크립트로 그대로 활용 가능할 것
