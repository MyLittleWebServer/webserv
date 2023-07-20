## 🌐 Webserv
<img width="100" height="100" src="tests/html/asset/index.ico/apple-icon.png">

#### My little web server

## 🤝 Introduction
 - C++ 98로 구현한 기본에 충실한 웹서버입니다.

## 🧩 Functions
```
- C++, CGI, Kernel Queue, POSIX, HTML, CSS 등을 사용하여 구현하였습니다.
- 객체지향 프로그래밍을 사용하여 유지보수 및 확장성을 높였습니다. (Interface 구조)
- Config 및 types 같은 외부 종속성을 Singleton Pattern을 사용하여 구현하였습니다.
- CGI 기능으로 서버에서 server-side advanced task를 훨씬 쉽게 수행할 수 있습니다.
- Http 1.1 프로토콜을 구현하였습니다.
- 해당 프로토콜을 준수하는 GET, POST, DELETE, PUT, HEAD 메소드를 구현하였습니다.
- 비동기 방식으로 클라이언트 요청을 처리합니다.
- 기본(index.html) 웹페이지를 구현하였습니다.
- 에러 페이지들을 구현하였습니다.
```

## 📸 Screenshots
|GET|POST|DELETE|PUT|Siege Test|
|---|---|---|---|---|
|<img width="150" height="300" alt="GET" src="https://avatars.githubusercontent.com/u/85754295?v=4">|<img width="150" height="300"  alt="POST" src="https://avatars.githubusercontent.com/u/67998022?v=4">|<img width="150" height="300"  alt="DELETE" src="https://avatars.githubusercontent.com/u/76660692?v=4">|<img width="150" height="300"  alt="PUT" src="https://avatars.githubusercontent.com/u/83046766?v=4">|<img width="150" height="300"  alt="Siege Test" src="tests/html/asset/git/sige-test.png">|

