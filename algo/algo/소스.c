#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#define INF 999;
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SUBMIT 4
#define INVEN 5
#define MAX(x, y) x > y ? x : y
//컷씬만 이제 잘 보정하면 끝날듯
int visit[9];		// 해당 집에 방문을 했는지?
int vinum = 0;		// 집들을 몇 번 방문했는지?
int isHouse[9][4];		// cur 위치 집에서 인접한 집들이 각 몇 번째 집인지?
int house[9];			// cur 위치 집에서 인접한 집의 수
int road[9][9];		// road[i][j]면 i에서 j까지의 도로 가중치를 나타낸 배열
int room[9][5];		// 0 = 보석, 1 = 백금, 2 = 금, 3 = 은, 4 = 철
int itemweight[5] = { 2, 3, 4, 5, 6 };		// 각 아이템의 무게
int itemvalue[5] = { 10, 6, 8, 12, 13 };	// 각 아이템의 가치
int bag[5];		// 가방 배열, 인덱스는 room의 배열과 같음
int weight;		// 가방 무게
int worth = 0;	// 총 훔친 물건의 가치
int dist = 0;	// 총 다녀온 길의 가중치

int d[100][100];	// 배낭 알고리즘에 쓰이는 dp 배열
int maxworth;	// 최소 경로에서 얻을 수 있는 최대의 가치
int mindist;	// 최소 경로 가중치
int path[9][9];	// 최소 거리를 구하기 위한 경로 배열
int minpath[9];	// 최소 경로 추적 배열, 보기용(1번째 집 = 0)으로 되어있으므로 실제 배열에 넣을 때 -1 할 것
int minpIndex;	// minpath index, 최소 경로가 몇 곳을 다녀왔는지
int minpitem[5];	// 최소 경로에서 빼올 수 있는 모든 아이템들

void notcon(); // 키보드 깜빡이는거 없애기
void gotoxy(int x, int y); // 이동
int keycontrol(); //wasd space
int title(); // 타이틀 화면
void prologue();	// 프롤로그 씬
void setting(); // 게임 구성 함수
void Map();		// 맵 씬
void selectMap(int cur);	// 현재 위치(cur)에서 갈 집 고르기
void showBag(int cur);	// 가방 보여주는 함수
int isDiscovered();	// 훔치다 들켰다면? 1:0
void knapSack();
void floyd();	// 최소 경로 추적 함수
void printPath(int(*path)[9], int s, int e);	// 추적한 경로를 기록하는 함수
char setRankD();	// 가치에 따른 랭크 표기
char setRankD();	// 거리에 따른 랭크 표기

void enterHouse(int cur);	// 집 들어가면 나오는 화면
void endRoom();		// 금은방 들가면 나오는 엔딩
int endRank();
void stuck();		// 현재 집에서 갈 집이 없을 때 나오는 엔딩
void discovered();	// 집 들어갔는데 집주인에게 들키는 엔딩
void bagBurst();	// 가방의 무게를 초과해서 가방 터지는 엔딩

void SScene();
void AScene();
void BScene();
void CScene();
void DScene();
void FScene();

int main() {
	system("mode con cols=100 lines=30");	// 콘솔창 크기 구성

	if (title() == 19) {
		prologue();
	}
	else {
		system("cls");
		exit(0);
	}
	setting();
	Map();
	selectMap(0);

	return 0;
}

void notcon()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = 0;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(consoleHandle, &ConsoleCursor);
}

