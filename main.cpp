//
//  main.cpp
//  Snake
//
//  Created by Lukas Sanner on 26.01.18.
//  Copyright © 2018 Lukas Sanner. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <unistd.h>

using namespace std;
#define DIM 14
#define MAXLENGTH 195
#define DELAY 	100000

unsigned char backGround   = '.';
unsigned char colorOfSnake = 'o';
unsigned char colorOfHead  = 'O';
unsigned char colorOfCherry = 'C';
unsigned char colorOfDeath = 'x';

void DrawBot(unsigned char (&px)[DIM][DIM])
{
	for(int i=0; i<DIM; i++)
	{
		for(int j=0; j<DIM; j++)
		{
			cout<<px[i][j]<<" ";
		}
		cout<<" "<<i<<endl;
	}
	for(int i=0; i<DIM; i++) cout<<i<<" ";
	cout<<endl;
	for(int i=0; i<20; i++) cout<<"-";
	cout<<endl<<endl;
}

void DrawCross(unsigned char (&px)[DIM][DIM])
{
	int halfDim = static_cast<int>( round( static_cast<float>(DIM)/2));
	for(int i = 0; i<DIM; i++)
	{
		for(int j = 0; j<DIM; j++)
		{
			if(i == halfDim || j == halfDim)
			{
				px[i][j] = colorOfDeath;
			}
			else px[i][j] = backGround;
		}
	}
}

void DrawX(unsigned char (&px)[DIM][DIM])
{
	for(int i = 0; i<DIM; i++)
	{
		for(int j = 0; j<DIM ; j++)
		{
			if(i==j)
			{
				px[i][j] = colorOfDeath;
			}
			if(j==DIM-i-1)
			{
				px[i][j] = colorOfDeath;
			}
			else px[i][j] = backGround;
		}
	}
}

class Cherry
{
public:
	Cherry(int x, int y): posX(x), posY(y){};
	int GetX(){return posX;};
	int GetY(){return posY;};
	void New();
	
private:
	int posX;
	int posY;
};

void Cherry::New()
{
	posX = rand()%DIM;
	posY = rand()%DIM;
}


class Snake
{
public:
	Snake(int x, int y, int l);
	void Add();
	void Move(Cherry cherry);
	void Kill();
	int GetLength(){return length;};
	int GetHeadX(){ return head[0];};
	int GetHeadY(){ return head[1];};
	void CheckBorder();
	void GetPosition(int ioPos[2][MAXLENGTH+1]);
	
private:
	int head[2];
	int oldHead[2];
	int length;
	int tail[2][MAXLENGTH];
	int lastMove;
	void MoveX(int d);
	void MoveY(int d);
	void MoveTail();
	int CheckEatTail();
	void AnimateDeath();

};

Snake::Snake(int x, int y, int l)
{
	lastMove = 1;
	head[0] = x;
	head[1] = y;
	length = l;
	
	for(int i=0; i<MAXLENGTH; i++)
	{
		if(i<l)
		{
			tail[0][i] = head[0]+i;
			tail[1][i] = head[1];
		}
		else
		{
			tail[0][i] = 0;
			tail[1][i] = 0;
		}
	}
}


void Snake::AnimateDeath()
{
	int nLoops = 4;
	unsigned char px[DIM][DIM];
	for(int i = 0; i< nLoops; i++)
	{
		DrawX(px);
		DrawBot(px);
		usleep(2*DELAY);
		DrawCross(px);
		DrawBot(px);
		usleep(2*DELAY);
	}
	

}


void Snake::GetPosition(int ioPos[2][MAXLENGTH+1])
{
	ioPos[0][0] = head[0];
	ioPos[1][0] = head[1];
	
	for(int i=0; i<length; i++)
	{
		ioPos[0][i+1] = tail[0][i];
		ioPos[1][i+1] = tail[1][i];
	}
};

void Snake::Kill()
{
	length = 0;
	head[0] = rand()%DIM;
	head[1] = rand()%DIM;
	
	AnimateDeath();
}


