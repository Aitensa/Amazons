//***************copyright by Jackson Ai****************//
//***************This program is just for study, please don't use for other goals without permission*******//
#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<algorithm>
#include<cmath>
#include<conio.h>
#include<windows.h>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")
#include <graphics.h>
#include<easyx.h>
#include<assert.h>
//调用的参数，作为权值，评估当前局面好坏
double f1[23] = { 0.1080, 0.1080, 0.1235, 0.1332, 0.1400,
				0.1468, 0.1565, 0.1720, 0.1949, 0.2217,
				0.2476, 0.2680, 0.2800, 0.2884, 0.3000,
				0.3208, 0.3535, 0.4000, 0.4613, 0.5350,
				0.6181, 0.7075, 0.8000 };
double f2[23] = { 0.3940, 0.3940, 0.3826, 0.3753, 0.3700,
				0.3647, 0.3574, 0.3460, 0.3294, 0.3098,
				0.2903, 0.2740, 0.2631, 0.2559, 0.2500,
				0.2430, 0.2334, 0.2200, 0.2020, 0.1800,
				0.1550, 0.1280, 0.1000 };
double f3[23] = { 0.1160, 0.1160, 0.1224, 0.1267, 0.1300,
				0.1333, 0.1376, 0.1440, 0.1531, 0.1640,
				0.1754, 0.1860, 0.1944, 0.1995, 0.2000,
				0.1950, 0.1849, 0.1700, 0.1510, 0.1287,
				0.1038, 0.0773, 0.0500 };
double f4[23] = { 0.1160, 0.1160, 0.1224, 0.1267, 0.1300,
				0.1333, 0.1376, 0.1440, 0.1531, 0.1640,
				0.1754, 0.1860, 0.1944, 0.1995, 0.2000,
				0.1950, 0.1849, 0.1700, 0.1510, 0.1287,
				0.1038, 0.0773, 0.0500 };
double f5[23] = { 0.2300, 0.2300, 0.2159, 0.2067, 0.2000,
				0.1933, 0.1841, 0.1700, 0.1496, 0.1254,
				0.1010, 0.0800, 0.0652, 0.0557, 0.0500,
				0.0464, 0.0436, 0.0400, 0.0346, 0.0274,
				0.0190, 0.0097, 0.0000 };
#define GRID_ITEM_SIZE 50
#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1
#define RADIUS 15
#define MAX  50000.0
#define FILE_NAME "registe.txt"
using namespace std;
int turnID;
int x[4], y[4],playercolor,reset[8][8];
int nexttype;
struct tem {
	int num, x, y;
}play[3000],playerpos;
struct chess {
	double queen, king, queenevaluate, kingevaluate;
}white[8][8], black[8][8];
int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };//8个方向，方便遍历
int tempx1, tempy1, tempx2, tempy2, tempx3, tempy3;
int mx[4], my[4], ox[4], oy[4];
//******预设函数声明*********//
void SetNextType(int color);
int OutIntro();
bool IsHighLight(int x, int y);//判断是否可以选中
bool HasSetHightLight(int* x, int* y);//是否已经选中
bool IsValid(int x_1, int y_1, int x_2, int y_2);//判定所实现的操作是否合法
bool PreMove(int x,int y,int xx,int yy,int type);
bool IsEnd(int type);//判断是否结束
void UnSetHightLight(int x, int y);//恢复未标记设置
void SetHightLight(int x, int y);//将棋子标红
void input(int i);
void Initgame();
void ReUi();
void showroute(int x, int y);
void Searchchess();
void BotMove();
void Reset();
void Rem();
int Loadchess();
int Savechess();
int Settype(int x,int y,int type);
int Quit();
int RunChess();
int showMenu();
int NewGame();
int MoveTo(int x, int y, int xx, int yy, int type);
int Save(int board[8][8], int next);
int Read(int(*board)[8][8], int* next);
// 判断是否在地图内
inline bool inMap(int x, int y)
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
}
inline double maxnum(double a, double b) {
	if (a < b)return b;
	else return a;
}
inline double minnum(double a, double b) {
	if (a < b)return a;
	else return b;
}
//由于估值过程中对棋盘上棋子的判定原有判定已经不足
bool recheck(int i, int j) {
	if (i >= 0 && i < GRIDSIZE && j >= 0 && j < GRIDSIZE && gridInfo[i][j] != 2 && gridInfo[i][j] != 1 && gridInfo[i][j] != -1)return true;
	else return false;
}
// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color, bool check_only)
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0)
		return false;//取用棋子非相应方棋色或落点已被其他棋子占据
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0))
		return false;//目标位置被占据但是启用棋子的点也可落子
	if (!check_only)
	{
		gridInfo[x0][y0] = 0;
		gridInfo[x1][y1] = color;
		gridInfo[x2][y2] = OBSTACLE;
	}//完成落子
	return true;
}
double t1, t2;//记录queen、king的territory的特征值,正值为白方，负值为黑方，以下函数申明均由1指代白方，2指代黑方

			  
			  //***********估值部分***********//
