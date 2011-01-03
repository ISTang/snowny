#include "StdAfx.h"
#include "Fairy.h"
#include "showwnd.h"
#include "showcutrectwnd.h"
#include <shlwapi.h>

CFairy::CFairy(void)
{
	GdiplusStartup(&m_gdiplusToken,&m_gdiplusStartupInput, NULL);
}

CFairy::~CFairy(void)
{
	GdiplusShutdown(m_gdiplusToken);
}

void CFairy::CreateTransparentWnd(int id, std::wstring picName, CPoint pt, int width /*= 0*/, int height/* = 0*/)
{
	if ( width == 0 || height == 0)
	{
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
	}

	m_showWnd = new CShowWnd(picName);
	m_showWnd->CreateEx(WS_EX_TOPMOST   |   WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,::LoadCursor(NULL,   IDC_ARROW),  
		HBRUSH(COLOR_WINDOW+1),   NULL),
		_T("MyPopupWindow "), WS_POPUP,
		CRect(CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT), NULL ,NULL ,NULL); 

	m_showWnd->ShowWindow(SW_HIDE);

	m_showWnd->MoveWindow(pt.x, pt.y, width, height);
	m_showWndList.insert(std::make_pair(id, m_showWnd));
}


void CFairy::CreateCoinInsert(int id, int life, int coins, int cointsOneLife ,CPoint pt, int width/* = 0*/ , int height/* = 0*/)
{
	if ( width == 0 || height == 0)
	{
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
	}

	CShowCutRectWnd::CutRectList cutRectList = GetCutRectList(life, coins, cointsOneLife);

	wchar_t res[512]= {0};
	HMODULE handle = GetModuleHandle(L"JBInjectDll.dll");
	GetModuleFileName(handle, res, sizeof(res) * sizeof(wchar_t));
	PathRemoveFileSpec(res);

	std::wstring path = res;
	std::wstring backgroundPath = path + L"\\Num.png";

	mInsertCointSingleWnd = new CShowCutRectWnd(backgroundPath, cutRectList);
	mInsertCointSingleWnd->CreateEx(WS_EX_TOPMOST   |   WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,::LoadCursor(NULL,   IDC_ARROW),  
		HBRUSH(COLOR_WINDOW+1),   NULL), 
		_T("MyPopupWindow "), WS_POPUP,
		CRect(CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT), NULL ,NULL ,NULL); 

	mInsertCointSingleWnd->ShowWindow(SW_HIDE);
	mInsertCointSingleWnd->MoveWindow(pt.x, pt.y, width, height);

	m_showWndList.insert(std::make_pair(id, mInsertCointSingleWnd));
}

void CFairy::DestroyWnd( int id )
{
	CWnd* wnd = m_showWndList[id];
	wnd->DestroyWindow();
	delete m_showWnd;
}

void CFairy::DestroyAllWnd()
{
	std::map<int, CWnd*>::iterator iter  = m_showWndList.begin();
	for (; iter != m_showWndList.end(); ++iter)
	{
		(*iter).second->DestroyWindow();
		delete (*iter).second;
	}
}
void CFairy::ShowWnd( int id )
{
	m_showWndList[id]->ShowWindow(SW_NORMAL);
}

void CFairy::HideWnd( int id )
{
	m_showWndList[id]->ShowWindow(SW_HIDE);
}

void CFairy::ShowAllWnd()
{
	std::map<int, CWnd*>::iterator iter  = m_showWndList.begin();
	for (; iter != m_showWndList.end(); ++iter)
	{
		(*iter).second->ShowWindow(SW_NORMAL);
	}
}

void CFairy::HideAllWnd()
{
	std::map<int, CWnd*>::iterator iter  = m_showWndList.begin();
	for (; iter != m_showWndList.end(); ++iter)
	{
		(*iter).second->ShowWindow(SW_HIDE);
	}
}

std::vector<int> CFairy::GetNumList( int number )
{
	std::vector<int> numList;
	while (true)
	{
		if (number)
		{
			numList.push_back(number % 10);
		}
		number = number / 10;
		if (number == 0)
		{
			break;
		}
	}
	return numList;
}

CShowCutRectWnd::CutRectList CFairy::GetCutRectList( int life, int coins, int cointsOneLife )
{
	CShowCutRectWnd::CutRectList cutRectList;

	std::vector<int> lifeList;
	std::vector<int> coinList;
	std::vector<int> coinsOneLifeList;
	lifeList = GetNumList(life);
	coinList = GetNumList(coins);
	coinsOneLifeList = GetNumList(cointsOneLife);
	RectF rect;
	rect.Y = 0;
	rect.Width = 20;
	rect.Height = 40;
	//life
	std::vector<int>::reverse_iterator iter = lifeList.rbegin();
	for (; iter != lifeList.rend(); ++iter)
	{
		rect.X = ( *iter ) * 20;
		cutRectList.push_back(rect);
	}
	//:
	rect.X = 10 * 20;
	cutRectList.push_back(rect);

	//[
	rect.X = 11 * 20;
	cutRectList.push_back(rect);

	//coins
	iter = coinList.rbegin();
	for (; iter != coinList.rend(); ++iter)
	{
		rect.X = ( *iter ) * 20;
		cutRectList.push_back(rect);
	}

	//  /
	rect.X = 12 * 20;
	cutRectList.push_back(rect);

	//cointsOneLife
	iter = coinsOneLifeList.rbegin();
	for (; iter != coinsOneLifeList.rend(); ++iter)
	{
		rect.X = ( *iter ) * 20;
		cutRectList.push_back(rect);
	}

	//  ]
	rect.X = 13 * 20;
	cutRectList.push_back(rect);

	return cutRectList;
}