void gotoxy(int x, int y) {
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int keycontrol()
{
	char temp = _getch();

	if (temp == 'w' || temp == 'W')
	{
		return UP;
	}
	else if (temp == 'a' || temp == 'A')
	{
		return LEFT;
	}
	else if (temp == 's' || temp == 'S')
	{
		return DOWN;
	}
	else if (temp == 'd' || temp == 'D')
	{
		return RIGHT;
	}
	else if (temp == ' ' || temp == 13)
	{
		return SUBMIT;
	}
	else if (temp == 'e' || temp == 'E')
	{
		return INVEN;
	}
}

int title() {
	int y = 19;
	gotoxy(48, 10); printf("도"); Sleep(50);
	gotoxy(52, 10); printf("둑"); Sleep(50);
	gotoxy(42, 12); printf("시"); Sleep(50);
	gotoxy(46, 12); printf("뮬"); Sleep(50);
	gotoxy(50, 12); printf("레"); Sleep(50);
	gotoxy(54, 12); printf("이"); Sleep(50);
	gotoxy(58, 12); printf("터"); Sleep(50);
	gotoxy(50, 19); printf("실행하기");
	gotoxy(50, 21); printf("종료하기");
	gotoxy(39, 25); printf("WASD 이동 SPACE 상호작용");
	gotoxy(46, y); printf("▶");

	while (1) {
		int Key = keycontrol();
		switch (Key) {
		case UP:
			if (y == 21) {
				gotoxy(46, y); printf("  ");
				y -= 2;
				gotoxy(46, y); printf("▶");
			}
			break;
		case DOWN:
			if (y == 19) {
				gotoxy(46, y); printf("  ");
				y += 2;
				gotoxy(46, y); printf("▶");
			}
			break;
		case SUBMIT:
			gotoxy(0, 0);
			return y;
		}
	}
}

void prologue() {
	system("cls");
	gotoxy(30, 5); printf("나는 도둑이다.");
	gotoxy(45, 5); printf("나는 도둑치곤 똑똑하다.");
	gotoxy(32, 8); printf("효율을 굉장히 잘 따지기 때문이다."); Sleep(1000);
	gotoxy(33, 11); printf("그렇기에 가는 루트도 최선으로,"); Sleep(1000);
	gotoxy(34, 14); printf("훔치는 물건도 최대한 적으면서");  Sleep(1000);
	gotoxy(34, 17); printf("가치가 높은 것만 털어갈 거다."); Sleep(1000);
	gotoxy(19, 20); printf("과연 나는 얼마나 효율적으로 집을 방문하고 털어갈 수 있을까?"); Sleep(1000);
	gotoxy(67, 29); printf("계속 하려면 아무 키나 누르시오..");
	_getch();
	system("cls");
	return;
}

void setting() {		// 게임 구성 함수
	srand((unsigned)time(NULL));
	weight = rand() % 50 + 50;	// 초기 가방 무게 구성
	for (int i = 0; i < 9; i++) {		// 집 간의 거리(가중치) 구성
		for (int j = 0; j < 9; j++) {
			road[i][j] = INF;
			path[i][j] = -1;
			if ((j - i == 3 && i <= 5) || (((i % 3) != 2) && (j - i == 1))) {
				road[i][j] = rand() % 30 + 5;
			}
			if (i == j) road[i][j] = 0;
		}
	}

	for (int i = 0; i < 9; i++) {	// 양방향 그래프 구성
		for (int j = 0; j < 9; j++)
			road[j][i] = road[i][j];
	}
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 4; j++) {
			isHouse[i][j] = -1;		// isHouse 초기화
		}
	}

	for (int i = 0; i < 9; i++) {	// 각 집의 인접 배열 구성
		house[i] = 0;
		visit[i] = 0;
		int t = 0;
		for (int j = 0; j < 9; j++) {
			if (road[i][j] > 0 && road[i][j] < 999) {
				house[i]++;
				isHouse[i][t++] = j;
			}
		}
	}
	house[8]++;		//---------------------------------금은방 구성
	isHouse[8][2] = 9;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 5; j++) {
			room[i][j] = 0;
		}
	}

	for (int i = 0; i < 9; i++) {		// 각 집 안의 내용물 구성
		int howmany = rand() % 10 + 7;	// 집에 물건 몇개? -> 7 ~ 17
		for (int j = 0; j < howmany; j++) {
			int item = rand() % 100;
			if (0 <= item && item < 5) room[i][0]++;		//다이아
			else if (5 <= item && item < 25) room[i][1]++;	//백금
			else if (25 <= item && item < 50) room[i][2]++;	//금
			else if (50 <= item && item < 75) room[i][3]++;	//은
			else room[i][4]++;								//철
		}
	}

	minpIndex = 1;
	minpath[0] = 1;
	for (int i = 1; i < 9; i++) minpath[i] = -1;
	for (int i = 0; i < 5; i++) minpitem[i] = 0;
	floyd();
	printPath(path, 1, 9);
	minpath[minpIndex++] = 9;
	for (int i = 0; i < minpIndex; i++) {
		for (int j = 0; j < 5; j++) {
			minpitem[j] += room[minpath[i] - 1][j];
		}
	}
	knapSack();
}

