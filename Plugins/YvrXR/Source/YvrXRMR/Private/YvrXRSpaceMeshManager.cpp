// Fill out your copyright notice in the Description page of Project Settings.


#include "YvrXrSpaceMeshManager.h"

FYvrXRSpaceMeshManager::FYvrXRSpaceMeshManager()
{
}

FYvrXRSpaceMeshManager::~FYvrXRSpaceMeshManager()
{
}


FYvrXRSpaceMeshManager* FYvrXRSpaceMeshManager::GetInstance()
{
	static FYvrXRSpaceMeshManager Instance;
	return &Instance;
}


void FYvrXRSpaceMeshManager::PollEvent(XrEventDataBuffer EventData)
{
	if (int(EventData.type) == 1000514001)
	{
		const XrEventDataMeshBlockUpdateYVR& MeshBlock = reinterpret_cast<const XrEventDataMeshBlockUpdateYVR&>(EventData);

		if (MeshBlock.changeState == XR_MESH_BLOCK_CHANGE_STATE_REMOVED_YVR) 
		{
			FYvrAnchor anchor = (uint64_t)MeshBlock.meshBlockSpace;
			SpaceMeshComponent->RemoveMesh(anchor);
		}
		else {

			PFN_xrGetSpaceTriangleMeshMETA GetSpaceTriangleMeshMETA;

			XrSpaceTriangleMeshMETA SpaceTriangleMeshMETACount;
			SpaceTriangleMeshMETACount.type = XR_TYPE_SPACE_TRIANGLE_MESH_META;
			SpaceTriangleMeshMETACount.next = nullptr;
			SpaceTriangleMeshMETACount.indexCapacityInput = 0;
			SpaceTriangleMeshMETACount.indexCountOutput = 0;
			SpaceTriangleMeshMETACount.indices = nullptr;
			SpaceTriangleMeshMETACount.vertexCapacityInput = 0;
			SpaceTriangleMeshMETACount.vertexCountOutput = 0;
			SpaceTriangleMeshMETACount.vertices = nullptr;


			XrSpaceTriangleMeshGetInfoMETA SpaceTriangleMeshGetInfoMETA;
			SpaceTriangleMeshGetInfoMETA.type = XR_TYPE_SPACE_TRIANGLE_MESH_GET_INFO_META;
			SpaceTriangleMeshGetInfoMETA.next = nullptr;

			XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrGetSpaceTriangleMeshMETA", (PFN_xrVoidFunction*)&GetSpaceTriangleMeshMETA));

			XrResult CountResult = GetSpaceTriangleMeshMETA(MeshBlock.meshBlockSpace, &SpaceTriangleMeshGetInfoMETA, &SpaceTriangleMeshMETACount);

			if (SpaceTriangleMeshMETACount.indexCountOutput > 0)
			{
				XrSpaceLocation DeviceLocation = { XR_TYPE_SPACE_LOCATION, nullptr };
				XrSpace BaseSpace = YvrXRHMD->GetTrackingSpace();
				XrSpace DeviceSpace = MeshBlock.meshBlockSpace;
				XrTime TargetTime = YvrXRHMD->GetDisplayTime();
				XrResult Results = xrLocateSpace(DeviceSpace, BaseSpace, TargetTime, &DeviceLocation);

				float WorldToMetersScale = SpaceMeshComponent->GetWorld()->GetWorldSettings()->WorldToMeters;;
				FTransform UnrealPose = ToFTransform(DeviceLocation.pose, WorldToMetersScale);
				FTransform TrackingToWorld = YvrXRHMD->GetTrackingToWorldTransform();
				FTransform ResultPose = UnrealPose;
				ResultPose.SetLocation(TrackingToWorld.TransformPosition(UnrealPose.GetLocation()));
				ResultPose.SetRotation(TrackingToWorld.TransformRotation(UnrealPose.GetRotation()));

				TArray< XrVector3f > MyVertices;
				TArray< uint32_t > MyTriangles;
				MyVertices.SetNum(SpaceTriangleMeshMETACount.vertexCountOutput);
				MyTriangles.SetNum(SpaceTriangleMeshMETACount.indexCountOutput);

				XrSpaceTriangleMeshMETA SpaceTriangleMeshMETData;
				SpaceTriangleMeshMETData.type = XR_TYPE_SPACE_TRIANGLE_MESH_META;
				SpaceTriangleMeshMETData.next = nullptr;
				SpaceTriangleMeshMETData.indexCapacityInput = SpaceTriangleMeshMETACount.indexCountOutput;
				SpaceTriangleMeshMETData.indexCountOutput = 0;
				SpaceTriangleMeshMETData.indices = MyTriangles.GetData();
				SpaceTriangleMeshMETData.vertexCapacityInput = SpaceTriangleMeshMETACount.vertexCountOutput;
				SpaceTriangleMeshMETData.vertexCountOutput = 0;
				SpaceTriangleMeshMETData.vertices = MyVertices.GetData();

				XrResult DataResult = GetSpaceTriangleMeshMETA(MeshBlock.meshBlockSpace, &SpaceTriangleMeshGetInfoMETA, &SpaceTriangleMeshMETData);

				FYvrAnchor AnchorHandle = (uint64_t)MeshBlock.meshBlockSpace;

				TArray< FVector > Vertices;
				TArray< int32 > Triangles;
				Vertices.AddZeroed(SpaceTriangleMeshMETACount.vertexCountOutput);
				Triangles.AddZeroed(SpaceTriangleMeshMETACount.indexCountOutput);

				for (uint32_t i = 0; i < SpaceTriangleMeshMETACount.vertexCountOutput; i++)
				{
					Vertices[i] = FVector(-MyVertices[i].z * WorldToMetersScale, MyVertices[i].x * WorldToMetersScale, MyVertices[i].y * WorldToMetersScale);
				}

				for (uint32_t i = 0; i < SpaceTriangleMeshMETACount.indexCountOutput; i++)
				{
					Triangles[i] = static_cast<int32>(MyTriangles[i]);
				}

				SpaceMeshComponent->UpdateMeshState( AnchorHandle, Vertices, Triangles, ResultPose);
			}

		}
	}
}

