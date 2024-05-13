#include "Sence.h"

void Sence::Init(int _row, int _col, int _boomNum)
{
	srand(time(0));

	row = _row;
	col = _col;
	boomNum = _boomNum >= row * col ? row * col - 1 : _boomNum;

	board = new int* [row];
	for (int i = 0; i < row; i++) {
		board[i] = new int[col];
	}

	HideBoard = new bool* [row];
	for (int i = 0; i < row; i++) {
		HideBoard[i] = new bool[col];
	}
	
	FlagBoard = new bool* [row];
	for (int i = 0; i < row; i++) {
		FlagBoard[i] = new bool[col];
	}

	int i = boomNum;
	while (i) {	//随机生成 boomNum 个炸弹
		int r = rand() % row;
		int l = rand() % col;
		if (board[r][l] == BOOM)
			continue;
		board[r][l] = BOOM; //BOOM 为 -1
		--i;
	}

	caculate();	//生成各个格子数字

	InitImage();	//生成图形
	InitBoard();	//初始化HideBoard和FlagBoard
	displayText();
}

void Sence::PlayGame()
{
	ExMessage msg;
	while (1) {
		displayTime();

		if (peekmessage(&msg, EX_MOUSE)) {	// 得到消息就处理，没得到消息就一直循环而不是阻塞
			if (msg.message == WM_LBUTTONDOWN) {
				if (findPosition_and_setImage_and_checkBoom(msg))
					return;
				// test();
				// displayText();
			}

			else if (msg.message == WM_RBUTTONDOWN) {
				findPosition_and_setFlag(msg);
				displayText();
			}


			if (Check_if_win()) {
				WinGame();
				return;
			}
		}
	}
}

void Sence::test()
{
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			cout << board[i][j] << '\t';
		}
		cout << endl;
	}
	cout << endl;
}

void Sence::caculate()
{
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (board[i][j] == BOOM)	continue;	//跳过炸弹格子

			// 对每一个点，确认周围八个格子是否有炸弹
			int boomNum_point = 0;
			
			//八个方向
			for (int x = -1; x <= 1; x++) {
				for (int y = -1; y <= 1; y++) {
					// 自己那个点计算了也没关系，不可能是炸弹
					// board[i + y][j + x]
					if (i + y < 0 || i + y >= row || j + x < 0 || j + x >= col)
						continue;	// 超出边界就跳过

					if (board[i + y][j + x] == BOOM) {
						boomNum_point++;

					}
				}
			}
			board[i][j] = boomNum_point;
		}
	}
}

void Sence::InitImage()
{
	IMAGE* m = new IMAGE;
	loadimage(m, "res/image/10.png");
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			putimage(j * GridSize, i * GridSize, m);
		}
	}
	delete m;
}

void Sence::displayText()
{
	static char str[25] = { 0 };	//用static是为了存储上一次输出的文字
	// char str[25] = { 0 };

	settextcolor(getbkcolor());	//将文字设置为背景色，再输出一遍，即可消除上一次的文字！
	outtextxy(col * GridSize - 130, row * GridSize + 10, str);

	settextcolor(BLACK);
	sprintf_s(str, sizeof(str), "还剩 %d 颗雷", getLeftBoomNum());
	outtextxy(col * GridSize - 130, row * GridSize + 10, str);
}

void Sence::displayTime()
{
	static time_t beginTime = clock();
	time_t curTime = clock();

	static char str[25] = { 0 };

	// settextcolor(getbkcolor());	//将文字设置为背景色，再输出一遍，即可消除上一次的文字！
	// outtextxy(30, row * GridSize + 10, str);

	settextcolor(BLACK);
	sprintf_s(str, sizeof(str), "用时：%d", (int)(curTime - beginTime) / 1000);
	outtextxy(30, row * GridSize + 10, str);
}

void Sence::InitBoard()
{
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			HideBoard[i][j] = true;
			FlagBoard[i][j] = false;
		}
	}
}

