// std=c++98 버전 코드

// kqueue 사용

int webserve(int ac, char **av)
{
	// Server Socket 생성
	// sockaddr_in 초기화

	// server socket & server addr binding

	// server status listen 

	// fcntl server socket non blocking

	// kqueue instance 생성

	// key(socket descriptor) / value(socket data) map 생성

	// kevent 구조체 배열 (change_event를 감지하기 위함.) <change list>

	// event가 감지 됐을 때 저장될 이벤트 kevent 구조체 배열. [8]

	// 서버 소켓 이벤트 등록. EV_SET 사용

	// host loop

	// kevent를 사용해서 new event의 갯수를 받아오고 오류처리 

	// 변경된 change list clear ()

	// for new event

	// current 를 event list에서 가져오고. 에러처리

	// Server 입장에서 본 이벤트 분기처리

	// Event 분기 처리
	// - 서버소켓에 이벤트 일어난 경우 (클라이언트가 처음들어온 경우) 최초 1회 실행
	//   - accept사용해서 client socket 선언 및 초기화.
	//   - nonblock 방식
	// - 해당하는 클라이언트가 있을때.
	//   - data 읽기. recv()
	//     -  조건 분기처리 *(htpp 요청 들)
	//       - GET     
	//       - POST    
	//       - DELETE  

	return 0;
}
