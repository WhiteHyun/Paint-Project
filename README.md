## Lab4 - Embedded Systems Team Project

### 팀원

- 201601563: 김동언
- 201601639: 홍승현
- 201801527: 김대성
- 201801546: 김태현

### 개발선정

- 그림판

### 일정

#### 6월 13일 Lab4 사전회의(9시~10시 p.m.)

- 논의사항
  - `개발 선택`: 그림판, 스마트폰 한글 입력지 중 택일
  - 간단한 질의응답
- 논의결과
  - `그림판` 개발 선정
    1.  UI 구현 및 화면 클리어 구현 ( 2명 ) - 김대성, 김태현
    2.  기초 코드 세팅 ( 2명 ) - 김동언, 홍승현

#### 6월 15일 회의 (9시~10시 10분)

- 논의사항
  - 전반적인 코드 흐름 분석
  - 도형과 이벤트에 `interactive`한 방식에 대한 토의
  - 아이디어 공유
- 논의결과
  1.  Shape 구조체 논의 보류
  2.  도형에 필요한 matrix만큼 동적 할당하여 가지는 것이 구현 용이성이 있을 것으로 판단
  3.  각자 도형에 대한 구현방안 고려
  4.  FreeDraw에 QnA 답변에 따라 해당되는 내용에 맞는 방안으로 구현 고려
  5.  위의 내용을 상기하여 17일 회의 진행

#### 6월 17일 기초구현점검 및 회의(9시~10시 p.m.)

- 구현점검
  1. [완료](https://github.com/WhiteHyun/DrawingBoard-Project/pull/1)
- 논의사항
  1.  13일 코드 리뷰
  2.  Line & Pen & Clear 기능 ( 2명 )
  3.  Oval & Rectangle & Select & Erase ( 2명 )
  4.  Fill & 색상 ( 미정 )
- 논의결과
  1. 파일 분할 (set.c -> main , btn -> touch sense Events, draw -> Draw UI)
  2. 각 도형 이벤트 분할
  - 김대성 : `Line`
  - 김동언 : `Oval`, **터치 이벤트** 구현
  - 김태현 : `Rectangle`
  - 홍승현 : `FreeDraw` 및 기초적인 **struct, code** 구현
  3. 위의 내용을 상기하여 19일 3시 회의 진행

#### 6월 19일 회의(3시~5시 p.m.)

- 논의사항
  1. 러버밴드
  2. 러버밴드로 인한 기존 도형 삭제 문제
  3. 각자 진행한 코드 리뷰
- 논의결과
  1. sketchbook이라는 배열전역변수를 두어 `논의사항 1`과 `논의사항 2` 해결 (idea by `김동언`)
  2. [List 구현](https://github.com/WhiteHyun/DrawingBoard-Project/pull/2) by `홍승현`
  3. 자연스러운 Line 형태 구현 by `김대성`
  4. 맡은 도형의 러버밴드 구현
  5. SELECT 구현예정 by `홍승현`
  6. 레포트 작성 준비

#### 6월 23일 회의(8시~9시 p.m.)

- 논의사항
  1. 각 진행결과 발표
- 논의결과
  1. LINE 러버밴드 중 맨 위쪽 줄 1비트씩 검은칸 or 흰 칸이 나오는 버그 발생 향후 수정할 것
  2. OVAL 러버밴드 중 잔존현상 버그 발생 수정할 것임
  3. SELECT 구현 80% 완료
  4. ERASE 기능 구현예정 by `김대성`

#### 6월 25일 회의(10시~12시30분 p.m.)

- 논의사항
  1. 진행결과 발표
- 논의결과
  1. LINE 구현 완료
  2. OVAL 구현 완료
  3. SELECT 구현 완료
  4. ERASE 구현 일부 완료
  5. FILL 구현중 by `김동언`

#### 6월 26일 Zoom 발표

### 개발 요구사항

- 변수명 : addToList (`카멜 표기법`)
- 함수명 : AddToList (`파스칼 표기법`)
- 함수들의 경우 초반부에 역할에 따른 주석 표기
- `if`, `for`문등의 경우 1줄이라도 중괄호`{}` 표기

### Role

- Main Developer
- Sub Developer
- Hard Copy Writer

### Technology

- Git
- Git Kraken

### Language

- C
