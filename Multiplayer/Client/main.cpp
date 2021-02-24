//COMMAND SET
//"PLAYERID" == 'P'
//"TOTAlPLAYER" == 'T'
//"DATA" == 'D'
//"ATTACK" == 'A'
//"DOOR" == 'R'
//"MAP" == 'M'
//"HP" == 'H'


#define _CRT_SECURE_NO_WARNINGS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include<GL/glut.h>
#include<GL/glu.h>
#include<Gl/gl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/time.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>

#define W_WIDTH 960
#define W_HEIGHT 640+100
#define TEXTURE_SIZE 32*32*28
#define mapX 10
#define mapY 10
#define mapS 100
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //one degree in radians
#define PIXEL_SIZE   3    // �ȼ� �� ���� ũ�� 3����Ʈ(24��Ʈ)
#define PIXEL_ALIGN  4    // �ȼ� ������ ���� �� ���� 4�� ��� ũ��� �����
#define max_client 30
#pragma comment(lib, "Ws2_32.lib")

#pragma pack(push, 1)                // ����ü�� 1����Ʈ ũ��� ����

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
Mix_Chunk *game_over_sound;

typedef struct _BITMAPFILEHEADER     // BMP ��Ʈ�� ���� ��� ����ü
{
    unsigned short bfType;           // BMP ���� ���� �ѹ�
    unsigned int   bfSize;           // ���� ũ��
    unsigned short bfReserved1;      // ����
    unsigned short bfReserved2;      // ����
    unsigned int   bfOffBits;        // ��Ʈ�� �������� ���� ��ġ
} BMFH;

typedef struct _BITMAPINFOHEADER     // BMP ��Ʈ�� ���� ��� ����ü(DIB ���)
{
    unsigned int   biSize;           // ���� ����ü�� ũ��
    int            biWidth;          // ��Ʈ�� �̹����� ���� ũ��
    int            biHeight;         // ��Ʈ�� �̹����� ���� ũ��
    unsigned short biPlanes;         // ����ϴ� �������� ��
    unsigned short biBitCount;       // �ȼ� �ϳ��� ǥ���ϴ� ��Ʈ ��
    unsigned int   biCompression;    // ���� ���
    unsigned int   biSizeImage;      // ��Ʈ�� �̹����� �ȼ� ������ ũ��
    int            biXPelsPerMeter;  // �׸��� ���� �ػ�(���ʹ� �ȼ�)
    int            biYPelsPerMeter;  // �׸��� ���� �ػ�(���ʹ� �ȼ�)
    unsigned int   biClrUsed;        // ���� ���̺��� ���� ���Ǵ� ���� ��
    unsigned int   biClrImportant;   // ��Ʈ���� ǥ���ϱ� ���� �ʿ��� ���� �ε��� ��
} BMIH;

typedef struct _RGBTRIPLE            // 24��Ʈ ��Ʈ�� �̹����� �ȼ� ����ü
{
    unsigned char rgbtBlue;          // �Ķ�
    unsigned char rgbtGreen;         // �ʷ�
    unsigned char rgbtRed;           // ����
} RGBTP;

#pragma pack(pop)



typedef struct
{
    int w,a,s,d, space;
}ButtonKeys;
ButtonKeys Keys;

int playerID;
int totalplayer;

//int eneHP[max_client]={0};
float eneX[max_client]={0};
float eneY[max_client]={0};

WSADATA wsa;
SOCKET s;
struct sockaddr_in server;
char message[100];
char server_reply[200];
int recv_size;

int pHP=100; //player Health Point


float px, py, pdx, pdy, pa=P3; //player position
float frame1, frame2, fps;
float px_arr[]={300,300};
float py_arr[]={300,300};
float pa_arr[]={P3, P3};



unsigned long long last_time=0; //�÷��̾ ���������� ������ �ð�
unsigned long long last_send_time=0;
long long player_death_time=-1; //�׾��� �� �ð��� ����

int red[TEXTURE_SIZE]={0};
int green[TEXTURE_SIZE]={0};
int blue[TEXTURE_SIZE]={0};



int decon=4;
float decox[]={300,350,400,450};
float decoy[]={100,100,100,100};
int decoMI[]={0,0,0};
int decoTexture[]={21,22,23,24};



int mapW[mapS]=
{
1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,
};