void Map() {
	gotoxy(0, 4); printf("아지트");
	gotoxy(0, 6); printf("~~~~~~~~~~~~~");
	gotoxy(0, 7); printf("~~~~~~~~~~~~~");

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			gotoxy(21 + 30 * j, 6 + 7 * i); printf("~~~~~~~~~~~~~~~~~~~~~~~~~");
			gotoxy(21 + 30 * j, 7 + 7 * i); printf("~~~~~~~~~~~~~~~~~~~~~~~~~");
		}
	}
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 3; j++) {
			gotoxy(13 + 30 * j, 8 + 7 * i); printf("{    }");
			gotoxy(13 + 30 * j, 9 + 7 * i); printf("{    }");
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (!visit[j * 3 + i]) {
				gotoxy(13 + 30 * i, 3 + j * 7); printf(" ____");
				gotoxy(13 + 30 * i, 4 + j * 7); printf("／   ＼");
				gotoxy(13 + 30 * i, 5 + j * 7); printf("┬-----┬");
				gotoxy(13 + 30 * i, 6 + j * 7); printf("│  %d  │", 3 * j + i + 1);
				gotoxy(13 + 30 * i, 7 + j * 7); printf("└-----┘");
			}
			else {
				gotoxy(13 + 30 * i, 3 + j * 7); printf("┌------┐");
				gotoxy(13 + 30 * i, 4 + j * 7); printf("│      │");
				gotoxy(13 + 30 * i, 5 + j * 7); printf("│ %d번  │", 3 * j + i + 1);
				gotoxy(13 + 30 * i, 6 + j * 7); printf("│털음^^│");
				gotoxy(13 + 30 * i, 7 + j * 7); printf("└------┘");
			}

		}
	}
	for (int i = 0; i < 3; i++) {
		gotoxy(33, 5 + 7 * i); printf("%d", road[i * 3][i * 3 + 1]);
		gotoxy(63, 5 + 7 * i); printf("%d", road[i * 3 + 1][i * 3 + 2]);
		if (i < 2) {
			gotoxy(15, 9 + 7 * i); printf("%d", road[i * 3][i * 3 + 3]);
			gotoxy(45, 9 + 7 * i); printf("%d", road[i * 3 + 1][i * 3 + 4]);
			gotoxy(75, 9 + 7 * i); printf("%d", road[i * 3 + 2][i * 3 + 5]);
		}
	}
	gotoxy(81, 20); printf("~~~~~~");
	gotoxy(81, 21); printf("~~~~~~");
	gotoxy(86, 19); printf("┌-------┐");
	gotoxy(86, 20); printf("│금은방 │\n");
	gotoxy(86, 21); printf("└-------┘\n");
	gotoxy(0, 23); printf("---------------------------------------------------------------------------------------------------");
}

void selectMap(int cur) {
	system("cls");
	int isCon = 0;
	for (int i = 0; i < house[cur]; i++) {
		if (visit[isHouse[cur][i]] == 1)
			isCon++;
	}
	if (isCon == house[cur]) stuck();	// 해당 집에서 더 이상 갈 곳 없으면 ㅈㅈ
	Map();
	if (vinum >= 5) {		// 5번 이상 방문 시 들킬 위험 생김
		gotoxy(73, 1); printf("동이 텄다.. 슬슬 가야 해");
	}
	gotoxy(0, 24); printf("털고 싶은 집을 선택해주세요	E : 가방");
	gotoxy(0, 27); printf("현재 위치 : ");
	if (!visit[0]) {
		printf("아지트");
		gotoxy(70, 24); printf("1번 집");
		gotoxy(90, 24); printf("없어용");
		gotoxy(70, 27); printf("없어용");
		gotoxy(90, 27); printf("없어용");
	}
	else {
		printf("%d번 집", cur + 1);
		switch (house[cur]) {
		case 1:
			gotoxy(70, 24); printf("%d번 집", isHouse[cur][0] + 1);
			gotoxy(90, 24); printf("없어용");
			gotoxy(70, 27); printf("없어용");
			gotoxy(90, 27); printf("없어용");
			break;

		case 2:
			gotoxy(70, 24); printf("%d번 집", isHouse[cur][0] + 1);
			gotoxy(90, 24); printf("%d번 집", isHouse[cur][1] + 1);
			gotoxy(70, 27); printf("없어용");
			gotoxy(90, 27); printf("없어용");
			break;
		case 3:
			gotoxy(70, 24); printf("%d번 집", isHouse[cur][0] + 1);
			gotoxy(90, 24); printf("%d번 집", isHouse[cur][1] + 1);
			gotoxy(70, 27);
			if (cur == 8) {
				printf("금은방");
			}
			else printf("%d번 집", isHouse[cur][2] + 1);
			gotoxy(90, 27); printf("없어용");
			break;
		case 4:
			gotoxy(70, 24); printf("%d번 집", isHouse[cur][0] + 1);
			gotoxy(90, 24); printf("%d번 집", isHouse[cur][1] + 1);
			gotoxy(70, 27); printf("%d번 집", isHouse[cur][2] + 1);
			gotoxy(90, 27); printf("%d번 집", isHouse[cur][3] + 1);
			break;
		}
	}

	int x = 66, y = 24;
	gotoxy(x, y); printf("▶");
	while (1) {
		int Key = keycontrol();
		switch (Key) {
		case UP:
			if (y == 27) {
				gotoxy(x, y); printf("  ");
				y -= 3;
				gotoxy(x, y); printf("▶");
			}
			else if (y == 24) {
				gotoxy(x, y); printf("▶");
			}
			break;
		case DOWN:
			if (y == 24) {
				gotoxy(x, y); printf("  ");
				y += 3;
				gotoxy(x, y); printf("▶");
			}
			else if (y == 27) {
				gotoxy(x, y); printf("▶");
			}
			break;
		case LEFT:
			if (x == 86) {
				gotoxy(x, y); printf("  ");
				x -= 20;
				gotoxy(x, y); printf("▶");
			}
			else if (x == 66) {
				gotoxy(x, y); printf("▶");
			}
			break;
		case RIGHT:
			if (x == 66) {
				gotoxy(x, y); printf("  ");
				x += 20;
				gotoxy(x, y); printf("▶");
			}
			else if (x == 86) {
				gotoxy(x, y); printf("▶");
			}
			break;
		case SUBMIT:
			if (x == 66 && y == 24) {
				if (visit[isHouse[cur][0]] == 1) {
					gotoxy(50, 24); printf("방문 이미 했음");
					break;
				}
				if (!visit[0]) {
					enterHouse(0);
					visit[0] = 1;
					selectMap(0);
				}
				enterHouse(isHouse[cur][0]);
				visit[isHouse[cur][0]] = 1;
				dist += road[cur][isHouse[cur][0]];
				selectMap(isHouse[cur][0]);
			}
			else if (x == 86 && y == 24) {
				if (visit[isHouse[cur][1]] == 1) {
					gotoxy(50, 24); printf("방문 이미 했음");
					break;
				}
				if (isHouse[cur][1] == -1) {
					gotoxy(50, 24); printf("갈 곳이 없어요");
					break;
				}
				enterHouse(isHouse[cur][1]);
				visit[isHouse[cur][1]] = 1;
				dist += road[cur][isHouse[cur][1]];
				selectMap(isHouse[cur][1]);
			}
			else if (x == 66 && y == 27) {
				if (visit[isHouse[cur][2]] == 1) {
					gotoxy(50, 24); printf("방문 이미 했음");
					break;
				}
				if (isHouse[cur][2] == -1) {
					gotoxy(50, 24); printf("갈 곳이 없어요");
					break;
				}
				if (isHouse[cur][2] > 8) endRoom();
				else {
					enterHouse(isHouse[cur][2]);
					visit[isHouse[cur][2]] = 1;
					dist += road[cur][isHouse[cur][2]];
					selectMap(isHouse[cur][2]);
				}
			}
			else if (x == 86 && y == 27) {
				if (visit[isHouse[cur][3]] == 1) {
					gotoxy(50, 24); printf("방문 이미 했음");
					break;
				}
				if (isHouse[cur][3] == -1) {
					gotoxy(50, 24); printf("갈 곳이 없어요");
					break;
				}
				enterHouse(isHouse[cur][3]);
				visit[isHouse[cur][3]] = 1;
				dist += road[cur][isHouse[cur][3]];
				selectMap(isHouse[cur][3]);
			}
		case INVEN:
			showBag(cur);
		}

	}
}

