//Copyright 2021. 김교일 all rights reserved.



//시간 , 타이밍을 다루는 변수에서 오버플로우가 일어나 unsigned long long 으로 변환함.

#define _CRT_SECURE_NO_WARNINGS

#include<GL/glut.h>
#include<GL/glu.h>
#include<Gl/gl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/time.h>
#include<time.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>

#define W_WIDTH 960
#define W_HEIGHT 640+100
#define TEXTURE_SIZE 32*32*31
#define WINSCREEN_SIZE 128*80
#define GAMEOVERSCREEN_SIZE 128*96
#define mapX 10
#define mapY 10
#define mapS 100
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //one degree in radians

#define max_bullets 5
#define max_goose_bullets 5
#define max_ponix_bullets 16
#define max_hubo_beepers 5

#define mobn 10
#define itemn 10

#define WIN_SCORE 10000

#define PIXEL_SIZE   3    // 픽셀 한 개의 크기 3바이트(24비트)
#define PIXEL_ALIGN  4    // 픽셀 데이터 가로 한 줄은 4의 배수 크기로 저장됨

#pragma pack(push, 1)                // 구조체를 1바이트 크기로 정렬

Mix_Music *main_music;
Mix_Chunk *player_attck_sound;
Mix_Chunk *footstep_forward_sound;
Mix_Chunk *footstep_backward_sound;
Mix_Chunk *hubo_attack_sound;
Mix_Chunk *goose_attack_sound;
Mix_Chunk *ponix_attack_sound;
Mix_Chunk *door_open_sound;
Mix_Chunk *portal_open_sound;
Mix_Chunk *warp_sound;
Mix_Chunk *heal_item_sound;
Mix_Chunk *damage_item_sound;
Mix_Music *game_over_sound;
Mix_Music *win_sound;

typedef struct _BITMAPFILEHEADER     // BMP 비트맵 파일 헤더 구조체
{
    unsigned short bfType;           // BMP 파일 매직 넘버
    unsigned int   bfSize;           // 파일 크기
    unsigned short bfReserved1;      // 예약
    unsigned short bfReserved2;      // 예약
    unsigned int   bfOffBits;        // 비트맵 데이터의 시작 위치
} BMFH;

typedef struct _BITMAPINFOHEADER     // BMP 비트맵 정보 헤더 구조체(DIB 헤더)
{
    unsigned int   biSize;           // 현재 구조체의 크기
    int            biWidth;          // 비트맵 이미지의 가로 크기
    int            biHeight;         // 비트맵 이미지의 세로 크기
    unsigned short biPlanes;         // 사용하는 색상판의 수
    unsigned short biBitCount;       // 픽셀 하나를 표현하는 비트 수
    unsigned int   biCompression;    // 압축 방식
    unsigned int   biSizeImage;      // 비트맵 이미지의 픽셀 데이터 크기
    int            biXPelsPerMeter;  // 그림의 가로 해상도(미터당 픽셀)
    int            biYPelsPerMeter;  // 그림의 세로 해상도(미터당 픽셀)
    unsigned int   biClrUsed;        // 색상 테이블에서 실제 사용되는 색상 수
    unsigned int   biClrImportant;   // 비트맵을 표현하기 위해 필요한 색상 인덱스 수
} BMIH;

typedef struct _RGBTRIPLE            // 24비트 비트맵 이미지의 픽셀 구조체
{
    unsigned char rgbtBlue;          // 파랑
    unsigned char rgbtGreen;         // 초록
    unsigned char rgbtRed;           // 빨강
} RGBTP;

#pragma pack(pop)



typedef struct
{
    int w,a,s,d, space;
}ButtonKeys;
ButtonKeys Keys;



int pHP=100; //player Health Point
int pMP=100;
int score=0;

float px, py, pdx, pdy, pa=P3; //player position
float frame1, frame2, fps;

float bulletX[max_bullets]={0};
float bulletY[max_bullets]={0};
float bulletA[max_bullets]={0};
int bulletC[max_bullets]={0};
int bulletI=0; //bullet index
bool player_attack_bool=false;

float goose_bulletX[max_goose_bullets]={0};
float goose_bulletY[max_goose_bullets]={0};
float goose_bulletA[max_goose_bullets]={0};
int goose_bulletC[max_goose_bullets]={0};
int goose_bulletI=0; //goose bullet index

float ponix_bulletX[max_ponix_bullets]={0};
float ponix_bulletY[max_ponix_bullets]={0};
float ponix_bulletA[max_ponix_bullets]={0};
int ponix_bulletC[max_ponix_bullets]={0};
int ponix_bulletI=0; //goose bullet index

float hubo_beeperX[max_hubo_beepers]={0};
float hubo_beeperY[max_hubo_beepers]={0};
int hubo_beeperC[max_hubo_beepers]={0};
int hubo_beeperI=0;


unsigned long long last_monster_spawn_time=0;
unsigned long long last_item_spawn_time=0;
unsigned long long last_time=0; //플레이어가 마지막으로 공격한 시각
unsigned long long last_monster_move_time=0;
unsigned long long player_death_time=0; //죽었을 때 시각을 저장
unsigned long long win_time=0;

int red[TEXTURE_SIZE]={0};
int green[TEXTURE_SIZE]={0};
int blue[TEXTURE_SIZE]={0};

int win_red[WINSCREEN_SIZE]={0};
int win_green[WINSCREEN_SIZE]={0};
int win_blue[WINSCREEN_SIZE]={0};

int gameover_red[GAMEOVERSCREEN_SIZE]={0};
int gameover_green[GAMEOVERSCREEN_SIZE]={0};
int gameover_blue[GAMEOVERSCREEN_SIZE]={0};



float mobx[mobn]={0};
float moby[mobn]={0};
int mobHP[mobn]={0}; //monster Health Point
unsigned long long mob_last_time[mobn]={0}; //몬스터가 마지막으로 공격한 시간
int mobTexture[mobn]={0};//15~20
int mobI=0;

float itemx[itemn]={0};
float itemy[itemn]={0};
int itemHP[itemn]={0};
int itemTexture[itemn]={0};
int itemI=0;

int mapW[mapS]=
{
1,1,1,1,6,6,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
6,0,0,0,0,0,0,0,0,6,
6,0,0,0,0,0,0,0,0,6,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,1,1,1,6,6,1,1,1,1,
};

//------------------------

float dist(float ax, float ay, float bx, float by)
{
    return( sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay)) );
}

int floatcomp(const void* elem1, const void* elem2) //내림차순 정렬
{
    if(*(const float*)elem1 > *(const float*)elem2)
        return -1;
    return *(const float*)elem1 < *(const float*)elem2;
}

int floatcomp_rev(const void* elem1, const void* elem2) //오름차순 정렬
{
    if(*(const float*)elem1 < *(const float*)elem2)
        return -1;
    return *(const float*)elem1 > *(const float*)elem2;
}


