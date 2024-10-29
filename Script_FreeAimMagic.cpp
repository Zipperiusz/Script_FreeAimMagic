#include "Script_FreeAimMagic.h"

#include "util/Memory.h"
#include "util/Logging.h"
#include "util/Hook.h"
#include "util/Util.h"
#include "Script.h"
#include <iostream>
#include "util/ScriptUtil.h"
#include "AimHelper.h"
#include "CollisionAimSpells.h"
#include "FreeAimMagic.h"

gSScriptInit& GetScriptInit()
{
	static gSScriptInit s_ScriptInit;
	return s_ScriptInit;
}

void CastSpell(eCEntity* source, eCEntity* target) {
	Entity Spell = Template("Spl_Lightningbolt");
	Entity Spawn = Spell.Magic.GetSpawn();
	bCMatrix SpawnMatrix = source->GetWorldMatrix();
	SpawnMatrix.AccessTranslation().AccessY() += 50;

	Entity SpawnedEntity = Spawn.Spawn(Spawn.GetTemplate(), SpawnMatrix);
	SpawnedEntity.Interaction.SetOwner(Entity::GetPlayer());
	SpawnedEntity.Interaction.SetSpell(Spell);

	SpawnedEntity.EnableCollisionWith(SpawnedEntity, GEFalse);
	SpawnedEntity.EnableCollisionWith(source, GEFalse);

	SpawnedEntity.CollisionShape.CreateShape(eECollisionShapeType_Point, eEShapeGroup_Projectile, bCVector(0, 0, 0), bCVector(0, 0, 0));
	SpawnedEntity.Projectile.SetTarget(target);
	SpawnedEntity.Projectile.AccessProperty<PSProjectile::PropertyPathStyle>() = gEProjectilePath_Missile;
	bCVector TargetDirection = SpawnedEntity.GetDirectionTo(target).Normalize();

	SpawnedEntity.Projectile.AccessProperty<PSProjectile::PropertyTargetDirection>() = TargetDirection;
	SpawnedEntity.Damage.AccessProperty<PSDamage::PropertyManaUsed>() = Spell.Magic.GetProperty<PSMagic::PropertyMaxManaCost>();
	SpawnedEntity.Projectile.Shoot();
}

eCEntity* source;
bTPtrArray<eCEntity*> entitiesToShoot;

bTPropertyObject<mCTest, eCEngineComponentBase> mCTest::ms_PropertyObjectInstance_mCTest(GETrue);
void mCTest::Process(void) {

	if (!gCSession::GetSession().IsValid() || gCSession::GetSession().IsPaused() || !gCSession::GetSession().GetGUIManager())
	{
		return;
	}

	if (gCSession::GetInstance().GetGUIManager()->IsMenuOpen() || gCSession::GetInstance().GetGUIManager()->IsAnyPageOpen() || gCInfoManager_PS::GetInstance()->IsRunning)
	{
		return;
	}

	if (eCApplication::GetInstance().GetConsole().IsActive())
	{
		return;
	}

	if (entitiesToShoot.GetCount() > 0) {
		for (int i = 0; i < entitiesToShoot.GetCount(); i++) {
			CastSpell(source, entitiesToShoot.GetAt(i));
			entitiesToShoot.RemoveAt(i);
		}		
	}

	if (eCApplication::GetInstance().GetKeyboard().KeyPressed(eCInpShared::eEKeyboardStateOffset_APOSTROPHE)) {
		if (!testKeyPressed) {
			testKeyPressed = GETrue;
			auto focusEnt = Entity::GetPlayer().Focus.GetFocusEntity();
			focusEnt.Inventory.GetItemFromSlot(gESlot::gESlot_LeftHand).Item.ModifyQuality(gEItemQuality::gEItemQuality_Burning, gEItemQuality::gEItemQuality_Burning);

			std::cout << (focusEnt.Inventory.GetItemFromSlot(gESlot::gESlot_LeftHand).Item.GetQuality() & gEItemQuality::gEItemQuality_Burning);

		}
	}
	else {
		testKeyPressed = GEFalse;
	}

	if (eCApplication::GetInstance().GetMouse().ButtonPressed(eCInpShared::eEMouseOffset_Button0) && aimKeyPressed && AimHelper::IsUsingFreeAimSpell()) {
		if (!actionKeyPressed) {
			//LightningChain();
		}
		actionKeyPressed = GETrue;
	}
	else {
		actionKeyPressed = GEFalse;
	}

	if (eCApplication::GetInstance().GetMouse().ButtonPressed(eCInpShared::eEMouseOffset_Button1)) {
		aimKeyPressed = GETrue;
		
		if (AimHelper::IsUsingFreeAimSpell()) {
			gui2.SetCrossHairSize(0.2);
			gui2.ShowCrossHair(GETrue);
		}
		if (AimHelper::IsUsingCollisionSpell()) {
			AimHelper::DrawBoxAtAimPosition();
		}

	}
	else {
		gui2.ShowCrossHair(GEFalse);
		aimKeyPressed = GEFalse;
	}

}

mCTest::~mCTest(void) {

}

mCTest::mCTest(void) {
	eCModuleAdmin::GetInstance().RegisterModule(*this);
}

extern "C" __declspec(dllexport)
gSScriptInit const* GE_STDCALL ScriptInit(void)
{
	GetScriptAdmin().LoadScriptDLL("Script_Game.dll");
	GetScriptAdmin().LoadScriptDLL("Script_NewBalance.dll");

	if (!GetScriptAdmin().IsScriptDLLLoaded("Script_NewBalance.dll")) {
		GE_FATAL_ERROR_EX("Script_Test", "Missing Script_NewBalance.dll.");
	}

	static bCAccessorCreator Test(bTClassName<mCTest>::GetUnmangled());

	return &GetScriptInit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		::DisableThreadLibraryCalls(hModule);
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

ME_DEFINE_AND_REGISTER_SCRIPT(SayHit)
{
	UNREFERENCED_PARAMETER(a_iArgs);
	INIT_SCRIPT_EXT(Target, _);

	bTPtrArray<eCEntity*> entities = AimHelper::GetEntitiesInRadius(Target.GetPosition(), 1000);
	eCEntity* GameEntity = Target.GetGameEntity();

	std::cout << "Trafiony: "<< Target.GetName()<<"Obok: "<<entities.GetCount() << std::endl;
	
	for (auto It = entities.Begin(); It != entities.End(); It++) {
		eCEntity* ent = *It;
		if (ent == GameEntity) {
			source = GameEntity;
			continue;
		}

		if (ent->HasPropertySet(eEPropertySetType::eEPropertySetType_DamageReceiver)) {
			gCDamageReceiver_PS* pDamageRec = dynamic_cast<gCDamageReceiver_PS*>(ent->GetPropertySet(eEPropertySetType::eEPropertySetType_DamageReceiver));
			if (pDamageRec->GetHitPoints() <= 0) continue;
		}

		entitiesToShoot.Add(ent);
	}


	return 0;
}

ME_DEFINE_AND_REGISTER_SCRIPT(OnCastSpell)
{
	UNREFERENCED_PARAMETER(a_iArgs);
	INIT_SCRIPT_EXT(Target, _);
	LightningChain();

	return 0;
}


