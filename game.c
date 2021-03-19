#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <termio.h>
#define false 0
#define true 1
#define Q_KEY 113
#define A_KEY 97
#define S_KEY 115
#define D_KEY 100

const static int height = 10;
const static int width = 40;
static int ch = 0;
static int score = 0;
static int p = 20;
static int life = 9;
static int islife = false;
static int x = 0;
static float y = 0;

//init the game windows
void printinit(){
	system("clear");
	printf("score: %d\n",score);
	printf("there are %d lives now!\n",life);
	for(int i = 0; i < width; i++){
		printf("*");
	}
	printf("\n");
	for(int i = 0; i < height; i++){
		printf("\n");
	}
	for(int i = 0; i < width; i++){
		printf("*");
	}
	printf("\n");
	printf("操作方法：\na键左移，d键右移，s键宝物瞬间落下，q键退出");
	printf("\033[3A\033[?25l"); //up 3 lines and hide the cursor
}

void lose(int signum)
{
	if(islife==false){
		life--;
	}
	printinit();
}

void get(int signum)
{
	if(islife==true){
		life++;
	}
    else{
		score++;
	}
	printinit();
}

void down(int signum){
	y = height - 1;
}

//get the input from keyboard
static int get_char()
{
    fd_set rfds;
    struct timeval tv;
    int ch = 0;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 10; //set waiting time

    //check if there is a input from keyboard
    if (select(1, &rfds, NULL, NULL, &tv) > 0)
    {
        ch = getchar(); 
    }

    return ch;
}

int main()
{
	srand((unsigned int)time(NULL));
	x = rand()%width;
	y = 0;
	system("stty -icanon");
	system("clear");
    fflush(stdout);
    //Handle SIGINT
    signal(SIGUSR1, lose);
	signal(SIGUSR2, get);
	signal(SIGINT, down);
    printinit();
	//loop forever!
	while (life > 0){
		srand((unsigned int)time(NULL));
		if(y>=0 && y < height){ //down speed
			y+=0.005 * (1+score/5);
		}
		else if(y>=height){ //prop return to top 
			y = -1;
			raise(SIGUSR1);
		}else if(y==-1){ //set the params of prop down
			y = 0;
			x = rand()%width;
			int life_p = rand()%100;
			if(life_p <= 90){
				islife = false;
			}else{
				islife = true;
			}
		}
		if((int)y == height-1 && p == x){ //check if get the prop
			y = -1;
			raise(SIGUSR2);
		}
		printf("\033[10A"); //cursor return to top
		ch = get_char();
		fflush(stdin);
		//check the input from keyboard
		if(ch == Q_KEY){
			system("clear");
			raise(SIGQUIT);
		}else if(ch == A_KEY){
			if(p > 0){
				p--;
			}
		}else if(ch == D_KEY){
			if(p < width-1){
				p++;
			}
		}else if(ch == S_KEY){
			raise(SIGINT);
		}
		//print the main game interface
		for(int j = 0; j < height; j++){
			for(int i = 0; i < width; i++){
				if(j == height-1 && i == p){
					printf("$");
				}else if(j == (int)y && i == x){
					if(islife==false){
						printf("|");
					}
					else
					{
						printf("$");
					}
					
				}else{
					printf(" ");
				}
			}
			printf("\n");
		}
		usleep(1000);
	}
    printf("\033[3BGame Over!\n");
    return 0;
}
