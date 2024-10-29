
#include "util/Memory.h"
#include "util/ScriptUtil.h"
#include <iostream>
#include "util/Logging.h"
#include "util/Hook.h"
#include "util/Util.h"
#include "AimHelper.h"

bCVector AimHelper::GetAimPosition() {
	eCEntity* pPlayer = gCSession::GetInstance().GetPlayer();
	auto player = Entity::GetPlayer();
	auto playerPos = player.GetPosition();
	playerPos.AccessY() += 130;

	eCCameraBase& cam = eCApplication::GetInstance().GetCurrentCamera();
	bCVector cameraDirection = cam.GetViewMatrix().GetZAxis();
	cameraDirection.AccessX() = -cameraDirection.GetX();
	bCVector lookDirection = bCVector(cameraDirection.AccessX(), cam.GetRotation().GetX(), cameraDirection.AccessZ()).Normalize();
	eCPhysicsScene& PhysicsScene = eCPhysicsScene::GetInstance();
	if (PhysicsScene.TraceRayNearestHit(playerPos, lookDirection, pPlayer, eETraceRayHints_TestStaticShapes | eETraceRayHints_TestDynamicShapes | eETraceRayHints_CreatureClassPlayerOrNPC | eETraceRayHints_TestIfHasItemPropertySet | eETraceRayHints_FaceNormal | eETraceRayHints_Normal | eETraceRayHints_Impact | eETraceRayHints_Distance | eETraceRayHints_MaterialIndex | eETraceRayHints_UserData | eETraceRayHints_MaxDistanceNotFromDirection | eETraceRayHints_IncludeCollisionGroup17 | eETraceRayHints_IncludeWater)) {
		eCPhysicQueryReport& report = PhysicsScene.GetQueryReport();
		bCVector impact = report.m_NearestWorldImpact;
		GEFloat distance = report.m_NearestDistance;
		if (distance > 50 && distance < MAX_DISTANCE) {
			return impact;
		}
	}
	return bCVector(0, 0, 0);
}

bCVector AimHelper::GetAimDirection() {
	eCEntity* pPlayer = gCSession::GetInstance().GetPlayer();
	auto player = Entity::GetPlayer();
	auto playerPos = player.GetPosition();
	playerPos.AccessY() += 130;

	eCCameraBase& cam = eCApplication::GetInstance().GetCurrentCamera();
	bCVector cameraDirection = cam.GetViewMatrix().GetZAxis();
	cameraDirection.AccessX() = -cameraDirection.GetX();
	bCVector lookDirection = bCVector(cameraDirection.AccessX(), cam.GetRotation().GetX(), cameraDirection.AccessZ()).Normalize();
	return lookDirection;
}


GEBool AimHelper::ValidAimPosition(bCVector vector) {
	if (vector == bCVector(0, 0, 0)) return false;
	return true;
}

bTPtrArray<eCEntity*> AimHelper::GetEntitiesInRadius(bCVector position, GEI32 radius) {
	bTPtrArray<eCEntity*> entities;
	bCSphere sphere = bCSphere(position, radius);
	eCPhysicsScene& PhysicsScene = eCPhysicsScene::GetInstance();
	PhysicsScene.CollectEntitiesInSphere(sphere, GETrue, (eCEntity*)0x0, &entities, 2, GEFalse, eEPropertySetType::eEPropertySetType_NPC, 0);
	return entities;
}


void AimHelper::DrawSphereAtAimPosition() {
	eCGfxAdmin* admin = FindModule<eCGfxAdmin>();
	if (admin) {
		bCMatrix wMat;
		eCGfxMixerAdmin& mixerAdmin = admin->GetAccessToMixer();
		mixerAdmin.BeginScene();
		mixerAdmin.GetWorldMatrix(wMat);
		auto pos = AimHelper::GetAimPosition();
		if (AimHelper::ValidAimPosition(pos)) {
			bCSphere sphere = bCSphere(pos, 20);
			mixerAdmin.DrawSphere(sphere, 4278190335, wMat, GEFalse, GEFalse, GEFalse);
		}
		mixerAdmin.EndScene();
	}
}

void AimHelper::DrawSphereAroundFocus(Entity focus, GEI32 radius) {
	if (focus.NPC.IsValid()) {
		std::cout << focus.GetName() << std::endl;

		eCGfxAdmin* admin = FindModule<eCGfxAdmin>();
		if (admin) {
			bCMatrix wMat;
			eCGfxMixerAdmin& mixerAdmin = admin->GetAccessToMixer();
			mixerAdmin.BeginScene();
			mixerAdmin.GetWorldMatrix(wMat);
			auto pos = focus.GetPosition();
			if (AimHelper::ValidAimPosition(pos)) {
				bCSphere sphere = bCSphere(pos, radius);
				mixerAdmin.DrawSphere(sphere, 4278190335, wMat, GEFalse, GEFalse, GEFalse);
			}
			mixerAdmin.EndScene();
		}
	}
}


void AimHelper::DrawBoxAtAimPosition() {
	eCGfxAdmin* admin = FindModule<eCGfxAdmin>();
	if (admin) {
		bCMatrix wMat;
		eCGfxMixerAdmin& mixerAdmin = admin->GetAccessToMixer();
		mixerAdmin.BeginScene();
		mixerAdmin.GetWorldMatrix(wMat);
		auto pos = AimHelper::GetAimPosition();
		if (AimHelper::ValidAimPosition(pos)) {
			bCBox box = bCBox(pos, 100);
			//mixerAdmin.DrawBox(box, 4278190335, wMat, GEFalse, GEFalse, GEFalse);
			mixerAdmin.DrawCircle(pos, 50, bCVector(0, -1, 0), 4278190335, wMat, GEFalse, GEFalse, GEFalse);
		}
		mixerAdmin.EndScene();
	}
}

void AimHelper::TeleportToAimPosition() {
	eCEntity* pPlayer = gCSession::GetInstance().GetPlayer();
	auto pos = AimHelper::GetAimPosition();
	if (AimHelper::ValidAimPosition(pos)) {
		pPlayer->SetWorldPosition(pos);
	}
}



GEBool AimHelper::IsUsingFreeAimSpell() {
	GEBool res = GEFalse;
	bCString handItemName = Entity::GetPlayer().Inventory.GetItemFromSlot(gESlot_RightHand).GetName();
	if (
		handItemName == "It_Spell_FreeAim_Lightningbolt" ||
		handItemName == "It_Spell_FreeAim_LightningChain"
		) res = GETrue;
	return res;
}
GEBool AimHelper::IsUsingCollisionSpell() {
	GEBool res = GEFalse;
	bCString handItemName = Entity::GetPlayer().Inventory.GetItemFromSlot(gESlot_RightHand).GetName();
	if (
		handItemName == "It_Spell_Collision_Teleport"
		) res = GETrue;
	return res;
}