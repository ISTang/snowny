#include "stdafx.h"
#include "Player.h"

DWORD CPlayer::m_UnionCoins = 0;
DWORD CPlayer::m_OldUnionCoins = 0;

CPlayer::CPlayer(DWORD id)
: m_dId(id)
, m_Status(PS_IDLE)
, m_OldCoins(0)
, m_dCoins(0)
, m_StatusChangeTime(GetTickCount())
{

}

void CPlayer::RefreshStatus( GAMEFLOW gameFlow )
{
	if (m_Status != PS_IDLE)
	{
		if (gameFlow == flow_mainmenu && m_Status == PS_CLICKSTART)
		{
			SetStatus(PS_STARTTING);
			TRACE(TEXT("SF4 player[%d] PS_STARTTING"), m_dId);
		}
		else if (gameFlow == flow_game && m_Status == PS_STARTTING)
		{
			m_NoHpTime = 0xFFFFFFFF;
			SetStatus(PS_GAMING);
			TRACE(TEXT("SF4 player[%d] PS_GAMING"), m_dId);
		}
		else if (gameFlow == flow_continue && m_Status == PS_GAMING)
		{
			SetStatus(PS_DEAD);
			TRACE(TEXT("SF4 player[%d] PS_DEAD"), m_dId);
		}
		else if (gameFlow == flow_mainmenu && m_Status == PS_DEAD)
		{
			SetStatus(PS_GAMEOVER);
			TRACE(TEXT("SF4 player[%d] PS_GAMEOVER"), m_dId);
		}
		else if (gameFlow == flow_titlemenu && m_Status == PS_GAMEOVER)
		{
			SetStatus(PS_IDLE);
			TRACE(TEXT("SF4 player[%d] PS_IDLE"), m_dId);
		}
		else if (gameFlow == flow_mainmenu && m_Status == PS_GAMING)	// 转入对战模式
		{
			SetStatus(PS_STARTTING);
			TRACE(TEXT("SF4 player[%d] PS_STARTTING"), m_dId);
		}
		else if (gameFlow == flow_titlemenu && m_Status == PS_WINNER)
		{
			m_Status = PS_CLICKSTART;
		}

 		if (m_Status == PS_GAMING)
 		{
 			if (m_NoHpTime == 0xFFFFFFFF
 				&& GetTickCount() - m_StatusChangeTime > 10000 // 进入游戏10秒以上
 				&& QueryHP() <= 0)
 			{
 				// 发现无血，死亡计时，解决游戏分回合导致不能直接使用死亡状态，因为还有下一回合
 				// 且暂未拿到游戏结束的标志，使用计时的方式：依据是下一回合会在一定时间内开始
 				TRACE(TEXT("SF4 player[%d] begin time of dead"), m_dId);
 				m_NoHpTime = GetTickCount();
 
 			}
 			else if (m_NoHpTime != 0xFFFFFFFF && GetTickCount() - m_NoHpTime > 15000)// 15秒
 			{
 				TRACE(TEXT("SF4 player[%d] Dead"), m_dId);
 				m_Status = PS_DEAD;
 			}
 			else if (QueryHP() > 0)
 			{
 				m_NoHpTime = 0xFFFFFFFF;
 			}
 		}
	}
}

BOOL CPlayer::ClickStart()
{
	if (m_Status == PS_IDLE || m_Status == PS_DEAD)
	{
		DWORD *pCoins = g_Config.CoinMode == 0 ? &m_UnionCoins : &m_dCoins; // 单式、双式
		if (*pCoins >= g_Config.UnitCoin)
		{
			*pCoins -= g_Config.UnitCoin;
			m_Status = PS_CLICKSTART;

			TRACE(TEXT("SF4 player[%d] start success"), m_dId);
			return TRUE;
		}
		else
		{
			TRACE(TEXT("SF4 player[%d] start fail: has no coins %d, %d"), m_dId, *pCoins, g_Config.UnitCoin);
		}
	}
	else
	{
		TRACE(TEXT("SF4 player[%d] start fail: error status %d"), m_dId, m_Status);
	}

	return FALSE;
}

void CPlayer::IncrementCoin()
{
	g_Config.CoinMode == 0 ? m_UnionCoins ++ : m_dCoins ++; // 单式、双式
}

BOOL CPlayer::CoinsChanged()
{
	BOOL changed = FALSE;
	DWORD *pCoins = g_Config.CoinMode == 0 ? &m_UnionCoins : &m_dCoins;			// 单式、双式
	DWORD *pOldCoins = g_Config.CoinMode == 0 ? &m_OldUnionCoins : &m_OldCoins; // 单式、双式

	changed = *pCoins != *pOldCoins;
	*pOldCoins = *pCoins;

	return changed;
}

DWORD CPlayer::GetCoinNumber()
{
	return g_Config.CoinMode == 0 ? m_UnionCoins : m_dCoins;// 单式、双式
}

int CPlayer::QueryHP()
{
	DWORD getGlobalDataAddr = 0x0043A670,
		getPlayerDataAddr = 0x00439F80,
		globalDataAddr = 0,
		playerAddr = 0;
	int playerHP = -1;

	DWORD id = m_dId;

	__asm
	{
		pushad
		mov eax, getGlobalDataAddr
		call eax
		mov globalDataAddr, eax
	
		push id
		mov ecx, globalDataAddr
		mov edx, getPlayerDataAddr
		call edx
		test eax, eax
		jz jmp1
		mov playerAddr, eax
		movsx edi, word ptr [eax + 51F6h]
		mov playerHP, edi
jmp1:
		popad
	}

	//TRACE(TEXT("SF4 player[%d] playeraddr:0x%x HP: %d"), m_dId, playerAddr, playerHP);
	return playerHP;
}