void showBag(int cur) {
	system("cls");
	for (int i = 0; i < 3; i++) {
		gotoxy(36, 1 + 8 * i); printf("┌-------------------------┐");
		gotoxy(36, 2 + 8 * i); printf("│                         │");
		gotoxy(36, 3 + 8 * i); printf("│                         │");
		gotoxy(36, 4 + 8 * i); printf("│                         │");
		gotoxy(36, 5 + 8 * i); printf("│                         │");
		gotoxy(36, 6 + 8 * i); printf("│                         │");
		gotoxy(36, 7 + 8 * i); printf("│                         │");
		gotoxy(36, 8 + 8 * i); printf("└-------------------------┘");
	}

	gotoxy(8, 9); printf("┌-------------------------┐");
	gotoxy(8, 10); printf("│                         │");
	gotoxy(8, 11); printf("│                         │");
	gotoxy(8, 12); printf("│                         │");
	gotoxy(8, 13); printf("│                         │");
	gotoxy(8, 14); printf("│                         │");
	gotoxy(8, 15); printf("│                         │");
	gotoxy(8, 16); printf("└-------------------------┘");

	gotoxy(64, 9); printf("┌-------------------------┐");
	gotoxy(64, 10); printf("│                         │");
	gotoxy(64, 11); printf("│                         │");
	gotoxy(64, 12); printf("│                         │");
	gotoxy(64, 13); printf("│                         │");
	gotoxy(64, 14); printf("│                         │");
	gotoxy(64, 15); printf("│                         │");
	gotoxy(64, 16); printf("└-------------------------┘");
	gotoxy(8, 2); printf("가방");
	gotoxy(8, 4); printf("남은 가방 무게 : %d", weight);
	gotoxy(74, 25); printf("E를 눌러 맵으로");
	gotoxy(45, 3); printf("다이아몬드");
	gotoxy(20, 11); printf("백금");
	gotoxy(49, 11); printf("금");
	gotoxy(77, 11); printf("은");
	gotoxy(49, 19); printf("철");

	gotoxy(49, 5); printf("%d개", bag[0]);
	gotoxy(21, 13); printf("%d개", bag[1]);
	gotoxy(49, 13); printf("%d개", bag[2]);
	gotoxy(77, 13); printf("%d개", bag[3]);
	gotoxy(49, 21); printf("%d개", bag[4]);
	int Key = keycontrol();
	if (Key == INVEN) selectMap(cur);
}

