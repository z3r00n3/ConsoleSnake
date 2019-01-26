///////////////////////////////////////////////////////////////////////////////
//                  ����������� ������������ ������                          //
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>

///////////////////////////////////////////////////////////////////////////////
//                          ����������������                                 //
///////////////////////////////////////////////////////////////////////////////

#define WIDTH      40 // ������ ���� ����
#define HEIGHT     20 // ������ ���� ����

// ������������� ����� ��� ��������
#define BORDER_CHAR     219
#define SNAKE_HEAD_CHAR 15
#define SNAKE_TAIL_CHAR 249
#define FRUIT_CHAR      3

///////////////////////////////////////////////////////////////////////////////
//                    ���������������� ���� ������                           //
///////////////////////////////////////////////////////////////////////////////

// ������������� ����� ��� ����� ������
enum Keys 
{ 
	KEY_UP      = 72, // ������� ������� �����
	KEY_DOWN    = 80, // ������� ������� ����
	KEY_RIGHT   = 77, // ������� ������� ������
	KEY_LEFT    = 75, // ������� ������� �����
	KEY_EXIT    = 27, // ������� Esc
	KEY_RESTART = 'r',
};

// ������������� ����� ��� ����������� ��������
enum Directions { NODIR, UP, DOWN, RIGHT, LEFT };

// ������������� ����� ��� ������� ����
enum GameMode { BORDER, NO_BORDER, ISLANDS };

// ��������� ��� ������������� ���������� ���������
struct Coord
{
	int x;
	int y;
};

///////////////////////////////////////////////////////////////////////////////
//                       ���������� ����������                               //
///////////////////////////////////////////////////////////////////////////////

bool GameOver;
Coord Snake, Fruit, Field[1024];
Directions Direction;
int Islands[100][100], Num;

///////////////////////////////////////////////////////////////////////////////
//                         ��������� �������                                 //
///////////////////////////////////////////////////////////////////////////////

void Settings           (void);     // ��������� ����
void FruitGeneration    (void);     // ��������� �������
void IslandGeneration   (void);     // ��������� ���������
void DrawFrame          (GameMode); // �������, ����������� ���������� �����
void DrawWithoutIslands (void);     // ��������� ����� � ����������
void DrawWithIslands    (void);     // ��������� ����� ��� ���������
void GetInput           (void);     // ��������� ����������������� �����
void SetCursorCoord     (int, int); // ��������� ��������� ������� � �������
void ChangeDirection    (void);     // �������, ����������� ���������� ����������� ��������
void SnakeLogic         (void);     // ������ ������
void BorderLogic        (bool);     // ������ ������ �������� ����
void CheckTailCollision (void);     // �������� �� ������������ � �������
void FruitLogic         (void);     // ������ �������
void IslandLogic        (bool);     // ������ ���������
void GameLogic          (GameMode); // �������, ����������� ���� ������� �������
void EndGame            (void);     // �������, �������������� ���������� ����

///////////////////////////////////////////////////////////////////////////////
//                        ����������� �������                                //
///////////////////////////////////////////////////////////////////////////////

// ������� �������
int main(void)
{
	while (!GameOver)
	{
		Settings();
		printf("Press [1] for play game with border\n");
		printf("Press [2] for play game without border\n");
		printf("Press [3] for play game with islands\n");
		
		char choice = _getch();
		system("cls");

		switch (choice)
		{
		case '1':
			while (!GameOver)
			{
				DrawFrame(BORDER);
				GetInput();
				GameLogic(BORDER);
			}
		case '2':
			while (!GameOver)
			{
				DrawFrame(NO_BORDER);
				GetInput();
				GameLogic(NO_BORDER);
			}
		case '3':
			IslandGeneration();
			while (!GameOver)
			{
				DrawFrame(ISLANDS);
				GetInput();
				GameLogic(ISLANDS);
			}
			if (GameOver == true)
				EndGame();
		}
	}

	return 0;
}

