#ifndef __BATTLEGROUNDFD_H
#define __BATTLEGROUNDFD_H

#include "Battleground.h"

#define BG_FD_TIME_LIMIT          (5*MINUTE*IN_MILLISECONDS);
#define SEND_MSG_NEAR_LOSE               120

enum BG_FD_WorldStates
{
    BG_FD_TIME_REMAINING          = 4248,
	FD_Alliance_Score = 3127,
	FD_Horde_Score = 3128,
	FD_SHOW_H_SCORE = 3133,
	FD_SHOW_A_SCORE = 3134,
};
enum BG_FD_Sounds
{
	FD_SOUND_NEAR_VICTORY = 8456,
};
enum BG_FD_Graveyards
{
    FD_GRAVEYARD_MAIN_ALLIANCE     = 1800,
    FD_GRAVEYARD_MAIN_HORDE        = 1801
};

class BattleGroundFD : public Battleground
{
    friend class BattleGroundMgr;
    public:
        BattleGroundFD();
        ~BattleGroundFD();
        void Update(uint32 diff);

        void AddPlayer(Player *plr);

        void HandleKillPlayer(Player *player, Player *killer);
        bool SetupBattleGround();
        void Reset();
        void EndBattleGround();
        WorldSafeLocsEntry const* GetClosestGraveYard(Player* player);
        uint32 GetRemainingTimeInMinutes() { return m_EndTimer ? (m_EndTimer-1) / (MINUTE * IN_MILLISECONDS) + 1 : 0; }

		void UpdatePlayerScore(Player* player, uint32 type, uint32 value, bool doAddHonor = true);
        void FillInitialWorldStates(WorldPacket& data);
		void UpdateScore(uint16 team, int16 points);
		void StartingEventOpenDoors();
    private:
        uint32 m_HonorEndKills;
        uint32 m_EndTimer;
		int32 m_Team_Scores[2];
		bool m_IsInformedNearVictory[2];
};
#endif