int isDiscovered() {
	srand((unsigned)time(NULL));
	int n = rand() % vinum;
	if (vinum > 5) n += vinum - 5;
	if (n > 5) return 1;
	else return 0;
}

void knapSack() {
	int sum = 0, t = 0;		//sum = 최소 경로에 훔칠 수 있는 물건 총 개수, t = tem 인덱스
	for (int i = 0; i < 5; i++) {
		sum += minpitem[i];
	}
	int tem[100][2];	//tem[n][0] = 무게, tem[n][1] = 가치, n = 최소 경로에 훔칠 수 있는 물건

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < minpitem[i]; j++) {
			tem[t][0] = itemweight[i];
			tem[t][1] = itemvalue[i];
			t++;
		}
	}
	int v = 0, w = weight;

	for (int i = 1; i <= sum; i++) {
		for (int j = 1; j <= w; j++) {
			int wi = tem[i - 1][0];
			int vi = tem[i - 1][1];

			if (wi > j) {
				d[i][j] = d[i - 1][j];
			}
			else {
				d[i][j] = MAX(d[i - 1][j], vi + d[i - 1][j - wi]);
			}
		}
	}
	maxworth = d[sum][w];
}

void floyd() {
	int d[9][9];
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++)
			d[i][j] = road[i][j];
	}

	for (int k = 0; k < 9; k++) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (d[i][k] + d[k][j] < d[i][j]) {
					path[i][j] = k + 1;
					d[i][j] = d[i][k] + d[k][j];
				}
			}
		}
	}
	mindist = d[0][8];
}

void printPath(int(*path)[9], int s, int e) {
	if (path[s - 1][e - 1] != -1) {
		printPath(path, s, path[s - 1][e - 1]);
		//printf("%2d->", path[s - 1][e - 1]);
		minpath[minpIndex++] = path[s - 1][e - 1];
		printPath(path, path[s - 1][e - 1], e);
	}
}

char setRankW() {
	int perW = (int)(((double)worth / (double)maxworth) * 100);
	if (perW >= 100) return 'S';
	else if (perW >= 80) return 'A';
	else if (perW >= 70) return 'B';
	else if (perW >= 60) return 'C';
	else if (perW >= 50) return 'D';
	else return 'F';
}

char setRankD() {
	int perD = (int)(((double)mindist / (double)dist) * 100);
	if (perD >= 100) return 'S';
	else if (perD >= 90) return 'A';
	else if (perD >= 80) return 'B';
	else if (perD >= 70) return 'C';
	else if (perD >= 60) return 'D';
	else return 'F';
}

