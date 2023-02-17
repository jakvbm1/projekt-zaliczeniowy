#include <iostream>
#include <random>
#include <conio.h>
#include<windows.h>
#include<fstream>
#include<string>

using namespace std;

int random_number(int min, int max);
void generating_map(bool** map);
void displaying_map(bool** map);
struct Room
{
	int position_y;
	int position_x;
	char** room;

	void displaying();
};
struct Player
{
	int y_on_map;
	int x_on_map;
	int y_in_room;
	int x_in_room;

	void moving(char** room);
	bool winning();
};
void display_rooms(Room** level);
void applying_player(Player player, Room(*rooms)[4]);
enum directions
{
	up, down, rght, lft, none
};
enum enemy_type
{
	vertical, diagonal, following, still, horizontal
};
struct Enemy {
	int enemy_y;
	int enemy_x;
	enemy_type type;
	int turn{};
	directions dir = up;
	directions dir_2 = lft;

	void moving(int y, int x, Room room);
};
void applying_enemy(Enemy enemy, Room room);
enemy_type choosing_type();
bool game_is_over(Player player, Enemy enemy);
void generating_room(char** room, bool** map, int y, int x);
void displaying_menu(bool* start, bool* hard, bool* end);
void saving_record(int result);

int main()
{
	static CONSOLE_FONT_INFOEX  fontex;
	fontex.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetCurrentConsoleFontEx(hOut, 0, &fontex);
	fontex.FontWeight = 700;
	fontex.dwFontSize.X = 36;
	fontex.dwFontSize.Y = 36;
	SetCurrentConsoleFontEx(hOut, NULL, &fontex);

	bool end = true;

	while (end)
	{

	bool start = false;
	bool hard = false;
	displaying_menu(&start, &hard, &end);

	if(start)
	{ 
	bool** map;
	map = new bool* [4];

	Player player = { 3, 0, 3, 3 };

	for (int i = 0; i < 4; i++)
	{
		map[i] = new bool[4];
		for (int j = 0; j < 4; j++)
		{
			map[i][j] = false;
		}
	}

	generating_map(map);

	Room rooms[4][4];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			rooms[i][j].position_y = i;
			rooms[i][j].position_x = j;

			rooms[i][j].room = new char* [7];
			for (int k = 0; k < 7; k++)
			{
				rooms[i][j].room[k] = new char[7];
			}

			generating_room(rooms[i][j].room, map, i, j);
		}
	}

	applying_player(player, rooms);

	Enemy enemies[4][4];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{


			enemies[i][j] = { random_number(2,4), random_number(2, 4), choosing_type()};
			if (i != 3 || j != 0)
			{
				if (i != 0 || j != 3)
				{
					applying_enemy(enemies[i][j], rooms[i][j]);
				}
			}
		}
	}

	Enemy extra_enemies[4][4];

	if (hard)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{


				extra_enemies[i][j] = { random_number(2,4), random_number(2, 4), choosing_type() };
				if (i != 3 || j != 0)
				{
					if (i != 0 || j != 3)
					{
						applying_enemy(extra_enemies[i][j], rooms[i][j]);
					}
				}
			}
		}

	}
	//uzna³em, ¿e ³atwiej bêdzie zrobiæ dla pokoju startowego i koñcowego przeciwników którzy siê nie poruszaj¹ i nie pokazuj¹ ni¿ próbowaæ ich nie robiæ
	enemies[3][0] = { 6, 6, still };
	enemies[0][3] = { 6, 6, still };
	if (hard)
	{
		extra_enemies[3][0] = { 6, 6, still };
		extra_enemies[0][3] = { 6, 6, still };
	}

	rooms[0][3].room[3][3] = 'T';

	//ten fragment kodu wykorzystywa³em przy testowaniu projektu, ¿eby zobaczyæ czy pokoje s¹ generowane prawid³owo
	//applying_player(player, rooms);

	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		displaying_map(map);

	//		cout << rooms[i][j].position_y << " " << rooms[i][j].position_x << endl;
	//		rooms[i][j].displaying();
	//		Sleep(200);
	//		system("cls");
	//	}
	//}

	displaying_map(map);
	Sleep(800);
	system("cls");

	bool game_over = false;
	bool win = false;
	bool game_over_extra = false;

	while (!game_over && !win && !game_over_extra)
	{
		rooms[player.y_on_map][player.x_on_map].displaying();
		player.moving(rooms[player.y_on_map][player.x_on_map].room);

		enemies[player.y_on_map][player.x_on_map].moving(player.y_in_room, player.x_in_room, rooms[player.y_on_map][player.x_on_map]);
		if (hard)
		{
			extra_enemies[player.y_on_map][player.x_on_map].moving(player.y_in_room, player.x_in_room, rooms[player.y_on_map][player.x_on_map]);
		}
		system("cls");

		win = player.winning();

		game_over = game_is_over(player, enemies[player.y_on_map][player.x_on_map]);
		if (hard)
		{
			game_over_extra = game_is_over(player, extra_enemies[player.y_on_map][player.x_on_map]);
		}
	

		applying_enemy(enemies[player.y_on_map][player.x_on_map], rooms[player.y_on_map][player.x_on_map]);
		if (hard)
		{
			applying_enemy(extra_enemies[player.y_on_map][player.x_on_map], rooms[player.y_on_map][player.x_on_map]);
		}
		applying_player(player, rooms);

	}

	cout << "Game over" << endl;

	if (win)
	{
		cout << "you won!" << endl;
		Beep(440, 500);
		Beep(660, 750);
		Beep(880, 1000);

		int players_turns{};

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				players_turns += enemies[i][j].turn;
			}
		}

		saving_record(players_turns);
	}

	else
	{
		cout << "you lost.." << endl;
		Beep(466, 500);
		Beep(233, 750);
		Beep(167, 1000);
	}
	Sleep(5000);
	system("cls");
	}
	}
}