int All_maps[]=
{
3,3,1,1,1,1,1,1,1,1,
3,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,1,1,1,4,1,1,0,0,1,
1,0,0,0,0,0,4,0,0,1,
1,1,1,1,1,1,1,1,1,1,

2,2,2,2,2,2,2,2,2,2,
2,0,0,0,0,0,0,4,0,2,
2,0,0,0,0,0,0,2,0,2,
2,0,0,0,0,0,0,2,0,2,
2,0,0,0,0,0,0,2,0,2,
2,0,0,0,0,0,0,2,0,2,
2,0,0,0,0,0,0,2,0,2,
2,0,0,0,0,0,0,2,0,2,
2,0,0,0,0,0,0,2,0,2,
2,2,2,2,2,2,2,2,2,2,
};

float portalX[]={-1,0};
float portalY[]={-1,0};
int map_index=0; //���� ���� ���° ���� ǥ��





//------------------------

float dist(float ax, float ay, float bx, float by)
{
    return( sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay)) );
}

int floatcomp(const void* elem1, const void* elem2) //�������� ����
{
    if(*(const float*)elem1 > *(const float*)elem2)
        return -1;
    return *(const float*)elem1 < *(const float*)elem2;
}

int floatcomp_rev(const void* elem1, const void* elem2) //�������� ����
{
    if(*(const float*)elem1 < *(const float*)elem2)
        return -1;
    return *(const float*)elem1 > *(const float*)elem2;
}