void enterHouse(int cur) {
	system("cls");
	vinum++;
	if (isDiscovered()) discovered();
	gotoxy(23, 10); printf("다이아몬드 %d개, 백금 %d개, 금 %d개, 은 %d개, 철 %d개가 있다!", room[cur][0], room[cur][1], room[cur][2], room[cur][3], room[cur][4]);
	gotoxy(40, 14); printf("무엇을 훔쳐갈까?"); Sleep(500);
	gotoxy(0, 19); printf("------------------------┐");
	gotoxy(24, 20); printf("│");
	gotoxy(24, 21); printf("│");
	gotoxy(24, 22); printf("│");
	gotoxy(1, 21); printf("남은 가방의 무게 : %d", weight);
	gotoxy(0, 23); printf("------------------------┴---------------------------------------------------------------------------");
	gotoxy(50, 24); printf("다이아몬드");
	gotoxy(70, 24); printf("백금");
	gotoxy(90, 24); printf("금");
	gotoxy(50, 27); printf("은");
	gotoxy(70, 27); printf("철");
	gotoxy(90, 27); printf("나가기");
	int x = 46, y = 24, steal = 0;
	gotoxy(x, y); printf("▶");
	while (1) {
		int Key = keycontrol();
		switch (Key) {
		case UP:
			if (y == 27) {
				gotoxy(x, y); printf("  ");
				y -= 3;
				gotoxy(x, y); printf("▶");
			}
			else if (y == 24) {
				gotoxy(x, y); printf("▶");
			}
			break;
		case DOWN:
			if (y == 24) {
				gotoxy(x, y); printf("  ");
				y += 3;
				gotoxy(x, y); printf("▶");
			}
			else if (y == 27) {
				gotoxy(x, y); printf("▶");
			}
			break;
		case LEFT:
			if (x == 86 || x == 66) {
				gotoxy(x, y); printf("  ");
				x -= 20;
				gotoxy(x, y); printf("▶");
			}
			else if (x == 46) {
				gotoxy(x, y); printf("▶");
			}
			break;
		case RIGHT:
			if (x == 66 || x == 46) {
				gotoxy(x, y); printf("  ");
				x += 20;
				gotoxy(x, y); printf("▶");
			}
			else if (x == 86) {
				gotoxy(x, y); printf("▶");
			}
			break;
		case SUBMIT:
			if (x == 46 && y == 24) {
				gotoxy(1, 27); printf("                             ");
				if (room[cur][0] <= 0) {
					gotoxy(1, 27); printf("다야가 없어요ㅠ");
					break;
				}
				else {
					gotoxy(1, 27); printf("                             ");
					gotoxy(1, 27); printf("몇 개 훔쳐갈까? : ");
					scanf_s("%d", &steal);
					if (steal > room[cur][0]) {
						gotoxy(1, 27); printf("이 집에 그만큼 많지 않다..");
						break;
					}
					else {
						bag[0] += steal;
						weight -= itemweight[0] * steal;
						room[cur][0] -= steal;
						gotoxy(1, 27); printf("                   ");
						gotoxy(1, 27); printf("다이아 %d개를 훔쳤다!", steal);
						gotoxy(23, 10); printf("다이아몬드 %d개, 백금 %d개, 금 %d개, 은 %d개, 철 %d개가 있다!", room[cur][0], room[cur][1], room[cur][2], room[cur][3], room[cur][4]);
						break;
					}
				}
			}
			else if (x == 66 && y == 24) {
				if (room[cur][1] <= 0) {
					gotoxy(1, 27); printf("백금이 없어요ㅠ");
					break;
				}
				else {
					gotoxy(1, 27); printf("                             ");
					gotoxy(1, 27); printf("몇 개 훔쳐갈까? : ");
					scanf_s("%d", &steal);
					if (steal > room[cur][1]) {
						gotoxy(1, 27); printf("이 집에 그만큼 많지 않다..");
						break;
					}
					else {
						bag[1] += steal;
						weight -= itemweight[1] * steal;
						room[cur][1] -= steal;
						gotoxy(1, 27); printf("                   ");
						gotoxy(1, 27); printf("백금 %d개를 훔쳤다!", steal);
						gotoxy(23, 10); printf("다이아몬드 %d개, 백금 %d개, 금 %d개, 은 %d개, 철 %d개가 있다!", room[cur][0], room[cur][1], room[cur][2], room[cur][3], room[cur][4]);
						break;
					}
				}
			}
			else if (x == 86 && y == 24) {
				if (room[cur][2] <= 0) {
					gotoxy(1, 27); printf("금이 없어요ㅠㅠ");
					break;
				}
				else {
					gotoxy(1, 27); printf("                             ");
					gotoxy(1, 27); printf("몇 개 훔쳐갈까? : ");
					scanf_s("%d", &steal);
					if (steal > room[cur][2]) {
						gotoxy(1, 27); printf("이 집에 그만큼 많지 않다..");
						break;
					}
					else {
						bag[2] += steal;
						weight -= itemweight[2] * steal;
						room[cur][2] -= steal;
						gotoxy(1, 27); printf("                   ");
						gotoxy(1, 27); printf("금 %d개를 훔쳤다!", steal);
						gotoxy(23, 10); printf("다이아몬드 %d개, 백금 %d개, 금 %d개, 은 %d개, 철 %d개가 있다!", room[cur][0], room[cur][1], room[cur][2], room[cur][3], room[cur][4]);
						break;
					}
				}
			}
			else if (x == 46 && y == 27) {
				if (room[cur][3] <= 0) {
					gotoxy(1, 27); printf("은이 없어요ㅠㅠ");
					break;
				}
				else {
					gotoxy(1, 27); printf("                             ");
					gotoxy(1, 27); printf("몇 개 훔쳐갈까? : ");
					scanf_s("%d", &steal);
					if (steal > room[cur][3]) {
						gotoxy(1, 27); printf("이 집에 그만큼 많지 않다..");
						break;
					}
					else {
						bag[3] += steal;
						weight -= itemweight[3] * steal;
						room[cur][3] -= steal;
						gotoxy(1, 27); printf("                   ");
						gotoxy(1, 27); printf("은 %d개를 훔쳤다!", steal);
						gotoxy(23, 10); printf("다이아몬드 %d개, 백금 %d개, 금 %d개, 은 %d개, 철 %d개가 있다!", room[cur][0], room[cur][1], room[cur][2], room[cur][3], room[cur][4]);
						break;
					}
				}
			}
			else if (x == 66 && y == 27) {
				if (room[cur][4] <= 0) {
					gotoxy(1, 27); printf("철이 없어요ㅠㅠ");
					break;
				}
				else {
					gotoxy(1, 27); printf("                             ");
					gotoxy(1, 27); printf("몇 개 훔쳐갈까? : ");
					scanf_s("%d", &steal);
					if (steal > room[cur][4]) {
						gotoxy(1, 27); printf("이 집에 그만큼 많지 않다..");
						break;
					}
					else {
						bag[4] += steal;
						weight -= itemweight[4] * steal;
						room[cur][4] -= steal;
						gotoxy(1, 27); printf("                   ");
						gotoxy(1, 27); printf("철 %d개를 훔쳤다!", steal);
						gotoxy(23, 10); printf("다이아몬드 %d개, 백금 %d개, 금 %d개, 은 %d개, 철 %d개가 있다!", room[cur][0], room[cur][1], room[cur][2], room[cur][3], room[cur][4]);
						break;
					}
				}
			}
			else if (x == 86 && y == 27) {
				return;
			}
		}
		gotoxy(1, 24); printf("                          ");
		gotoxy(1, 24);
		if (x == 46 && y == 24) {
			printf("다이아몬드 무게 %d, 가치 %d", itemweight[0], itemvalue[0]);
		}
		else if (x == 66 && y == 24) {
			printf("백금 무게 %d, 가치 %d", itemweight[1], itemvalue[1]);
		}
		else if (x == 86 && y == 24) {
			printf("금 무게 %d, 가치 %d", itemweight[2], itemvalue[2]);
		}
		else if (x == 46 && y == 27) {
			printf("은 무게 %d, 가치 %d", itemweight[3], itemvalue[3]);
		}
		else if (x == 66 && y == 27) {
			printf("철 무게 %d, 가치 %d", itemweight[4], itemvalue[4]);
		}
		else if (x == 86 && y == 27) {
			printf("나가기 버튼");
		}
		if (weight < 0) bagBurst();
		gotoxy(1, 21); printf("                      ");
		gotoxy(1, 21); printf("남은 가방의 무게 : %d", weight);
	}
}

