#include "BattleGroundFD.h"
#include "Battleground.h"
#include "Creature.h"
#include "GameObject.h"
#include "Language.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"

// fury d�chain�

BattleGroundFD::BattleGroundFD()
{
	StartMessageIds[BG_STARTING_EVENT_FIRST] = 0;
	StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_WS_START_ONE_MINUTE;
	StartMessageIds[BG_STARTING_EVENT_THIRD] = LANG_BG_WS_START_HALF_MINUTE;
	StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_WS_HAS_BEGUN;
}
void BattleGroundFD::StartingEventOpenDoors()
{
	UpdateWorldState(FD_SHOW_H_SCORE, 0);
	UpdateWorldState(FD_SHOW_A_SCORE, 0);
}
BattleGroundFD::~BattleGroundFD()
{
}
void BattleGroundFD::StartingEventCloseDoors()
{
	UpdateWorldState(BG_FD_STATE_TIMER_ACTIVE, 1);
	UpdateWorldState(BG_FD_TIME_REMAINING, 25);
}
void BattleGroundFD::UpdateTeamScore(uint32 team)
{
	if (team == TEAM_ALLIANCE)
		UpdateWorldState(FD_Alliance_Score, GetTeamScore(team));
	else
		UpdateWorldState(FD_Horde_Score, GetTeamScore(team));
}
void BattleGroundFD::Update(uint32 diff)
{
    Battleground::Update(diff);

    if (GetStatus() == STATUS_IN_PROGRESS)
    {

        if (m_EndTimer <= diff)
            EndBattleGround();
        else
        {
            uint32 minutesLeftPrev = GetRemainingTimeInMinutes();
            m_EndTimer -= diff;
            uint32 minutesLeft = GetRemainingTimeInMinutes();

            if (minutesLeft != minutesLeftPrev)
                UpdateWorldState(BG_FD_TIME_REMAINING, minutesLeft);
        }
    }
}

void BattleGroundFD::AddPlayer(Player *plr)
{
	Battleground::AddPlayer(plr);
}

bool BattleGroundFD::SetupBattleGround()
{
	UpdateWorldState(ALLIANCE, 0);
	UpdateWorldState(HORDE, 0);
    return true;
}

void BattleGroundFD::Reset()
{
    //call parent's class reset
	Battleground::Reset();

    for(uint32 i = 0; i < BG_TEAMS_COUNT; ++i)
        m_TeamScores[i]      = 0;
    m_HonorEndKills = 2;
    m_EndTimer = BG_FD_TIME_LIMIT;
}

void BattleGroundFD::EndBattleGround()
{
    RewardHonorToTeam(GetBonusHonorFromKill(m_HonorEndKills), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(m_HonorEndKills), HORDE);

	Battleground::EndBattleground(0);
}

void BattleGroundFD::HandleKillPlayer(Player *player, Player *killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	if (player->GetTeam() == TEAM_ALLIANCE)
		UpdateScore(TEAM_ALLIANCE, (+1));
	else
		UpdateScore(TEAM_HORDE, (+1));
	Battleground::HandleKillPlayer(player, killer);
}

void BattleGroundFD::UpdatePlayerScore(Player* player, uint32 type, uint32 value, bool doAddHonor)
{
	BattlegroundScoreMap::iterator itr = PlayerScores.find(player->GetGUID());
	if (itr == PlayerScores.end())                         // player not found
        return;

	Battleground::UpdatePlayerScore(player, type, value, doAddHonor);
}

WorldSafeLocsEntry const* BattleGroundFD::GetClosestGraveYard(Player* player)
{
    if (player->GetTeam() == ALLIANCE)
        return sWorldSafeLocsStore.LookupEntry(FD_GRAVEYARD_MAIN_ALLIANCE);
    else
        return sWorldSafeLocsStore.LookupEntry(FD_GRAVEYARD_MAIN_HORDE);
}

void BattleGroundFD::FillInitialWorldStates(WorldPacket& data)
{
	if (GetStatus() == STATUS_IN_PROGRESS)
	{
		data << uint32(BG_FD_STATE_TIMER_ACTIVE) << uint32(1);
		data << uint32(BG_FD_TIME_REMAINING) << uint32(25 - _minutesElapsed);
	}
	else
		data << uint32(BG_FD_STATE_TIMER_ACTIVE) << uint32(0);
}
void BattleGroundFD::PostUpdateImpl(uint32 diff)
{
	if (GetStatus() == STATUS_IN_PROGRESS)
	{
		if (GetStartTime() >= 27 * MINUTE*IN_MILLISECONDS)
		{
			if (m_Team_Scores[0] > m_Team_Scores[1])
				EndBattleground(ALLIANCE);
			else if (m_Team_Scores[1] > m_Team_Scores[0])
				EndBattleground(HORDE);
			else
				EndBattleground(WINNER_NONE);
		}
		else if (GetStartTime() > uint32(_minutesElapsed * MINUTE * IN_MILLISECONDS) + 3 * MINUTE * IN_MILLISECONDS)
		{
			++_minutesElapsed;
			UpdateWorldState(BG_FD_TIME_REMAINING, 25 - _minutesElapsed);
		}
	}
}
void BattleGroundFD::UpdateScore(uint16 team, int16 points)
{ //note: to remove reinforcementpoints points must be negative, for adding reinforcements points must be positive
	ASSERT(team == ALLIANCE || team == HORDE);
	uint8 teamindex = GetTeamIndexByTeamId(team); //0=ally 1=horde
	m_Team_Scores[teamindex] += points;

	UpdateWorldState(((teamindex == TEAM_HORDE) ? FD_Horde_Score : FD_Alliance_Score), m_Team_Scores[teamindex]);
	if (points > 99)
	{
		if (m_Team_Scores[teamindex] > 99)
		{
			m_Team_Scores[teamindex] = 100;
			EndBattleground(((teamindex == TEAM_HORDE) ? ALLIANCE : HORDE));
		}
		else if (!m_IsInformedNearVictory[teamindex] && m_Team_Scores[teamindex] < SEND_MSG_NEAR_LOSE)
		{
			SendMessageToAll(teamindex == TEAM_HORDE ? LANG_BG_AV_H_NEAR_LOSE : LANG_BG_AV_A_NEAR_LOSE, teamindex == TEAM_HORDE ? CHAT_MSG_BG_SYSTEM_HORDE : CHAT_MSG_BG_SYSTEM_ALLIANCE);
			PlaySoundToAll(FD_SOUND_NEAR_VICTORY);
			m_IsInformedNearVictory[teamindex] = true;
		}
	}
}