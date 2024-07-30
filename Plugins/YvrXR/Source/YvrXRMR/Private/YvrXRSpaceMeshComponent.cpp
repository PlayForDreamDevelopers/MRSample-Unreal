// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRSpaceMeshComponent.h"
#include "openxr.h"


// Sets default values for this component's properties
UYvrXRSpaceMeshComponent::UYvrXRSpaceMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UYvrXRSpaceMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UYvrXRSpaceMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UYvrXRSpaceMeshComponent::UpdateMeshState( FYvrAnchor key, TArray< FVector > MyVertices,  TArray< int32 > MyTriangles, FTransform Base)
{
	if (Meshs.Contains(key))
	{
		Meshs[key]->SetActorTransform(Base);
		Meshs[key]->UpdateMesh(MyVertices, MyTriangles, MeshVisible, CreateCollision);
	}
	else
	{
		FActorSpawnParameters SpawnInfo;
		AYvrXRSpaceMeshActor* MyActor = (AYvrXRSpaceMeshActor*)GetWorld()->SpawnActor<AYvrXRSpaceMeshActor>();
		MyActor->SetActorTransform(Base);
		MyActor->UpdateMesh(MyVertices, MyTriangles, MeshVisible, CreateCollision);
		Meshs.Add(key, MyActor);
	}
}

void UYvrXRSpaceMeshComponent::RemoveMeshs()
{
	for (TMap<FYvrAnchor, AYvrXRSpaceMeshActor*>::TConstIterator iter = Meshs.CreateConstIterator(); iter; ++iter)
	{
		iter->Value->Destroy();
	}
	Meshs.Empty();
}

void UYvrXRSpaceMeshComponent::RemoveMesh(FYvrAnchor key)
{

	if (Meshs.Contains(key))
	{
		Meshs[key]->Destroy();
		Meshs.Remove(key);
	}
}