int loadTexture(void){

    FILE *fpBmp;                    // 비트맵 파일 포인터
    BMFH fileHeader;    // 비트맵 파일 헤더 구조체 변수
    BMIH infoHeader;    // 비트맵 정보 헤더 구조체 변수

    unsigned char *image;    // 픽셀 데이터 포인터
    int size;                // 픽셀 데이터 크기
    int width, height;       // 비트맵 이미지의 가로, 세로 크기
    int padding;             // 픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기


    fpBmp = fopen("texture\\texture.bmp", "rb");    // 비트맵 파일을 바이너리 모드로 열기
    if (fpBmp == NULL)    // 파일 열기에 실패하면
        return -1;         // 프로그램 종료

    // 비트맵 파일 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&fileHeader, sizeof(BMFH), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return -1;
    }

    // 매직 넘버가 MB가 맞는지 확인(2바이트 크기의 BM을 리틀 엔디언으로 읽었으므로 MB가 됨)
    // 매직 넘버가 맞지 않으면 프로그램 종료
    if (fileHeader.bfType != 'MB')
    {
        fclose(fpBmp);
        return -1;
    }

    // 비트맵 정보 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&infoHeader, sizeof(BMIH), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return -1;
    }

    // 24비트 비트맵이 아니면 프로그램 종료
    if (infoHeader.biBitCount != 24)
    {
        fclose(fpBmp);
        return -1;
    }

    size = infoHeader.biSizeImage;    // 픽셀 데이터 크기
    width = infoHeader.biWidth;       // 비트맵 이미지의 가로 크기
    height = infoHeader.biHeight;     // 비트맵 이미지의 세로 크기

    // 이미지의 가로 크기에 픽셀 크기를 곱하여 가로 한 줄의 크기를 구하고 4로 나머지를 구함
    // 그리고 4에서 나머지를 빼주면 남는 공간을 구할 수 있음.
    // 만약 남는 공간이 0이라면 최종 결과가 4가 되므로 여기서 다시 4로 나머지를 구함
    padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

    if (size == 0)    // 픽셀 데이터 크기가 0이라면
    {
        // 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
        // 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
        size = (width * PIXEL_SIZE + padding) * height;
    }

    image = (unsigned char *)malloc(size);    // 픽셀 데이터의 크기만큼 동적 메모리 할당

    // 파일 포인터를 픽셀 데이터의 시작 위치로 이동
    fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

    // 파일에서 픽셀 데이터 크기만큼 읽음. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(image, size, 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }

    fclose(fpBmp);    // 비트맵 파일 닫기


    // 픽셀 데이터는 아래 위가 뒤집혀서 저장되므로 아래쪽부터 반복
    // 세로 크기만큼 반복
    for (int y = height - 1; y >= 0; y--)
    {
        // 가로 크기만큼 반복
        for (int x = 0; x < width; x++)
        {
            // 일렬로 된 배열에 접근하기 위해 인덱스를 계산
            // (x * 픽셀 크기)는 픽셀의 가로 위치
            // (y * (세로 크기 * 픽셀 크기))는 픽셀이 몇 번째 줄인지 계산
            // 남는 공간 * y는 줄별로 누적된 남는 공간
            int index = (x * PIXEL_SIZE) + (y * (width * PIXEL_SIZE)) + (padding * y);

            // 현재 픽셀의 주소를 RGBTRIPLE 포인터로 변환하여 RGBTRIPLE 포인터에 저장
            RGBTP *pixel = (RGBTP *)&image[index];

            // RGBTRIPLE 구조체로 파랑, 초록, 빨강값을 가져옴

            red[(height-1-y)*width+x] = (int)(pixel->rgbtRed);
            green[(height-1-y)*width+x] = (int)(pixel->rgbtGreen);
            blue[(height-1-y)*width+x] = (int)(pixel->rgbtBlue);
        }
    }

    free(image);
    return 0;

}

int loadWinscreen(void){

    FILE *fpBmp;                    // 비트맵 파일 포인터
    BMFH fileHeader;    // 비트맵 파일 헤더 구조체 변수
    BMIH infoHeader;    // 비트맵 정보 헤더 구조체 변수

    unsigned char *image;    // 픽셀 데이터 포인터
    int size;                // 픽셀 데이터 크기
    int width, height;       // 비트맵 이미지의 가로, 세로 크기
    int padding;             // 픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기


    fpBmp = fopen("texture\\winscreen.bmp", "rb");    // 비트맵 파일을 바이너리 모드로 열기
    if (fpBmp == NULL)    // 파일 열기에 실패하면
        return -1;         // 프로그램 종료

    // 비트맵 파일 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&fileHeader, sizeof(BMFH), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return -1;
    }

    // 매직 넘버가 MB가 맞는지 확인(2바이트 크기의 BM을 리틀 엔디언으로 읽었으므로 MB가 됨)
    // 매직 넘버가 맞지 않으면 프로그램 종료
    if (fileHeader.bfType != 'MB')
    {
        fclose(fpBmp);
        return -1;
    }

    // 비트맵 정보 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&infoHeader, sizeof(BMIH), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return -1;
    }

    // 24비트 비트맵이 아니면 프로그램 종료
    if (infoHeader.biBitCount != 24)
    {
        fclose(fpBmp);
        return -1;
    }

    size = infoHeader.biSizeImage;    // 픽셀 데이터 크기
    width = infoHeader.biWidth;       // 비트맵 이미지의 가로 크기
    height = infoHeader.biHeight;     // 비트맵 이미지의 세로 크기

    // 이미지의 가로 크기에 픽셀 크기를 곱하여 가로 한 줄의 크기를 구하고 4로 나머지를 구함
    // 그리고 4에서 나머지를 빼주면 남는 공간을 구할 수 있음.
    // 만약 남는 공간이 0이라면 최종 결과가 4가 되므로 여기서 다시 4로 나머지를 구함
    padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

    if (size == 0)    // 픽셀 데이터 크기가 0이라면
    {
        // 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
        // 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
        size = (width * PIXEL_SIZE + padding) * height;
    }

    image = (unsigned char *)malloc(size);    // 픽셀 데이터의 크기만큼 동적 메모리 할당

    // 파일 포인터를 픽셀 데이터의 시작 위치로 이동
    fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

    // 파일에서 픽셀 데이터 크기만큼 읽음. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(image, size, 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }

    fclose(fpBmp);    // 비트맵 파일 닫기


    // 픽셀 데이터는 아래 위가 뒤집혀서 저장되므로 아래쪽부터 반복
    // 세로 크기만큼 반복
    for (int y = height - 1; y >= 0; y--)
    {
        // 가로 크기만큼 반복
        for (int x = 0; x < width; x++)
        {
            // 일렬로 된 배열에 접근하기 위해 인덱스를 계산
            // (x * 픽셀 크기)는 픽셀의 가로 위치
            // (y * (세로 크기 * 픽셀 크기))는 픽셀이 몇 번째 줄인지 계산
            // 남는 공간 * y는 줄별로 누적된 남는 공간
            int index = (x * PIXEL_SIZE) + (y * (width * PIXEL_SIZE)) + (padding * y);

            // 현재 픽셀의 주소를 RGBTRIPLE 포인터로 변환하여 RGBTRIPLE 포인터에 저장
            RGBTP *pixel = (RGBTP *)&image[index];

            // RGBTRIPLE 구조체로 파랑, 초록, 빨강값을 가져옴

            win_red[(height-1-y)*width+x] = (int)(pixel->rgbtRed);
            win_green[(height-1-y)*width+x] = (int)(pixel->rgbtGreen);
            win_blue[(height-1-y)*width+x] = (int)(pixel->rgbtBlue);
        }
    }

    free(image);
    return 0;

}

