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

		// strcpy ���ڿ� ���� �Լ�

		printf("���� �޽��� �Է� : ");
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

	// �ʱ�ȭ ���� 0, ���� -1
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		printf("winsock �ʱ�ȭ ����\n");
		return -1;
	}

	printf("winsock �ʱ�ȭ ����!!\n");

	// 3. ���� ������ �ݱ�.
	// socket(af, type, protocol) - ������ �������ִ� �Լ�.
	// af : �ּ�ü�� (ipv4 - AF_INET, ipv6 - AF_INET6)
	// type : ������ ������ ���� Ÿ�� (TCP / UDP) - SOCK_STREAM, SOCK_DGRAM 
	// protocol : �������� ����(���� 0���� ����)  
	// closesocket(sock): �ش� ������ �ڿ� �ݳ�

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == SOCKET_ERROR) {
		printf("���� ������ ���� �߻�!!\n");
		return -1;
	}

	printf("���� ���� �Ϸ�!!\n");


	// 4. �ּҿ� ��Ʈ��ȣ ����
	// sin_family : �ּ�ü��
	// sin_port : ��Ʈ��ȣ
	// sin_addr : IP �ּ� (ADDR_ANY ������ �ּҸ� �ڵ� ����)
	// htons : ��Ʋ�ε�� > ���ε�� 



	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999); // 9000���� ���ķδ� �� �������. 9000 ~ 63000
	addr.sin_addr.S_un.S_addr = ADDR_ANY;

	// 5. ���ε�
	// bind(sock, addr, addrlen)
	// sock : ���ε��� ��� ����
	// addr : ��� ���Ͽ� ������ �ּ� ����ü(sockaddr) �ּ�
	// addrlen : �ּ� ����ü�� ũ��
	// sockaddr* : sockaddr_in ip v4 Ưȭ�� ����ü�ε�, �̰� ���� ���뼺�ְ� �ٲ۰��� sockaddr
	if (bind(sock, (SOCKADDR_IN*)&addr, sizeof(addr)) != 0) {
		printf("���ε� �� ����!!\n");
		return -1;
	}

	//6. ������(listen) - listen()
	// ���� ��û�� �������� ������ ����ҿ� ���ʴ�� ����.
	// listen(sock, backlog)
	// sock - ���� ��� ���� ����
	// backlog - ����ҿ� ������ ���� ���� �ִ� ����. ���� ���� ����� ���� ���� ���� ����.(ť)

	printf("���ε� ����!!\n");
	listen(sock, 5);
	printf("���� ���\n");

	// 7. ���� ���� - accept()
	// accept(sock, caddr, caddrlen) : ����ִ� caddr �ѱ�� accept() �Լ��� ���������� ������� �� caddr ����ü�� ���� ��û�� Ŭ���̾�Ʈ�� �ּ� ������ ���� ����. �����͸� ���� Ŭ���̾�Ʈ �ּҸ� �� �� ����.
	// sock - ���ε��� ���� ����
	// caddr - Ŭ���̾�Ʈ �ּ� ������ ���� ����ü
	// caddrlen - caddr�� ũ��
	// return : ���� ��û�� Ŭ���̾�Ʈ�� ���� ����

	SOCKADDR_IN caddr;
	SOCKET csock;
	int csize = sizeof(caddr);

	csock = accept(sock, (SOCKADDR_IN*)&caddr, &csize);

	if (csock == SOCKET_ERROR) {
		printf("���� ������ ���� �߻�!!\n");
	}

	printf("���� ����!!\n");
	printf("����� ���� ��ȣ : %d \n", csock); 

	// 8. ������ �ۼ��� - send() / recv()
	// send(sock, buff, len, flags)
	// recv(sock, buff, len, flags)
	// sock : send - �����͸� ���� ��� ����, recv - �����͸� ���� ��� ����
	// buff : send - ���� �����͸� ���� ����, recv - ���� �����͸� ���� ����
	// len : ������ ũ��
	// flags : �Ϲ������� 0

	// Ŭ���̾�Ʈ�� ������ ������
	pthread_t t1;
	pthread_t t2;
	pthread_create(&t1, NULL, send_msg, csock);
	pthread_create(&t2, NULL, recv_msg, csock);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	

	closesocket(sock);
	printf("���� �ݱ� �Ϸ�!!\n");

	WSACleanup();
	printf("winsock �ڿ� �ݳ�\n");


	return 0;
}