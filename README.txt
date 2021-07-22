Copyright 2021. 김교일 all rights reserved.
=============================================================
KOOM is a wolfenstein 3D style FPS game.
=============================================================
---General---
KOOM is Wolfenstein 3D style FPS game written in C(++). 
There are singleplayer version and multiplayer version. This directory is about singleplayer. You can get information about multiplayer version in KOOM/Multiplayer.
I am sorry that there isn't great comments in source file. I will edit them.

Demo : https://www.youtube.com/watch?v=4Tpoqmd-HOg

---About Game---
Download Release0.0.2.zip, then unzip it. Double click GAME.exe to play this game. (Only windows x64 is competible)
[Goal]
The game is starts right away. There is 4 portals which monsters are sponed. The goal of the game is to survive and kill the monsters until player gets 10000 points.

[Story]
One day our mascot 'Nubjuki' is kidnapped and put in to dungeon by Ponix. Help Nubjuki to punish Ponix and escape the dungeon!

[Player Control] 
Press 'w'to move forward, 's'to move back.
Press 'a'to look left, 'd'to look right.
Press 'space bar' to attack.

[Monsters]
1. Hubo(from CS101) : melee attack,  mine(비퍼) attack.
2. Goose : long distance attack with ultrasonic wave(one at a time).
3. Ponix : long distance attack. (shoots fireballs in 8 direction at a time)

You can get score when you kill monsters. (Hubo:100, Goose:200, Ponix:300)
The higher the score is the higher the difficulty is.

[Items]
1. Strawberry : recover 10 HP.
2. Coffee : recover 10 MP.


---How to compile---
(In Windows10 64bit, Code::Blocks(MinGW))

KOOMis written in C(++) and used external libraries are below.
Graphics: OpenGL (freeglut)
Audio : SDL2_mixer

1. Download external libraries.
freeglut : https://www.transmissionzero.co.uk/files/software/development/GLUT/freeglut-MinGW.zip
SDL2 : https://www.libsdl.org/release/SDL2-devel-2.0.14-mingw.tar.gz
SDL2_mixer : https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.0.4-mingw.tar.gz

2. Install external libraries
*if you want your program is for x64, do below under x64 folder.
(1)
put bin folder in compile destination.

(2)
combine external librarie's include folder in below location.
CodeBlocks\MinGW\x86_64-w64-mingw32\include

(3)
combine external librarie's lib folder in below location
CodeBlocks\MinGW\x86_64-w64-mingw32\lib

3. Library link in Code:Blocks
Add below in Build Option -> Linker settings.
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

4.Compile

---Helpful Links---

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

11.OpenGL Tutorial by soen
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
------

Future :
Implement delta time.
