#pragma once

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
