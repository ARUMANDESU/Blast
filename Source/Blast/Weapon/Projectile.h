
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLAST_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	
protected:
	
	virtual void BeginPlay() override;

public:	
	
	

};