void Sence::setImage(int x, int y, int type)
{
	IMAGE m;
	char filename[100] = { 0 };
	sprintf_s(filename, "res/image/%d.png", type);
	loadimage(&m, filename);
	putimage(x, y, &m);

	int curRow = y / GridSize;
	int curCol = x / GridSize;

	if (type != HIDE && type != FLAG) {
		HideBoard[curRow][curCol] = false;
		FlagBoard[curRow][curCol] = false;
	}
	if (type == HIDE) {
		HideBoard[curRow][curCol] = true;
		FlagBoard[curRow][curCol] = false;
	}
	if (type == FLAG) {
		HideBoard[curRow][curCol] = true;
		FlagBoard[curRow][curCol] = true;
	}

	// static int i = 0;
	// cout << i++ << endl;

}

bool Sence::findPosition_and_setImage_and_checkBoom(ExMessage msg)
{
	for (int curRow = 0; curRow < row; curRow++) {
		for (int curCol = 0; curCol < col; curCol++) {
			int x = curCol * GridSize;
			int y = curRow * GridSize;
			if (msg.x > x && msg.x < x + GridSize && msg.y > y && msg.y < y + GridSize && FlagBoard[curRow][curCol] == false) {
				int type = board[curRow][curCol];
				setImage(x, y, type);
				if (type == 0) {
					openSuround(curRow, curCol);
				}
				if (checkBoom(type)) {
					LoseGame(curRow, curCol);
					return true;
				}
				return false;
			}
		}
	}
	return false;
}

void Sence::findPosition_and_setFlag(ExMessage msg)
{
	for (int curRow = 0; curRow < row; curRow++) {
		for (int curCol = 0; curCol < col; curCol++) {
			int x = curCol * GridSize;
			int y = curRow * GridSize;
			if (msg.x > x && msg.x < x + GridSize && msg.y > y && msg.y < y + GridSize && HideBoard[curRow][curCol] == true) {
				if (FlagBoard[curRow][curCol] == false) {
					setImage(x, y, FLAG);
				}
				else {
					setImage(x, y, HIDE);
				}
				return;
			}
		}
	}
}

int Sence::getLeftBoomNum()
{
	int flagNum = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (FlagBoard[i][j] == true)
				flagNum++;
		}
	}
	return boomNum - flagNum >= 0 ? boomNum - flagNum : 0;
}

bool Sence::checkBoom(int type)
{
	if (type != BOOM)
		return false;

	return true;
}

void Sence::openSuround(int curRow, int curCol)
{
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (x == 0 && y == 0)	continue;	// 自己这个点跳过
			if (curRow + y < 0 || curRow + y >= row || curCol + x < 0 || curCol + x >= col)
						continue;	// 超出边界就跳过
			if (board[curRow + y][curCol + x] == BOOM)	continue;	//其实当有一片空白的时候，不可能先遇到炸弹，一定是先遇到数字。但这样保险一点
			if (HideBoard[curRow + y][curCol + x] == false)	continue;	// 如果已经被打开了，就不再打开
			if (FlagBoard[curRow + y][curCol + x] == true)	continue;	// 如果已经插旗了，就不再打开

			int type = board[curRow + y][curCol + x];
			setImage((curCol + x) * GridSize, (curRow + y) * GridSize, type);

			if (type == 0) {
				openSuround(curRow + y, curCol + x);
			}
		}
	}
}

void Sence::LoseGame(int curRow, int curCol)
{
	// 全显示版本
	// for (int i = 0; i < row; i++) {
	//	for (int j = 0; j < col; j++) {
	//		setImage(j * GridSize, i * GridSize, board[i][j]);
	//	}
	// }

	// 只显示雷版本
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (board[i][j] == BOOM)
				setImage(j * GridSize, i * GridSize, BOOM);
		}
	}

	setImage(curCol * GridSize, curRow * GridSize, REDBOOM);

	MessageBox(NULL, "你踩到雷了！", "游戏结束", MB_OKCANCEL);
}

void Sence::WinGame()
{
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (board[i][j] == BOOM)
				setImage(j * GridSize, i * GridSize, FLAG);
		}
	}

	MessageBox(NULL, "你成功找到了所有的雷！", "游戏结束", MB_OKCANCEL);
}

bool Sence::Check_if_win()
{
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (board[i][j] != BOOM && HideBoard[i][j] == true)
				return false;
		}
	}
	return true;
}