int loadGameoverscreen(void){

    FILE *fpBmp;                    // 비트맵 파일 포인터
    BMFH fileHeader;    // 비트맵 파일 헤더 구조체 변수
    BMIH infoHeader;    // 비트맵 정보 헤더 구조체 변수

    unsigned char *image;    // 픽셀 데이터 포인터
    int size;                // 픽셀 데이터 크기
    int width, height;       // 비트맵 이미지의 가로, 세로 크기
    int padding;             // 픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기


    fpBmp = fopen("texture\\gameoverscreen.bmp", "rb");    // 비트맵 파일을 바이너리 모드로 열기
    if (fpBmp == NULL)    // 파일 열기에 실패하면
        return -1;         // 프로그램 종료

    // 비트맵 파일 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&fileHeader, sizeof(BMFH), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return -1;
    }

    // 매직 넘버가 MB가 맞는지 확인(2바이트 크기의 BM을 리틀 엔디언으로 읽었으므로 MB가 됨)
    // 매직 넘버가 맞지 않으면 프로그램 종료
    if (fileHeader.bfType != 'MB')
    {
        fclose(fpBmp);
        return -1;
    }

    // 비트맵 정보 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&infoHeader, sizeof(BMIH), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return -1;
    }

    // 24비트 비트맵이 아니면 프로그램 종료
    if (infoHeader.biBitCount != 24)
    {
        fclose(fpBmp);
        return -1;
    }

    size = infoHeader.biSizeImage;    // 픽셀 데이터 크기
    width = infoHeader.biWidth;       // 비트맵 이미지의 가로 크기
    height = infoHeader.biHeight;     // 비트맵 이미지의 세로 크기

    // 이미지의 가로 크기에 픽셀 크기를 곱하여 가로 한 줄의 크기를 구하고 4로 나머지를 구함
    // 그리고 4에서 나머지를 빼주면 남는 공간을 구할 수 있음.
    // 만약 남는 공간이 0이라면 최종 결과가 4가 되므로 여기서 다시 4로 나머지를 구함
    padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

    if (size == 0)    // 픽셀 데이터 크기가 0이라면
    {
        // 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
        // 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
        size = (width * PIXEL_SIZE + padding) * height;
    }

    image = (unsigned char *)malloc(size);    // 픽셀 데이터의 크기만큼 동적 메모리 할당

    // 파일 포인터를 픽셀 데이터의 시작 위치로 이동
    fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

    // 파일에서 픽셀 데이터 크기만큼 읽음. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(image, size, 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }

    fclose(fpBmp);    // 비트맵 파일 닫기


    // 픽셀 데이터는 아래 위가 뒤집혀서 저장되므로 아래쪽부터 반복
    // 세로 크기만큼 반복
    for (int y = height - 1; y >= 0; y--)
    {
        // 가로 크기만큼 반복
        for (int x = 0; x < width; x++)
        {
            // 일렬로 된 배열에 접근하기 위해 인덱스를 계산
            // (x * 픽셀 크기)는 픽셀의 가로 위치
            // (y * (세로 크기 * 픽셀 크기))는 픽셀이 몇 번째 줄인지 계산
            // 남는 공간 * y는 줄별로 누적된 남는 공간
            int index = (x * PIXEL_SIZE) + (y * (width * PIXEL_SIZE)) + (padding * y);

            // 현재 픽셀의 주소를 RGBTRIPLE 포인터로 변환하여 RGBTRIPLE 포인터에 저장
            RGBTP *pixel = (RGBTP *)&image[index];

            // RGBTRIPLE 구조체로 파랑, 초록, 빨강값을 가져옴

            gameover_red[(height-1-y)*width+x] = (int)(pixel->rgbtRed);
            gameover_green[(height-1-y)*width+x] = (int)(pixel->rgbtGreen);
            gameover_blue[(height-1-y)*width+x] = (int)(pixel->rgbtBlue);
        }
    }

    free(image);
    return 0;

}


void drawSprite(int a, int b, int s, int n) //x좌표, y좌표, 크기, 텍스쳐 인덱스
{
        int x=a;
        int y=b;
        for(int i=0;i<32;i++)
        {
            for(int j=0;j<32;j++)
            {
                int color_index=32*32*(n-1)+j+i*32;
                if(red[color_index]==255 && green[color_index]==255 && blue[color_index]==255){continue;}
                else{glColor3f((float)red[color_index]/255,(float)green[color_index]/255,(float)blue[color_index]/255);}
                x=a+j*s;
                y=b+i*s;
                glPointSize(s);glBegin(GL_POINTS);glVertex2i(x,y);glEnd();
            }
        }

}


void playerAttack() //몬스터의 x좌표, y좌표, 텍스쳐 인덱스
{

    //create bullet
    //max bullet is 5
    if(player_attack_bool)
    {
        Mix_PlayChannel(-1, player_attck_sound,0);
        bulletX[bulletI]=px;
        bulletY[bulletI]=py;
        bulletA[bulletI]=pa;
        bulletC[bulletI]=1; //bullet condition (on=1/off=0)
        player_attack_bool=false;
        ++bulletI;if(bulletI>=max_bullets){bulletI=0;}
    }

    for(int j=0;j<max_bullets;j++)
    {
        if(bulletC[j]!=1) continue;
        //몬스터 맞추면
        for(int i=0; i<mobn; i++)
        {
            if(dist(bulletX[j], bulletY[j], mobx[i], moby[i])<=30 && mobHP[i]>=0)
            {
                mobHP[i]--;
                if(mobHP[i]<=0)
                {
                    if(mobTexture[i]==15||mobTexture[i]==16) score+=100;
                    else if(mobTexture[i]==17||mobTexture[i]==18) score+=200;
                    else if(mobTexture[i]==19||mobTexture[i]==20) score+=300;
                    mobx[i]=-10; moby[i]=-10;
                }
                bulletC[j]=0; bulletX[j]=0; bulletY[j]=0;
                break;
            }
        }
        //벽에 부딛히면
        int mx=(int)(bulletX[j])>>6; int my=(int)(bulletY[j])>>6; int mp=my*mapX+mx;
        if(mapW[mp]!=0)//(mapW[(int)(bulletY[j]/64)*mapX+(int)(bulletX[j]/64)]!=0)
        {
            bulletC[j]=0;bulletX[j]=0; bulletY[j]=0;
            continue;
        }

        else
        {
            bulletX[j]+=cos(bulletA[j])*7;
            bulletY[j]+=sin(bulletA[j])*7;
        }

    }
}

