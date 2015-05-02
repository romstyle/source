#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_DECHAINEMENT_DE_LAME	     	 = 75125,
    SPELL_ENCHAINEMENT	             	 = 74524,
    SPELL_FENDRE_ARMURE				     = 74367,
	SPELL_AGITTION_FRENETIQUE			 = 50188,
	SPELL_BERSERK						 = 41107,
};

class boss_ahnkarok : public CreatureScript
{
public:
    boss_ahnkarok() : CreatureScript("boss_ahnkarok") { }

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
		return new boss_ahnkarokAI(creature);
    }
	struct boss_ahnkarokAI : public ScriptedAI
	{
		boss_ahnkarokAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }
        uint32 enchainement;
        uint32 fendre_armure;
        uint32 dechainement_de_lame;
		uint32 agitation;
		InstanceScript* instance;
		bool buffed = false;

		void Reset() OVERRIDE
        {
            enchainement = 60000;
            dechainement_de_lame = 5000;
            fendre_armure = 45000;
			agitation = 75000;
        }
		void EnterCombat(Unit* /*who*/) OVERRIDE
        {
			me->MonsterYell("Je vais vous arracher le cœur !!", LANG_UNIVERSAL, me->GetGUID());
        }
		void KilledUnit(Unit* /*victim*/) OVERRIDE
        {
			me->MonsterYell("Délicieux bain de sang !", LANG_UNIVERSAL, me->GetGUID());
        }
		void JustDied(Unit* killer) OVERRIDE
        {
			me->MonsterYell("Imp-imposs-Aaaaaaaaaaarrrrh...", LANG_UNIVERSAL, me->GetGUID());
			char msg[255];
			snprintf(msg, 255, "Felicitation ! %s ainsi que sa guilde on vaincu Ahn'Karok le berserk", killer->GetName());
			sWorld->SendServerMessage(SERVER_MSG_STRING, msg);
        }
		void UpdateAI(uint32 diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;
            if (enchainement <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_ENCHAINEMENT);
                enchainement = 60000;
				me->MonsterYell("Je vais faire un feu avec vos restes !", LANG_UNIVERSAL, me->GetGUID());
            }
			else enchainement -= diff;
            if (fendre_armure <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_FENDRE_ARMURE);
                fendre_armure = urand(10000, 20000);
				me->MonsterYell("VIENS LA !", LANG_UNIVERSAL, me->GetGUID());
            }
			else fendre_armure -= diff;
            if (dechainement_de_lame <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_DECHAINEMENT_DE_LAME);
                dechainement_de_lame = 45000;
				me->MonsterYell("SANG ET CHAIR !!!", LANG_UNIVERSAL, me->GetGUID());
            }
			else dechainement_de_lame -= diff;
			if (agitation <= diff)
            {
                DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_DECHAINEMENT_DE_LAME);
                agitation = 75000;
				me->MonsterYell("Rhaaaaaaa MOURREZ !!", LANG_UNIVERSAL, me->GetGUID());
            }
			else agitation -= diff;
			if (me->HealthBelowPct(10) && buffed == false)
			{
				me->MonsterYell("JE VAIS BRISER VOS OS !", LANG_UNIVERSAL, me->GetGUID());
				DoCast(me, SPELL_BERSERK, true);
				buffed = true;
			}
            DoMeleeAttackIfReady();
        }
	};
};
void AddSC_boss_ahnkarok()
{
    new boss_ahnkarok();
}