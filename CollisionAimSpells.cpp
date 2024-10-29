#include "util/Memory.h"
#include "util/Logging.h"
#include "util/Hook.h"
#include "util/Util.h"
#include "Script.h"
#include <iostream>
#include "util/ScriptUtil.h"
#include "AimHelper.h"



bTPtrArray<eCEntity*> entities;

void ThunderSpell() {

	Entity player = Entity::GetPlayer();
	auto targetPos = AimHelper::GetAimPosition();
	bCSphere sphere = bCSphere(targetPos, ATTACK_RADIUS);
	eCPhysicsScene& PhysicsScene = eCPhysicsScene::GetInstance();
	PhysicsScene.CollectEntitiesInSphere(sphere, GETrue, (eCEntity*)0x0, &entities, 2, GEFalse, eEPropertySetType::eEPropertySetType_NPC, 0);

	GEU16 targetCount = 0;
	for (auto It = entities.Begin(); It != entities.End(); It++) {
		eCEntity* ent = *It;
		if (ent == GetPlayer()) continue;

		if (ent->HasPropertySet(eEPropertySetType::eEPropertySetType_DamageReceiver)) {
			gCDamageReceiver_PS* pDamageRec = dynamic_cast<gCDamageReceiver_PS*>(ent->GetPropertySet(eEPropertySetType::eEPropertySetType_DamageReceiver));
			if (pDamageRec->GetHitPoints() <= 0) continue;
		}

		if (targetCount >= MAX_TARGETS) continue;
		Entity Spell = Template("Spl_Lightningbolt");
		Entity Spawn = Spell.Magic.GetSpawn();
		auto SpawnMatrix = ent->GetWorldMatrix();
		SpawnMatrix.AccessTranslation().AccessY() += Entity::GetRandomNumber(500) + 1500;
		SpawnMatrix.AccessTranslation().AccessZ() += Entity::GetRandomNumber(3000) - 1500;
		SpawnMatrix.AccessTranslation().AccessX() += Entity::GetRandomNumber(3000) - 1500;


		Entity SpawnedEntity = Spawn.Spawn(Spawn.GetTemplate(), SpawnMatrix);
		SpawnedEntity.Interaction.SetOwner(player);
		SpawnedEntity.Interaction.SetSpell(Spell);

		SpawnedEntity.EnableCollisionWith(SpawnedEntity, GEFalse);
		SpawnedEntity.EnableCollisionWith(player, GEFalse);
		SpawnedEntity.CollisionShape.CreateShape(eECollisionShapeType_Point, eEShapeGroup_Projectile, bCVector(0, 0, 0), bCVector(0, 0, 0));
		SpawnedEntity.Projectile.SetTarget(ent);
		SpawnedEntity.Projectile.AccessProperty<PSProjectile::PropertyPathStyle>() = gEProjectilePath_Missile;
		bCVector TargetDirection = SpawnedEntity.GetDirectionTo(ent).Normalize();

		SpawnedEntity.Projectile.AccessProperty<PSProjectile::PropertyTargetDirection>() = TargetDirection;
		SpawnedEntity.Damage.AccessProperty<PSDamage::PropertyManaUsed>() = Spell.Magic.GetProperty<PSMagic::PropertyMaxManaCost>();
		SpawnedEntity.Projectile.Shoot();
		targetCount++;
		entities.Clear();
	}
}