void monsterAttack()
{
//-----clear dead mobs' bullets
    int hubo_check=0;
    int goose_check=0;
    int ponix_check=0;


    for(int i=0; i<mobn;i++)
    {
        if(mobTexture[i]==15 || mobTexture[i]==16)
        {
            hubo_check+=mobHP[i];
            continue;
        }

        if(mobTexture[i]==17 || mobTexture[i]==18)
        {
            goose_check+=mobHP[i];
            continue;
        }

        if(mobTexture[i]==19 || mobTexture[i]==20)
        {
            ponix_check+=mobHP[i];
        }
    }

    if(hubo_check==0)
    {
        for(int i=0;i<max_hubo_beepers;i++)
        {
            hubo_beeperC[i]=0;
        }
    }

    if(goose_check==0)
    {
        //goose_bulletI=0;
        for(int i=0;i<max_goose_bullets;i++)
        {
            goose_bulletC[i]=0;
        }
    }
    if(ponix_check==0)
    {
        //ponix_bulletI=0;
        for(int i=0;i<max_ponix_bullets;i++)
        {
            ponix_bulletC[i]=0;
        }
    }

//-------------------


    for(int i=0; i<mobn; i++)
    {
        if(mobHP[i]<=0 || pHP<=0 || score>=WIN_SCORE)
        {
            continue;
        }

        struct timeval time_now{};
        gettimeofday(&time_now, nullptr);

        unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);

        //HUBO near attack
        if((mobTexture[i]==15 || mobTexture[i]==16) && current_time-mob_last_time[i]>=2000)
        {
            Mix_PlayChannel(-1, hubo_attack_sound,0);
            mob_last_time[i]=current_time;
            if(dist(mobx[i], moby[i], px, py)<=100 )
            {
                pHP-=4;
                if(pHP<0){pHP=0;}
                continue;
            }
            else
            {
                hubo_beeperX[hubo_beeperI]=mobx[i];
                hubo_beeperY[hubo_beeperI]=moby[i];
                hubo_beeperC[hubo_beeperI]=1; //beeper condition(on=1/off=0)
                ++hubo_beeperI; if(hubo_beeperI>=max_hubo_beepers) {hubo_beeperI=0;}
            }
        }

        //GOOSE bullet attack
        if(mobTexture[i]==17 || mobTexture[i]==18)
        {
            float mx=mobx[i];
            float my=moby[i];

            double ma=atan2((double)(my-py), (double)(mx-px));
            double theta=(py-my)/(px-mx);

            if((py-my)>0 && (px-mx)>0){ma=atan(theta);}
            else if((py-my)>0 && (px-mx)<0){ma=PI+atan(theta);}
            else if((py-my)<0 && (px-mx)<0){ma=PI+atan(theta);}
            else if((py-my)<0 && (px-mx)>0){ma=2*PI+atan(theta);}

            if(current_time-mob_last_time[i]>=1000)
            {
                Mix_PlayChannel(-1, goose_attack_sound,0);
                goose_bulletX[goose_bulletI]=mobx[i];
                goose_bulletY[goose_bulletI]=moby[i];
                goose_bulletA[goose_bulletI]=ma;
                goose_bulletC[goose_bulletI]=1; //bullet condition (on=1/off=0)
                mob_last_time[i]=current_time;
                ++goose_bulletI;if(goose_bulletI>=max_goose_bullets){goose_bulletI=0;}
            }

            for(int j=0;j<max_goose_bullets;j++)
            {
                if(goose_bulletC[j]!=1) {continue;}
                //플레이어 맞추면
                if(dist(goose_bulletX[j], goose_bulletY[j], px, py)<=15 && pHP>=0)
                {
                    pHP-=4;
                    goose_bulletC[j]=0; goose_bulletX[j]=0; goose_bulletY[j]=0;
                    continue;
                }

                //벽에 부딛히면
                int mx=(int)(goose_bulletX[j])>>6; int my=(int)(goose_bulletY[j])>>6; int mp=my*mapX+mx;
                if(mapW[mp]!=0)//(mapW[(int)(bulletY[j]/64)*mapX+(int)(bulletX[j]/64)]!=0)
                {
                    goose_bulletC[j]=0;goose_bulletX[j]=0; goose_bulletY[j]=0;
                    continue;
                }

                //GOOSE가 죽으면 goose_bullet 어떻게 없애지

                goose_bulletX[j]+=cos(goose_bulletA[j])*2;
                goose_bulletY[j]+=sin(goose_bulletA[j])*2;
            }

        }

        //PONIX bullet attack
        if(mobTexture[i]==19 || mobTexture[i]==20)
        {
            double ma=0;

            if(current_time-mob_last_time[i]>=1000)
            {
                Mix_PlayChannel(-1, ponix_attack_sound,0);
                for(int k=0;k<8;k++)
                {
                    ma=(2*PI/8)*k;
                    ponix_bulletX[ponix_bulletI]=mobx[i];
                    ponix_bulletY[ponix_bulletI]=moby[i];
                    ponix_bulletA[ponix_bulletI]=ma;
                    ponix_bulletC[ponix_bulletI]=1; //bullet condition (on=1/off=0)
                    ++ponix_bulletI;if(ponix_bulletI>=max_ponix_bullets){ponix_bulletI=0;}
                }
                mob_last_time[i]=current_time;
            }

            for(int j=0;j<max_ponix_bullets;j++)
            {
                if(ponix_bulletC[j]!=1) {continue;}
                //플레이어 맞추면
                if(dist(ponix_bulletX[j], ponix_bulletY[j], px, py)<=15 && pHP>=0)
                {
                    pHP-=4;
                    ponix_bulletC[j]=0; ponix_bulletX[j]=0; ponix_bulletY[j]=0;
                    continue;
                }

                //벽에 부딛히면
                int mx=(int)(ponix_bulletX[j])>>6; int my=(int)(ponix_bulletY[j])>>6; int mp=my*mapX+mx;
                if(mapW[mp]!=0)//(mapW[(int)(bulletY[j]/64)*mapX+(int)(bulletX[j]/64)]!=0)
                {
                    ponix_bulletC[j]=0;ponix_bulletX[j]=0; ponix_bulletY[j]=0;
                    continue;
                }

                //PONIX가 죽으면 ponix_bullet 어떻게 없애지

                ponix_bulletX[j]+=cos(ponix_bulletA[j])*3;
                ponix_bulletY[j]+=sin(ponix_bulletA[j])*3;
            }

        }
    }

}