int random_number(int min, int max)
{
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<mt19937::result_type> dist(min, max);
	return dist(dev);
}

void generating_map(bool** map)
{
	map[3][0] = true;
	map[0][3] = true;


	//1 - gora, 2 - prawo, 3 - lewo

	int current_y = 3;
	int current_x = 0;

	while (current_x < 3 || current_y > 0)
	{

		int direction = random_number(1, 3);

		switch (direction)
		{
		case 1:
		{
			if (current_y > 0)
			{
				current_y--;
				map[current_y][current_x] = true;
			}
			break;
		}

		case 2:
		{
			if (current_x < 3)
			{
				current_x++;
				map[current_y][current_x] = true;
			}
			break;
		}

		case 3:
		{
			if (current_x > 0)
			{
				current_x--;
				map[current_y][current_x] = true;
			}
			break;
		}
		}
	}

}

void displaying_map(bool** map)
{
	system("cls");
	int turn{};

	for (int a = 0; a < 8; a++)
	{
		cout << "Level's map:" << endl;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i != 3 || j != 0)
				{
					if (map[i][j])
					{
						cout.width(2);
						cout << char(254);
					}
					else
					{
						cout.width(2);
						cout << char(32);
					}
				}

				else
				{
					cout.width(2);
					if (turn % 2 == 0)
					{
						cout << char(254);
					}

					else
					{
						cout << char(32);
					}
				}

			}
			cout << endl;
		}
		turn++;
		Sleep(300);
		system("cls");
	}

}

void display_rooms(Room** level)
{

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cout << "pokoj " << i << " " << j << endl;
			for (int k = 0; k < 7; k++)
			{
				for (int l = 0; l < 7; l++)
				{
					cout.width(2);
					cout << level[i][j].room[k][l];
				}
				cout << endl;
			}
			Sleep(500);
			system("cls");

		}
	}
}

void Room::displaying()
{
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			cout.width(2);
			cout << room[i][j];
		}
		cout << endl;
	}
}

void applying_player(Player player, Room(*rooms)[4])
{
	rooms[player.y_on_map][player.x_on_map].room[player.y_in_room][player.x_in_room] = 'p';
}

