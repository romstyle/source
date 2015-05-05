#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_POURFENDRE			     	 = 772, //
    SPELL_ATTAQUE_BONDISANTE           	 = 74524, // random target
    SPELL_ENRAGER					     = 74367, // 45s
	SPELL_FRAPPE_TRAMBLANTE				 = 50188, // tank
	SPELL_MARQUE_DE						 = 41107, //24s
	SPELL_FEAR 							 = 31013,
	SPELL_PROTEC 							 = 871,
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
        uint32 pourfendre;
        uint32 bondisante;
        uint32 enrage;
		uint32 tramblante;
		uint32 marque;
		InstanceScript* instance;
		bool phase2 = false;
		uint32 mawHealth;
		bool phase3 = false;

		void Reset() OVERRIDE
        {
			_Reset();
			me->RemoveAllAuras();
            pourfendre = 12000;
            enrage = 30000;
            bondisante = 18000;
			tramblante = 7000;
			marque = 24000;
			phase2 = false;
			phase3 = false;
			mawHealth = me->GetHealth();
        }
		void EnterCombat(Unit* /*who*/) OVERRIDE
        {
			_EnterCombat();
			me->MonsterYell("Je vais vous arracher le cœur !!", LANG_UNIVERSAL, me->GetGUID());
        }
		void KilledUnit(Unit* /*victim*/) OVERRIDE
        {
			me->MonsterYell("Délicieux bain de sang !", LANG_UNIVERSAL, me->GetGUID());
        }
		void JustDied(Unit* killer) OVERRIDE
        {
			_JustDied();
			me->MonsterYell("Imp-imposs-Aaaaaaaaaaarrrrh...", LANG_UNIVERSAL, me->GetGUID());
        }
		void UpdateAI(uint32 diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;
            if (pourfendre <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_POURFENDRE);
                pourfendre = 12000;
				me->MonsterYell("Je vais faire un feu avec vos restes !", LANG_UNIVERSAL, me->GetGUID());
            }
			else pourfendre -= diff;
            if (bondisante <= diff)
            {
				Unit* UnitAI = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
				DoCast(UnitAI, SPELL_ATTAQUE_BONDISANTE);
                bondisante = 18000;
				DoCast(UnitAI, SPELL_POURFENDRE);
				me->MonsterYell("VIENS LA !", LANG_UNIVERSAL, me->GetGUID());
            }
			else bondisante -= diff;
            if (enrage <= diff)
            {
				DoCast(me, SPELL_ENRAGER, true);
                enrage = 30000;
				me->MonsterYell("SANG ET CHAIR !!!", LANG_UNIVERSAL, me->GetGUID());
            }
			else enrage -= diff;
			if (tramblante <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_FRAPPE_TRAMBLANTE);
                tramblante = 7000;
				me->MonsterYell("Rhaaaaaaa MOURREZ !!", LANG_UNIVERSAL, me->GetGUID());
            }
			else tramblante -= diff;
			if (marque <= diff)
			{
				DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_MARQUE_DE);
                marque = 24000;
				me->MonsterYell("Rhaaaaaaa MOURREZ !!", LANG_UNIVERSAL, me->GetGUID());
			}
			else marque -= diff;
			if (me->HealthBelowPct(10) && phase2 == false)
			{
				me->MonsterYell("JE VAIS BRISER VOS OS !", LANG_UNIVERSAL, me->GetGUID());
				me->SetHealth(me->GetHealth + me->GetHealth)
				DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_FEAR, true);
				phase2 = true;
			}
			if (me->HealthBelowPct(10) && phase3 == false)
			{
				me->MonsterYell("JE VAIS BRISER VOS OS !", LANG_UNIVERSAL, me->GetGUID());
				me->SetHealth(me->GetHealth + (me->GetHealth / 2))
				DoCast(me, SPELL_PROTEC, true);
				phase3 = true;
			}
            DoMeleeAttackIfReady();
        }
	};
};
void AddSC_boss_ahnkarok()
{
    new boss_ahnkarok();
}