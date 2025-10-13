// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadActor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"


// Sets default values
ARoadActor::ARoadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RoadSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RoadSpline"));
	RootComponent = RoadSpline;

}

// Called when the game starts or when spawned
void ARoadActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoadActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#if WITH_EDITOR
void ARoadActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Remove old spline meshes so it doesn’t duplicate
	TArray<UActorComponent*> OldMeshes;
	GetComponents(USplineMeshComponent::StaticClass(), OldMeshes);
	for (UActorComponent* Comp : OldMeshes)
	{
		Comp->DestroyComponent();
	}

	// Build road again
	GenerateRoad();
}
#endif

void ARoadActor::GenerateRoad()
{
	if (!RoadMesh) return;
	const int32 NumPoints = RoadSpline->GetNumberOfSplinePoints();
	UE_LOG(LogTemp, Warning, TEXT("NumPoints For Spline Mesh: %d"), NumPoints);
	for (int32 i = 0; i < NumPoints - 1; i++)
	{
		FVector StartPos, StartTangent, EndPos, EndTangent;
		StartPos = RoadSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		StartTangent = RoadSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
        EndPos = RoadSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		EndTangent = RoadSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		// Create Mesh
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);


		// set Mobility First before set mesh
		SplineMesh->SetMobility(EComponentMobility::Movable);


		SplineMesh->SetStaticMesh(RoadMesh);

		if (RoadMaterial)
		{
			SplineMesh->SetMaterial(0, RoadMaterial);
		}

		SplineMesh->RegisterComponentWithWorld(GetWorld());
		SplineMesh->AttachToComponent(RoadSpline, FAttachmentTransformRules::KeepRelativeTransform);

		SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);

		UE_LOG(LogTemp, Warning, TEXT("SplineMesh Set: %s"), *SplineMesh->GetName());



	}

}