int fileLoad(void){

    FILE *fpBmp;                    // ��Ʈ�� ���� ������
    BMFH fileHeader;    // ��Ʈ�� ���� ��� ����ü ����
    BMIH infoHeader;    // ��Ʈ�� ���� ��� ����ü ����

    unsigned char *image;    // �ȼ� ������ ������
    int size;                // �ȼ� ������ ũ��
    int width, height;       // ��Ʈ�� �̹����� ����, ���� ũ��
    int padding;             // �ȼ� �������� ���� ũ�Ⱑ 4�� ����� �ƴ� �� ���� ������ ũ��


    fpBmp = fopen("texture.bmp", "rb");    // ��Ʈ�� ������ ���̳ʸ� ���� ����
    if (fpBmp == NULL)    // ���� ���⿡ �����ϸ�
        return -1;         // ���α׷� ����

    // ��Ʈ�� ���� ��� �б�. �б⿡ �����ϸ� ���� �����͸� �ݰ� ���α׷� ����
    if (fread(&fileHeader, sizeof(BMFH), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return -1;
    }

    // ���� �ѹ��� MB�� �´��� Ȯ��(2����Ʈ ũ���� BM�� ��Ʋ ��������� �о����Ƿ� MB�� ��)
    // ���� �ѹ��� ���� ������ ���α׷� ����
    if (fileHeader.bfType != 'MB')
    {
        fclose(fpBmp);
        return -1;
    }

    // ��Ʈ�� ���� ��� �б�. �б⿡ �����ϸ� ���� �����͸� �ݰ� ���α׷� ����
    if (fread(&infoHeader, sizeof(BMIH), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return -1;
    }

    // 24��Ʈ ��Ʈ���� �ƴϸ� ���α׷� ����
    if (infoHeader.biBitCount != 24)
    {
        fclose(fpBmp);
        return -1;
    }

    size = infoHeader.biSizeImage;    // �ȼ� ������ ũ��
    width = infoHeader.biWidth;       // ��Ʈ�� �̹����� ���� ũ��
    height = infoHeader.biHeight;     // ��Ʈ�� �̹����� ���� ũ��

    // �̹����� ���� ũ�⿡ �ȼ� ũ�⸦ ���Ͽ� ���� �� ���� ũ�⸦ ���ϰ� 4�� �������� ����
    // �׸��� 4���� �������� ���ָ� ���� ������ ���� �� ����.
    // ���� ���� ������ 0�̶�� ���� ����� 4�� �ǹǷ� ���⼭ �ٽ� 4�� �������� ����
    padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

    if (size == 0)    // �ȼ� ������ ũ�Ⱑ 0�̶��
    {
        // �̹����� ���� ũ�� * �ȼ� ũ�⿡ ���� ������ �����ָ� ������ ���� �� �� ũ�Ⱑ ����
        // ���⿡ �̹����� ���� ũ�⸦ �����ָ� �ȼ� �������� ũ�⸦ ���� �� ����
        size = (width * PIXEL_SIZE + padding) * height;
    }

    image = (unsigned char *)malloc(size);    // �ȼ� �������� ũ�⸸ŭ ���� �޸� �Ҵ�

    // ���� �����͸� �ȼ� �������� ���� ��ġ�� �̵�
    fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

    // ���Ͽ��� �ȼ� ������ ũ�⸸ŭ ����. �б⿡ �����ϸ� ���� �����͸� �ݰ� ���α׷� ����
    if (fread(image, size, 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }

    fclose(fpBmp);    // ��Ʈ�� ���� �ݱ�


    // �ȼ� �����ʹ� �Ʒ� ���� �������� ����ǹǷ� �Ʒ��ʺ��� �ݺ�
    // ���� ũ�⸸ŭ �ݺ�
    for (int y = height - 1; y >= 0; y--)
    {
        // ���� ũ�⸸ŭ �ݺ�
        for (int x = 0; x < width; x++)
        {
            // �Ϸķ� �� �迭�� �����ϱ� ���� �ε����� ���
            // (x * �ȼ� ũ��)�� �ȼ��� ���� ��ġ
            // (y * (���� ũ�� * �ȼ� ũ��))�� �ȼ��� �� ��° ������ ���
            // ���� ���� * y�� �ٺ��� ������ ���� ����
            int index = (x * PIXEL_SIZE) + (y * (width * PIXEL_SIZE)) + (padding * y);

            // ���� �ȼ��� �ּҸ� RGBTRIPLE �����ͷ� ��ȯ�Ͽ� RGBTRIPLE �����Ϳ� ����
            RGBTP *pixel = (RGBTP *)&image[index];

            // RGBTRIPLE ����ü�� �Ķ�, �ʷ�, �������� ������

            red[(height-1-y)*width+x] = (int)(pixel->rgbtRed);
            green[(height-1-y)*width+x] = (int)(pixel->rgbtGreen);
            blue[(height-1-y)*width+x] = (int)(pixel->rgbtBlue);
        }
    }

    free(image);
    return 0;

}



void networking()
{
    //networking
    recv_size=recv(s, server_reply, 200, 0);
    server_reply[recv_size]='\0';
    //printf("server_reply: %s\n", server_reply);

    char *ptr =strtok(server_reply, " ");

    //TOTALPLAYER num /HP num /DATA id x y id x y id x y ...
    //recv TOTALPLAYER
    if(ptr[0]=='T')
    {
        ptr=strtok(NULL, " ");
        totalplayer=atoi(ptr);

        ptr=strtok(NULL, " ");
        if(ptr[0]=='H')
        {
            ptr=strtok(NULL, " ");
            pHP=atoi(ptr);
        }
        ptr=strtok(NULL, " ");
        if(ptr[0]=='D')
        {
            for(int i=0; i<totalplayer-1; i++)
            {
                ptr=strtok(NULL, " ");
                int id=atoi(ptr);

                ptr=strtok(NULL, " ");
                eneX[id]=atof(ptr);

                ptr=strtok(NULL, " ");
                eneY[id]=atof(ptr);
                //printf("ene: %d, %f, %f \n", id, eneX[id], eneY[id]);
            }
        }

    }
/*
    else if(ptr[0]=='K')
    {
        ptr=strtok(NULL, " ");
        int id=atoi(ptr);
        eneX[id]=-10;
        eneY[id]=-10;
    }
*/
    //recv HP hp
    /*
    else if(ptr[0]=='H')
    {
        ptr=strtok(NULL, " ");
        printf("HP:%d", atoi(ptr));
        pHP=atoi(ptr);
    }

    //recv DATA id x y id x y
    else if(ptr[0]=='D')
    {
        for(int i=0; i<totalplayer-1; i++)
        {
        ptr=strtok(NULL, " ");
        int id=atoi(ptr);

        ptr=strtok(NULL, " ");
        eneX[id]=atof(ptr);

        ptr=strtok(NULL, " ");
        eneY[id]=atof(ptr);
        //printf("ene: %d, %f, %f \n", id, eneX[id], eneY[id]);
        }

    }
    */
    //recv KILL id



    //--------------------
    //send DATA id x y a

    sprintf(message, "D %d %f %f %f \n", playerID, px, py, pa);
    send(s, message, strlen(message), 0);

    /*

    for(int i=0;i<max_client;i++)
    {
        if(eneX[i]==0.0)continue;
        printf("totalplayer:%d \n pHP:%d \n eneX:%f \n eneY:%f \n\n\n", totalplayer, pHP,eneX[i], eneY[i]);
    }
    */
}


void drawSprite(int a, int b, int s, int n) //x��ǥ, y��ǥ, ũ��, �ؽ��� �ε���
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




void playerAttack() //������ x��ǥ, y��ǥ, �ؽ��� �ε���
{
    Mix_PlayChannel(-1, player_attck_sound,0);
    sprintf(message, "A %d \n", playerID);
    send(s, message, strlen(message), 0);
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

    //�⺻ �µθ�
    glColor3f(1, 1, 1); glBegin(GL_QUADS);glVertex2i(0, W_HEIGHT-100);glVertex2i(0,W_HEIGHT);glVertex2i(W_WIDTH, W_HEIGHT);glVertex2i(W_WIDTH, W_HEIGHT-100);glEnd();

    //hand
    int fire=7;

    struct timeval time_now{};
    gettimeofday(&time_now, nullptr);

    unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);
    if(Keys.space==1 && current_time-last_time >= 500)
    {
            fire=8;
            last_time=current_time;
            playerAttack();
    }
    else if(current_time-last_time < 300)
    {
        fire=8;
    }
    drawSprite(W_WIDTH/2-32*5/2, W_HEIGHT-100-32*5, 5,fire);


    //status texts
    char hp_text[]="HP";
    //char * call_text=hp_text;
    printText(1, 0, 0, 10, 670, hp_text);

    //hp bar
    glColor3f(0,0,0);glBegin(GL_QUADS); glVertex2i(60, 655);glVertex2i(60, 655+20-1);glVertex2i(60+200-1, 655+20-1);glVertex2i(60+200-1, 655);glEnd();

    for(int i=0; i<10;i++)
    {
        for(int j=0;j<pHP;j++)
        {
            glColor3f(1,0,0);glPointSize(2);glBegin(GL_POINTS);glVertex2i(60+2*j, 655+2*i);glEnd();
        }
    }

    //status image
    if(70<=pHP){drawSprite(W_WIDTH/3, W_HEIGHT-100+4, 3, 9); drawSprite(W_WIDTH/3+96, W_HEIGHT-100+4, 3, 10);}
    else if(pHP<70 && 30<=pHP){drawSprite(W_WIDTH/3, W_HEIGHT-100+4, 3, 11); drawSprite(W_WIDTH/3+96, W_HEIGHT-100+4, 3, 12);}
    else if(pHP<30 && 0<pHP){drawSprite(W_WIDTH/3, W_HEIGHT-100+4, 3, 13); drawSprite(W_WIDTH/3+96, W_HEIGHT-100+4, 3, 14);}
}


void drawObject(float mx, float my, int n) //������Ʈ�� x��ǥ, y��ǥ, �ؽ��� �ε���
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
        x=480-pointsize*13-(pa-ma)*960; //�����ϱ� ��ƴ�
        if(disTR>disT){drawSprite(x,290-pointsize,pointsize, n);}
    }
}


