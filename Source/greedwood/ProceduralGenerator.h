// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"

#include "ProceduralGenerator.generated.h"



UCLASS()
class GREEDWOOD_API AProceduralGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties

    UPROPERTY(VisibleAnywhere, Category="Procedural")
        UProceduralMeshComponent* MeshComponent;

    AProceduralGenerator();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Procedural")
        int XSize = 100;   // number of quads along X

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Procedural")
        int YSize = 10;   // number of quads along Y

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Procedural")
        float Smoothing = 100.f; // distance between vertices

    UFUNCTION(BlueprintCallable, Category = "Procedural")
            void GenerateTerrain();

    UFUNCTION(BlueprintCallable, Category = "Procedural")
            void GenerateObjects();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
            UMaterialInterface* Material;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
            UStaticMesh* ObjectToSpawn;

    UFUNCTION(BlueprintCallable, Category="Procedural")
        float GetGroundHeightAt(FVector Location);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
            int32 NumberOfObjects = 20;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
//	    void GenerateTerrain();
//    void GenerateObjects();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
