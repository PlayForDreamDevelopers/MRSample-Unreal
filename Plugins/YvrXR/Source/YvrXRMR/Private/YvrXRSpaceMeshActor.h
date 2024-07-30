// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/LineBatchComponent.h"

#include "YvrXRSpaceMeshActor.generated.h"

UCLASS()
class AYvrXRSpaceMeshActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AYvrXRSpaceMeshActor();
	void UpdateMesh(TArray< FVector > Vertices, TArray< int32 > Triangles, bool MeshVisible, bool CreateCollision);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UProceduralMeshComponent* ProceduralMesh;
	TArray< FVector > MyVertices;
	TArray< int32 > MyTriangles;
	TArray< FVector > Normals;
	TArray< FVector2D > UV;
	TArray< FColor > VertexColors;
	TArray< FProcMeshTangent > Tangents;

	bool MyMeshVisible = true;
	bool MyCreateCollision = false;

	ULineBatchComponent* LineBatcher;

};