void drawObjects()
{
    int objn=max_client;
    float arr[max_client]={0};

    for(int i=0; i<max_client; i++)
    {
        arr[i]=dist(px, py, eneX[i], eneY[i]);
    }

    qsort(arr, objn, sizeof(float), floatcomp);

    //for(int i=0; i<objn;i++){printf("%f ", arr[i]); if(i==objn-1){printf("\n");}}

    for(int i=0; i<max_client;i++)
    {

        for(int j=0;j<max_client; j++)
        {
            if(arr[i]==dist(px, py, eneX[j], eneY[j]))
            {
                if(px==eneX[j] && py==eneY[j]) continue;
                else if(eneX[j]==0 && eneY[j]==0) continue;
                drawObject(eneX[j],eneY[j],19); break;
            }
        }

    }
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
        /*
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(px,py);
        glVertex2i(rx,ry);
        glEnd();
        */

        //Draw 3D walls
        float ca=pa-ra; if(ca<0) {ca+=2*PI;} if(ca>2*PI){ca-=2*PI;} disT=disT*cos(ca); //fix fisheye
        float lineH=(mapS*640)/disT; //�������� �𸣰����� ������ (mapS*640)/disT ����
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
            //please add texture with color
            int color_index=(int) (ty)*32+(int)(tx);
            glColor3f((float)red[color_index]/255*shade,(float)green[color_index]/255*shade,(float)blue[color_index]/255*shade);
            glPointSize(8);glBegin(GL_POINTS);glVertex2i(r*8, y+lineO);glEnd();
            ty+=ty_step;
        }
        //glLineWidth(1);glBegin(GL_LINES);glVertex2i(r*1+530,lineO);glVertex2i(r*1+530,lineH+lineO);glEnd();



        //draw floors
        for(int y=lineO+lineH;y<640;y++)
        {
/*
            float dy=y-(640/2.0);
            float raFix=pa-ra;
            if(raFix<0){raFix+=2*PI;} if(raFix>2*PI){raFix-=2*PI;}
            raFix=cos(raFix);
            tx=px/2 + cos(ra)*158*32/dy/raFix;
            ty=py/2 - sin(ra)*158*32/dy/raFix;
            int mp=mapF[(int)(ty/32.0)*mapX+(int)(tx/32.0)]*32*32;
            int color_index=((int)(ty)&31)*32 + ((int)(tx)&31)+mp;//someday i could fix this...maybe...
            glColor3f((float)red[color_index]/255*0.7,(float)green[color_index]/255*0.7,(float)blue[color_index]/255*0.7);

*/

            float c=0.7;glColor3f(c,c,c);
            glPointSize(8);glBegin(GL_POINTS);glVertex2i(r*8, y);glEnd();

            //draw ceiling
            /*
            mp=mapC[(int) (ty/32.0)*mapX + (int)(tx/32.0)*32*32];
            c=All_Textures[(int)(ty)&31]*32+((int)(tx)&31+mp)*0.7;
            */

        //glColor3f(0,0,0);glPointSize(1);glBegin(GL_POINTS);glVertex2i(r*1+530, 640-y);glEnd();
        //since i already render many pixels, ceiling makes program too slow and i think that this is unnessecary

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
    networking();
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
    //drawMap2D();
    //drawPlayer();
    drawRays2D();
    drawObjects();
    drawGUI();
    //monsterAttack();
    //portal();

    //game over screen
    if(pHP<=0)
    {
        Mix_PauseMusic();
        Mix_PlayChannel(-1, game_over_sound,0);

        struct timeval time_now{};
        gettimeofday(&time_now, nullptr);

        unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);
        if(player_death_time==-1){player_death_time=current_time;}

        if(current_time-player_death_time<=6000)
        {
            glColor3f(0, 0, 0); glBegin(GL_QUADS);glVertex2i(0, 0);glVertex2i(0,W_HEIGHT);glVertex2i(W_WIDTH, W_HEIGHT);glVertex2i(W_WIDTH, 0);glEnd();
            char message[]="GAME OVER";
            printText(1,0,0,400, 300, message);
        }
        else {Mix_CloseAudio();exit(0);}
    }



    glutSwapBuffers();
}

