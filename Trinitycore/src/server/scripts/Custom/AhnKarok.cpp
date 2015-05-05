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
enum Sounds
{
	AHRKAROK_CARNAGE = 20000,
	AHRKAROK_PAS_ECHAPATOIR = 20001,
	AHRKAROK_COULER_LE_SANG = 20002,
	AHRKAROK_EVENT_1 = 20003,
	AHRKAROK_DEAD = 20004,
	AHRKAROK_KILL = 20005,
};
class boss_ahnkarok : public CreatureScript
{
public:
    boss_ahnkarok() : CreatureScript("boss_ahnkarok") { }

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
		return new boss_ahnkarokAI(creature);
    }
	struct boss_ahnkarokAI : public BossAI
	{
		boss_ahnkarokAI(Creature* creature) : BossAI(creature, 0)
        {
            instance = creature->GetInstanceScript();
        }
        uint32 enchainement;
        uint32 fendre_armure;
        uint32 dechainement_de_lame;
		uint32 agitation;
		InstanceScript* instance;
		bool buffed = false;
		void Talk(uint32 id)
		{
			DoPlaySoundToSet(me, id);
		}
		void Reset() OVERRIDE
        {
			_Reset();
			me->RemoveAllAuras();
            enchainement = 60000;
            dechainement_de_lame = 5000;
            fendre_armure = 45000;
			agitation = 75000;
        }
		void EnterCombat(Unit* /*who*/) OVERRIDE
        {
			_EnterCombat();
			me->MonsterYell("Je vais vous arracher le cœur !!", LANG_UNIVERSAL, me->GetGUID());
        }
		void KilledUnit(Unit* /*victim*/) OVERRIDE
        {
			me->MonsterYell("Délicieux bain de sang !", LANG_UNIVERSAL, me->GetGUID());
			Talk(AHRKAROK_KILL);
        }
		void JustDied(Unit* killer) OVERRIDE
        {
			_JustDied();
			me->MonsterYell("Aaaaaaaaaaarrrrh... Mrphhh... Mrphhh... !!", LANG_UNIVERSAL, me->GetGUID());
			Talk(AHRKAROK_DEAD);
        }
		void UpdateAI(uint32 diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;
            if (enchainement <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_ENCHAINEMENT);
                enchainement = 15000;
				me->MonsterYell("Je vais faire un feu avec vos restes !", LANG_UNIVERSAL, me->GetGUID());
            }
			else enchainement -= diff;
            if (fendre_armure <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_FENDRE_ARMURE);
				fendre_armure = 7000;
				me->MonsterYell("Pas d'échappatoire !!", LANG_UNIVERSAL, me->GetGUID());
				Talk(AHRKAROK_PAS_ECHAPATOIR);
            }
			else fendre_armure -= diff;
            if (dechainement_de_lame <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_DECHAINEMENT_DE_LAME);
                dechainement_de_lame = 45000;
				me->MonsterYell("CARNAGE !!", LANG_UNIVERSAL, me->GetGUID());
				Talk(AHRKAROK_CARNAGE);
            }
			else dechainement_de_lame -= diff;
			if (agitation <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_AGITTION_FRENETIQUE);
                agitation = 35000;
				me->MonsterYell("Il est temps de faire couler le SANG !!!", LANG_UNIVERSAL, me->GetGUID());
				Talk(AHRKAROK_COULER_LE_SANG);
            }
			else agitation -= diff;
			if (me->HealthBelowPct(10) && buffed == false)
			{
				me->MonsterYell("Mahhhhh... J'ai joué avec vous assez longtemps !!", LANG_UNIVERSAL, me->GetGUID());
				Talk(AHRKAROK_EVENT_1);
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