void Snake::CheckBorder()
{
	if(head[0]<0) head[0]= DIM-1;
	if(head[0]>=DIM) head[0] = 0;
	if(head[1]<0) head[1]=DIM -1;
	if(head[1]>=DIM) head[1] = 0;
}
void Snake::MoveX(int d)
{
	if(d<0)
		head[0]--;
	else
		head[0]++;
}

void Snake::MoveY(int d)
{
	if(d<0)
		head[1]--;
	else
		head[1]++;
}

void Snake::Move(Cherry cherry)
{
	oldHead[0] = head[0];
	oldHead[1] = head[1];

	int distX = cherry.GetX() - head[0];
	int distY = cherry.GetY() - head[1];
	
	if(distX != 0)
	{
		MoveX(distX);
	}
	else
	{
		MoveY(distY);
	}
	int s = CheckEatTail();
	int maxIter = 100;
	int i = 0;
	while(s!=0)
	{
		if(rand()%2==0)
		{
			MoveX(distX);
		}
		else
		{
			MoveY(distY);
		}
		s = CheckEatTail();
		i++;
		if (i>= maxIter)
			break;
	}
	
	MoveTail();
	
	CheckBorder();
	CheckEatTail();
}

int Snake::CheckEatTail()
{
	int status = 0;
	for(int i = 0; i<length; i++)
	{
		if(head[0]==tail[0][i] && head[1]==tail[1][i])
		{
			Kill();
			status = 1;
			return status;
		}
		
	}
	return status;
}
void Snake::MoveTail()
{
	int oldTail [2][length];
	for(int i = 0; i<length; i++)
	{
		oldTail[0][i] = tail[0][i];
		oldTail[1][i] = tail[1][i];
	}

	
	tail[0][0] = oldHead[0];
	tail[1][0] = oldHead[1];
	
	for(int i=1; i<length; i++)
	{
		tail[0][i] = oldTail[0][i-1];
		tail[1][i] = oldTail[1][i-1];
	}
	
	//CheckEatTail();
	CheckBorder();
}

void Snake::Add()
{
	if(length<MAXLENGTH) length++;
	
	if(tail[0][length-1]==tail[0][length-2])
	{
		tail[0][length] = tail[0][length-1];
		tail[1][length] = tail[1][length-1] + 1;
	}
	else
	{
		tail[0][length] = tail[0][length-1] + 1;
		tail[1][length] = tail[1][length-1];
	}
}



void Draw(Snake& snake, Cherry& cherry)
{
	unsigned char color_px[DIM][DIM];
	// clear image
	for(int i=0; i<DIM; i++)
		for(int j=0; j<DIM; j++)
			color_px[i][j] = backGround;
	
	
	int pos[2][MAXLENGTH+1];

	// total length = length + head
	int totalLength = snake.GetLength() +1;
	snake.GetPosition(pos);
	
	for(int i=0; i<totalLength; i++)
	{
		if(pos[0][i]<0 || pos[0][i]>=DIM || pos[1][i]<0 || pos[1][i]>=DIM) break;
		
		if(i==0) color_px[pos[0][i]][pos[1][i]] = colorOfHead;
		else color_px[pos[0][i]][pos[1][i]] = colorOfSnake;
	}
	
	// draw cherry
	color_px[cherry.GetX()][cherry.GetY()] = colorOfCherry;
	
	// put color_px to LED:
	// your code...
	DrawBot(color_px);
}



void CheckEat(Snake& snake, Cherry& cherry)
{
	if( (snake.GetHeadX()==cherry.GetX()) && snake.GetHeadY()== cherry.GetY())
	{
		snake.Add();
		cherry.New();
	}
}


int main()
{
	// initiate snake and cherry with random position
	Snake snake(rand()%DIM,rand()%DIM,5);
	Cherry cherry(rand()%DIM,rand()%DIM);
	
	while(1)
	{
		Draw(snake,cherry);
		usleep(DELAY);
		snake.Move(cherry);
		usleep(DELAY);
		CheckEat(snake,cherry);
		if(snake.GetLength() >= MAXLENGTH)
			snake.Kill();
		usleep(DELAY);
	}
	return 0;
}
