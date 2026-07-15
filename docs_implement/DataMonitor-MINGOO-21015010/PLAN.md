# PoC 3: DataMonitor 구현 계획

## 1. 목적

콘솔에서 **현재 저장된 데이터 상태를 실시간으로 조회**할 수 있는 관리자 도구를 검증한다.
`DataPersistence` PoC에서 만든 JSON 저장소를 읽어, 별도 프로세스/메뉴에서 데이터 현황을
사람이 보기 좋은 형태로 표시하는 것이 핵심이다.

## 2. 저장소 정보

- Repository: `DataMonitor-MINGOO-21015010` (Public, 형제 디렉토리에 신규 생성)
- 기술 스택: C++ / Visual Studio, GoogleTest/GoogleMock
- 영속성: `DataPersistence` PoC와 동일한 JSON 저장 포맷을 읽기 전용으로 사용

## 3. 패키지 구조

```
DataMonitor-MINGOO-21015010/
├── src/
│   ├── Model/            # DataPersistence PoC와 동일한 엔티티 정의(or 공용화 검토)
│   ├── Repository/       # 읽기 전용 JSON 로더 (DataPersistence의 IRepository 재사용/축소)
│   ├── View/              # 콘솔 테이블/요약 출력 포맷터
│   └── main.cpp            # 조회 루프 (수동 새로고침 또는 주기적 폴링)
├── test/
├── CLAUDE.md
└── README.md
```

## 4. Phase 계획

### Phase 1 — 저장소 연동
- DataPersistence PoC의 Repository/직렬화 로직을 참고하여 읽기 전용 로더 구현
- 동일한 JSON 스키마를 공유한다는 전제로 파일 경로 설정

### Phase 2 — 조회 화면 설계
- 전체 목록 조회, 개수 요약(총 건수, 상태별 집계 등) 화면 구성
- 필터/검색(예: 이름, 상태별) 기능 추가

### Phase 3 — 실시간 갱신
- 사용자가 "새로고침" 명령을 입력하면 파일을 다시 읽어 최신 상태 반영
- (선택) 일정 주기로 자동 재조회하는 폴링 모드 지원

### Phase 4 — 통합 및 테스트
- 파일 변경 전/후 조회 결과가 달라지는지 검증하는 통합 테스트
- 빈 데이터, 대량 데이터 상황에서의 출력 포맷 테스트

## 5. 검증 기준

- 원본 데이터 파일이 갱신되면, 재조회 시 최신 값이 반영될 것
- 조회 전용 도구이므로 데이터를 변경하는 어떤 write 연산도 수행하지 않을 것
- Main 프로젝트의 "모니터링" 메뉴(상태별 주문 수, 재고 현황 여유/부족/고갈)에 적용 가능한
  집계·표시 로직 패턴을 확보할 것