bool loadcheck();
void territory() //计算双方领地
{
	memset(white, 0, sizeof(white));
	memset(black, 0, sizeof(black));
	double maxn = 5000;
	int r = 1, w = 1;//实现类似队列的更新与读取
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (gridInfo[i][j] == -1)white[i][j].queen = white[i][j].king = 1, black[i][j].queen = black[i][j].king = maxn;
			else if (gridInfo[i][j] == 1)black[i][j].queen = black[i][j].king = 1, white[i][j].queen = white[i][j].king = maxn;
			else if (gridInfo[i][j] == 2)black[i][j].king = black[i][j].queen = white[i][j].queen = white[i][j].king = maxn;
		}
	}//划定不可到达的点，棋子所在点，以便后续估值
	//首先对黑方territory进行评估
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (gridInfo[i][j] == grid_black)play[w].num = 1, play[w].x = i, play[w].y = j, w++;
		}
	}//记录黑棋位置
	while (r < w) //此处实现查找
	{
		int temi, temj, times;//temi，temj分别用于描述试探出发的点，times用于表示从出发点到当前的位置需要多少步
		temi = play[r].x;
		temj = play[r].y;
		times = play[r].num + 1;
		//由此开始对8个方向的走法开始探索
		for (int k = 0; k < 8; k++) {
			for (int delta = 1; delta < 8; delta++) {
				if (recheck(temi + dx[k] * delta, temj + dy[k] * delta))//确定是否是合法走步及是否可以到达
				{
					//之前未到达该点
					if (black[temi + dx[k] * delta][temj + dy[k] * delta].queen== 0) {
						play[w].x = temi + dx[k] * delta;
						play[w].y = temj + dy[k] * delta;
						play[w].num = times;
						black[play[w].x][play[w].y].queen = times;
						w++;
					}
					else//之前已经到达过该点
					{
						black[temi + dx[k] * delta][temj + dy[k] * delta].queen =min(times, black[temi + dx[k] * delta][temj + dy[k] * delta].queen);
					}//维护到达同一位置需要的最小步数
				}
				else break;
			}
		}
		r++;
	}
	r = 1, w = 5;
	//查king
	while (r < w) {
		int temi, temj, times;
		temi = play[r].x;
		temj = play[r].y;
		times = play[r].num + 1;
		//由此开始对8个方向的走法开始探索
		for (int k = 0; k < 8; k++) {

			if (recheck(temi + dx[k], temj + dy[k])) {
				//此处判定确保该点可到达
				if (black[temi + dx[k]][temj + dy[k]].king== 0) {
					play[w].x = temi + dx[k];
					play[w].y = temj + dy[k];
					play[w].num = times;
					black[play[w].x][play[w].y].king = times;
					w++;
				}
				else {
					black[temi + dx[k]][temj + dy[k]].king = minnum(times, black[temi + dx[k]][temj + dy[k]].king);
				}
			}
			else continue;
		}
		r++;
	}
	r = 1, w = 1;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (gridInfo[i][j] == -1)play[w].num = 1, play[w].x = i, play[w].y = j,w++;
		}
	}
	while (r < w) {
		int temi, temj, times;
		temi = play[r].x;
		temj = play[r].y;
		times = play[r].num + 1;
		//由此开始对8个方向的走法开始探索
		for (int k = 0; k < 8; k++) {
			for (int delta = 1; delta < 8; delta++) {
				if (recheck(temi + dx[k] * delta, temj + dy[k] * delta)) {
					//此处判定确保该点可到达
					if (white[temi + dx[k] * delta][temj + dy[k] * delta].queen==0) {
						play[w].x = temi + dx[k] * delta;
						play[w].y = temj + dy[k] * delta;
						play[w].num = times;
						white[play[w].x][play[w].y].queen = times;
						w++;
					}
					else {
						white[temi + dx[k] * delta][temj + dy[k] * delta].queen =min(times, white[temi + dx[k] * delta][temj + dy[k] * delta].queen);
					}
				}
				else break;
			}
		}
		r++;
	}
	r = 1, w = 5;
	while (r < w) {
		int temi, temj, times;
		temi = play[r].x;
		temj = play[r].y;
		times = play[r].num + 1;
		//由此开始对8个方向的走法开始探索
		for (int k = 0; k < 8; k++) {

			if (recheck(temi + dx[k], temj + dy[k])) {
				//此处判定确保该点可到达
				if (white[temi + dx[k]][temj + dy[k]].king== 0) {
					play[w].x = temi + dx[k];
					play[w].y = temj + dy[k];
					play[w].num = times;
					white[play[w].x][play[w].y].king = times;
					w++;
				}
				else {
					white[temi + dx[k]][temj + dy[k]].king = minnum(times, white[temi + dx[k]][temj + dy[k]].king);
				}
			}
			else continue;
		}
		r++;
	}



	//以下进行局势的判定
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (black[i][j].queen== 0)black[i][j].queen = maxn;
			if (black[i][j].king==0)black[i][j].king = maxn;
			if (white[i][j].queen== 0)white[i][j].queen = maxn;
			if (white[i][j].king== 0)white[i][j].king = maxn;
		}
	}//对于双方均无法到达的地方标记为无穷大
	double k = 0.3;//先行可有开局优势
	if (currBotColor == grid_black)k = -0.3;

	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (gridInfo[i][j] == 0) {
				if (black[i][j].queen == white[i][j].queen) {
					if (black[i][j].queen == maxn)t1 += 0;
					else t1 += k;
				}
				else if (black[i][j].queen > white[i][j].queen)t1 += 1.0;
				else if (black[i][j].queen < white[i][j].queen)t1 -= 1.0;
			}
		}
	}
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (gridInfo[i][j] == 0) {
				if (black[i][j].king == white[i][j].king) {
					if (black[i][j].king == maxn)t2 += 0;
					else t2 += k;
				}
				else if (black[i][j].king > white[i][j].king)t2 += 1.0;
				else if (black[i][j].king < white[i][j].king)t2 -= 1.0;
			}
		}
	}
}
double p1, p2;
void position() {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (gridInfo[i][j] == 0) {
				p1 += 2 * (pow(2, -white[i][j].queen) - pow(2, -black[i][j].queen));
				p2 += minnum(1, maxnum(-1, (black[i][j].king - white[i][j].king) / 6.0));
			}
		}
	}
	//统计queen，king行走下双方对单个空格的控制权的大小
}
double mob;//记录灵活度，正值为白，负值为黑
void mobility() //灵活度计算
{
	double mw = 0.0, mb = 0.0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (gridInfo[i][j] == 1) {
				for (int k = 0; k < 8; k++) {
					for (int delta = 1; delta <= 8; delta++) {
						if (recheck(i + dx[k] * delta, j + dy[k] * delta)) {
							int x = i + dx[k] * delta, y = j + dy[k] * delta;
							mb += black[x][y].king / black[x][y].queen;//判断黑方queen和king走法谁更优
						}
						else break;
					}
				}
			}
			if (gridInfo[i][j] == -1) {
				for (int k = 0; k < 8; k++) {
					for (int delta = 1; delta <= 8; delta++) {
						if (recheck(i + dx[k] * delta, j + dy[k] * delta)) {
							int x = i + dx[k] * delta, y = j + dy[k] * delta;
							mw += white[x][y].king / white[x][y].queen;//判断白方queen和king走法谁更优
						}
						else break;
					}
				}
			}
		}
	}

	mob = mw - mb;
}
double evaluate() {
	//初始化
	t1 = t2 = 0.0;
	mob = 0.0;
	p1 = p2 = 0.0;
	territory();
	position();
	mobility();
	double value = 0.0;
	if (turnID < 23) {
		value = f1[turnID] * t1 + f2[turnID] * t2 + f3[turnID] * p1 + f4[turnID] * p2 + f5[turnID] * mob;
	}
	else value = 0.80 * t1 + 0.10 * t2 + 0.05 * p1 + 0.05 * p2;
	if (currBotColor == grid_white) return value;//白方时返回正值
	else return -value;
}


