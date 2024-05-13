#pragma once

#include <time.h>
#include <cstdlib>
#include <easyx.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#define BOOM -1
#define REDBOOM -2
#define HIDE 10
#define FLAG 11
#define GridSize 39

class Sence
{
public:
	Sence() = default;
	void Init(int _row, int _col, int _boomNum);
	void PlayGame();
	void test();	// 在控制台输出每个格子的数字
	// void EndGame();

private:
	void caculate();
	void InitImage();
	void displayText();
	// void removeText();
	void displayTime();
	void InitBoard();
	void setImage(int x, int y, int type);
	bool findPosition_and_setImage_and_checkBoom(ExMessage msg);
	void findPosition_and_setFlag(ExMessage msg);
	int getLeftBoomNum();	// 根据插旗数来计算剩余雷数，不是真正剩余的雷数
	bool checkBoom(int type);
	void openSuround(int curRow, int curCol);
	void LoseGame(int curRow, int curCol);	// 需要将最后一次点击处变为红色的雷，故需要传入当前点击位置
	void WinGame();
	bool Check_if_win();

private:
	int** board;
	bool** HideBoard;	// 用于存储是否已经打开，避免重复打开
	bool** FlagBoard;	// 用于存储是否已经插旗，避免重复插旗
	int row;
	int col;
	int boomNum; //炸弹数量
	// int boomLeftNum;	//剩余炸弹数量
};

