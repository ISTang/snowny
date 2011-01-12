#include "stdafx.h"
#include "Player.h"

DWORD CPlayer::m_UnionCoins = 0;
DWORD CPlayer::m_OldUnionCoins = 0;

CPlayer::CPlayer(DWORD id)
: m_dId(id)
, m_Status(PS_IDLE)
, m_OldCoins(0)
, m_dCoins(0)
{

}

void CPlayer::RefreshStatus( GAMEFLOW gameFlow )
{
	if (m_Status != PS_IDLE)
	{
		if (gameFlow == flow_mainmenu && m_Status == PS_CLICKSTART)
		{
			m_Status = PS_STARTTING;
		}
		else if (gameFlow == flow_game && m_Status == PS_STARTTING)
		{
			m_Status = PS_GAMING;
		}
		else if (gameFlow == flow_continue && m_Status == PS_GAMING)
		{
			m_Status = PS_DEAD; // maybe winner
		}
		else if (gameFlow == flow_mainmenu && m_Status == PS_DEAD)
		{
			m_Status = PS_GAMEOVER;
		}
		else if (gameFlow == flow_titlemenu && m_Status == PS_GAMEOVER)
		{
			m_Status = PS_IDLE;
		}
	}
}

BOOL CPlayer::ClickStart()
{
	if (m_Status == PS_IDLE)
	{
		DWORD *pCoins = NULL;

		if (g_Config.CoinMode == 0) // 单式
		{
			pCoins = &m_UnionCoins;
		}
		else
		{
			pCoins = &m_dCoins;
		}
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
	if (g_Config.CoinMode == 0) // 单式
	{
		m_UnionCoins ++;
	}
	else
	{
		m_dCoins ++;
	}
}

BOOL CPlayer::CoinsChanged()
{
	BOOL changed = FALSE;

	if (g_Config.CoinMode == 0) // 单式
	{
		changed = m_OldUnionCoins != m_UnionCoins;
		m_OldUnionCoins = m_UnionCoins;
	}
	else
	{
		changed = m_OldCoins != m_dCoins;
		m_OldCoins = m_dCoins;
	}

	return changed;
}

DWORD CPlayer::GetCoinNumber()
{
	if (g_Config.CoinMode == 0) // 单式
	{
		return m_UnionCoins;
	}
	return m_dCoins;
}