void endRoom() {
	for (int i = 0; i < 5; i++) {
		worth += bag[i] * itemvalue[i];
	}

	system("cls");
	gotoxy(2, 5); printf("벌어 온 가치 : %d\t『 %c 』", worth, setRankW()); Sleep(1000);
	gotoxy(2, 8); printf("다녀 온 거리 : %d\t『 %c 』", dist, setRankD()); Sleep(1000);

	if (endRank() != 1) {
		gotoxy(2, 21); printf("나는 ");
		for (int i = 0; i < minpIndex; i++) {
			if (i != minpIndex - 1) printf("%2d번-> ", minpath[i]);
			else printf("%2d번", minpath[i]);
		}
		printf("에 가서");
		gotoxy(2, 23); printf("시간을 %d만 쓸 수 있었고", mindist);
		gotoxy(2, 25); printf("%d 만큼 벌 수 있었다..ㅠ", maxworth);
	}
	else {
		gotoxy(2, 21); printf("나는 세계 최고의 도둑이다. ㄹㅇ");
	}
	gotoxy(1, 27);
	exit(0);
}

int endRank() {
	char wRank = setRankW(), dRank = setRankD();
	int wScore = 0, dScore = 0, score = 0;
	if (wRank == 'S') wScore = 50;
	else if (wRank == 'A') wScore = 40;
	else if (wRank == 'B') wScore = 30;
	else if (wRank == 'C') wScore = 20;
	else if (wRank == 'D') wScore = 10;
	else if (wRank == 'F') wScore = 0;
	if (dRank == 'S') dScore = 50;
	else if (dRank == 'A') dScore = 40;
	else if (dRank == 'B') dScore = 30;
	else if (dRank == 'C') dScore = 20;
	else if (dRank == 'D') dScore = 10;
	else if (dRank == 'F') dScore = 0;

	score = wScore + dScore;
	if (score == 100) {
		SScene();
		return 1;
	}
	else if (score >= 80) {
		AScene();
		return 2;
	}
	else if (score >= 60) {
		BScene();
		return 3;
	}
	else if (score >= 30) {
		CScene();
		return 4;
	}
	else if (score >= 10) {
		DScene();
		return 5;
	}
	else {
		FScene();
		return 6;
	}
}

void stuck() {
	system("cls");
	gotoxy(40, 10); printf("아이고 꼼짝없이 갇혔다!"); gotoxy(0, 29);
	exit(0);
}

void discovered() {
	system("cls");
	gotoxy(38, 10); printf("아이고 집주인에게 들켰다!"); gotoxy(0, 29);
	exit(0);
}

void bagBurst() {
	system("cls");
	gotoxy(40, 10); printf("아이고 가방이 터졌다!"); gotoxy(0, 29);
	exit(0);
}

