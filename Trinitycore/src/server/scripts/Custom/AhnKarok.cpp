#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_DECHAINEMENT_DE_LAME	     	 = 75125,
    SPELL_ENCHAINEMENT	             	 = 74524,
    SPELL_FENDRE_ARMURE				     = 74367,
    SPELL_MARQUE_AFFAIBLISSEMENT         = 74502,
    SPELL_AGITTION_FRENETIQUE	         = 50188,
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
        uint32 marque_affaiblissement;
        uint32 enchainement;
        uint32 fendre_armure;
        uint32 dechainement_de_lame;
		uint32 agitation;
		InstanceScript* instance;
		void Reset() OVERRIDE
        {
            marque_affaiblissement = 45000;
            enchainement = 60000;
            dechainement_de_lame = 5000;
            fendre_armure = 45000;
			agitation = 75000;
        }
		void EnterCombat(Unit* /*who*/) OVERRIDE
        {
			me->MonsterYell("Comme vous voulez !", LANG_UNIVERSAL, me->GetGUID());
        }
		void KilledUnit(Unit* /*victim*/) OVERRIDE
        {
			me->MonsterYell("Ahahaha encore du sang ! ENCORE !", LANG_UNIVERSAL, me->GetGUID());
        }
		void JustDied(Unit* killer) OVERRIDE
        {
			me->MonsterYell("Le.. maître... vous vaincra.... ahhhhh...!", LANG_UNIVERSAL, me->GetGUID());
        }
		void UpdateAI(uint32 diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;
            if (marque_affaiblissement <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(target, SPELL_MARQUE_AFFAIBLISSEMENT);
                marque_affaiblissement = urand(45000, 60000);
				//talk
            }
			else marque_affaiblissement -= diff;
            if (enchainement <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_ENCHAINEMENT);
                enchainement = 60000;
				me->MonsterYell("Ahahah, goutez à ma lame !!", LANG_UNIVERSAL, me->GetGUID());
            }
			else enchainement -= diff;
            if (fendre_armure <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_FENDRE_ARMURE);
                fendre_armure = urand(10000, 20000);
				me->MonsterYell("Vous êtes faibles !!", LANG_UNIVERSAL, me->GetGUID());
            }
			else fendre_armure -= diff;
            if (dechainement_de_lame <= diff)
            {
				DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 100, true), SPELL_DECHAINEMENT_DE_LAME);
                dechainement_de_lame = 45000;
				me->MonsterYell("Que ma lame vous poufande !!", LANG_UNIVERSAL, me->GetGUID());
            }
			else dechainement_de_lame -= diff;
			if (agitation <= diff)
            {
                DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_DECHAINEMENT_DE_LAME);
                agitation = 75000;
				me->MonsterYell("Vous commencez à m'enerver !!", LANG_UNIVERSAL, me->GetGUID());
            }
			else agitation -= diff;
            DoMeleeAttackIfReady();
        }
	};
};
void AddSC_boss_ahnkarok()
{
    new boss_ahnkarok();
}