void init()
{
    fileLoad();
    glClearColor(0.3,0.3,0.3,0);
    gluOrtho2D(0,W_WIDTH,W_HEIGHT,0);
    px=300; py=300; pdx=cos(pa)*5;pdy=sin(pa)*5;//init of player position

    //-----------------network init
    printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		exit(1);
	}

	printf("Initialised.\n");

	//Create a socket
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Socket created.\n");

    server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		exit(1);
	}

	puts("Connected");

	//resv player number
	recv_size = recv(s, server_reply, 200, 0);
    server_reply[recv_size]='\0';
    char *ptr =strtok(server_reply, " ");
    if(ptr[0]=='P')
    {
        ptr=strtok(NULL, " ");
        playerID=atoi(ptr);
    }
    //-----------------

}

void resize(int w, int h)
{
    glutReshapeWindow(W_WIDTH, W_HEIGHT);
}


int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    main_music = Mix_LoadMUS("mainmusic.wav");
    player_attck_sound = Mix_LoadWAV("sound\\playerattack.wav");
    footstep_forward_sound=Mix_LoadWAV("sound\\footstep_forward.ogg");
    footstep_backward_sound=Mix_LoadWAV("sound\\footstep_backward.ogg");
    hubo_attack_sound=Mix_LoadWAV("sound\\hubo_attack.wav");
    goose_attack_sound=Mix_LoadWAV("sound\\goose_attack.mp3");
    ponix_attack_sound=Mix_LoadWAV("sound\\ponix_attack.wav");
    door_open_sound=Mix_LoadWAV("sound\\door.ogg");
    portal_open_sound=Mix_LoadWAV("sound\\portal_open.ogg");
    warp_sound=Mix_LoadWAV("sound\\warp.ogg");
    heal_item_sound=Mix_LoadWAV("sound\\heal_item.aif");
    damage_item_sound=Mix_LoadWAV("sound\\damage_item.wav");
    game_over_sound=Mix_LoadWAV("sound\\gameover.mp3");



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
    networking();

    //Mix_FreeMusic();
    //Mix_FreeChunk();
    //Mix_CloseAudio();
    //SDL_Quit();
}