void Settings(void)
{
	CONSOLE_CURSOR_INFO CCI;
	CCI.bVisible = false;
	CCI.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CCI);
	srand(time(NULL));
	GameOver = false;
	Direction = NODIR;
	Snake.x = WIDTH / 2 - 1;
	Snake.y = HEIGHT / 2 - 1;
	FruitGeneration();
	Num = 0;
}

void FruitGeneration(void)
{
	Fruit.x = rand() % (WIDTH - 2);
	Fruit.y = rand() % HEIGHT;
}

void IslandGeneration(void)
{
	int x = rand() % WIDTH;
	int y = rand() % HEIGHT;

	for (int i = 0; i <= 99; i++)
		for (int j = 0; j <= 99; j++)
			Islands[i][j] = 0;

	while (x < WIDTH)
	{
		while (y < HEIGHT)
		{
			Snake.x = x - rand() % 10;
			Snake.y = y - rand() % 10;

			for (int i = 0; i < 3; i++)
				Islands[Snake.x][Snake.y] = 1;

			y += 9;
		}

		y = 10;
		x += 9;
	}
}

void SetCursorCoord(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void DrawFrame(GameMode mode)
{
	switch (mode)
	{
	case BORDER:
	case NO_BORDER:
		DrawWithoutIslands();
		break;
	case ISLANDS:
		DrawWithIslands();
		break;
	default:
		break;
	}
}

void DrawWithoutIslands(void)
{
	// ������������� ���������� ������� � ����� ������� ���� �������
	SetCursorCoord(0, 0);
	
	// ������ ������� �������
	for (int i = 0; i < WIDTH + 1; i++)
		putchar(BORDER_CHAR);
	printf("\n");
	
	// ������ ��� ���������
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			// ������ ������� �������
			if (j == 0 || j == WIDTH - 1)
				putchar(BORDER_CHAR);

			// ������ ������ ����
			if (Snake.y == i && Snake.x == j)
				putchar(SNAKE_HEAD_CHAR);
			// ������ �����
			else if (Fruit.y == i && Fruit.x == j)
				putchar(FRUIT_CHAR);
			else
			{
				bool print = false;
				for (int k = 0; k < Num; k++)
				{
					// ������ ����� ����
					if (Field[k].x == j && Field[k].y == i)
					{
						print = true;
						putchar(SNAKE_TAIL_CHAR);
					}
				}
				// ������ ������ ����
				if (print == false)
					printf(" ");
			}
		}
		printf("\n");
	}

	// ������ ������ �������
	for (int i = 0; i < WIDTH + 1; i++)
		putchar(BORDER_CHAR);
	
	// �������� ����
	printf("\nTotal score = %i\n", Num);
}

void DrawWithIslands(void) 
{
	// ������������� ���������� ������� � ����� ������� ���� �������
	SetCursorCoord(0, 0);

	// ������ ������� �������
	for (int i = 0; i < WIDTH + 1; i++)
		putchar(BORDER_CHAR);
	printf("\n");

	// ������ ��� ���������
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			// ������ ������� �������
			if (j == 0 || j == WIDTH - 1)
				putchar(BORDER_CHAR);

			if (Snake.y == i && Snake.x == j)
			{
				// ������ ������ ����
				while (Islands[Snake.x][Snake.y] == 1) 
				{
					Snake.x = WIDTH / 2 - 1;
					Snake.y = HEIGHT / 2 - 1;
				}
				putchar(SNAKE_HEAD_CHAR);
			}
			else if (Fruit.y == i && Fruit.x == j)
			{
				// ������ �����
				while (Islands[Fruit.x][Fruit.y] == 1)
				{
					FruitGeneration();
				}
				putchar(FRUIT_CHAR);
			}
			// ������ ��������
			else if (Islands[j][i] == 1)
				putchar(BORDER_CHAR);
			else 
			{
				bool print = false;
				for (int k = 0; k < Num; k++)
				{
					// ������ ����� ����
					if (Field[k].x == j && Field[k].y == i)
					{
						print = true;
						putchar(SNAKE_TAIL_CHAR);
					}
				}
				// ������ ������ ����
				if (print == false)
					printf(" ");
			}
		}
		printf("\n");
	}

	// ������ ������ �������
	for (int i = 0; i < WIDTH + 1; i++)
		putchar(BORDER_CHAR);

	// �������� ����
	printf("\nTotal score = %i\n", Num);
}

