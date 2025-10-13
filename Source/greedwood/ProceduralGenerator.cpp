#include "ProceduralGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ProceduralMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/BodySetup.h"

// Sets default values
AProceduralGenerator::AProceduralGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    RootComponent = MeshComponent;

    // FIX: Enable synchronous cooking to ensure collision is ready before GenerateObjects() is called
    MeshComponent->bUseAsyncCooking = false;
}

// Called when the game starts or when spawned
void AProceduralGenerator::BeginPlay()
{
	Super::BeginPlay();
	GenerateTerrain();
}

// Called every frame
void AProceduralGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AProceduralGenerator::GetGroundHeightAt(FVector Location)
{
    float GridX = Location.X / Smoothing;
    float GridY = Location.Y / Smoothing;

    // FIX: Removed RandomZScale to match terrain generation (now deterministic)
    float dNoise = FMath::PerlinNoise2D(FVector2D(GridX * 0.2f, GridY * 0.2f)) * 10.f;
    float originalNoise = FMath::PerlinNoise2D(FVector2D(GridX * 0.05f, GridY * 0.05f)) * 50.f;
    return dNoise + originalNoise;
}

void AProceduralGenerator::GenerateObjects()
{
    if (ObjectsToSpawn.IsEmpty()) return;
    if (NumberOfObjectsPerType.Num() != ObjectsToSpawn.Num()) return;
    UE_LOG(LogTemp, Warning, TEXT("***********Starting Object Generation....**************"));
    int32 Counter = 0;
    for (const TSoftObjectPtr<UStaticMesh>& MeshPtr : ObjectsToSpawn)
    {
        // load individual static mesh --> maybe consider making Actor static mesh in future
        UStaticMesh* ObjectToSpawn = MeshPtr.LoadSynchronous();
        if (!ObjectToSpawn) continue;

        UE_LOG(LogTemp, Warning, TEXT("Spawning Object: %s"), *ObjectToSpawn->GetName());
		FString MeshName = ObjectToSpawn->GetName();
        TArray<FVector> SpawnedPositions; // Track spawned positions to avoid overlap
        int32 NumberOfObjects = NumberOfObjectsPerType[Counter++];
		int32 RandomInt = FMath::RandRange(0,100);
		int32 NewNumberOfObjects = NumberOfObjects + RandomInt;
        UE_LOG(LogTemp, Warning, TEXT("Number of Objects to Spawn: %d"), NewNumberOfObjects);
        for (int32 i = 0; i < NewNumberOfObjects; i++)
        {
            // Generate random position within terrain bounds

            float RandX = FMath::FRandRange(0.f, XSize * Smoothing ) ;
            float RandY = FMath::FRandRange(0.f, YSize * Smoothing );

            // Calculate height using the same noise function as terrain
            float GridX = RandX / Smoothing;
            float GridY = RandY / Smoothing;
            float dNoise = FMath::PerlinNoise2D(FVector2D(GridX * 0.2f, GridY * 0.2f)) * 10.f;
            float originalNoise = FMath::PerlinNoise2D(FVector2D(GridX * 0.05f, GridY * 0.05f)) * 50.f;
            float TerrainHeight = dNoise + originalNoise;

            // Create spawn location in world space
            FVector SpawnLocation = GetActorLocation() + FVector(RandX, RandY, TerrainHeight);

            // Adjust for mesh pivot
            float PivotOffset = FMath::Abs(ObjectToSpawn->GetBoundingBox().Min.Z);
            SpawnLocation.Z += PivotOffset;

            // Check if too close to existing objects
            bool bTooClose = false;
            for (const FVector& ExistingPos : SpawnedPositions)
            {
                if (FVector::Dist2D(ExistingPos, SpawnLocation) < 200.f) // 200 unit minimum spacing
                {
                    bTooClose = true;
                    break;
                }
            }

            if (bTooClose)
            {
                i--; // Retry this spawn
                continue;
            }

            // Spawn the actor
            FRotator SpawnRotation = FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f);
            FActorSpawnParameters SpawnParams;
            AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

            if (MeshActor && MeshActor->GetStaticMeshComponent())
            {

                UStaticMeshComponent* MeshComp = MeshActor->GetStaticMeshComponent();

                // 1. Set mobility FIRST (before anything else)

                MeshComp->SetMobility(EComponentMobility::Movable);
                UE_LOG(LogTemp, Warning, TEXT("Mesh Actor: %s,"), *MeshName);


                // 2. NOW set the mesh
                MeshComp->SetStaticMesh(ObjectToSpawn);

                // 3. Set collision
                MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                MeshComp->SetCollisionResponseToAllChannels(ECR_Block);

                // 4. NO RegisterComponent call - it's already registered!

                SpawnedPositions.Add(SpawnLocation);

                DrawDebugSphere(GetWorld(), SpawnLocation, 50.f, 12, FColor::Green, false, 10.f);

				// Set Tag to "EnemyCover"

         		if (MeshName.Contains("Tree"))
				{
					MeshActor->Tags.Add(FName("EnemyCover"));
					MeshActor->Tags.Add(FName("WindObject")); // these are so bp can find tree to wave leaves
					UE_LOG(LogTemp, Warning, TEXT("Tree Tag Set...."));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("No Tree Tag Set for Mesh Name: %s"), *MeshName);
				}

                UE_LOG(LogTemp, Log, TEXT("Mesh Set...."));

//
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("***********Ending Object Generation....**************"));
}



