#define HAVE_STRUCT_TIMESPEC
#include <stdio.h>
#include <pthread.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

//int sharedData = 0;
//pthread_mutex_t mutex;
//void push() {
//	for (int i = 0; i < 100000; i++) {
//		pthread_mutex_lock(&mutex);
//		sharedData++;
//		pthread_mutex_unlock(&mutex);
//	}
//}
void send_msg(SOCKET csock) {
	while (1) {
		char sbuff[100] = { 0 };

		// strcpy 문자열 복사 함수

		printf("보낼 메시지 입력 : ");
		scanf_s("%s", sbuff, 100);
		send(csock, sbuff, 100, 0);
	}
}

void recv_msg(SOCKET csock) {
	while (1) {
		char rbuff[100] = { 0 };
		recv(csock, rbuff, 100, 0);

		printf("%s", rbuff);
	}
}
int main() {

	/*pthread_t threads[2];
	pthread_mutex_init(&mutex, NULL);

	if (pthread_create(&threads[0], NULL, push, NULL) != 0) {
		fprintf(stderr, "Error");
	}

	if (pthread_create(&threads[1], NULL, push, NULL) != 0) {
		fprintf(stderr, "Error");
	}

	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	printf("%d\n", sharedData);*/


	WSADATA wsadata;

	// 초기화 성공 0, 실패 -1
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		printf("winsock 초기화 실패\n");
		return -1;
	}

	printf("winsock 초기화 성공!!\n");

	// 3. 소켓 생성과 닫기.
	// socket(af, type, protocol) - 소켓을 생성해주는 함수.
	// af : 주소체계 (ipv4 - AF_INET, ipv6 - AF_INET6)
	// type : 소켓의 데이터 전송 타입 (TCP / UDP) - SOCK_STREAM, SOCK_DGRAM 
	// protocol : 프로토콜 선택(보통 0으로 해줌)  
	// closesocket(sock): 해당 소켓의 자원 반납

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == SOCKET_ERROR) {
		printf("소켓 생성시 문제 발생!!\n");
		return -1;
	}

	printf("소켓 생성 완료!!\n");


	// 4. 주소와 포트번호 설정
	// sin_family : 주소체계
	// sin_port : 포트번호
	// sin_addr : IP 주소 (ADDR_ANY 로컬의 주소를 자동 세팅)
	// htons : 리틀인디안 > 빅인디안 



	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999); // 9000번대 이후로는 다 비어있음. 9000 ~ 63000
	addr.sin_addr.S_un.S_addr = ADDR_ANY;

	// 5. 바인딩
	// bind(sock, addr, addrlen)
	// sock : 바인딩할 대상 소켓
	// addr : 대상 소켓에 설정한 주소 구조체(sockaddr) 주소
	// addrlen : 주소 구조체의 크기
	// sockaddr* : sockaddr_in ip v4 특화된 구조체인데, 이걸 좀더 범용성있게 바꾼것이 sockaddr
	if (bind(sock, (SOCKADDR_IN*)&addr, sizeof(addr)) != 0) {
		printf("바인딩 중 에러!!\n");
		return -1;
	}

	//6. 연결대기(listen) - listen()
	// 연결 요청한 접속자의 정보를 저장소에 차례대로 저장.
	// listen(sock, backlog)
	// sock - 연결 대기 서버 소켓
	// backlog - 저장소에 저장할 연결 정보 최대 갯수. 제일 먼저 저장된 것을 제일 먼저 뺀다.(큐)

	printf("바인딩 성공!!\n");
	listen(sock, 5);
	printf("연결 대기\n");

	// 7. 연결 수립 - accept()
	// accept(sock, caddr, caddrlen) : 비어있는 caddr 넘기면 accept() 함수가 성공적으로 수행됐을 때 caddr 구조체에 연결 요청한 클라이언트의 주소 정보를 세팅 해줌. 데이터를 보낼 클라이언트 주소를 알 수 있음.
	// sock - 바인딩된 서버 소켓
	// caddr - 클라이언트 주소 정보를 담을 구조체
	// caddrlen - caddr의 크기
	// return : 연결 요청한 클라이언트의 소켓 정보

	SOCKADDR_IN caddr;
	SOCKET csock;
	int csize = sizeof(caddr);

	csock = accept(sock, (SOCKADDR_IN*)&caddr, &csize);

	if (csock == SOCKET_ERROR) {
		printf("연결 수립중 에러 발생!!\n");
	}

	printf("연결 성공!!\n");
	printf("연결된 소켓 번호 : %d \n", csock); 

	// 8. 데이터 송수신 - send() / recv()
	// send(sock, buff, len, flags)
	// recv(sock, buff, len, flags)
	// sock : send - 데이터를 보낼 대상 소켓, recv - 데이터를 받을 대상 소켓
	// buff : send - 보낼 데이터를 담은 버퍼, recv - 받은 데이터를 담을 버퍼
	// len : 버퍼의 크기
	// flags : 일반적으로 0

	// 클라이언트에 데이터 보내기
	pthread_t t1;
	pthread_t t2;
	pthread_create(&t1, NULL, send_msg, csock);
	pthread_create(&t2, NULL, recv_msg, csock);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	

	closesocket(sock);
	printf("소켓 닫기 완료!!\n");

	WSACleanup();
	printf("winsock 자원 반납\n");


	return 0;
}