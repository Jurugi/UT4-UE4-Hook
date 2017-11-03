/*
Unreal Tournament 4 UE4 Hook

// gObjects
// 48 8D 0D ?? ?? ?? ?? C6

// gNames
// 48 89 1D ?? ?? ?? ?? 48 8B 5C 24 20
*/

// Includes
#include "Main.h"
#include "Logging.h"
#include "Signatures.h"
#include "Misc.h"
#include "SDK.hpp"

// Globals
HMODULE g_hInst = 0;
unsigned long long dwGObjects = 0;
unsigned long long dwGNames = 0;
gUnrealInfo gUE;

// Custom
void SetRotation(UCanvas* Canvas, AController* Target, FVector AimLocation)
{
	FRotator AimRotation;
//	FRotator AimRotation_Temp;
	FVector  MyLocation = FVector();
	FRotator MyRotation = FRotator();
	FVector Velocity = FVector();
	FVector Difference = FVector();

	if (!Target || !gUE.gPlayerController || !gUE.gPlayerController->PlayerCameraManager) return;

	// Get player location and rotator
	MyLocation = gUE.gPlayerController->PlayerCameraManager->CameraCache.POV.Location;
	MyRotation = gUE.gPlayerController->PlayerCameraManager->CameraCache.POV.Rotation;

	// Check angles
	//float ViewAngle = GetViewAngle(Canvas, Target->Location, MyLocation, MyRotation);
	//if (!CheckAngle(ViewAngle, gUE.gPlayerController->FOVAngle / 5.0f)) return;

	// Check visibility
	if (!Target->Character->Mesh->IsVisible()) return;

	// Accuracy adjustments
	float fDistance;
	Difference = VectorSubtract(AimLocation, MyLocation);
	fDistance = gUE.gMath->STATIC_VSize(Difference);
	float TimeScale = 0.0193f;
	short Ping = gUE.gPlayerController->Pawn->PlayerState->Ping * 4;
	float ScaleValue = (float)(Ping) + (float)(fDistance * TimeScale); // most bullets here are just projectiles, so do a bit of velocity tweaking
	if (ScaleValue < 1.0f) ScaleValue = 1.0;
	float fVelocity;
	Velocity = Target->Pawn->GetVelocity();
	fVelocity = gUE.gMath->STATIC_VSize(Velocity);
	if (fVelocity > 100.0f)
	{
		Velocity.X *= ScaleValue / fVelocity; Velocity.Y *= ScaleValue / fVelocity; Velocity.Z *= ScaleValue / fVelocity;
		AimLocation = VectorAdd(AimLocation, Velocity);
	}

	AimRotation = gUE.gMath->STATIC_FindLookAtRotation(MyLocation, AimLocation);

	//gUE.gPlayerController->PlayerCameraManager->ViewTarget.POV.Rotation = AimRotation;
	gUE.gPlayerController->UTClientSetRotation(AimRotation);
}

// Hooks
typedef signed int(__thiscall *tProcessEvent) (UObject*, UFunction*, void*, void* );
tProcessEvent oProcessEvent = NULL;
signed int __stdcall hProcessEvent(UObject* pCallObject, UFunction* pUFunc, void* pParms, void* pResult)
{
	if (pUFunc)
	{
		//add_log("0x%p, 0x%p %s (%i)(%i), %s (%i)(%i), 0x%p, 0x%p", pCallObject, pUFunc, pUFunc->GetFullName(), pUFunc->Name.Index, pUFunc->InternalIndex, pCallObject->GetFullName(), pCallObject->Name.Index, pCallObject->InternalIndex, pParms, pResult);
		if(pCallObject->Name.Index == 38040) // UTPlayerController, not really using this, it's from my UE3 base.. but helps to tell when your player has fully loaded
		{
			if (pUFunc->Name.Index == 1484 && (DWORD64)gUE.gPlayerController != (DWORD64)pCallObject) // .ReceiveTick
				gUE.gPlayerController = (AUTPlayerController*)pCallObject;
			else if (pUFunc->Name.Index == 2604 && (DWORD64)gUE.gPlayerController == (DWORD64)pCallObject) // .ReceiveDestroyed
				gUE.gPlayerController = 0;
		}
	}

	return oProcessEvent(pCallObject, pUFunc, pParms, pResult);
}

UObject* GetInstanceOf(UClass* Class)
{
	static UObject* ObjectInstance;

	ObjectInstance = NULL;

	for (int i = 0; i < UObject::GObjects->ObjObjects.Num(); ++i)
	{
		UObject* CheckObject = UObject::GObjects->ObjObjects.GetByIndex(i);

		if (CheckObject && CheckObject->IsA(Class))
		{
			if (!strstr(CheckObject->GetFullName(), "Default"))
				ObjectInstance = CheckObject;
		}
	}

	return ObjectInstance;
};

