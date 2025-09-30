// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ProceduralMeshComponent.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AProceduralGenerator::AProceduralGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    RootComponent = MeshComponent;
//    Mesh->bUseAsyncCooking = true;
}

// Called when the game starts or when spawned
void AProceduralGenerator::BeginPlay()
{
	Super::BeginPlay();
	GenerateTerrain();
//	GenerateObjects();
	
}

// Called every frame
void AProceduralGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AProceduralGenerator::GetGroundHeightAt(FVector Location)
{
    FHitResult HitResult;
    FVector Start = FVector(Location.X, Location.Y, 10000.f);  // way above terrain
    FVector End   = FVector(Location.X, Location.Y, -10000.f); // way below terrain

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this); // ignore self

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, Params))
    {
        return HitResult.Location.Z;
    }

    return 0.f;
}

void AProceduralGenerator::GenerateObjects()
{
   if (!ObjectToSpawn) return;

   // DON'T multiply by 10! Use the actual terrain size
   float MaxX = XSize * Smoothing;  // This gives you 100 * 10 = 1000
   float MaxY = YSize * Smoothing;  // This gives you 100 * 10 = 1000

   for (int32 i = 0; i < NumberOfObjects; i++)
   {
      float RandX = FMath::FRandRange(0.f, MaxX);
      float RandY = FMath::FRandRange(0.f, MaxY);

      FHitResult HitResult;
      FVector Start = FVector(RandX, RandY, 10000.f);
      FVector End   = FVector(RandX, RandY, -10000.f);

      FCollisionQueryParams Params;
      Params.AddIgnoredActor(this);

      if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, Params))
      {
          float GroundZ = HitResult.Location.Z;

          // Your pivot is at the bottom (Min Z = -20), so offset UP by that amount
          float PivotOffset = FMath::Abs(ObjectToSpawn->GetBoundingBox().Min.Z);

          FVector SpawnLocation = FVector(RandX, RandY, GroundZ + PivotOffset);
          FRotator SpawnRotation = FRotator::ZeroRotator;

          AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnLocation, SpawnRotation);
          if (MeshActor)
          {
              MeshActor->GetStaticMeshComponent()->SetStaticMesh(ObjectToSpawn);
              MeshActor->SetMobility(EComponentMobility::Movable);
          }
      }
      else
      {
          UE_LOG(LogTemp, Error, TEXT("Line trace failed at X:%f Y:%f - Outside terrain bounds (0-%.0f)!"),
                 RandX, RandY, MaxX);
      }
   }
}


void AProceduralGenerator::GenerateTerrain()
{
    // Implement level generation logic here

    // Logic here will set vertices, triangles, UVs, and normals for the procedural mesh

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FProcMeshTangent> Tangents;
    TArray<FColor> VertexColors;



//    int XSize = 100;   // number of quads along X
//    int YSize = 100;   // number of quads along Y
//    float Smoothing = 100.f; // distance between vertices
    // Sample Log To INIT
    UE_LOG(LogTemp, Warning, TEXT("******Procedural Generation Started*******"));
    // log the sizes from BP
    UE_LOG(LogTemp, Warning, TEXT("XSize: %d, YSize: %d, Smoothin: %f"), XSize, YSize, Smoothing);

    // Generate vertices
    for (int y = 0; y <= YSize; y++)
    {
        for (int x = 0; x <= XSize; x++)
        {
            float RandomZScale = FMath::FRandRange(-0.1f, 1.0f);

            float Z = FMath::PerlinNoise2D(FVector2D(x * 0.1f, y * 0.1f)) * (200.f * RandomZScale); // this is the height and its smoothed with PErlin
            Vertices.Add(FVector(x * Smoothing, y *Smoothing, Z));
            UVs.Add(FVector2D((float)x / XSize, (float)y / YSize));
        }
    }

    // Generate triangles --> Note to self: These are how the vertices are connected to form the mesh
    for (int y = 0; y < YSize; y++)
    {
        for (int x = 0; x < XSize; x++)
        {
            int i0 = y * (XSize + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (XSize + 1);
            int i3 = i2 + 1;

            // First triangle
            Triangles.Add(i0);
            Triangles.Add(i2);
            Triangles.Add(i1);

            // Second triangle
            Triangles.Add(i1);
            Triangles.Add(i2);
            Triangles.Add(i3);
        }
    }

    // Create mesh section
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

    MeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
    MeshComponent->SetMaterial(0, Material);

    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
    // Adjust Character Heigh

    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerCharacter)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        float GroundZ = GetGroundHeightAt(PlayerLocation);
        float PlayerHeight = PlayerCharacter->GetSimpleCollisionHalfHeight();

        PlayerCharacter->SetActorLocation(FVector(PlayerLocation.X, PlayerLocation.Y, GroundZ + PlayerHeight));
    }

    // After generating terrain, generate objects
    GenerateObjects();


}



//void AProceduralGenerator::GenerateObjects()
//{
//    // Implement object generation logic here
//
//    // Logic here will spawn objects like trees, rocks, etc. at random or predefined locations
//}

