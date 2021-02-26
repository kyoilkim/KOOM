Copyright 2021. 김교일 all rights reserved.
=============================================================
KOOM is a wolfenstein 3D style FPS game.
=============================================================
---General---
KOOM은 1학년 겨울방학 동안 취미로 만든 울펜슈타인 3D 스타일의 FPS게임 입니다.
KOOM 은 KAIST + DOOM을 뜻하는데, 이는 게임요소들이 카이스트 캠퍼스에서 영감을 얻어 만들어졌기 때문입니다.

KOOM은 싱글플레이어 버전과 멀티플레이어 버전이 따로 존재합니다. 현 디렉토리는 싱글플레이어에 대한 내용이고, 멀티플레이어 버전에 대한 내용은 KOOM/Multiplayer 를 참고해주세요. 

아직 주석을 제대로 작성하지 않아 가독성이 떨어지고, 맞지 않는 부분이 많습니다. 이후 수정하도록 하겠습니다.

---About Game---
Release0.0.2.zip을 다운로드, 압축 해제 한 후, GAME.exe를 눌러 게임을 실행하세요.(오직 windows x64 호환입니다.)
[Goal]
게임은 실행과 동시에 시작됩니다. 맵의 4곳의 포탈에서 몬스터가 무작위로 소환되고, 플레이어는 몬스터를 처치하며 살아남아 score를 10000점에 이르게 하는 것이 목적입니다.

[Story]
우리의 주인공 넙죽이가 어느날 포닉스에게 납치되어 지하 던전에 갇혔습니다. 세뇌당한 휴보와 거위, 그리고 포닉스가 넙죽이를 공격해옵니다. 적들을 쓰러뜨리고 넙죽이가  탈출할 수 있도록 도와주세요!

[Player Control] 
'w'로 전진, 's'로 후진합니다.
'a'로 시선을 좌로 이동하고, 'd'로 시선을 우로 이동합니다.
'space bar' 로 공격합니다.

[Monsters]
1. 휴보(from CS101) : 근접 공격 또는 비퍼(지뢰) 놓아 공격합니다.
2. 거위 : 플레이어를 조준하여 초음파로 공격합니다.
3. 포닉스 : 8방위로 파이어볼을 날려 공격합니다.

몬스터를 처치할 때마다 score 가 각각 100, 200, 300 씩 증가합니다. 
스코어가 높아질수록 게임의 난이도는 증가합니다.

[Items]
1. 딸기 : HP를 10 회복합니다.
2. 커피 : MP를 10 회복합니다.


---How to compile---
(윈도우10 64bit, Code::Blocks(MinGW) 기준입니다.)

KOOM은 C++로 작성되었으며 사용된 라이브러리는 다음과 같습니다. 
그래픽: OpenGL (freeglut)
오디오: SDL2_mixer

1. 외부 라이브러리 다운로드
freeglut : https://www.transmissionzero.co.uk/files/software/development/GLUT/freeglut-MinGW.zip
SDL2 : https://www.libsdl.org/release/SDL2-devel-2.0.14-mingw.tar.gz
SDL2_mixer : https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.0.4-mingw.tar.gz

2. 라이브러리 설치
*컴파일된 프로그램이 64비트 용이면, x64라는 폴더 아래의 내용을 사용해주세요.
(1)
다운로드 받은 라이브러리의 bin 폴더 내용은 컴파일된 프로그램과 같은 폴더에 추가해주세요. 

(2)
다운로드 받은 라이브러리의 include 폴더를 아래 폴더와 합쳐주세요.
CodeBlocks\MinGW\x86_64-w64-mingw32\include

(3)
다운로드 받은 라이브러리의 lib 폴더를 아래 폴더와 합쳐주세요.
CodeBlocks\MinGW\x86_64-w64-mingw32\lib

3. Code:Blocks에서 라이브러리 링크
Build Option -> Linker settings 에 다음을 순서대로 추가해주세요.
========
freeglut
opengl32
glu32
winmm
gdi32
mingw32
SDL2main
SDL2
SDL2_mixer
========

4.컴파일

---Helpful Links---
제가 도움 받았던 링크들을 모아보았습니다.

1. Make Your Own Raycaster Part 1
https://www.youtube.com/watch?v=gYRrGTC7GtA&t=17s

2. Make Your Own Raycaster Part 2
https://www.youtube.com/watch?v=PC1RaETIx3Y&t=575s

3. Installing FreeGLut [or GLUT] | C++ & OpenGL development environment setup(Code::Blocks) on windows
https://www.youtube.com/watch?v=HtJAQS2YDCY

4. GLUT 튜토리얼 (Grafix3d.net 한글 번역)
https://sungcheol-kim.gitbook.io/glut-tutorials/

5. Learn OpenGL 번역
https://heinleinsgame.tistory.com/3?category=757483

6. Learn OpenGL
https://learnopengl.com/Introduction

7. Lode's Computer Graphics Tutorial
https://lodev.org/cgtutor/index.html

8. Ray-Casting Tutorial (by permadi)
https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/

9. 게임 관련 라이브러리 리스트
http://web.archive.org/web/20160318233829/http://content.gpwiki.org/Libraries

10.SDL_mixer Documentation
https://www.libsdl.org/projects/SDL_mixer/docs/index.html

11.OpenGL 강좌 by soen
http://soen.kr/

12. C언어 코딩 도장 중 비트맵 파일 관련 부분
https://dojang.io/mod/page/view.php?id=701

13.SDL2 wiki , FAQ : Windows
https://wiki.libsdl.org/FAQWindows#I_get_.22Undefined_reference_to_.27WinMain.4016.27.22

14. Play a sound with SDL2(without SDL_mixer) by armornick
https://gist.github.com/armornick/3447121

15. Get Time in Milliseconds in C++ (DelftStack)
https://www.delftstack.com/howto/cpp/how-to-get-time-in-milliseconds-cpp/

16. Youtube channel : thecplusplusguy
https://www.youtube.com/user/thecplusplusguy/playlists

더 생각나는 대로 추가하겠습니다.
------
