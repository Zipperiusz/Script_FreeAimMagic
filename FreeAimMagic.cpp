#include "util/Memory.h"
#include "util/Logging.h"
#include "util/Hook.h"
#include "util/Util.h"
#include "Script.h"
#include <iostream>
#include "util/ScriptUtil.h"
#include "AimHelper.h"





void FireSpell() {
	Entity player = Entity::GetPlayer();
	auto targetPos = AimHelper::GetAimPosition();

	bCMatrix bonePose = player.Inventory.GetItemFromSlot(gESlot_RightHand).GetPose();
	Entity Spell = Template("Spl_Fireball");
	Entity Spawn = Spell.Magic.GetSpawn();
	Entity SpawnedEntity = Spawn.Spawn(Spawn.GetTemplate(), bonePose);

	SpawnedEntity.Interaction.SetOwner(player);
	SpawnedEntity.Interaction.SetSpell(Spell);

	SpawnedEntity.EnableCollisionWith(SpawnedEntity, GEFalse);
	SpawnedEntity.EnableCollisionWith(player, GEFalse);
	SpawnedEntity.CollisionShape.CreateShape(eECollisionShapeType_Point, eEShapeGroup_Projectile, bCVector(0, 0, 0), bCVector(0, 0, 0));
	SpawnedEntity.Projectile.AccessProperty<PSProjectile::PropertyPathStyle>() = gEProjectilePath_Missile;
	bCVector TargetDirection = AimHelper::GetAimDirection();

	SpawnedEntity.Projectile.AccessProperty<PSProjectile::PropertyTargetDirection>() = TargetDirection;
	SpawnedEntity.Damage.AccessProperty<PSDamage::PropertyManaUsed>() = Spell.Magic.GetProperty<PSMagic::PropertyMaxManaCost>();
	SpawnedEntity.Projectile.Shoot();

}

void LightningChain() {
	Entity player = Entity::GetPlayer();
	auto targetPos = AimHelper::GetAimPosition();

	bCMatrix bonePose = player.Inventory.GetItemFromSlot(gESlot_RightHand).GetPose();
	Entity Spell = Template("Spl_FreeAim_LightningChain");
	Entity Spawn = Spell.Magic.GetSpawn();
	Entity SpawnedEntity = Spawn.Spawn(Spawn.GetTemplate(), bonePose);

	SpawnedEntity.Interaction.SetOwner(player);
	SpawnedEntity.Interaction.SetSpell(Spell);

	SpawnedEntity.EnableCollisionWith(SpawnedEntity, GEFalse);
	SpawnedEntity.EnableCollisionWith(player, GEFalse);
	SpawnedEntity.CollisionShape.CreateShape(eECollisionShapeType_Point, eEShapeGroup_Projectile, bCVector(0, 0, 0), bCVector(0, 0, 0));
	SpawnedEntity.Projectile.AccessProperty<PSProjectile::PropertyPathStyle>() = gEProjectilePath_Missile;
	bCVector TargetDirection = AimHelper::GetAimDirection();

	SpawnedEntity.Projectile.AccessProperty<PSProjectile::PropertyTargetDirection>() = TargetDirection;
	SpawnedEntity.Damage.AccessProperty<PSDamage::PropertyManaUsed>() = Spell.Magic.GetProperty<PSMagic::PropertyMaxManaCost>();
	SpawnedEntity.Projectile.Shoot();
}