void monsterAI()
{
    struct timeval time_now{};
    gettimeofday(&time_now, nullptr);

    unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);

    if(current_time-last_monster_move_time>=20)
    {
        last_monster_move_time=current_time;
        for(int i=0; i<mobn; i++)
        {
            if(mobHP[i]<=0) continue;

            int speed, mx, my, mp;

            if(mobTexture[i]==15 || mobTexture[i]==16) speed=1; //i don't know why but speed with less than 1.0 doesn't work.
            else if(mobTexture[i]==17 || mobTexture[i]==18) speed=1.1;
            else if(mobTexture[i]==19 || mobTexture[i]==20) speed=1.2;

            mx=(int)(mobx[i]+20)>>6; my=(int)(moby[i]+0)>>6; mp=my*mapX+mx;
            if(mapW[mp]!=0)
            {
                mobx[i]-=3;
                continue;
            }

            mx=(int)(mobx[i]-20)>>6; my=(int)(moby[i]+0)>>6; mp=my*mapX+mx;
            if(mapW[mp]!=0)
            {
                mobx[i]+=3;
                continue;
            }

            mx=(int)(mobx[i]+0)>>6; my=(int)(moby[i]+20)>>6; mp=my*mapX+mx;
            if(mapW[mp]!=0)
            {
                moby[i]-=3;
                continue;
            }

            mx=(int)(mobx[i]+0)>>6; my=(int)(moby[i]-20)>>6; mp=my*mapX+mx;
            if(mapW[mp]!=0)
            {
                moby[i]+=3;
                continue;
            }



            if(mobx[i]<px) {mobx[i]+=speed;}
            else if(mobx[i]>px) {mobx[i]-=speed;}

            if(moby[i]<py) {moby[i]+=speed;}
            else if(moby[i]>py) {moby[i]-=speed;}

        }
    }
}

void printText(int r, int g, int b, int x, int y, char *message)
{
    glColor3f(r, g, b); glRasterPos2i(x, y);

    int len, i;
    //char *message = "Testing the message";
    len=(int)strlen(message);
    for(i=0;i<len;i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
    }
}

void drawGUI()
{

    //---GUI----

    //기본 태두리
    glColor3f(1, 1, 1); glBegin(GL_QUADS);glVertex2i(0, W_HEIGHT-100);glVertex2i(0,W_HEIGHT);glVertex2i(W_WIDTH, W_HEIGHT);glVertex2i(W_WIDTH, W_HEIGHT-100);glEnd();

    //hand
    int fire=7;

    struct timeval time_now{};
    gettimeofday(&time_now, nullptr);

    unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);

    if(Keys.space==1 && current_time-last_time >= 500 && pMP>=4 && pHP>0 &&score<WIN_SCORE)
    {
            pMP-=4;
            player_attack_bool=true;
            fire=8;
            last_time=current_time;
            //playerAttack();
    }
    else if(current_time-last_time < 300)
    {
        fire=8;
    }
    drawSprite(W_WIDTH/2-32*5/2, W_HEIGHT-100-32*5+3, 5,fire); //i don't know bu some offset(3) happern in y axis.


    //-----status-----
    char hp_text[]="HP";
    printText(1, 0, 0, 10, 670, hp_text);

    //hp bar
    glColor3f(0.7,0.7,0.7);glBegin(GL_QUADS); glVertex2i(60, 655);glVertex2i(60, 655+20-1);glVertex2i(60+200-1, 655+20-1);glVertex2i(60+200-1, 655);glEnd();

    for(int i=0; i<10;i++)
    {
        for(int j=0;j<pHP;j++)
        {
            glColor3f(1,0,0);glPointSize(2);glBegin(GL_POINTS);glVertex2i(60+2*j, 655+2*i);glEnd();
        }
    }
    //mp bar
    char MP_text[]="MP";
    printText(0, 0, 1, 10, 670+30, MP_text);

    glColor3f(0.7,0.7,0.7);glBegin(GL_QUADS); glVertex2i(60, 655+30);glVertex2i(60, 655+30+20-1);glVertex2i(60+200-1, 655+30+20-1);glVertex2i(60+200-1, 655+30);glEnd();

    for(int i=0; i<10;i++)
    {
        for(int j=0;j<pMP;j++)
        {
            glColor3f(0,0,1);glPointSize(2);glBegin(GL_POINTS);glVertex2i(60+2*j, 655+30+2*i);glEnd();
        }
    }
    //score
    char score_text[]="default text";
    sprintf(score_text, "SCORE: %d", score);
    printText(0,0,0, 10, 670+30+30, score_text);

    //status image
    if(70<=pHP){drawSprite(384, W_HEIGHT-100+4, 3, 9); drawSprite(384+96, W_HEIGHT-100+4, 3, 10);}
    else if(pHP<70 && 30<=pHP){drawSprite(384, W_HEIGHT-100+4, 3, 11); drawSprite(384+96, W_HEIGHT-100+4, 3, 12);}
    else if(pHP<30 && 0<pHP){drawSprite(384, W_HEIGHT-100+4, 3, 13); drawSprite(384+96, W_HEIGHT-100+4, 3, 14);}
}

void drawPlayer()
{
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px,py);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px,py);
    glVertex2i(px+pdx*5, py+pdy*5);
    glEnd();
}


