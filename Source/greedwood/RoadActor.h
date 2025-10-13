// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/SplineComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoadActor.generated.h"


UCLASS()
class GREEDWOOD_API ARoadActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoadActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spline")
		USplineComponent* RoadSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Road")
		UStaticMesh* RoadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Road")
		UMaterialInterface* RoadMaterial;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Road")
	void GenerateRoad();

# if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
# endif


};
