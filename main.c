#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include "thief.h"

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