void drawObject(float mx, float my, int n) //오브젝트의 x좌표, y좌표, 텍스쳐 인덱스
{
    float disT=dist(px, py, mx, my);
    float pointsize=((mapS*640)/disT)/32;
    double ma=atan2((double)(my-py), (double)(mx-px));
    double theta=(my-py)/(mx-px);
    int x;

    if((my-py)>0 && (mx-px)>0){ma=atan(theta);}
    else if((my-py)>0 && (mx-px)<0){ma=PI+atan(theta);}
    else if((my-py)<0 && (mx-px)<0){ma=PI+atan(theta);}
    else if((my-py)<0 && (mx-px)>0){ma=2*PI+atan(theta);}

    //printf("%f\n", ma);

    int mix,miy, mp, dof; float rx, ry , xo,yo, disTR;
    double ra=ma; if(ra<0){ra+=2*PI;} if(ra>2*PI){ra-=2*PI;}

    int vmt=0;
    int hmt=0;//vertical and horizontal map texture number, ooo map texture
    //----check horizontal lines
    dof=0;
    float disH=1000000,hx=px, hy=py;
    float aTan=-1/tan(ra);
    if(ra>PI){ry=(((int)py>>6)<<6)-0.001; rx=(py-ry)*aTan+px; yo=-64; xo=-yo*aTan;} //looking down
    if(ra<PI){ry=(((int)py>>6)<<6)+64; rx=(py-ry)*aTan+px; yo=64; xo=-yo*aTan;} //looking up
    if(ra==0 || ra==PI){rx=px;ry=py;dof=8;}
    while(dof<8)
    {
        mix=(int)(rx)>>6; miy=(int)(ry)>>6; mp=miy*mapX+mix;
        if(mp>0 && mp<mapX*mapY && mapW[mp]>0){hmt=mapW[mp]-1;hx=rx; hy=ry; disH=dist(px,py,hx,hy); dof=8;}//hit wall
            else{rx+=xo;ry+=yo;dof+=1;} //next line
    }

    //check vertical lines
    dof=0;
    float disV=1000000,vx=px, vy=py;
    float nTan=-tan(ra);

    if(ra>P2 && ra<P3){rx=(((int)px>>6)<<6)-0.001; ry=(px-rx)*nTan+py; xo=-64; yo=-xo*nTan;} //looking left
    if(ra<P2 || ra>P3){rx=(((int)px>>6)<<6)+64; ry=(px-rx)*nTan+py; xo=64; yo=-xo*nTan;} //looking right
    if(ra==0 || ra==PI){rx=px;ry=py;dof=8;} //looking straight up or down
    while(dof<8)
    {
        mix=(int)(rx)>>6; miy=(int)(ry)>>6; mp=miy*mapX+mix;
        if(mp>0 &&mp<mapX*mapY && mapW[mp]>0){vmt=mapW[mp]-1;vx=rx; vy=ry; disV=dist(px,py,vx,vy);dof=8;}//hit wall

        else{rx+=xo;ry+=yo;dof+=1;} //next line
    }
    if(disV<disH){hmt=vmt;rx=vx; ry=vy;disTR=disV;}
    if(disH<disV){rx=hx; ry=hy;disTR=disH;}


    if(((pa-ma<0)&&(pa-ma>-PI/6)) || ((pa-ma>0)&&(pa-ma<PI/6)))
    {
        x=480-pointsize*15-(pa-ma)*960; //조정하기 어렵다
        if(disTR>disT){drawSprite(x,290-pointsize,pointsize, n);}
    }
}


void drawObjects()
{
    int objn=mobn+itemn+max_bullets+max_goose_bullets+max_ponix_bullets+max_hubo_beepers;
    float arr[mobn+itemn+max_bullets+max_goose_bullets+max_ponix_bullets+max_hubo_beepers]={0};
    for(int i=0; i<mobn;i++)
    {
        arr[i]=dist(px, py, mobx[i], moby[i]);
    }
    for(int i=0; i<itemn; i++)
    {
        arr[i+mobn]=dist(px, py, itemx[i], itemy[i]);
    }
    for(int i=0; i<max_bullets; i++)
    {
        arr[i+mobn+itemn]=dist(px, py, bulletX[i], bulletY[i]);
    }
    for(int i=0; i<max_goose_bullets; i++)
    {
        arr[i+mobn+itemn+max_bullets]=dist(px, py, goose_bulletX[i], goose_bulletY[i]);
    }
    for(int i=0; i<max_ponix_bullets; i++)
    {
        arr[i+mobn+itemn+max_bullets+max_goose_bullets]=dist(px, py, ponix_bulletX[i], ponix_bulletY[i]);
    }
    for(int i=0; i<max_hubo_beepers;i++)
    {
        arr[i+mobn+itemn+max_bullets+max_goose_bullets+max_ponix_bullets]=dist(px, py, hubo_beeperX[i], hubo_beeperY[i]);
    }

    qsort(arr, objn, sizeof(float), floatcomp);


    for(int i=0; i<objn;i++)
    {
        //ITEMS
        for(int j=0;j<itemn; j++)
        {
            if(arr[i]==dist(px, py, itemx[j], itemy[j]) && itemHP[j]>0)
            {
                drawObject(itemx[j],itemy[j],itemTexture[j]); break;
            }
        }
        //MONSTERS
        for(int j=0;j<mobn; j++)
        {
            if(arr[i]==dist(px, py, mobx[j], moby[j]) && mobHP[j]>0)
            {
                int fire=mobTexture[j];

                struct timeval time_now{};
                gettimeofday(&time_now, nullptr);

                unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);

                if(current_time-mob_last_time[j] < 300)
                {
                    fire=mobTexture[j]+1;
                }
                drawObject(mobx[j],moby[j],fire); break;
            }
        }
        //BULLETS

        for(int j=0;j<max_bullets;j++)
        {
            if(arr[i]==dist(px, py, bulletX[j], bulletY[j])&&bulletC[j]==1)
            {
                drawObject(bulletX[j], bulletY[j], 29); break;
            }
        }

        //GOOSE BULLETS
        for(int j=0;j<max_goose_bullets;j++)
        {
            if(arr[i]==dist(px, py, goose_bulletX[j], goose_bulletY[j])&& goose_bulletC[j]==1)
            {
                drawObject(goose_bulletX[j], goose_bulletY[j], 30); break;
            }
        }

        //PONIX BULLETS
        for(int j=0;j<max_ponix_bullets;j++)
        {
            if(arr[i]==dist(px, py, ponix_bulletX[j], ponix_bulletY[j])&& ponix_bulletC[j]==1)
            {
                drawObject(ponix_bulletX[j], ponix_bulletY[j], 31); break;
            }
        }

        //HUBO BEEPERS
        for(int j=0;j<max_hubo_beepers;j++)
        {
            if(arr[i]==dist(px, py, hubo_beeperX[j], hubo_beeperY[j])&& hubo_beeperC[j]==1)
            {
                drawObject(hubo_beeperX[j], hubo_beeperY[j], 27); break;
            }
        }

    }
}


void ItemEffect()
{
    for(int i=0;i<itemn;i++)
    {
        if(dist(px, py, itemx[i], itemy[i])<=32 && itemHP[i]>0 && 0<pHP && pHP<=100)
        {
            if(itemTexture[i]==25){ pMP+=20;Mix_PlayChannel(-1, heal_item_sound,0);} //should i have to forbid eating this when pHP is full?
            else if(itemTexture[i]==26){pHP+=20;Mix_PlayChannel(-1, heal_item_sound,0);}
            else if(itemTexture[i]==27 || itemTexture[i]==28){ pHP-=10; Mix_PlayChannel(-1, damage_item_sound,0);}

            if(pHP>100){pHP=100;}
            if(pMP>100){pMP=100;}
            itemHP[i]--;
        }
    }
}

void beeperEffect()
{
    for(int i=0;i<max_hubo_beepers;i++)
    {
        if(dist(px, py, hubo_beeperX[i], hubo_beeperY[i])<=32 && hubo_beeperC[i]>0 && 0<pHP && pHP<=100)
        {
            pHP-=5; Mix_PlayChannel(-1, damage_item_sound,0);
            hubo_beeperC[i]=0;
        }
    }
}