//************算法实现部分——α—β剪枝算法**************//
double searchstep(int nowstep, int nowcolor, int goalstep, double alpha, double beta)
{
	if (nowstep == goalstep)//到达预设搜索深度，返回
	{
		double c;
		c = evaluate();
		return c;
	}

	//存储可能走法
	int beginPos[3000][2], possiblePos[3000][2], obstaclePos[3000][2];
	int posCount = 0;

	for (int i = 0; i < GRIDSIZE; ++i)
	{
		for (int j = 0; j < GRIDSIZE; ++j)
		{

			if (gridInfo[i][j] != nowcolor) continue;

			for (int k = 0; k < 8; ++k)
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
				{
					int xx = i + dx[k] * delta1;
					int yy = j + dy[k] * delta1;//选择要下的位置

					if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))break;

					for (int l = 0; l < 8; ++l) {
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++) {
							int xxx = xx + dx[l] * delta2;
							int yyy = yy + dy[l] * delta2;//预设障碍
							if (!inMap(xxx, yyy))
								break;
							if (gridInfo[xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (ProcStep(i, j, xx, yy, xxx, yyy, nowcolor, true))
							{
								posCount++;
								beginPos[posCount][0] = i;
								beginPos[posCount][1] = j;
								possiblePos[posCount][0] = xx;
								possiblePos[posCount][1] = yy;
								obstaclePos[posCount][0] = xxx;
								obstaclePos[posCount][1] = yyy;
							}
						}

					}
				}

			}
		}
	}

	if (posCount == 0)
	{
		if (nowcolor == currBotColor) return -MAX;
		if (nowcolor == -currBotColor) return MAX;
	}
	//无路可走，返回无穷值表示无可行方案，极大极小算法核心
	else
	{
		double val[3000] = { 0 };
		int posc = posCount;
		int pick[3000] = { 0 };
		for (int i = 1; i <= posc; i++) pick[i] = i;
		random_shuffle(pick + 1, pick + posc + 1);//随机化，提高数据使用效率,进一步提高搜索能力
		//**************由于领地为空的性质，对于领地的状态可直接由nowcolor运算更新**************//
		if (nowcolor == currBotColor)//我行动
		{
			int maxplan;
			for (int i = 1; i <= posc; i++)
			{
				gridInfo[beginPos[pick[i]][0]][beginPos[pick[i]][1]] -= nowcolor;//回复领地空的状态
				gridInfo[possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] += nowcolor;//到达点标记
				gridInfo[obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] += OBSTACLE;//障碍设置
				//搜索预设

				val[pick[i]] = searchstep(nowstep + 1, -nowcolor, goalstep, alpha, beta);
				if (val[pick[i]] > alpha)//更新最大值
				{
					alpha = val[pick[i]];
					if (nowstep == 1) maxplan = pick[i];//最优放置点
				}
				gridInfo[beginPos[pick[i]][0]][beginPos[pick[i]][1]] += nowcolor;
				gridInfo[possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] -= nowcolor;
				gridInfo[obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] -= OBSTACLE;
				//回溯
				if (beta < alpha) break;//剪枝
			}
			if (nowstep == 1)//记录搜索所得最优解
			{
				tempx1 = beginPos[maxplan][0];
				tempy1 = beginPos[maxplan][1];
				tempx2 = possiblePos[maxplan][0];
				tempy2 = possiblePos[maxplan][1];
				tempx3 = obstaclePos[maxplan][0];
				tempy3 = obstaclePos[maxplan][1];

			}
			return alpha;
		}

		else//假设对手行动，搜索其能达成的最差局面
		{
			for (int i = 1; i <= posc; i++)
			{
				gridInfo[beginPos[pick[i]][0]][beginPos[pick[i]][1]] -= nowcolor;
				gridInfo[possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] += nowcolor;
				gridInfo[obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] += OBSTACLE;


				val[pick[i]] = searchstep(nowstep + 1, -nowcolor, goalstep, alpha, beta);
				if (val[pick[i]] < beta)
				{
					beta = val[pick[i]];//更新使我方最小的解

				}
				gridInfo[beginPos[pick[i]][0]][beginPos[pick[i]][1]] += nowcolor;
				gridInfo[possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] -= nowcolor;
				gridInfo[obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] -= OBSTACLE;
				//回溯
				if (beta < alpha) break;//达到剪枝条件
			}
			return beta;
		}
	}
}//alpha beta剪枝算法

 
 //**************Amazons棋Bot接入口*********************//