void GetInput(void)
{
	if (_kbhit())
	{
		switch (_getch())
		{
		case KEY_LEFT:
			if (Direction != RIGHT)
				Direction = LEFT;
			break;
		case KEY_RIGHT:
			if (Direction != LEFT)
				Direction = RIGHT;
			break;
		case KEY_UP:
			if (Direction != DOWN)
				Direction = UP;
			break;
		case KEY_DOWN:
			if (Direction != UP)
				Direction = DOWN;
			break;
		case KEY_EXIT:
			GameOver = true;
			break;
		default:
			break;
		}
	}
}

void ChangeDirection(void)
{
	switch (Direction)
	{
	case LEFT:
		if (Direction != RIGHT)
			Snake.x--;
		break;
	case RIGHT:
		if (Direction != LEFT)
			Snake.x++;
		break;
	case UP:
		if (Direction != DOWN)
			Snake.y--;
		break;
	case DOWN:
		if (Direction != UP)
			Snake.y++;
		break;
	default:
		break;
	}
}

void SnakeLogic(void)
{
	Coord prev, buff;

	prev.x = Field[0].x;
	prev.y = Field[0].y;
	Field[0].x = Snake.x;
	Field[0].y = Snake.y;

	for (int i = 1; i < Num; i++)
	{
		buff.x = Field[i].x;
		buff.y = Field[i].y;
		Field[i].x = prev.x;
		Field[i].y = prev.y;
		prev.x = buff.x;
		prev.y = buff.y;
	}
}

void BorderLogic(bool border)
{
	if (border)
	{
		if (Snake.x < 0 || Snake.x > WIDTH - 2 || Snake.y < 0 || Snake.y > HEIGHT)
			GameOver = true;
	}
	else
	{
		if (Snake.x >= WIDTH - 1)
			Snake.x = 0;
		else if (Snake.x < 0)
			Snake.x = WIDTH - 2;
		if (Snake.y > HEIGHT - 1)
			Snake.y = 0;
		else if (Snake.y < 0)
			Snake.y = HEIGHT - 1;
	};
}

void CheckTailCollision(void)
{
	for (int i = 0; i < Num; i++)
	{
		if (Field[i].x == Snake.x && Field[i].y == Snake.y)
			GameOver = true;
	}
}

void FruitLogic(void)
{
	if (Snake.x == Fruit.x && Snake.y == Fruit.y) {
		Num++;
		FruitGeneration();
	}
}

void IslandLogic(bool islands)
{
	if (islands)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
				if (i == Snake.y && j == Snake.x && Islands[Snake.x][Snake.y] == 1)
					GameOver = true;
		}
	}
}

void GameLogic(GameMode mode)
{
	bool border, islands;

	switch (mode)
	{
	case BORDER:
		border  = true;
		islands = false;
		break;
	case NO_BORDER:
		border  = false;
		islands = false;
		break;
	case ISLANDS:
		border  = false;
		islands = true;
		break;
	default:
		break;
	}

	SnakeLogic();
	ChangeDirection();
	BorderLogic(border);
	CheckTailCollision();
	IslandLogic(islands);
	FruitLogic();
}

void EndGame(void)
{
	system("cls");
	printf("GAME OVER\n");
	printf("Total score: %i\n", Num);
	printf("\n");
	printf("press [ESC] to quit or [R] to restart\n");
	
	char c = _getch();
	switch (c)
	{
	case KEY_EXIT:
		GameOver = true;
		break;
	case KEY_RESTART:
		GameOver = false;
		Settings();
		system("cls");
		break;
	default:
		break;
	}
}

