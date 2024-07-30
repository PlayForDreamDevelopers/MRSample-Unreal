// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRSpaceMeshActor.h"

// Sets default values
AYvrXRSpaceMeshActor::AYvrXRSpaceMeshActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CustomScene"));
	RootComponent = SceneComponent;
	SceneComponent->SetVisibility(true);
}

// Called when the game starts or when spawned
void AYvrXRSpaceMeshActor::BeginPlay()
{
	Super::BeginPlay();
}


void AYvrXRSpaceMeshActor::UpdateMesh(TArray< FVector > Vertices, TArray< int32 > Triangles,  bool MeshVisible, bool CreateCollision)
{
	if (!ProceduralMesh)
	{
		ProceduralMesh = Cast<UProceduralMeshComponent>(this->AddComponentByClass(UProceduralMeshComponent::StaticClass(), false, FTransform::Identity, false));
		ProceduralMesh->SetupAttachment(RootComponent);
		ProceduralMesh->RegisterComponent();

		LineBatcher = Cast<ULineBatchComponent>(this->AddComponentByClass(ULineBatchComponent::StaticClass(), false, FTransform::Identity, false));
		LineBatcher->SetupAttachment(RootComponent);
		LineBatcher->RegisterComponent();
	}
	else {
		ProceduralMesh->ClearMeshSection(0);
		LineBatcher->Flush();
	}

	MyVertices = Vertices;
	MyTriangles = Triangles;
	MyMeshVisible = MeshVisible;
	MyCreateCollision = CreateCollision;
	ProceduralMesh->CreateMeshSection(0, MyVertices, MyTriangles, Normals, UV, VertexColors, Tangents, MyCreateCollision);
	ProceduralMesh->SetMeshSectionVisible(0, false);
	LineBatcher->SetVisibility(MyMeshVisible);

	LineBatcher->BatchedLines.SetNum(MyTriangles.Num());

	for (int i = 0; i < MyTriangles.Num(); i = i + 3)
	{
		FVector a = GetTransform().TransformPosition(MyVertices[MyTriangles[i]]);
		FVector b = GetTransform().TransformPosition(MyVertices[MyTriangles[i + 1]]);
		FVector c = GetTransform().TransformPosition(MyVertices[MyTriangles[i + 2]]);
		FBatchedLine linea(a, b, FColor(255.0f, 0.0f, 0.0f), 0, 0.1f, 0.f);
		FBatchedLine lineb(b, c, FColor(255.0f, 0.0f, 0.0f), 0, 0.1f, 0.f);
		FBatchedLine linec(a, c, FColor(255.0f, 0.0f, 0.0f), 0, 0.1f, 0.f);
		LineBatcher->BatchedLines[i] = linea;
		LineBatcher->BatchedLines[i + 1] = lineb;
		LineBatcher->BatchedLines[i + 2] = linec;
	}

	LineBatcher->MarkRenderStateDirty();

}

// Called every frame
void AYvrXRSpaceMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