void Player::moving(char** room)
{
	directions dir;
	char a;
	bool correct_input = false;

	while (!correct_input)
	{
		a = _getch();
		switch (a)
		{
		case 'w':
		{
			dir = up;
			correct_input = true;
			break;
		}
		case 's':
		{
			dir = down;
			correct_input = true;
			break;
		}
		case 'a':
		{
			dir = lft;
			correct_input = true;
			break;
		}
		case 'd':
		{
			dir = rght;
			correct_input = true;
			break;
		}
		default:
		{dir = none;
		break; }
		}
	}



	switch (dir)
	{
	case down:
	{
		if (room[y_in_room + 1][x_in_room] != char(254) && y_in_room < 6)
		{
			room[y_in_room][x_in_room] = char(32);
			y_in_room++;
		}

		if (y_in_room == 6)
		{
			room[y_in_room][x_in_room] = char(32);
			y_on_map++;
			y_in_room = 1;

		}

		break;
	}

	case up:
	{
		if (room[y_in_room - 1][x_in_room] != char(254) && y_in_room > 0)
		{
			room[y_in_room][x_in_room] = char(32);
			y_in_room--;
		}

		if (y_in_room == 0)
		{
			room[y_in_room][x_in_room] = char(32);
			y_on_map--;
			y_in_room = 5;

		}
		break;
	}

	case lft:
	{
		if (room[y_in_room][x_in_room - 1] != char(254) && x_in_room > 0)
		{
			room[y_in_room][x_in_room] = char(32);
			x_in_room--;
		}

		if (x_in_room == 0)
		{
			room[y_in_room][x_in_room] = char(32);
			x_on_map--;
			x_in_room = 5;

		}
		break;
	}

	case rght:
	{
		if (room[y_in_room][x_in_room + 1] != char(254) && x_in_room < 6)
		{
			room[y_in_room][x_in_room] = char(32);
			x_in_room++;
		}

		if (x_in_room == 6)
		{
			room[y_in_room][x_in_room] = char(32);
			x_on_map++;
			x_in_room = 1;

		}

		break;
	}

	default:
	{
		break;
	}

	}
}

bool Player::winning()
{
	if (y_on_map == 0 && x_on_map == 3 && x_in_room == 3 && y_in_room == 3)
	{
		return true;
	}

	else
	{
		return false;
	}
}

void Enemy::moving(int y, int x, Room room)
{
	if (turn > 0)
	{
		switch (type)
		{
		case vertical:
		{
			turn++;
			room.room[enemy_y][enemy_x] = char(32);
			switch (dir)
			{
			case down:
			{
				enemy_y++;
				if (enemy_y >= 5)
				{
					dir = up;
				}
				break;
			}
			case up:
			{
				enemy_y--;
				if (enemy_y <= 1)
				{
					dir = down;
				}
				break;
			}
			}

			break;

		}

		case horizontal:
		{
			turn++;
			room.room[enemy_y][enemy_x] = char(32);
			switch (dir_2)
			{
			case lft:
			{
				enemy_x--;
				if (enemy_x <= 1)
				{
					dir_2 = rght;
				}
				break;
			}
			case rght:
			{
				enemy_x++;
				if (enemy_x >= 5)
				{
					dir_2 = lft;
				}
				break;
			}
			}

			break;

		}

		case diagonal:
		{
			turn++;
			room.room[enemy_y][enemy_x] = char(32);
			//if (turn % 2 == 1)
			//{
			switch (dir)
			{
			case down:
			{
				enemy_y++;
				if (enemy_y >= 5)
				{
					dir = up;
				}
				break;
			}
			case up:
			{
				enemy_y--;
				if (enemy_y <= 1)
				{
					dir = down;
				}
				break;
			}
			}

			//break;
		//}

		//else if (turn % 2 == 0)
		//{
			switch (dir_2)
			{
			case rght:
			{
				enemy_x++;
				if (enemy_x >= 5)
				{
					dir_2 = lft;
				}
				break;
			}
			case lft:
			{
				enemy_x--;
				if (enemy_x <= 1)
				{
					dir_2 = rght;
				}
				break;
			}

			//break;
			}
			break;
			//}


		}

		case following:
		{
			turn++;
			if (turn % 2 == 0)
			{
				room.room[enemy_y][enemy_x] = char(32);

				if (enemy_y != y)
				{
					if (enemy_y > y)
					{
						enemy_y--;
					}

					else
					{
						enemy_y++;
					}
				}

				else if (enemy_x != x)
				{
					if (enemy_x > x)
					{
						enemy_x--;
					}

					else
					{
						enemy_x++;
					}
				}
			}
			break;
		}

		case still:
		{
			turn++;
			break;
		}

		}
	}

	else
	{
		turn++;
	}


}

void applying_enemy(Enemy enemy, Room room)
{
	if (enemy.type != still)
	{
		room.room[enemy.enemy_y][enemy.enemy_x] = 'e';
	}
}

enemy_type choosing_type()
{
	int i = random_number(1, 4);

	switch (i)
	{
	case 1:
	{
		return vertical;
		break;
	}
	case 2:
	{
		return diagonal;
		break;
	}
	case 3:
	{
		return following;
		break;
	}
	case 4:
	{
		return horizontal;
		break;
	}

	default:
	{
		return still;
		break;
	}
	}
}

bool game_is_over(Player player, Enemy enemy)
{
	if (player.x_in_room == enemy.enemy_x && player.y_in_room == enemy.enemy_y)
	{
		return true;
	}
	return false;
}