void AProceduralGenerator::GenerateTerrain()
{
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FProcMeshTangent> Tangents;
    TArray<FColor> VertexColors;

    UE_LOG(LogTemp, Warning, TEXT("******Procedural Generation Started for %s *******"), *GetName());
    UE_LOG(LogTemp, Warning, TEXT("Actor Location: %s"), *GetActorLocation().ToString());
    UE_LOG(LogTemp, Warning, TEXT("XSize: %d, YSize: %d, Smoothing: %f"), XSize, YSize, Smoothing);

    // Generate vertices
    for (int y = 0; y <= YSize; y++)
    {
        for (int x = 0; x <= XSize; x++)
        {
            // FIX: Removed RandomZScale to make terrain generation deterministic
            // This ensures GetGroundHeightAt() returns the same value as the actual terrain
            // float dNoise = FMath::PerlinNoise2D(FVector2D(x * 0.2f, y * 0.2f)) * 10.f;
            // float originalNoise = FMath::PerlinNoise2D(FVector2D(x * 0.05f, y * 0.05f)) * 50.f;
            float RandomZScale = FMath::FRandRange(0.f, 100.f);
			float SudoSmoothing = Smoothing ;
			float dNoise = FMath::PerlinNoise2D(FVector2D((x * SudoSmoothing + GetActorLocation().X) * 0.05f, (y * SudoSmoothing  + GetActorLocation().Y) * 0.05f)) * 5.f;
			float originalNoise = FMath::PerlinNoise2D(FVector2D((x * SudoSmoothing + GetActorLocation().X) * 0.01f, (y * SudoSmoothing + GetActorLocation().Y) * 0.01f)) * 20.f;
			float Z = (dNoise + originalNoise);

            Vertices.Add(FVector(x * SudoSmoothing + GetActorLocation().X, y * SudoSmoothing + GetActorLocation().Y, Z));
			FVector2D UV = FVector2D(x * SudoSmoothing + GetActorLocation().X, y * SudoSmoothing + GetActorLocation().Y);
			UVs.Add(UV);
            //UVs.Add(FVector2D((float)x / XSize, (float)y / YSize));
        }
    }

    // Generate triangles
    for (int y = 0; y < YSize; y++)
    {
        for (int x = 0; x < XSize; x++)
        {
            int i0 = y * (XSize + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (XSize + 1);
            int i3 = i2 + 1;

            Triangles.Add(i0);
            Triangles.Add(i2);
            Triangles.Add(i1);

            Triangles.Add(i1);
            Triangles.Add(i2);
            Triangles.Add(i3);
        }
    }

    // Build mesh
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

    // FIX: Set collision properties BEFORE creating mesh section
    MeshComponent->bUseComplexAsSimpleCollision = true;
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
    MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);

    // Now create the mesh section
    MeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
    UE_LOG(LogTemp, Warning, TEXT("%s: Created mesh with %d vertices, %d triangles"), *GetName(), Vertices.Num(), Triangles.Num() / 3);

    // Update bounds
    MeshComponent->UpdateBounds();

    // Force physics state recreation
    MeshComponent->RecreatePhysicsState();


    // Debug logging
    UBodySetup* BodySetup = MeshComponent->GetBodySetup();
    if (BodySetup)
    {
        UE_LOG(LogTemp, Warning, TEXT("BodySetup exists. TriMeshGeometries count: %d"), BodySetup->TriMeshGeometries.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BodySetup is NULL after collision setup!"));
    }

    // Apply material
    if (Material)
    {
        MeshComponent->SetMaterial(0, Material);
        UE_LOG(LogTemp, Warning, TEXT("%s: Material applied"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s: NO MATERIAL SET - Terrain will be invisible!"), *GetName());
    }

    // Place player above terrain
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerCharacter)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        float GroundZ = GetGroundHeightAt(PlayerLocation);
        float PlayerHeight = PlayerCharacter->GetSimpleCollisionHalfHeight();

        PlayerCharacter->SetActorLocation(FVector(PlayerLocation.X, PlayerLocation.Y, GroundZ + PlayerHeight + 10.f));
    }

    // Spawn objects after collision is ready
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AProceduralGenerator::GenerateObjects, 0.5f, false);
}




//void AProceduralGenerator::GenerateObjects()
//{
//    // Implement object generation logic here
//
//    // Logic here will spawn objects like trees, rocks, etc. at random or predefined locations
//}