void FYvrXRSpaceMeshManager::Initialize(FYvrXRHMD* InYvrXRHMD)
{
	YvrXRHMD = InYvrXRHMD;
	if (YvrXRHMD)
	{
		YvrXRHMD->OnPollEventDelegate().AddRaw(FYvrXRSpaceMeshManager::GetInstance(), &FYvrXRSpaceMeshManager::PollEvent);
	}
}

bool FYvrXRSpaceMeshManager::CreateMeshDetector( UYvrXRSpaceMeshComponent* inSpaceMeshComponent)
{
	XrMeshDetectorCreateInfoYVR createInfo;
	createInfo.type = XR_TYPE_MESH_DETECTOR_CREATE_INFO_YVR;
	createInfo.next = nullptr;

	XrMeshDetectorYVR meshDetector = 0;
	SpaceMeshComponent = inSpaceMeshComponent;

	PFN_xrCreateMeshDetectorYVR CreateMeshDetectorYVR;

	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrCreateMeshDetectorYVR", (PFN_xrVoidFunction*)&CreateMeshDetectorYVR));
	XrResult Result = CreateMeshDetectorYVR(YvrXRHMD->GetSession(), &createInfo, &meshDetector);
	SpaceMeshComponent->meshDetector = meshDetector;

	if (XR_FAILED(Result))
	{
		return false;
	}
	return true;
}

bool FYvrXRSpaceMeshManager::DestroyMeshDetector(UYvrXRSpaceMeshComponent* inSpaceMeshComponent)
{
	PFN_xrDestroyMeshDetectorYVR DestroyMeshDetectorYVR;
	XR_ENSURE(xrGetInstanceProcAddr(YvrXRHMD->GetInstance(), "xrDestroyMeshDetectorYVR", (PFN_xrVoidFunction*)&DestroyMeshDetectorYVR));
	XrResult Result = DestroyMeshDetectorYVR(inSpaceMeshComponent->meshDetector);
	if (XR_FAILED(Result))
	{
		return false;
	}
	inSpaceMeshComponent->RemoveMeshs();
	return true;
}


EYvrResult FYvrXRSpaceMeshManager::ToYvrResult(XrResult Result)
{
	EYvrResult YvrResult = EYvrResult::Error_Unknow;
	switch (Result)
	{
	case XrResult::XR_SUCCESS:
		YvrResult = EYvrResult::Success;
		break;
	case XrResult::XR_TIMEOUT_EXPIRED:
		YvrResult = EYvrResult::TimeoutExpired;
		break;
	case XrResult::XR_ERROR_VALIDATION_FAILURE:
		YvrResult = EYvrResult::Error_ValidationFailure;
		break;
	case XrResult::XR_ERROR_RUNTIME_FAILURE:
		YvrResult = EYvrResult::Error_RuntimeFailure;
		break;
	case XrResult::XR_ERROR_OUT_OF_MEMORY:
		YvrResult = EYvrResult::Error_OutOfMemory;
		break;
	case XrResult::XR_ERROR_API_VERSION_UNSUPPORTED:
		YvrResult = EYvrResult::Error_APIVersionUnsupported;
		break;
	case XrResult::XR_ERROR_INITIALIZATION_FAILED:
		YvrResult = EYvrResult::Error_InitializationFailed;
		break;
	case XrResult::XR_ERROR_FUNCTION_UNSUPPORTED:
		YvrResult = EYvrResult::Error_FunctionUnsupported;
		break;
	case XrResult::XR_ERROR_FEATURE_UNSUPPORTED:
		YvrResult = EYvrResult::Error_FeatureUnsupported;
		break;
	case XrResult::XR_ERROR_LIMIT_REACHED:
		YvrResult = EYvrResult::Error_LimitReached;
		break;
	case XrResult::XR_ERROR_SIZE_INSUFFICIENT:
		YvrResult = EYvrResult::Error_SizeInsufficient;
		break;
	case XrResult::XR_ERROR_HANDLE_INVALID:
		YvrResult = EYvrResult::Error_HandleInvalid;
		break;
	default:
		break;
	}

	return YvrResult;
}