void PostRender(UObject* aUTGameViewportClient, UCanvas* pCanvas)
{
	if (!pCanvas) return;

	if (!gUE.gEngine)
	{
		gUE.gEngine = (UEngine*)GetInstanceOf((UClass*)UEngine::StaticClass());
		return;
	}
	if (!gUE.gGameplayStatics) gUE.gGameplayStatics = (UGameplayStatics*)UGameplayStatics::StaticClass();
	if (!gUE.gMath) gUE.gMath = (UKismetMathLibrary*)UKismetMathLibrary::StaticClass();

	static auto* FilterClass = AController::StaticClass();
	static auto* UWorldClass = UWorld::StaticClass();

	if (GetAsyncKeyState(VK_INSERT) & 1) gUE.Initialize = !gUE.Initialize;
	if (!gUE.Initialize) return;

	if (gUE.gPlayerController && gUE.gGameplayStatics && FilterClass)
	{
		TArray<AActor*> FoundActors;
		if(!gUE.GWorld) gUE.GWorld = (UWorld*)GetInstanceOf(UWorldClass); // Not sure if I can improve this
		gUE.gGameplayStatics->STATIC_GetAllActorsOfClass(gUE.GWorld, FilterClass, &FoundActors); // Or we can try UPlayerControllerClass.GetAllChildActors

		for (int i = 0; i < FoundActors.Num(); i++)
		{
			AController* Temp = (AController*)FoundActors[i];
			if (Temp == gUE.gPlayerController) continue;

			if (Temp && Temp->Pawn)
			{
				FVector ActorLoc = Temp->Pawn->K2_GetActorLocation();
				FVector TempV = pCanvas->K2_Project(ActorLoc);
				FVector2D Size(64.0f, 64.0f);
				FVector2D Pos(TempV.X - Size.X / 2, TempV.Y - Size.Y / 2);
				//pCanvas->DrawColor doesn't work
				pCanvas->K2_DrawBox(Pos, Size, 2.0f);
				
				//Aimbot placeholder
				if (GetAsyncKeyState(0x02)) SetRotation(pCanvas, Temp, ActorLoc);
			}
		}
	}
}

typedef void(__thiscall *tPostRender) (UObject*, UCanvas*);
tPostRender oPostRender = NULL;
void __stdcall hPostRender(UObject* aUTGameViewportClient, UCanvas* pCanvas)
{
	PostRender(aUTGameViewportClient, pCanvas);
	return oPostRender(aUTGameViewportClient, pCanvas);
}

DWORD Initialize()
{
	//Get signatures
	LoadSignatures();
	dwGObjects = dwSignatures[0] + 7 + *(DWORD*)(dwSignatures[0] + 3);
	dwGNames = *(DWORD64*)(dwSignatures[1] + 7 + (*(DWORD*)(dwSignatures[1] + 3) - 0x8));
	gUE.gProcessEvent = dwSignatures[2];

	//Log signatures
	/*DWORD64 ModuleAddress = (DWORD64)GetModuleHandle(0);
	add_log("gObjects: 0x%llx (+0x%llx)", dwSignatures[0] + 7 + *(DWORD*)(dwSignatures[0] + 3), dwSignatures[0] + 7 + *(DWORD*)(dwSignatures[0] + 3) - ModuleAddress);
	add_log("gNames: 0x%llx (+0x%llx)", *(DWORD64*)(dwSignatures[1] + 7 + (*(DWORD*)(dwSignatures[1] + 3)) - 0x8), *(DWORD64*)(dwSignatures[1] + 7 + (*(DWORD*)(dwSignatures[1] + 3)) - 0x8) - ModuleAddress);
	add_log("ProcessEvent: 0x%llx (+0x%llx)", dwSignatures[2], dwSignatures[2] - ModuleAddress);
	add_log("PostRender: 0x%llx (+0x%llx)", dwSignatures[3], dwSignatures[3] - ModuleAddress);*/

	//Hook ProcessEvent
	oProcessEvent = (tProcessEvent)DetourFunc64((BYTE*)dwSignatures[2],(BYTE*)hProcessEvent,19);

	//Hook PostRender
	oPostRender = (tPostRender)DetourFunc64((BYTE*)dwSignatures[3], (BYTE*)hPostRender, 19);

	// Get global UWorld pointer (useless)
	//gUE.GWorld = (UWorld*)(dwSignatures[4] + 7 + *(DWORD*)(dwSignatures[4] + 0x3));

	return 0;
}

// DLL
BOOL WINAPI DllMain(HMODULE hinst, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		g_hInst = hinst;
		Initialize();
	}
	return true;
}
