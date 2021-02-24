Copyright 2021. 김교일 all rights reserved.
==========================================
---General---
KOOM의 멀티플레이어 버전입니다.
기본적으로 KOOM과 비슷하나, 서버에 접속한 다른 플레이어와 겨루는 형식입니다.
공격 방식을 바꾸기 전에 작성하였기 때문에, 현재의 KOOM과 공격방식이 다릅니다.(총알이 보이지 않고, 데미지가 즉시 적용됩니다.)

통신 부분은 Winsock2를 이용하여 작성하였습니다.(기타 부분은 KOOM과 동일합니다.)

---How to Compile---
1.추가적인 라이브러리 다운로드는 필요없습니다.

2.
builed option -> Linker settings
===
KOOM과 중복되는 내용
ws2_32
===
---Helpful Links---
1. https://www.binarytides.com/code-tcp-socket-server-winsock/
2. https://www.binarytides.com/winsock-socket-programming-tutorial/
3. https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client
4. https://beej.us/guide/bgnet/html/