void BotMove() //Bot部分
{
	double alpha = -MAX - 1, beta = MAX + 1;
	double goal;
	goal = searchstep(1, currBotColor, 3, alpha, beta);
	
	gridInfo[tempx1][tempy1] = 0;
	gridInfo[tempx2][tempy2] = currBotColor;
	gridInfo[tempx3][tempy3] = OBSTACLE;
	
	ReUi();
	
}


//***********************UI部分及主程序***********************//
void Reset()//******悔棋实现*****//
{
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			gridInfo[i][j] = reset[i][j];
		}
	}
	return;
}
void Rem() //记录上一步信息
{
	memset(reset, 0, sizeof(reset));
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j) {
			reset[i][j] = gridInfo[i][j];
		}
	}
	return;
}
void Initgame()//初始化游戏
{
	nexttype = grid_black;
	//*****初始化******//
	turnID = 1;
	memset(gridInfo, 0, sizeof(gridInfo));
	//***** 初始化棋盘*******//
	gridInfo[0][(GRIDSIZE - 1) / 3] = gridInfo[(GRIDSIZE - 1) / 3][0]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][0]
		= gridInfo[GRIDSIZE - 1][(GRIDSIZE - 1) / 3] = -playercolor;
	gridInfo[0][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = gridInfo[(GRIDSIZE - 1) / 3][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] =playercolor;
	//初始化设定黑子和白子的起始位置
	Rem();
}
void SetNextType(int color) //更新鼠标信息的指向
{
	nexttype = color;
}
bool IsHighLight(int x, int y) //用于实现撤销点击
{
	if (playerpos.x == x && playerpos.y == y)return true;
	else return false;
}
void UnSetHightLight(int x, int y)//去掉棋子的选中状态
{
	playerpos.x = -1;
	playerpos.y = -1;
}
void SetHightLight(int x, int y)//使选中棋子被提示
{
	playerpos.x = x;
	playerpos.y = y;
}
bool HasSetHightLight(int* x, int* y)//表示是否为选中棋子
{
	if (playerpos.x != -1 && playerpos.y != -1) {
		*x = playerpos.x;
		*y = playerpos.y;
		return true;
	}
	return false;
}
bool PreMove(int x, int y, int xx, int yy, int type)//预试是否可以到达目标点
{
	if (!IsValid(x, y, xx, yy))return false;
	return true;
}
bool IsEnd(int type)//返回局面信息，是否已经结束
{
	if (type == playercolor) {
		for (int i = 0; i < 4; i++) {
			for (int k = 0; k < 8; k++) {
				int xx = mx[i] + dx[k];
				int yy = my[i] + dy[k];
				if (recheck(xx, yy)) {
					if (gridInfo[xx][yy] == 0) {
						return false;
					}
				}
				else continue;
			}
		}
		return true;
	}
	else {
		for (int i = 0; i < 4; i++) {
			for (int k = 0; k < 8; k++) {
				int xx = ox[i] + dx[k];
				int yy = oy[i] + dy[k];
				if (recheck(xx, yy)) {
					if (gridInfo[xx][yy] == 0) {
						return false;
					}
				}
				else continue;
			}
		}
		return true;
	}
}
int MoveTo(int x, int y, int xx, int yy, int type) {
	if (!IsValid(x, y, xx, yy)) {
		return -1;
	}
	int t = -1;
	if (type == grid_white || type == grid_black) {
		t = Settype(xx, yy, type);
		t = Settype(x, y, 0);
	}
	else if (type == OBSTACLE) {
		t = Settype(xx, yy, type);
	}
	else {
		MessageBox(NULL, "WRONG!!!", "WARNING", MB_OK);
	}
	return 1;
}
bool IsValid(int x_1, int y_1, int x_2, int y_2) //*****判断路径是否合法*******//
{
	if (x_1 == x_2) //****横向走步*****//
	{
		if (y_1 == y_2)return 0;
		if (y_2 < y_1) {
			for (y_1--; y_1 > y_2; y_1--) {
				if (gridInfo[x_1][y_1] != 0)return false;
			}
		}//依据棋盘坐标原点位于左上角
		if (y_2 > y_1) {
			for (y_1++; y_2 > y_1; y_1++) {
				if (gridInfo[x_1][y_1] != 0)return false;
			}
		}
	}
	else if (y_1 == y_2) //*******纵向走步*******//
	{
		if (x_2 < x_1) {
			for (x_1--; x_2 < x_1; x_1--) {
				if (gridInfo[x_1][y_1] != 0)return false;
			}
		}
		else {
			for (x_1++; x_1 < x_2; x_1++) {
				if (gridInfo[x_1][y_1] != 0)return false;
			}
		}
	}
	else {
		if (abs(x_1 - x_2) != abs(y_1 - y_2))return false;//对于未在对角线上的棋子
		else 
		{
			if (x_1 < x_2) {
				if (y_1 < y_2) {
					for (x_1++, y_1++; x_1 < x_2; x_1++, y_1++) {
						if (gridInfo[x_1][y_1] != 0)return false;
					}
				}
				else {
					for (x_1++, y_1--; x_1<x_2; x_1++, y_1--) {
						if (gridInfo[x_1][y_1] != 0)return false;
					}
				}
			}
			else {
				if (y_1 < y_2) {
					for (x_1--, y_1++; x_1 > x_2; x_1--, y_1++) {
						if (gridInfo[x_1][y_1] != 0)return false;
					}
				}
				else {
					for (x_1--, y_1--; x_1<x_2; x_1--, y_1--) {
						if (gridInfo[x_1][y_1] != 0)return false;
					}
				}
			}
		}
	}
	return true;
}//检测预定走法是否可行
bool loadcheck() //检测读入裆是否存在问题
{
	int white=0, black=0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (gridInfo[i][j] == grid_white)white++;
			else if (gridInfo[i][j] == grid_black)black++;
		}
	}
	if (white != black||(white==0&&black==0))return true;
	else return false;
}
int Settype(int x, int y, int type) //****实现落子******//
{
	if (!inMap(x, y))return -1;
	gridInfo[x][y] = type;
	return 1;
}
int RunChess()	 {
	//*******棋盘及相应棋子信息******//
	//初始化信息
	initgraph(640, 480);

	ReUi();

	outtextxy(480, 0, _T("Welcome!!!Let's start!!!"));
	int end=0;//对当前局势进行判定，基础局势判定
	if (playercolor == grid_white){
		BotMove();
		SetNextType(playercolor);
	}
	while (true) {
		if (MouseHit()) {
			MOUSEMSG m = GetMouseMsg();
			if (m.mkLButton) {
				int i = m.x / GRID_ITEM_SIZE;
				int j = m.y / GRID_ITEM_SIZE;//依据int型除法的特性可通过此获得棋盘化的坐标值
				if (m.x >= 550 && m.x <= 640 && m.y >= 440 && m.y <= 480) //悔棋实现
				{
					Reset();
					ReUi();
					continue;
				}
				if (!inMap(i, j)&&(nexttype==playercolor)) {
					MessageBox(NULL, "Invalid operation,try again!", "Warning", MB_OK);
					continue;
				}
				if (gridInfo[i][j] == playercolor && nexttype == playercolor) {
					if (IsHighLight(i, j)) {
						UnSetHightLight(i, j);
					}
					else {
						SetHightLight(i, j);
					}
				}
				else if (gridInfo[i][j] == 0 && (nexttype == playercolor || nexttype == OBSTACLE)) {
					int x, y;
					if (HasSetHightLight(&x, &y)) {
						if (nexttype == playercolor) {
							if (PreMove(x, y, i, j, playercolor)) {
								Rem();//存下上一步的形势
								MoveTo(x, y, i, j, playercolor);
								SetHightLight(i, j);
								SetNextType(OBSTACLE);
							}
						}
						else if (nexttype == OBSTACLE) {
							if (PreMove(x, y, i, j, OBSTACLE)) {
								MoveTo(x, y, i, j, OBSTACLE);
								UnSetHightLight(x, y);
								SetNextType(-playercolor);
								Searchchess();
								if (IsEnd(-playercolor)) {
									end = -playercolor;
									goto END;
								}//机器输
								turnID++;
								BotMove();
								if (IsEnd(playercolor)) {
									end = playercolor;
									goto END;
								}//玩家输
								SetNextType(playercolor);
								turnID++;
							}
						}
					}
				}
				ReUi();//更新操作
			}
			else if (m.mkRButton)//右键点击回到起始界面//
			{
				end = showMenu();
				
					return end;
			
			}
		
		}
		
	}
END:

	settextstyle(30, 0, _T("Times New Roman"));
	if (end == playercolor) {
		MessageBox(NULL,"You    Lose   !  !  !","Attention!!!",MB_OK);
		return -1;
	}
	else if (end == currBotColor) {
		MessageBox(NULL,"You    Win    !  !  !","Congratuation!!!",MB_OK);
		return -1;
	}
	else {
		
	}
	_getch();
	closegraph();
	return end;
}
void showroute(int x, int y)//高亮显示可行路径
{
	setfillcolor(RGB(255,240,245));
	//深度优先找到不合适的就退出
	for (int i = 0; i < 8; ++i) {
		 for (int delta = 1; delta <= 8; ++delta){
			int xx = x + dx[i] * delta;
			int yy = y + dy[i] * delta;
			if (!recheck(xx, yy))break;
			else {
				solidrectangle(xx * 50 +1, yy * 50+1 , xx * 50 + 49, yy * 50 + 49);
				setlinecolor(YELLOW);
				rectangle(xx * 50, yy * 50, xx * 50 + 50, yy * 50 + 50);
			}
		}
	}
}
void ReUi()//游戏界面显示
{
	IMAGE img,c,obstacle,white,black,selectb,selectw;
	loadimage(&c, "C:\\Users\\Lenovo\\Desktop\\Amazons\\select.jpg");
	loadimage(&white, "C:\\Users\\Lenovo\\Desktop\\Amazons\\WHITE.jpg");
	loadimage(&obstacle, "C:\\Users\\Lenovo\\Desktop\\Amazons\\obstacle.jpg");
	loadimage(&black, "C:\\Users\\Lenovo\\Desktop\\Amazons\\BLACK.jpg"); 
	loadimage(&selectb, "C:\\Users\\Lenovo\\Desktop\\Amazons\\selectblack.jpg");
	loadimage(&selectw, "C:\\Users\\Lenovo\\Desktop\\Amazons\\selectwhite.jpg");
	loadimage(&img, "C:\\Users\\Lenovo\\Desktop\\Amazons\\background.jpg");
	putimage(0, 0, &img);
	setlinecolor(WHITE);

	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (gridInfo[i][j] == -playercolor) {
				if(playercolor==grid_black)putimage(50 * i, 50 * j, &white);
				else putimage(50 * i, 50 * j, &black);
			}
			else if (gridInfo[i][j] == OBSTACLE) {
				putimage(50 * i, 50 * j, &obstacle);
			}
			else if (gridInfo[i][j] == playercolor) {
				if (i == playerpos.x && j == playerpos.y) {
					if(playercolor==grid_black)putimage(50 * i, 50 * j, &selectb);
					else putimage(50 * i, 50 * j, &selectw);
					showroute(i, j);
				}
				else {
					if(playercolor==grid_black)putimage(50 * i, 50 * j, &black);
					else putimage(50 * i, 50 * j, &white);
				}
			}
		}
	}
}
int Save(int board[GRIDSIZE][GRIDSIZE], int next) //存档
{
	std::ofstream out;
	out.open(FILE_NAME, ios_base::out | ios::trunc);
	if (out.is_open())
	{
		for (int x = 0; x < GRIDSIZE; ++x)
		{
			for (int y = 0; y < GRIDSIZE; ++y)
			{
				out << board[x][y] << std::endl;
			}
		}
		out << turnID<<std::endl;
		out << playercolor << std::endl;
		out << currBotColor << std::endl;
		out.close();
		return 0;
	}

	return -1;//保存失败
}
int Read(int(*board)[GRIDSIZE][GRIDSIZE], int* next) //读档
{
	std::ifstream in;
	in.open(FILE_NAME, ios_base::in);

	if (in.is_open())
	{
		for (int x = 0; x < GRIDSIZE; ++x)
		{
			for (int y = 0; y < GRIDSIZE; ++y)
			{
				if (!in.eof())
				{
					in >> (*board)[x][y];
				}
			}
		}
		in >> turnID >> playercolor >> currBotColor;
		in.close();
		return 0;
	}

	return -1;//读取失败
}
int showMenu() //欢迎程序主页面
{
	closegraph();
	initgraph(640, 480);
	IMAGE img;
	loadimage(&img, "C:\\Users\\Lenovo\\Desktop\\Amazons\\UI.jpg");
	putimage(0, 0, &img);
	FlushMouseMsgBuffer();
	MOUSEMSG m;
	while(1){
		while (MouseHit()) {
			m = GetMouseMsg();
			if (m.mkLButton) {

				if (m.x >= 232 && m.x <= 376) {
					if (m.y >= 229 && m.y <= 272)return 1;
					else if (m.y >= 279 && m.y <= 321)return 2;
					else if (m.y >= 331 && m.y <= 373)return 3;
					else if (m.y >= 382 && m.y <= 426)return 4;
					else if (m.y >= 438 && m.y <= 480)return 5;
				}
			}
		}
	}
	
	return -1;
}//**********菜单显示*******//
int OutIntro() {
	IMAGE img;
	loadimage(&img, "C:\\Users\\Lenovo\\Desktop\\Amazons\\Intro.jpg");
	putimage(0, 0, &img);
	FlushMouseMsgBuffer();
	MOUSEMSG m; 
	while (1) {
		if (MouseHit()) {
			m = GetMouseMsg();
			if (m.mkLButton) {
				if (m.x >= 538 && m.x <= 640 && m.y >= 424 && m.y <= 480)return 1;
				else{}
			}
		}
	}
}
int NewGame() //开始新游戏
{
	
	IMAGE img;
	loadimage(&img, "C:\\Users\\Lenovo\\Desktop\\Amazons\\UI2.jpg");
	putimage(0, 0, &img);
	FlushMouseMsgBuffer();
	MOUSEMSG m;
	while (1) {
		if(MouseHit()) {
			m = GetMouseMsg();
			if (m.mkLButton) {

				if (m.x >= 201 && m.x <= 438) {
					if (m.y >= 123 && m.y <= 207) {
						playercolor = grid_black;
						currBotColor = grid_white;
						break;
					}
					else if (m.y >= 240 && m.y <= 325) {
						playercolor = grid_white;
							currBotColor = grid_black;
							break;
					}
				}
			}
		}
	}
	Initgame();
	return RunChess();
}
int Savechess() {
	if (Save(gridInfo, nexttype) < 0) {
		MessageBox(NULL, "Failed to Save!", "Attention!", MB_OK);
	}
	return 0;
}
int Loadchess() {
	if (Read(&(gridInfo), &nexttype) < 0) {
		MessageBox(NULL, "Failed to Read!", "Attention!", MB_OK);
		return -1;
	}
	//******对存档信息可能错误进行处理********//
	if (loadcheck()) {
		MessageBox(NULL, "Failed to Read!", "Attention!", MB_OK);
		return -1;
	}
	//******对读入档可能已经产生胜负的情况进行判断*****//
	if (IsEnd(playercolor)) {
		MessageBox(NULL, "You    Lose   !  !  !", "Attention!!!", MB_OK);
		return -1;
	}
	else if (IsEnd(-playercolor)) {
		MessageBox(NULL, "You    Win    !  !  !", "Congratuation!!!", MB_OK);
		return -1;
	}
	return RunChess();
}
void Searchchess() //获取白方黑方棋子所在位置
{
	int myi = 0, opi = 0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (gridInfo[i][j] == playercolor) {
				mx[myi] = i;
				my[myi++] = j;
			}
			else if (gridInfo[i][j] == -playercolor) {
				ox[opi] = i;
				oy[opi++] = j;
			}
		}
	}
}
int main()//主程序入口
{
	int ret = 0;
	mciSendString(_T("open C:\\Users\\Lenovo\\Desktop\\Amazons\\backsound.mp3 alias music"), NULL, 0, NULL);//打开音乐
	mciSendString(_T("play music repeat"), NULL, 0, NULL);//播放音乐
	ret = showMenu();
	while (true) {
		if (ret == -1) {
			ret = showMenu();
			continue;
		}
		if (ret == 1) {
			ret = NewGame();
			continue;
		}
		else if (ret == 2) {
			ret = Loadchess();
			continue;
		}
		else if (ret == 3) {
			Savechess();
			ret = showMenu();
		}
		else if (ret == 4) {
			mciSendString(_T("close music"), NULL, 0, NULL);//关闭音乐
			break;
		}
		else if (ret == 5) {
			OutIntro();
			ret = showMenu();
		}
	}
	return 0;
}