void levelGenerator()
{
    //몬스터 체력, 몬스터 이동 속도, 몬스터 공격력은 스코어에 비례
    //몬스터 스폰 수도 스코어에 비례
    //초반 휴보, 중반 거위, 후반 포닉스
    struct timeval time_now{};
    gettimeofday(&time_now, nullptr);

    srand(time(NULL));

    unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);

    //MONSTER SPAWN
    if(current_time-last_monster_spawn_time>=3000)
    {
        last_monster_spawn_time=current_time;
        for(int i=0; i<1+score/5000 ;i++)
        {
            //srand(time(NULL));
            if(mobI%4==0){mobx[mobI]=70 ;moby[mobI]=320;}
            else if(mobI%4==1){mobx[mobI]=320 ;moby[mobI]=570;}
            else if(mobI%4==2){mobx[mobI]=570 ;moby[mobI]=320;}
            else if(mobI%4==3){mobx[mobI]=320 ;moby[mobI]=70;}

            mobTexture[mobI]=15 + 2*(rand()%3);
            mobHP[mobI]=2+score/5000;

            ++mobI;if(mobI>=mobn){mobI=0;}
        }
    }

    //ITEM SPAWN

    if(current_time-last_item_spawn_time>=5000)
    {
        //srand(time(NULL));
        last_item_spawn_time=current_time;
        for(int i=0; i<1+score/5000; i++)
        {
            itemx[itemI]=70+(rand()%501); itemy[itemI]=70+(rand()%501);
            itemTexture[itemI]=25+(rand()%2);
            itemHP[itemI]=1;
            ++itemI; if(itemI>=itemn){itemI=0;}
        }
    }

}

void win()
{
    struct timeval time_now{};
    gettimeofday(&time_now, nullptr);

    unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);
    if(win_time==0)
    {
        win_time=current_time;
        Mix_PauseMusic();
        Mix_PlayMusic(win_sound, 1);
    }
    //printf("%d", current_time- player_death_time)
    if(current_time-win_time<=12000)
    {
        glColor3f(0, 0, 0); glBegin(GL_QUADS);glVertex2i(0, 0);glVertex2i(0,W_HEIGHT);glVertex2i(W_WIDTH, W_HEIGHT);glVertex2i(W_WIDTH, 0);glEnd();
        char message[]="YOU WIN!";
        printText(0,0,1,400, 50, message);

        char score_text[]="default";
        sprintf(score_text,"SCORE: %d", score);
        printText(0,0,1,400, 50+50, score_text);

        int x=160;
        int y=150;
        int a=160;
        int b=150;
        int s=5;
        for(int i=0;i<80;i++)
        {
            for(int j=0;j<128;j++)
            {
                int color_index=j+i*128;

                glColor3f((float)win_red[color_index]/255,(float)win_green[color_index]/255,(float)win_blue[color_index]/255);

                x=a+j*s;
                y=b+i*s;
                //printf("%d, %d, %d",gameover_red[color_index],gameover_green[color_index],gameover_blue[color_index]);
                glPointSize(s);glBegin(GL_POINTS);glVertex2i(x,y);glEnd();
            }
        }

    }
    else{Mix_CloseAudio();exit(0);}
}


void gameover()
{
    struct timeval time_now{};
    gettimeofday(&time_now, nullptr);

    unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);
    if(player_death_time==0)
    {
        player_death_time=current_time;
        Mix_PauseMusic();
        Mix_PlayMusic(game_over_sound, 1);
    }
    //printf("%d", current_time- player_death_time)
    if(current_time-player_death_time<=6000)
    {
        glColor3f(0, 0, 0); glBegin(GL_QUADS);glVertex2i(0, 0);glVertex2i(0,W_HEIGHT);glVertex2i(W_WIDTH, W_HEIGHT);glVertex2i(W_WIDTH, 0);glEnd();
        char message[]="GAME OVER";
        printText(1,0,0,400, 50, message);

        char score_text[]="default";
        sprintf(score_text,"SCORE: %d", score);
        printText(1,0,0,400, 50+50, score_text);

        int x=160;
        int y=150;
        int a=160;
        int b=150;
        int s=5;
        for(int i=0;i<96;i++)
        {
            for(int j=0;j<128;j++)
            {
                int color_index=j+i*128;

                glColor3f((float)gameover_red[color_index]/255,(float)gameover_green[color_index]/255,(float)gameover_blue[color_index]/255);

                x=a+j*s;
                y=b+i*s;
                //printf("%d, %d, %d",gameover_red[color_index],gameover_green[color_index],gameover_blue[color_index]);
                glPointSize(s);glBegin(GL_POINTS);glVertex2i(x,y);glEnd();
            }
        }

    }
    else{Mix_CloseAudio();SDL_Quit();exit(0);}
}


void drawRays2D()
{
    int r,mx,my, mp, dof; float rx, ry ,ra, xo,yo, disT;
    ra=pa-DR*30; if(ra<0){ra+=2*PI;} if(ra>2*PI){ra-=2*PI;}
    for(r=0;r<120;r++)
    {
        int vmt=0;
        int hmt=0;//vertical and horizontal map texture number, ooo map texture
        //----check horizontal lines
        dof=0;
        float disH=1000000,hx=px, hy=py;
        float aTan=-1/tan(ra);
        if(ra>PI){ry=(((int)py>>6)<<6)-0.001; rx=(py-ry)*aTan+px; yo=-64; xo=-yo*aTan;} //looking down
        if(ra<PI){ry=(((int)py>>6)<<6)+64; rx=(py-ry)*aTan+px; yo=64; xo=-yo*aTan;} //looking up
        if(ra==0 || ra==PI){rx=px;ry=py;dof=8;}
        while(dof<8)
        {
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && mapW[mp]>0){hmt=mapW[mp]-1;hx=rx; hy=ry; disH=dist(px,py,hx,hy); dof=8;}//hit wall
            else{rx+=xo;ry+=yo;dof+=1;} //next line
        }

        //check vertical lines
        dof=0;
        float disV=1000000,vx=px, vy=py;
        float nTan=-tan(ra);
        float shade=1;
        if(ra>P2 && ra<P3){rx=(((int)px>>6)<<6)-0.001; ry=(px-rx)*nTan+py; xo=-64; yo=-xo*nTan;} //looking left
        if(ra<P2 || ra>P3){rx=(((int)px>>6)<<6)+64; ry=(px-rx)*nTan+py; xo=64; yo=-xo*nTan;} //looking right
        if(ra==0 || ra==PI){rx=px;ry=py;dof=8;} //looking straight up or down
        while(dof<8)
        {
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
            if(mp>0 &&mp<mapX*mapY && mapW[mp]>0){vmt=mapW[mp]-1;vx=rx; vy=ry; disV=dist(px,py,vx,vy);dof=8;}//hit wall

            else{rx+=xo;ry+=yo;dof+=1;} //next line
        }
        if(disV<disH){hmt=vmt;shade=0.5;rx=vx; ry=vy;disT=disV;glColor3f(0.9,0,0);}
        if(disH<disV){rx=hx; ry=hy;disT=disH;glColor3f(0.7,0,0);}

        //Draw 3D walls
        float ca=pa-ra; if(ca<0) {ca+=2*PI;} if(ca>2*PI){ca-=2*PI;} disT=disT*cos(ca); //fix fisheye
        float lineH=(mapS*640)/disT; //왜인지는 모르겠으나 원래는 (mapS*640)/disT 였음
        float ty_step=32.0/(float)lineH;
        float ty_off=0;
        if(lineH>640){ty_off=(lineH-640)/2.0; lineH=640;}
        float lineO=320-lineH/2;

        float ty=ty_off*ty_step+hmt*32;
        float tx;
        if(shade==1){tx=(int)(rx/2.0)%32; if(PI>ra && ra>0){tx=31-tx;}}
        else        {tx=(int)(ry/2.0)%32; if(P3>ra && ra>P2){tx=31-tx;}}

        for(int y=0;y<lineH;y++)
        {
            //float c=All_Textures[(int) (ty)*32+(int)(tx)]*shade;
            int color_index=(int) (ty)*32+(int)(tx);
            glColor3f((float)red[color_index]/255*shade,(float)green[color_index]/255*shade,(float)blue[color_index]/255*shade);
            glPointSize(8);glBegin(GL_POINTS);glVertex2i(r*8, y+lineO);glEnd();
            ty+=ty_step;
        }
        //glLineWidth(1);glBegin(GL_LINES);glVertex2i(r*1+530,lineO);glVertex2i(r*1+530,lineH+lineO);glEnd();



        //draw floors
        for(int y=lineO+lineH;y<640;y++)
        {
            float c=0.7;glColor3f(c,c,c);
            glPointSize(8);glBegin(GL_POINTS);glVertex2i(r*8, y);glEnd();
        }

        ra+=DR/2; if(ra<0){ra+=2*PI;} if(ra>2*PI){ra-=2*PI;}

    }
}