void SScene() {
	gotoxy(60, 6);  printf("      *******");
	gotoxy(60, 7);  printf("    *       ***");
	gotoxy(60, 8);  printf("   *         **");
	gotoxy(60, 9);  printf("   **        *");
	gotoxy(60, 10); printf("    ***");
	gotoxy(60, 11); printf("   ** ***");
	gotoxy(60, 12); printf("    *** ***");
	gotoxy(60, 13); printf("      *** ***");
	gotoxy(60, 14); printf("        *** ***");
	gotoxy(60, 15); printf("          ** ***");
	gotoxy(60, 16); printf("           ** **");
	gotoxy(60, 17); printf("            * *");
	gotoxy(60, 18); printf("  ***        *");
	gotoxy(60, 19); printf(" *  *********");
	gotoxy(60, 20); printf("*     ***** ");
	gotoxy(60, 20); printf("*");
	gotoxy(60, 20); printf(" **");
}
void AScene() {
	gotoxy(60, 6);  printf("        **");
	gotoxy(60, 7);  printf("     *****");
	gotoxy(60, 8);  printf("    *  ***");
	gotoxy(60, 9);  printf("       ***");
	gotoxy(60, 10); printf("      *  **");
	gotoxy(60, 11); printf("      *  **");
	gotoxy(60, 12); printf("     *    ** ");
	gotoxy(60, 13); printf("     *    ** ");
	gotoxy(60, 14); printf("    *      **");
	gotoxy(60, 15); printf("    *********");
	gotoxy(60, 16); printf("   *        **");
	gotoxy(60, 17); printf("   *        **");
	gotoxy(60, 18); printf("  *****      **");
	gotoxy(60, 19); printf(" *   ****    ** *");
	gotoxy(60, 20); printf("*     **      **");
	gotoxy(60, 20); printf("*");
	gotoxy(60, 20); printf(" **");
}
void BScene() {
	gotoxy(60, 6);  printf("     ***** **");
	gotoxy(60, 7);  printf("  ******  ***");
	gotoxy(60, 8);  printf(" **   *  * **");
	gotoxy(60, 9);  printf("*    *  *  **");
	gotoxy(60, 10); printf("    *  *   *");
	gotoxy(60, 11); printf("   ** **  *");
	gotoxy(60, 12); printf("   ** ** * ");
	gotoxy(60, 13); printf("   ** ***");
	gotoxy(60, 14); printf("   ** ** ***");
	gotoxy(60, 15); printf("   ** **   ***");
	gotoxy(60, 16); printf("   *  **     **");
	gotoxy(60, 17); printf("      *      **");
	gotoxy(60, 18); printf("  ****     ***");
	gotoxy(60, 19); printf(" *  ********");
	gotoxy(60, 20); printf("*     ****");
	gotoxy(60, 20); printf("*");
	gotoxy(60, 20); printf(" **");
}
void CScene() {
	gotoxy(60, 6);  printf("      * ***");
	gotoxy(60, 7);  printf("    *  ****  *");
	gotoxy(60, 8);  printf("   *  *  ****");
	gotoxy(60, 9);  printf("  *  **   **");
	gotoxy(60, 10); printf(" *  ***");
	gotoxy(60, 11); printf("**   **");
	gotoxy(60, 12); printf("**   **");
	gotoxy(60, 13); printf("**   **");
	gotoxy(60, 14); printf("**   **");
	gotoxy(60, 15); printf("**   **");
	gotoxy(60, 16); printf(" **  **");
	gotoxy(60, 17); printf("  ** *      *");
	gotoxy(60, 18); printf("   ***     *");
	gotoxy(60, 19); printf("    ******");
	gotoxy(60, 20); printf("      ***");
}
void DScene() {
	gotoxy(60, 6);  printf("      ***** **");
	gotoxy(60, 7);  printf("   ******  ***");
	gotoxy(60, 8);  printf(" **    *  * ***");
	gotoxy(60, 9);  printf("*     *  *   ***");
	gotoxy(60, 10); printf("     *  *     ***");
	gotoxy(60, 11); printf("    ** **      **");
	gotoxy(60, 12); printf("    ** **      **");
	gotoxy(60, 13); printf("    ** **      **");
	gotoxy(60, 14); printf("    ** **      **");
	gotoxy(60, 15); printf("    ** **      **");
	gotoxy(60, 16); printf("    *  **      **");
	gotoxy(60, 17); printf("       *       *");
	gotoxy(60, 18); printf("  *****       *");
	gotoxy(60, 19); printf(" *   *********");
	gotoxy(60, 20); printf("*       ****");
	gotoxy(60, 20); printf("*");
	gotoxy(60, 20); printf(" **");
}
void FScene() {
	gotoxy(60, 6);  printf("     ***** **");
	gotoxy(60, 7);  printf("  ******  **** *");
	gotoxy(60, 8);  printf(" **   *  *  ***");
	gotoxy(60, 9);  printf("*    *  *    * ");
	gotoxy(60, 10); printf("    *  *");
	gotoxy(60, 11); printf("   ** **");
	gotoxy(60, 12); printf("   ** **");
	gotoxy(60, 13); printf("   ** ****** ");
	gotoxy(60, 14); printf("   ** *****");
	gotoxy(60, 15); printf("   ** **");
	gotoxy(60, 16); printf("   *  **");
	gotoxy(60, 17); printf("      *");
	gotoxy(60, 18); printf("  *****");
	gotoxy(60, 19); printf(" *  *****");
	gotoxy(60, 20); printf("*    ***");
	gotoxy(60, 20); printf("*");
	gotoxy(60, 20); printf(" **");
}
