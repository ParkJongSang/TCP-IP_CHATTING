# **SERVER** - **CLIENT** TCP CHATTING SERVICE

## **NEEDS**

> 모든 시스템은 IP 통신이 가능함

> 채팅 Server는 Client를 대상으로 부가서비스를 제공해야함
> - 레터링, 착신전환, 착신거부, 발신 제한 서비스 중 3가지 서비스를 제공

> Server는 1:1 또는 1 : N채팅 기능을 제공해야함

> Server는 최대 10개의 Client에 대해 접속을 허용 해야함.

> Server와 Client 모두 가입자 정보를 바꿀 수 있는 MMC 제공해야함

> 프로세스 실행 중 발생되는 주요 상황에 대해 로그파일에 기록 해야함.
#
## **CLIENT**
 > Client는 자신의 부가서비스 정보를 변경할 수 있는 MMC를 제공 해야함.

 > Request 메시지 전송 후 Server로부터 일정 시간 내에 응답을 수신하지 못하는 경우, 기존 Connection을 해제해야 함.
#
## **SERVER**
> Server는 가입자 정보를 생성/변경/삭제 하는 MMC 제공해야함.

> Server는 일정시간 Client로부터 유입되는 메시지가 없는 경우, Connectrion Check 메시지를 Client로 전송 해야함

> Client로부터 일정 시간 응답이 없는 경우, TCP 연결을 해제해야 함


2019.7.29
1. 테스트용 Server.c Clinet.c

2. ReadNWrite.h ReadNWrite.c

2019.7.30
1. 데이터베이스 동작 테스트용 main.c

2. 데이터베이스 삽입 조회 삭제 모두 가능.

2019.7.31
1. Packet관리용 Queue : Memory Pool 사용

2. Client용 신호 전송 기능 정의, 구현

2019.8.1
1. Server용 신호 전송 기능 정의, 구현

2. Server-Client NonBlocking 모드 연결

2019.8.2
1. ReadNWrite 해결

2.각 자료구조와 Table -> Server와 연결

2019.8.5
1. 자료구조 기능 정상 동작 확인

2. PING기능과 EXIT기능 작업 중

2019.8.6
1. PING 메세지 전송

2. switch 시간 줄이고 각 큐에 있는 요소에 시간 추가하면서 5초 이상된 큐들은 자동으로 pop -> 실패로 간주

2019.8.7
1. 서버에서 유저를 관리할 수 있는 MMC 구현

2. 신호 발송 및 자료구조에서 발생할 수 있는 실패 처리