void ButtonDown(unsigned char key, int x, int y)
{
    if(key=='a'){Keys.a=1;}
    if(key=='d'){Keys.d=1;}
    if(key=='w'){Keys.w=1;}
    if(key=='s'){Keys.s=1;}
    if(key=='e')
    {
        int xo=0; if(pdx<0){xo=-25;}else{xo=25;}
        int yo=0; if(pdy<0){yo=-25;}else{yo=25;}
        int ipx=px/64.0;
        int ipx_add_xo=(px+xo)/64.0;
        int ipy=py/64.0;
        int ipy_add_yo=(py+yo)/64.0;
        if(mapW[ipy_add_yo*mapX+ipx_add_xo]==4){mapW[ipy_add_yo*mapX+ipx_add_xo]=0;Mix_PlayChannel(-1, door_open_sound,0);}
    }
    if(key==' '){Keys.space=1;}

    glutPostRedisplay(); //what is this?
}

void ButtonUp(unsigned char key, int x, int y)
{
    if(key=='a'){Keys.a=0;}
    if(key=='d'){Keys.d=0;}
    if(key=='w'){Keys.w=0;}
    if(key=='s'){Keys.s=0;}
    if(key==' '){Keys.space=0;}
    glutPostRedisplay();
}


void display()
{
    //frames per second
    frame2=glutGet(GLUT_ELAPSED_TIME); fps=(frame2-frame1); frame1=glutGet(GLUT_ELAPSED_TIME);
    //buttons
    pdx=cos(pa)*5;pdy=sin(pa)*5;
    if(Keys.a==1){pa-=0.003*fps;if(pa<0){pa+=2*PI;}}
    if(Keys.d==1){pa+=0.003*fps;if(pa>2*PI){pa-=2*PI;}}

    int xo=0; if(pdx<0){xo=-20;} else{xo=20;}
    int yo=0; if(pdy<0){yo=-20;} else{yo=20;}
    int ipx=px/64.0, ipx_add_xo=(px+xo)/64.0, ipx_sub_xo=(px-xo)/64.0;
    int ipy=py/64.0, ipy_add_yo=(py+yo)/64.0, ipy_sub_yo=(py-yo)/64.0;

    if(Keys.w==1)
    {
        if(mapW[ipy*mapX+ipx_add_xo]==0){px+=pdx;}
        if(mapW[ipy_add_yo*mapX+ipx]==0){py+=pdy;}
    }
    if(Keys.s==1)
    {
        if(mapW[ipy*mapX+ipx_sub_xo]==0){px-=pdx;}
        if(mapW[ipy_sub_yo*mapX+ipx]==0){py-=pdy;}
    }
    glutPostRedisplay();//i dont know what this means.


    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    drawRays2D();
    drawObjects();
    drawGUI();
    playerAttack();
    levelGenerator();
    monsterAI();
    monsterAttack();
    ItemEffect();
    beeperEffect();

    //game over screen
    if(pHP<=0)
    {
        gameover();
    }

    if(score>=WIN_SCORE)
    {
        win();
    }
    glutSwapBuffers();
}

void init()
{
    loadTexture();
    loadWinscreen();
    loadGameoverscreen();
    glClearColor(0.3,0.3,0.3,0);
    gluOrtho2D(0,W_WIDTH,W_HEIGHT,0);
    px=320; py=320; pdx=cos(pa)*5;pdy=sin(pa)*5;//init of player position
}

void resize(int w, int h)
{
    glutReshapeWindow(W_WIDTH, W_HEIGHT);
}


int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    main_music = Mix_LoadMUS("sound\\Melissa.wav");
    player_attck_sound = Mix_LoadWAV("sound\\playerattack.wav");
    footstep_forward_sound=Mix_LoadWAV("sound\\footstep_forward.ogg");
    footstep_backward_sound=Mix_LoadWAV("sound\\footstep_backward.ogg");
    hubo_attack_sound=Mix_LoadWAV("sound\\hubo_attack.wav");
    goose_attack_sound=Mix_LoadWAV("sound\\goose_attack.mp3");
    ponix_attack_sound=Mix_LoadWAV("sound\\ponix_attack.wav");
    door_open_sound=Mix_LoadWAV("sound\\door.ogg");
    portal_open_sound=Mix_LoadWAV("sound\\portal_open.ogg");
    warp_sound=Mix_LoadWAV("sound\\warp.ogg");
    heal_item_sound=Mix_LoadWAV("sound\\heal_item.wav");
    damage_item_sound=Mix_LoadWAV("sound\\damage_item.wav");
    game_over_sound=Mix_LoadMUS("sound\\gameover.mp3");
    win_sound=Mix_LoadMUS("sound\\win.mp3");



    Mix_PlayMusic(main_music, -1);


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(W_WIDTH, W_HEIGHT);
    glutInitWindowPosition(20,20);
    glutCreateWindow("KOOM");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(ButtonDown);
    glutKeyboardUpFunc(ButtonUp);
    glutMainLoop();

    //Mix_FreeMusic();
    //Mix_FreeChunk();
    //Mix_CloseAudio();
    //SDL_Quit();
}