void generating_room(char** room, bool** map, int y, int x)
{
	for (int k = 0; k < 7; k++)
	{
		for (int l = 0; l < 7; l++)
		{
			room[k][l] = char(254);
		}

		if (map[y][x])
		{
			for (int k = 1; k < 6; k++)
			{
				for (int l = 1; l < 6; l++)
				{
					room[k][l] = char(32);
				}

				if (y > 0)
				{
					if (map[y - 1][x])
					{
						room[0][2] = char(32);
						room[0][3] = char(32);
						room[0][4] = char(32);
					}
				}

				if (y < 3)
				{
					if (map[y + 1][x])
					{
						room[6][2] = char(32);
						room[6][3] = char(32);
						room[6][4] = char(32);
					}
				}

				if (x > 0)
				{
					if (map[y][x - 1])
					{
						room[2][0] = char(32);
						room[3][0] = char(32);
						room[4][0] = char(32);
					}
				}

				if (x < 3)
				{
					if (map[y][x + 1])
					{
						room[2][6] = char(32);
						room[3][6] = char(32);
						room[4][6] = char(32);
					}
				}

			}
		}
	}
}

void displaying_menu(bool* start, bool* hard, bool* end)
{
	cout << char(201);
	for (int i = 0; i < 5; i++)
	{
		cout << char(205);
	}
	cout << char(187);
	cout << endl << char(186) << "Rouge" << char(186) << endl;
	cout << char(200);
	for (int i = 0; i < 5; i++)
	{
		cout << char(205);
	}
	cout << char(188) << endl << endl;

	cout << "Options:" << endl << "1. run a game" << endl << "2.run a game on hard mode" << endl << "3. show rules" << endl << "4. exit game" << endl;


	int a;
	a = _getch() - 48;
	cout << a;

	//if (_kbhit())
	//{ 
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	switch (a)
	{

	case 1:
	{
		system("cls");
		//cout << "Generating the level, it might take a while.."<<endl;
		*start = true;
		break;
	}
	case 2:
	{
		system("cls");
		//cout << "Generating the level, it might take a while.."<<endl;
		*start = true;
		*hard = true;
		break;
	}

	case 3:
	{
		;
		for (int i = 10; i > 0; i--)
		{
			system("cls");
			cout << "To win, the player have to go to the room in the upper right corner of a map and stand on a trophy" << endl;
			cout << "touching any of the enemies will result in loosing" << endl;
			cout << "use w, a, s, d to control movement" << endl;
			cout << "Enemies have different patterns of moving, so better watch out!" << endl;
			cout << "Legend:" << endl;

			cout << char(201) << char(205) << char(187) << endl;
			cout << char(186) << "p" << char(186) << " - player" << endl;
			cout << char(200) << char(205) << char(188) << endl << endl;

			cout << char(201) << char(205) << char(187) << endl;
			cout << char(186) << "e" << char(186) << " - enemy" << endl;
			cout << char(200) << char(205) << char(188) << endl << endl;

			cout << char(201) << char(205) << char(187) << endl;
			cout << char(186) << char(254) << char(186) << " - wall" << endl;
			cout << char(200) << char(205) << char(188) << endl << endl;

			cout << char(201) << char(205) << char(187) << endl;
			cout << char(186) << "T" << char(186) << " - trophy" << endl;
			cout << char(200) << char(205) << char(188) << endl << endl;

			//cout << "p - player" << endl << "e - enemy" << endl << char(254) << " - wall" << endl << "T - trophy" << endl;


			cout << endl << "menu will open automatically in " << i << " seconds" << endl;
			Sleep(1000);
		}

		system("cls");
		displaying_menu(start, hard, end);
		break;
	}

	case 4:
	{
		system("cls");
		cout << "closing game" << endl;
		*end = false;
		break;
	}
	default:
	{
		system("cls");
		cout << "inproper input" << endl;
		Sleep(500);
		system("cls");
		displaying_menu(start, hard, end);
		break;
	}

	}
	//}
}

void saving_record(int result)
{
	cout << "you achieved victory in: " << result << " turns" << endl;

	int record = 10000;
	string file_name = "record.txt";
	fstream plik;

	plik.open(file_name, ios::in);

	if (plik.good())
	{
		string rec;
		getline(plik, rec);
		record = stoi(rec);
		plik.close();
	}

	if (result < record)
	{
		cout << "It's a new record!" << endl;
		plik.open(file_name, ios::out);
		if (plik.good())
		{
			plik << result << "\n";
			plik.close();
		}
	}

	else
	{
		cout << "Current record is: " << record << " turns";
	}
}