## 🛠Development Environment
![Generic badge](https://img.shields.io/badge/C++-98-lightgrey.svg)
![Generic badge](https://img.shields.io/badge/VSCode-1.79.2(Universal)-blue.svg)

## ⚙️ Tools
```
1. Github (이슈 및 형상 관리)
2. Notion (커뮤니케이션)
3. Slack  (커뮤니케이션)
4. VSCode (개발)
```

## ✨ Skills & Tech Stack
```
 - C++98
 - POSIX
 - CGI
 - Kernel Queue
 - HTML
 - CSS
```

## 🔀 GIT

1. [Commit 컨벤션](https://github.com/MyLittleWebServer/webserv/discussions/3)
    -  `feat` : 애플리케이션이나 라이브러리에 새로운 기능이 추가될 때 사용.
    -  `fix` : 버그 수정을 하는 내용을 포함하는 경우.
    - `build` : 빌드 시스템 또는 외부 종속성에 영향을 미치는 변경 사항(예시 적용 범위: gulp, broccoli, npm)
    - `ci` : CI 구성 파일 및 스크립트 변경(예시 적용 범위: Travis, Circle, BrowserStack, SauceLabs)
    -  `chore` : (코드의 수정 없이) 다른 작업들을 수행 
    - `docs` : 문서만 변경 
    - `perf` : 성능 향상을 위한 코드 변경 
    - `refactor` : 버그를 수정하거나 기능을 추가하지 않는 코드 변경 사항 
    - `revert` : 코드를 되돌리는 경우
    - `style` : 코드의 의미에 영향을 주지 않는 변경 사항(공백, 서식, 세미콜론 누락 등)
    - `test` : 테스트 추가 또는 기존 테스트 수정
    -  `wip` : 진행 중인 커밋의 경우 

2. Git 브랜치
    - `main` : 배포
    - `develop` : 개발된 기능을 병합하는 브랜치
    - `#[Tracker ID] [Commit Convention Name] / [Function Name]` : 각 기능별 개발을 진행하는 브랜치

## 🗂 Directory Structure
```
📦srcs
 ┣ 📂clients
 ┃ ┣ 📂candidate_fields
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┣ 📜CandidateFields.hpp
 ┃ ┃ ┃ ┗ 📜ICandidateFields.hpp
 ┃ ┃ ┗ 📂srcs
 ┃ ┃ ┃ ┗ 📜CandidateFields.cpp
 ┃ ┣ 📂cgi
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┣ 📜CGI.hpp
 ┃ ┃ ┃ ┗ 📜ICGI.hpp
 ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┗ 📜CGI.cpp
 ┃ ┣ 📂client
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┗ 📜Client.hpp
 ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┗ 📜Client.cpp
 ┃ ┣ 📂method
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┣ 📜DELETE.hpp
 ┃ ┃ ┃ ┣ 📜DummyMethod.hpp
 ┃ ┃ ┃ ┣ 📜GET.hpp
 ┃ ┃ ┃ ┣ 📜IMethod.hpp
 ┃ ┃ ┃ ┗ 📜POST.hpp
 ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┣ 📜DELETE.cpp
 ┃ ┃ ┃ ┣ 📜DummyMethod.cpp
 ┃ ┃ ┃ ┣ 📜GET.cpp
 ┃ ┃ ┃ ┗ 📜POST.cpp
 ┃ ┣ 📂request
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┣ 📜IRequest.hpp
 ┃ ┃ ┃ ┗ 📜Request.hpp
 ┃ ┃ ┣ 📂request_parser
 ┃ ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┃ ┣ 📜IRequestParser.hpp
 ┃ ┃ ┃ ┃ ┗ 📜RequestParser.hpp
 ┃ ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┃ ┗ 📜RequestParser.cpp
 ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┗ 📜Request.cpp
 ┃ ┣ 📂response
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┣ 📜IResponse.hpp
 ┃ ┃ ┃ ┗ 📜Response.hpp
 ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┗ 📜Response.cpp
 ┃ ┗ 📜.DS_Store
 ┣ 📂config
 ┃ ┣ 📂child_config
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┗ 📜IChildConfig.hpp
 ┃ ┃ ┣ 📂location_config
 ┃ ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┃ ┣ 📜ILocationConfig.hpp
 ┃ ┃ ┃ ┃ ┗ 📜LocationConfig.hpp
 ┃ ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┃ ┗ 📜LocationConfig.cpp
 ┃ ┃ ┣ 📂mime_types_config
 ┃ ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┃ ┣ 📜IMimeTypesConfig.hpp
 ┃ ┃ ┃ ┃ ┗ 📜MimeTypesConfig.hpp
 ┃ ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┃ ┗ 📜MimeTypesConfig.cpp
 ┃ ┃ ┣ 📂proxy_config
 ┃ ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┃ ┣ 📜IProxyConfig.hpp
 ┃ ┃ ┃ ┃ ┗ 📜ProxyConfig.hpp
 ┃ ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┃ ┗ 📜ProxyConfig.cpp
 ┃ ┃ ┣ 📂root_config
 ┃ ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┃ ┣ 📜IRootConfig.hpp
 ┃ ┃ ┃ ┃ ┗ 📜RootConfig.hpp
 ┃ ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┃ ┗ 📜RootConfig.cpp
 ┃ ┃ ┗ 📂server_config
 ┃ ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┃ ┣ 📜IServerConfig.hpp
 ┃ ┃ ┃ ┃ ┗ 📜ServerConfig.hpp
 ┃ ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┃ ┗ 📜ServerConfig.cpp
 ┃ ┣ 📂include
 ┃ ┃ ┣ 📜Config.hpp
 ┃ ┃ ┗ 📜IConfig.hpp
 ┃ ┣ 📂parser
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┣ 📜ConfigParser.hpp
 ┃ ┃ ┃ ┗ 📜IConfigParser.hpp
 ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┗ 📜ConfigParser.cpp
 ┃ ┗ 📂src
 ┃ ┃ ┗ 📜Config.cpp
 ┣ 📂exception
 ┃ ┣ 📂include
 ┃ ┃ ┣ 📜ExceptionThrower.hpp
 ┃ ┃ ┗ 📜errorMessage.hpp
 ┃ ┗ 📂src
 ┃ ┃ ┗ 📜ExceptionThrower.cpp
 ┣ 📂server
 ┃ ┣ 📂include
 ┃ ┃ ┣ 📜EventHandler.hpp
 ┃ ┃ ┣ 📜Kqueue.hpp
 ┃ ┃ ┣ 📜Server.hpp
 ┃ ┃ ┗ 📜ServerManager.hpp
 ┃ ┗ 📂src
 ┃ ┃ ┣ 📜EventHandler.cpp
 ┃ ┃ ┣ 📜Kqueue.cpp
 ┃ ┃ ┣ 📜Server.cpp
 ┃ ┃ ┗ 📜ServerManager.cpp
 ┣ 📂utils
 ┃ ┣ 📂checker
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┣ 📜FileChecker.hpp
 ┃ ┃ ┃ ┣ 📜IChecker.hpp
 ┃ ┃ ┃ ┗ 📜IFileChecker.hpp
 ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┗ 📜FileChecker.cpp
 ┃ ┣ 📂reader
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┣ 📜IReader.hpp
 ┃ ┃ ┃ ┗ 📜Reader.hpp
 ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┗ 📜Reader.cpp
 ┃ ┗ 📂util
 ┃ ┃ ┣ 📂include
 ┃ ┃ ┃ ┣ 📜Color.hpp
 ┃ ┃ ┃ ┣ 📜Status.hpp
 ┃ ┃ ┃ ┣ 📜Utils.hpp
 ┃ ┃ ┃ ┗ 📜Utils.tpp
 ┃ ┃ ┗ 📂src
 ┃ ┃ ┃ ┣ 📜Status.cpp
 ┃ ┃ ┃ ┗ 📜Utils.cpp
 ┗ 📜main.cpp
```

## 🧑‍💻 Authors
[Chanheki](https://github.com/chanhihi)   
[Jang-cho](https://github.com/cjho0316)   
[Jincpark](https://github.com/Clearsu)   
[Sechung](https://github.com/middlefitting)   
