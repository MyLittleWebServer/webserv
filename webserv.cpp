// std=c++98 버전 코드

// kqueue 사용

// 구현 고려사항
// 	CGI fork() 어디에서?

int webserve(int ac, char **av)
{
	// Server Socket 생성
	// sockaddr_in 초기화

	// server socket & server addr binding

	// server status listen 

	// fcntl server socket non blocking

	// kqueue instance 생성

	// key(socket descriptor) / value(socket data) map 생성
	// kevent 구조체 vector (change_event를 감지하기 위함.) <change_list>
	// event가 감지 됐을 때 저장될 이벤트 kevent 구조체 배열. new_kevent[8]

	// 서버 소켓 이벤트 등록. EV_SET 사용

	// host loop

	//	kevent를 사용해서 change_list에 있는 kevent 구조체 커널큐에 등록,
	//	그리고 이벤트 발생한 것들이 인자로 들어간  new_kevent 배열에 받아짐, 리턴된 이벤트 수로 오류처리 

	//	변경된 change_list clear ()

	//	for new event(kevent()에서 리턴된 n만큼)

	//		current event를 new_kevent에서 가져오고. 에러처리

	//		Server 입장에서 본 이벤트 분기처리

	//		Event 분기 처리
	//		- current 이벤트가 READ 인 경우
	//			- 서버소켓에 이벤트 일어난 경우 (클라이언트가 연결 요청한 경우) 최초 1회 실행되게 됨
	//				- accept사용해서 client socket 선언 및 초기화, client 객체 생성.
	//				- EV_SET으로 해당 client socket에 대한 이벤트 구조체(kevent) 설정(읽기, 쓰기), change_list에 push_back
	//				- nonblock 방식(fnctl)
	//				- client socket을 map에 등록, value로 client 구조체 삽입
	//			- current 이벤트가 client socket인 경우
	//				- 이 때는 클라이언트가 서버에 무언가를 전송하여 해당 클라이언트 소켓으로부터 무언가를 읽을 수 있는 상태
	//				- 읽어서 map의 해당 클라이언트 키에 읽은 데이터 mapping(HTTP 요청 메시지)
	//				- 요청 완전히 수신하였다면 플래그 설정하여 상태 표시
	//		- current 이벤트가 WRITE 인 경우
	//			- 클라이언트 소켓이 정상적으로 열려 있으면 항상 여기로 들어옴
	//			- map에서 해당 client 객체 확인하여 요청이 완전히 수신 되었다면
	//				- HTTP요청 파싱
	//				- 요청 종류에 따른 HTTP 응답 메시지 생성
	//					- GET    
	//					- POST  
	//					- DELETE
	//				- send()로 client socket에 응답 메시지 전송
	return 0;
}
