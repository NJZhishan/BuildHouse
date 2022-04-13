// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildHouseCharacter.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "../Public/BuildHousePlayerController.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../Public/ModulBaseActor.h"
#include "../Public/BuildHouseGameInstance.h"
#include "Runtime/Engine/Classes/Engine/PostProcessVolume.h"
#include "Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"
#include "../BuildHouseGameModeBase.h"
#include "../Public/HouseGroupActor.h"

static const float gWallHeight = 300.0f;  //假设墙高3m
// Sets default values
ABuildHouseCharacter::ABuildHouseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));  //创建摇臂节点
	SpringArmComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	SpringArmComponent->bDoCollisionTest = false;
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCameraComponent"));  //创建第三人称漫游相机
	ThirdPersonCameraComponent->AttachToComponent(SpringArmComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ThirdPersonCameraComponent->bAutoActivate = false;
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCameraComponent"));  //创建第一人称漫游相机
	FirstPersonCameraComponent->AttachToComponent(SpringArmComponent, FAttachmentTransformRules::KeepRelativeTransform);
	FirstPersonCameraComponent->bAutoActivate = true;
	OrthoViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("OrthoViewCameraComponent"));  //创建2D正交视角相机
	OrthoViewCameraComponent->AttachToComponent(SpringArmComponent, FAttachmentTransformRules::KeepRelativeTransform);
	OrthoViewCameraComponent->bAutoActivate = false;
	OrthoViewCameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);
	OrthoViewCameraComponent->SetOrthoWidth(1920.0f);
	//设置根节点胶囊体的高度和半径都是0,否则二维下鼠标点击点的z坐标不是0
	UCapsuleComponent* capsulComponent = Cast<UCapsuleComponent>(GetRootComponent());
	if (capsulComponent)
	{
		capsulComponent->SetCapsuleRadius(0.0f);
		capsulComponent->SetCapsuleHalfHeight(0.0f);
	}
	m_MiniMap = nullptr;
}

// Called when the game starts or when spawned
void ABuildHouseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);//设置成飞行模式;
	GetCharacterMovement()->BrakingFrictionFactor = GetCharacterMovement()->MaxAcceleration = 2048.0f;//这两个值要相等，否则左右移动场景时有问题；
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn ; //不能因为碰撞修改charactor的位置
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);      //胶囊体不碰撞
	SetTravelType(TravelType::TT_2D_Ortho);  //默认二维俯视状态
	FTransform trans;
	FActorSpawnParameters aa;
	TSubclassOf<AActor>modulbaseInstance = LoadClass<AActor>(NULL, TEXT("Blueprint'/Game/MiniMap/BP_MiniMapCamera.BP_MiniMapCamera_C'"));
	m_MiniMap = Cast<AMiniMapCamera>(GetWorld()->SpawnActor(modulbaseInstance, &trans, aa));  //创建二维小地图
}

// Called every frame
void ABuildHouseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABuildHouseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	BindOperationBinding(PlayerInputComponent);  //绑定动作映射
}


//绑定动作映射
void ABuildHouseCharacter::BindOperationBinding(UInputComponent* PlayerInputComponent)
{
	FString leftMoveAxisName(TEXT("LeftMove"));
	m_LeftMoveBinding.AxisName = FName(*leftMoveAxisName);
	m_LeftMoveBinding.Key = EKeys::A;
	m_LeftMoveBinding.Scale = -1;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(m_LeftMoveBinding);  //左移动
	PlayerInputComponent->BindAxis(m_LeftMoveBinding.AxisName,this,&ABuildHouseCharacter::LeftMove);

	FString rightMoveAxisName(TEXT("RightMove"));
	m_RightMoveBinding.AxisName = FName(*rightMoveAxisName);
	m_RightMoveBinding.Key = EKeys::D;
	m_RightMoveBinding.Scale = 1;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(m_RightMoveBinding);  //右移动
	PlayerInputComponent->BindAxis(m_RightMoveBinding.AxisName, this, &ABuildHouseCharacter::LeftMove);

	FString forwardMoveAxisName(TEXT("ForwardMove"));
	m_ForwardMoveBinding.AxisName = FName(*forwardMoveAxisName);
	m_ForwardMoveBinding.Key = EKeys::W;
	m_ForwardMoveBinding.Scale = 1;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(m_ForwardMoveBinding);  //前移动
	PlayerInputComponent->BindAxis(m_ForwardMoveBinding.AxisName, this, &ABuildHouseCharacter::ForwardMove);

	FString forwardMoveAxisName2(TEXT("ForwardMove2"));
	m_ForwardMoveBinding2.AxisName = FName(*forwardMoveAxisName2);
	m_ForwardMoveBinding2.Key = EKeys::MouseWheelAxis;
	m_ForwardMoveBinding2.Scale = 1;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(m_ForwardMoveBinding2);  //前移动
	PlayerInputComponent->BindAxis(m_ForwardMoveBinding2.AxisName, this, &ABuildHouseCharacter::ForwardMove);

	FString backMoveAxisName(TEXT("BackMove"));
	m_BackMoveBinding.AxisName = FName(*backMoveAxisName);
	m_BackMoveBinding.Key = EKeys::S;
	m_BackMoveBinding.Scale = -1;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(m_BackMoveBinding);  //后移动
	PlayerInputComponent->BindAxis(m_BackMoveBinding.AxisName, this, &ABuildHouseCharacter::ForwardMove);

	FString upMoveAxisName(TEXT("UpMove"));
	m_UpMoveBinding.AxisName = FName(*upMoveAxisName);
	m_UpMoveBinding.Key = EKeys::Q;
	m_UpMoveBinding.Scale = 1;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(m_UpMoveBinding);  //上移动
	PlayerInputComponent->BindAxis(m_UpMoveBinding.AxisName, this, &ABuildHouseCharacter::UpMove);

	FString downMoveAxisName(TEXT("DownMove"));
	m_DownMoveBinding.AxisName = FName(*downMoveAxisName);
	m_DownMoveBinding.Key = EKeys::E;
	m_DownMoveBinding.Scale = -1;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(m_DownMoveBinding);  //上移动
	PlayerInputComponent->BindAxis(m_DownMoveBinding.AxisName, this, &ABuildHouseCharacter::UpMove);

	FString leftRotateAxisName(TEXT("LeftRotate"));
	m_LeftRotateBinding.AxisName = FName(*leftRotateAxisName);
	m_LeftRotateBinding.Key = EKeys::MouseX;
	m_LeftRotateBinding.Scale = 1;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(m_LeftRotateBinding);  //左右旋转
	PlayerInputComponent->BindAxis(m_LeftRotateBinding.AxisName, this, &ABuildHouseCharacter::LeftRotation);

	FString upRotateAxisName(TEXT("UpRotate"));
	m_UpRotateBinding.AxisName = FName(*upRotateAxisName);
	m_UpRotateBinding.Key = EKeys::MouseY;
	m_UpRotateBinding.Scale = 1;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(m_UpRotateBinding);  //上下旋转
	PlayerInputComponent->BindAxis(m_UpRotateBinding.AxisName, this, &ABuildHouseCharacter::UpRotation);

	FString lbtUpActionName(TEXT("LbtUp"));
	m_LbnUpBinding.ActionName = FName(*lbtUpActionName);
	m_LbnUpBinding.Key = EKeys::LeftMouseButton;
	m_LbnUpBinding.bAlt = m_LbnUpBinding.bCtrl = m_LbnUpBinding.bShift = m_LbnUpBinding.bCmd = false;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(m_LbnUpBinding);  //鼠标左键松开
	PlayerInputComponent->BindAction(m_LbnUpBinding.ActionName, IE_Released,this, &ABuildHouseCharacter::LeftMouseButtonUp);

	FString lbtDownActionName(TEXT("LbtDown"));
	m_LbnDownBinding.ActionName = FName(*lbtDownActionName);
	m_LbnDownBinding.Key = EKeys::LeftMouseButton;
	m_LbnDownBinding.bAlt = m_LbnDownBinding.bCtrl = m_LbnDownBinding.bShift = m_LbnDownBinding.bCmd = false;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(m_LbnDownBinding);  //鼠标左键按下
	PlayerInputComponent->BindAction(m_LbnDownBinding.ActionName, IE_Pressed, this, &ABuildHouseCharacter::LeftMouseButtonDown);

	FString delteteActionName(TEXT("deleteDown"));
	m_DeleteBinding.ActionName = FName(*delteteActionName);
	m_DeleteBinding.Key = EKeys::Delete;
	m_DeleteBinding.bAlt = m_DeleteBinding.bCtrl = m_DeleteBinding.bShift = m_DeleteBinding.bCmd = false;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(m_DeleteBinding); //delete键
	PlayerInputComponent->BindAction(m_DeleteBinding.ActionName, IE_Pressed, this, &ABuildHouseCharacter::DeleteModel);

	FString rbtUpActionName(TEXT("RbtUp"));
	m_RbnUpBinding.ActionName = FName(*rbtUpActionName);
	m_RbnUpBinding.Key = EKeys::RightMouseButton;
	m_RbnUpBinding.bAlt = m_RbnUpBinding.bCtrl = m_RbnUpBinding.bShift = m_RbnUpBinding.bCmd = false;
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(m_RbnUpBinding);  //鼠标右键松开
	PlayerInputComponent->BindAction(m_RbnUpBinding.ActionName, IE_Released, this, &ABuildHouseCharacter::RightMouseButtonUp);
}

//鼠标左右移动
void ABuildHouseCharacter::LeftRotation(float axisValue)
{
	if (GetWorld()/* && axisValue != 0.f*/)
	{
		if ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::LeftMouseButton)&&axisValue != 0.f)//左键按下才旋转
			|| GetOperationType() == OperationType::OT_CopyModel     //复制模型
			|| GetOperationType() == OperationType::OT_MoveModel    //移动模型
			|| GetOperationType() == OperationType::OT_MoveGroup)    //移动群组
		{
			if (GetOperationType() == OperationType::OT_Rotation3DView)
			{
				switch (GetTravelType())
				{
					case TravelType::TT_3D_ThirdPerson:
					case TravelType::TT_3D_FirstPerson:
					{//三维旋转场景
						FRotator curRot = SpringArmComponent->GetRelativeRotation();
						curRot.Yaw += axisValue * 2;
						curRot.Roll = 0;
						SpringArmComponent->SetRelativeRotation(curRot);
						SetSceneVisible();
					}
					break;
					case TravelType::TT_2D_Ortho :
					{//二维移动场景
						if (GetWorld() && GetWorld()->GetFirstPlayerController())
						{
							MoveView2D();
						}
					}
					break;
				}
			}
			else if ((GetOperationType() == OperationType::OT_MoveModel) || GetOperationType() == OperationType::OT_CopyModel || GetOperationType() == OperationType::OT_MoveGroup)
			{//移动模型、群组
				TravelType viewType = GetTravelType();
				if (viewType == TravelType::TT_2D_Ortho)
				{//二维中移动模型
					MoveModel2D();
				}
				else if (viewType == TravelType::TT_3D_FirstPerson || viewType == TravelType::TT_3D_ThirdPerson)
				{//三维中移动模型
					MoveModel3D();
				}
			}
		}
	}
	return;
}

//鼠标上下移动
void ABuildHouseCharacter::UpRotation(float axisValue)
{
	if (GetWorld())
	{
		if ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::LeftMouseButton)&&axisValue != 0.f) //左键按下才旋转
			|| GetOperationType() == OperationType::OT_CopyModel //复制模型
			|| GetOperationType() == OperationType::OT_MoveModel //移动模型
			|| GetOperationType() == OperationType::OT_MoveGroup)//移动群组
		{
			if (GetOperationType() == OperationType::OT_Rotation3DView)
			{//旋转场景
				switch (GetTravelType())
				{
				case TravelType::TT_3D_ThirdPerson:
				case TravelType::TT_3D_FirstPerson:
				{//三维旋转场景
					FRotator curRot = SpringArmComponent->GetRelativeRotation();
					curRot.Pitch += axisValue * 2;
					curRot.Roll = 0;
					SpringArmComponent->SetRelativeRotation(curRot);
					SetSceneVisible();
				}
				break;
				case TravelType::TT_2D_Ortho:
				{//二维移动场景
					if (GetWorld() && GetWorld()->GetFirstPlayerController())
					{
						MoveView2D();
					}
				}
				break;
				}
			}
			else if ((GetOperationType() == OperationType::OT_MoveModel)|| GetOperationType() == OperationType::OT_CopyModel || GetOperationType() == OperationType::OT_MoveGroup)
			{//移动模型
				TravelType viewType = GetTravelType();
				if (viewType == TravelType::TT_2D_Ortho)
				{//二维中移动模型
					MoveModel2D();
				}
				else if (viewType == TravelType::TT_3D_FirstPerson || viewType == TravelType::TT_3D_ThirdPerson)
				{//三维中移动模型
					MoveModel3D();
				}
			}
		}
	}
	return;
}

//前后移动
void ABuildHouseCharacter::ForwardMove(float axisValue)
{
	if (axisValue != 0.f)
	{
		//if (Controller)
		//{
			/*FRotator const ControlSpaceRot = Controller->GetControlRotation();
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), axisValue*10);*/
		//}
		switch (GetTravelType())
		{
			case TravelType::TT_3D_ThirdPerson:
			{//第三人称漫游
				float newLength = SpringArmComponent->TargetArmLength + axisValue * 2;
				newLength = newLength <= 0.0f ? 0.0f : newLength;  //模仿了酷家乐
				SpringArmComponent->TargetArmLength = newLength;	
				SetSceneVisible();
			}
			break;
			case TravelType::TT_3D_FirstPerson:
			{//第一人称漫游
				FVector forwardVec = UKismetMathLibrary::GetForwardVector(SpringArmComponent->GetTargetRotation());
				forwardVec *= axisValue * 2;
				RootComponent->AddRelativeLocation(forwardVec);
				SetSceneVisible();
			}
			case TravelType::TT_2D_Ortho:
			{//正交缩放
				int32 X = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
				int32 Y = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;   //获得视口大小
				FVector ltPos, ltDir, rbPos, rbDir;
				GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(0, 0, ltPos, ltDir);
				GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(X, Y, rbPos, rbDir);
				float xCmPerPixel = fabsf(rbPos.X - ltPos.X) / X;
				float yCmPerPixel = fabsf(rbPos.Y - ltPos.Y) / Y;   //获得每个像素的实际尺寸
				FVector2D mousePos;
				GetWorld()->GetFirstPlayerController()->GetMousePosition(mousePos.X, mousePos.Y);
				float deltaX = mousePos.X - X * 0.5;
				float deltaY = mousePos.Y - Y * 0.5;  //鼠标位置相对于视口中心的像素个数

				float oldOrthoWidth = OrthoViewCameraComponent->OrthoWidth;
				float newOrthoWidth = oldOrthoWidth - axisValue * 50;
				newOrthoWidth = newOrthoWidth < 20 ? 20 : newOrthoWidth;   //不能无限的放大或者缩小，限制在20到3000之间
				newOrthoWidth = newOrthoWidth > 3000 ? 3000 : newOrthoWidth;
				OrthoViewCameraComponent->SetOrthoWidth(newOrthoWidth);
				OrthoViewCameraComponent->UpdateComponentToWorld();

				float sceneScale = newOrthoWidth / oldOrthoWidth;  //视口的缩放值
				float deltaXNew = deltaX / sceneScale;    //新的鼠标位置相对于视口中心的像素个数
				float deltaYNew = deltaY / sceneScale;

				float deltaResX = deltaXNew - deltaX;      //随着视口缩放，鼠标位置相对于视口中心的像素的差值
				float deltaResY = deltaYNew - deltaY;

				FVector2D mousePosNew;
				GetWorld()->GetFirstPlayerController()->GetMousePosition(mousePosNew.X, mousePosNew.Y);
				float xMove = deltaResX * xCmPerPixel* sceneScale;
				float yMove = deltaResY * yCmPerPixel * sceneScale;   //像素差值对应的实际距离就是charactor应该移动的距离
				AddActorWorldOffset(FVector(xMove, yMove, 0));
			}
			break;
		}
	}
}

//左右移动
void ABuildHouseCharacter::LeftMove(float axisValue)
{
	if (axisValue != 0.f)
	{
		//if (Controller)
		//{
			/*FRotator const ControlSpaceRot = Controller->GetControlRotation();
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), axisValue * 10);*/
		//}
		switch (GetTravelType())
		{
		case TravelType::TT_3D_ThirdPerson:
		{//第三人称漫游
			FVector rightVec = UKismetMathLibrary::GetRightVector(SpringArmComponent->GetTargetRotation());
			rightVec *= axisValue * 2;
			RootComponent->AddRelativeLocation(rightVec);		
		}
		break;
		case TravelType::TT_3D_FirstPerson:
		{//第一人称漫游
			FVector rightVec = UKismetMathLibrary::GetRightVector(SpringArmComponent->GetTargetRotation());
			rightVec *= axisValue * 2;
			RootComponent->AddRelativeLocation(rightVec);
		}
		case TravelType::TT_2D_Ortho:
		{//正交
			
		}
		break;
		}
	}
	SetSceneVisible();
}

//上下移动
void ABuildHouseCharacter::UpMove(float axisValue)
{
	if (axisValue != 0.f)
	{
		//if (Controller)
		//{
			/*FRotator const ControlSpaceRot = Controller->GetControlRotation();
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Z), axisValue * 10);*/
		//}
		FVector upVec = UKismetMathLibrary::GetUpVector(SpringArmComponent->GetTargetRotation());
		upVec *= axisValue * 2;
		RootComponent->AddRelativeLocation(upVec);
		SetSceneVisible();
	}
}

//鼠标左键松开
void ABuildHouseCharacter::LeftMouseButtonUp()
{
	//记录鼠标松开左键时的位置
	GetWorld()->GetFirstPlayerController()->GetMousePosition(m_LeftMouseButtonUpPos.X, m_LeftMouseButtonUpPos.Y);
	if (m_LeftMouseButtonUpPos.Equals(m_LeftMouseButtonDownPos))
	{//如果左键按下和松开时是同一个位置
		if (GetWorld())
		{  
			if (GetOperationType() == OperationType::OT_MakeGroup)
			{//将鼠标选中的模型添加到群组中
				AddPickedThingsToGroup();
			}
			else
			{//射线检测
				ABuildHousePlayerController* MyPlayerController = ABuildHousePlayerController::GetPlayerController();
				if (MyPlayerController)
				{
					MyPlayerController->LeftMouseBtnUp();
				}
				//显示操作模型的UI和坐标轴
				UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
				ABuildHouseGameModeBase* gameModeBase = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
				if (gameModeBase)
				{
					gameModeBase->AddProdOperationWidget(false);  //先隐藏模型操作的UI
					gameModeBase->AddGizmo(false);//先隐藏坐标轴
				}
				if (!(GetOperationType() == OperationType::OT_MoveModel || GetOperationType() == OperationType::OT_CopyModel || GetOperationType() == OperationType::OT_MoveGroup))
				{//移动和复制模型过程中不显示菜单
					if (gameModeBase && gameInstance && gameInstance->ReturnSelectModulBaseActor() && gameInstance->ReturnSelectModulBaseActor()->ActorHasTag(TEXT("普通模型")))
					{//显示模型操作的UI:只有普通模型和灯显示模型信息和坐标轴
						OBJECT_STYLE objectType = gameInstance->ReturnSelectModulBaseActor()->ReturnModelType();
						if (objectType == OBJECT_STYLE::NORMAL || objectType == OBJECT_STYLE::LIGHT)
						{
							gameModeBase->AddProdOperationWidget(true);
							gameModeBase->AddGizmo(true, gameInstance->ReturnSelectModulBaseActor()->GetActorLocation(), gameInstance->ReturnSelectModulBaseActor()->GetActorRotation());
						}
					}
				}
				//显示模型或者群组信息
				if (gameModeBase)
				{
					gameModeBase->m_EntryWidget->UpdateProdInfor(gameInstance->ReturnSelectModulBaseActor(), gameInstance->ReturnSelectGroupActor());
				}
			}
		}
	}
	if (GetOperationType() == OperationType::OT_MoveModel || GetOperationType() == OperationType::OT_CopyModel || GetOperationType() == OperationType::OT_MoveGroup)
	{//左键松开时是在移动模型
		//恢复初始状态
		SetOperationType(OperationType::OT_Normal);
		//根据是否吸附模型，将模型的包围盒的碰撞关闭
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance->GetObjectAdsorb())
		{
			TArray<AActor*> objList;  //场景中其他的模型
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("普通模型")), objList);
			for (auto& obj : objList)
			{
				AModulBaseActor* actor = Cast<AModulBaseActor>(obj);
				if (actor && (obj->IsHidden() == false))
				{
					actor->BoundComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}
			}
		}
	}
}

//鼠标左键按下
void ABuildHouseCharacter::LeftMouseButtonDown()
{
	if (m_OperationType != OperationType::OT_MakeGroup)
	{//创建群组的过程中不旋转视角
		if(GetOperationType() == OperationType::OT_Normal)
			SetOperationType(OperationType::OT_Rotation3DView);  //默认操作类型是旋转场景
		//UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		//AModulBaseActor* selActor = gameInstance->ReturnSelectModulBaseActor();
		//if (selActor && selActor->ActorHasTag(TEXT("普通模型")))
		//{//有普通模型被选中，再进一步判断鼠标是否又按在了这个模型上,如果有又落在这个物体上，在按下鼠标左键并移动鼠标就相当于是移动物体
		//	FHitResult hitResult;
		//	if (GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, hitResult))
		//	{
		//		AModulBaseActor* underCursorActor = Cast<AModulBaseActor>(hitResult.Actor.Get());
		//		if (selActor == underCursorActor)
		//		{
		//			SetOperationType(OperationType::OT_MoveModel);
		//			//根据是否吸附模型，将模型的包围盒的碰撞打开
		//			if (gameInstance->GetObjectAdsorb())
		//			{
		//				TArray<AActor*> objList;  //场景中其他的模型
		//				UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("普通模型")), objList);
		//				for (auto& obj : objList)
		//				{
		//					AModulBaseActor* actor = Cast<AModulBaseActor>(obj);
		//					if (actor && (obj->IsHidden() == false))
		//					{
		//						actor->BoundComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//					}
		//				}
		//			}
		//			//移动模型之前先把坐标轴和模型操作UI隐藏
		//			ABuildHouseGameModeBase* gameModeBase = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
		//			if (gameModeBase)
		//			{
		//				gameModeBase->AddProdOperationWidget(false);  //先隐藏模型操作的UI
		//				gameModeBase->AddGizmo(false);//先隐藏坐标轴
		//			}
		//		}
		//	}
		//}
	}
	//记录鼠标按下左键时的位置
	GetWorld()->GetFirstPlayerController()->GetMousePosition(m_LeftMouseButtonDownPos.X, m_LeftMouseButtonDownPos.Y);
	return;
}

//右键松开
void ABuildHouseCharacter::RightMouseButtonUp()
{
	if (m_OperationType == OperationType::OT_MakeGroup)
	{//完成群组操作
		FinishGroupOperation();  //完成群组操作
		SetOperationType(OperationType::OT_Normal);
	}
	return;
}

//设置漫游类型
void ABuildHouseCharacter::SetTravelType(TravelType type)
{
	m_TravelType = type;
	switch (type)
	{
		case TravelType::TT_2D_Ortho:
		{
			UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("viewmode unlit"));
			HideCeilingModel(true);  //隐藏吊顶上的模型
			TArray<AActor*> ppvList;  //后期处理关闭，否则会造成颜色显示不正确
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), ppvList);
			for (auto& ppv : ppvList)
			{
				APostProcessVolume* tempPPV = Cast<APostProcessVolume>(ppv);
				tempPPV->bEnabled = false;
			}
			TSubclassOf<AActor> skyClass = LoadClass<AActor>(NULL, TEXT("Blueprint'/Engine/EngineSky/BP_Sky_Sphere.BP_Sky_Sphere_C'"));
			TArray<AActor*> skyList;  //天空球关闭
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), skyClass, skyList);
			for (auto& sky : skyList)
			{
				sky->SetActorHiddenInGame(true);
			}
			TArray<AActor*> wallList;  //显示场景中所有的墙体
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("墙体")), wallList);
			for (auto& wall : wallList)
			{
				Cast<AModulBaseActor>(wall)->SetModulBaseActorShow(true);
			}
			TArray<AActor*> floorList;  //显示场景中所有的地面
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("地面")), floorList);
			for (auto& floor : floorList)
			{
				Cast<AModulBaseActor>(floor)->SetModulBaseActorShow(true);
			}
			TArray<AActor*> ceilingList;  //隐藏场景中所有的吊顶
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("吊顶")), ceilingList);
			for (auto& ceiling : ceilingList)
			{
				Cast<AModulBaseActor>(ceiling)->SetModulBaseActorShow(false);
			}
			//将其他的相机激活状态设置为false
			FirstPersonCameraComponent->SetActive(false);
			ThirdPersonCameraComponent->SetActive(false);
			//将当前相机的激活状态设置为true
			OrthoViewCameraComponent->SetActive(true);
			//设置2D中charactor的位置
			ResetView2D();
			//隐藏小地图
			ABuildHouseGameModeBase* gameMode = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
			if (gameMode)
			{
				gameMode->m_EntryWidget->ShowMiniMap(false);
				gameMode->AddProdOperationWidget(false);
				gameMode->AddGizmo(false);
			}
		}
			break;
		case TravelType::TT_3D_ThirdPerson:
		{
			UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("viewmode lit"));
			//将其他的相机激活状态设置为false
			FirstPersonCameraComponent->SetActive(false);
			OrthoViewCameraComponent->SetActive(false);
			//将当前相机的激活状态设置为true
			ThirdPersonCameraComponent->SetActive(true);
			//第三人称的springArm的长度需要计算一个合适的值，现在先默认设置为300
			SpringArmComponent->TargetArmLength = 300;
			SpringArmComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
			//重新打开后期处理
			TArray<AActor*> ppvList; 
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), ppvList);
			for (auto& ppv : ppvList)
			{
				APostProcessVolume* tempPPV = Cast<APostProcessVolume>(ppv);
				tempPPV->bEnabled = true;
			}
			//天空球重新打开
			TSubclassOf<AActor> skyClass = LoadClass<AActor>(NULL, TEXT("Blueprint'/Engine/EngineSky/BP_Sky_Sphere.BP_Sky_Sphere_C'"));
			TArray<AActor*> skyList;  
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), skyClass, skyList);
			for (auto& sky : skyList)
			{
				sky->SetActorHiddenInGame(false);
			}
		}
			break;
		case TravelType::TT_3D_FirstPerson:
		{//3D第一人称漫游
			UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("viewmode lit"));
			//将其他的相机激活状态设置为false
			ThirdPersonCameraComponent->SetActive(false);
			OrthoViewCameraComponent->SetActive(false);
			//将当前相机的激活状态设置为true
			FirstPersonCameraComponent->SetActive(true);
			//第一人称的springArm的长度是0
			SpringArmComponent->TargetArmLength = 0;
			SpringArmComponent->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
			SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 170.0f));  //适当抬高，假设人有1.7m，所以抬高170cm
			//重新打开后期处理
			TArray<AActor*> ppvList;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), ppvList);
			for (auto& ppv : ppvList)
			{
				APostProcessVolume* tempPPV = Cast<APostProcessVolume>(ppv);
				tempPPV->bEnabled = true;
			}
			//天空球重新打开
			TSubclassOf<AActor> skyClass = LoadClass<AActor>(NULL, TEXT("Blueprint'/Engine/EngineSky/BP_Sky_Sphere.BP_Sky_Sphere_C'"));
			TArray<AActor*> skyList;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), skyClass, skyList);
			for (auto& sky : skyList)
			{
				sky->SetActorHiddenInGame(false);
			}
			//隐藏已经选择模型的包围盒
			UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
			if (gameInstance->ReturnSelectModulBaseActor())
			{
				gameInstance->ReturnSelectModulBaseActor()->SetOutline(true);
			}
			//相机的位置在第一个可见房间的内部
			if (gameInstance->m_pScene->m_floorVisibleIndexList.Num() > 0 && gameInstance->m_pScene->m_floorVisibleIndexList[0]< gameInstance->m_pScene->m_FloorList.size())
			{
				SetActorLocation(gameInstance->m_pScene->m_FloorList[gameInstance->m_pScene->m_floorVisibleIndexList[0]]->GetActorLocation());  //相机在房子内部
				gameInstance->m_pScene->UpdateRoomVisiblity(gameInstance->m_pScene->m_floorVisibleIndexList); //重置可见性
			}
			SetSceneVisible();  //重新计算可见性
			HideCeilingModel(false);  //吊顶上的模型显示
			//显示并初始化小地图
			ABuildHouseGameModeBase* gameMode = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
			if (gameMode)
			{
				float width, height;
				gameMode->m_EntryWidget->ShowMiniMap(true);
				gameMode->m_EntryWidget->ReturnMiniMapSize(width, height);
				if (m_MiniMap)
				{
					m_MiniMap->ChangeMiniMapShowType(m_MiniMap->m_MiniMapShowType);
				}
				gameMode->AddProdOperationWidget(false);
				gameMode->AddGizmo(false);
			}
		}
			break;
	default:
		break;
	}
	return;
}

//返回漫游类型
TravelType ABuildHouseCharacter::GetTravelType()
{
	return m_TravelType;
}

//返回相机的位置和方向
void ABuildHouseCharacter::ReturnCamera(FVector& cameraPos, FVector& cameraDir)
{
	switch (GetTravelType())
	{
		case TravelType::TT_3D_FirstPerson:
		{
			cameraPos = FirstPersonCameraComponent->K2_GetComponentLocation();  //相机的世界坐标
			cameraDir = UKismetMathLibrary::GetForwardVector(SpringArmComponent->GetTargetRotation());  //向前的向量
			break;
		}
	}
	return;
}

//设置场景中的墙体或者吊顶是否可见
void ABuildHouseCharacter::SetSceneVisible()
{
	if (GetTravelType() != TravelType::TT_3D_FirstPerson)
		return;  //只写了第一人称漫游的逻辑
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		FVector cameraPos, cameraDir;
		ReturnCamera(cameraPos, cameraDir);
		gameInstance->m_pScene->ModifyVisibility(cameraPos, cameraDir);
	}
	return;  //可见性的计算改到CScene中
	//判断是否在房间内部：向下做射线检测
	bool bInRoom = true;;
	FVector traceBegin, traceEnd, traceDir; //射线检测的起点、终点、方向
	FVector cameraPos, cameraDir; //相机的位置，相机的方向
	switch (GetTravelType())
	{
		case TravelType::TT_3D_FirstPerson:
		{
			cameraPos = FirstPersonCameraComponent->K2_GetComponentLocation();  //相机的世界坐标
			cameraDir = UKismetMathLibrary::GetForwardVector(SpringArmComponent->GetTargetRotation());  //向前的向量
			break;
		}
	}
	TArray<AActor*> ceilingList;  //场景中所有的吊顶
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("吊顶")), ceilingList);
	for (auto& ceiling : ceilingList)
	{
		ceiling->SetActorHiddenInGame(false);
		ceiling->SetActorEnableCollision(true);
	}
	TArray<AActor*> wallList;  //场景中所有的墙体
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("墙体")), wallList);
	for (auto& wall : wallList)
	{
		wall->SetActorHiddenInGame(false);
		wall->SetActorEnableCollision(true);
	}
	TArray<AActor*> floorList;  //场景中所有的地面
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("地面")), floorList);
	for (auto& floor : floorList)
	{
		floor->SetActorHiddenInGame(false);
		floor->SetActorEnableCollision(true);
	}

	//向下做射线检测
	traceBegin = cameraPos;
	traceDir = FVector(0.0f, 0.0f, -1.0f);
	traceEnd = traceBegin + traceDir * 50000;
	TArray<AActor*> ActorsToIgnore;  //忽略不检测的actor
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("普通模型")), ActorsToIgnore);
	TArray<FHitResult> OutHits;
	bool bTrace = UKismetSystemLibrary::LineTraceMulti(GetWorld(), traceBegin, traceEnd, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true);
	bool bUpCeiling = false;  //相机在吊顶的上面
	bool bUpFloor = false;    //相机在地面的上面
	if (bTrace)
	{
		//遍历射线检测到的actor
		for (auto& hitResult : OutHits)
		{
			AModulBaseActor* actor = Cast<AModulBaseActor>(hitResult.Actor.Get());
			if (actor)
			{
				if (actor->ActorHasTag(TEXT("吊顶")))
				{
					bUpCeiling = true;
					break;
				}
				if (actor->ActorHasTag(TEXT("地面")))
				{
					bUpFloor = true;
				}
			}
		}
		if (bUpCeiling)
		{//相机在吊顶上面，将所有的吊顶都隐藏
			for (auto& ceiling : ceilingList)
			{
				ceiling->SetActorHiddenInGame(true);
				ceiling->SetActorEnableCollision(false);
			}
		}
		else if (bUpFloor)
		{//相机在吊顶下面，地面上面，说明相机在房间内部,不需要做任何处理
			//所有的吊顶都显示出来
			//所有的墙体都显示出来
		}
	}
	else
	{//什么都没检测到，说明没在任何房间内
		//吊顶、地面、墙体是否可见取决于在相机方向上做射线检测时是否被检测到
		TArray<FHitResult> CameraDirOutHits;
		traceDir =cameraDir;
		traceEnd = traceBegin + traceDir * 50000;
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), traceBegin, traceEnd, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, EDrawDebugTrace::Type::None, CameraDirOutHits, true))
		{
			for (auto& traceActor : CameraDirOutHits)
			{
				AModulBaseActor* actor = Cast<AModulBaseActor>(traceActor.Actor.Get());
				if (actor)
				{
					if (actor->ActorHasTag(TEXT("吊顶")) || actor->ActorHasTag(TEXT("地面")) || actor->ActorHasTag(TEXT("墙体")))
					{
						actor->SetActorHiddenInGame(true);
						actor->SetActorEnableCollision(false);
					}
				}
			}
		}
	}
}

//移动模型 
void ABuildHouseCharacter::MoveModel3D()
{
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	AModulBaseActor* selActor = gameInstance->ReturnSelectModulBaseActor();  //在移动的模型
	AHouseGroupActor* selGroup = gameInstance->ReturnSelectGroupActor();  //在移动的群组
	if (!(selActor || selGroup))
		return;
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		FVector mouseWorldPos,mouseWorldDir;
		GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(mouseWorldPos, mouseWorldDir);
		TArray<AActor*> ActorsToIgnores;  //忽略不检测的actor
		ActorsToIgnores.Add(selActor);
		TArray<AActor*> ceilingList;  //场景中所有的吊顶
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("吊顶")), ceilingList);
		for (auto& ceiling : ceilingList)
		{
			if (ceiling->GetActorEnableCollision() == false)
			{
				ActorsToIgnores.Add(ceiling);
			}
		}
		TArray<AActor*> wallList;  //场景中所有的墙体
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("墙体")), wallList);
		for (auto& wall : wallList)
		{
			if (wall->GetActorEnableCollision() == false)
			{
				ActorsToIgnores.Add(wall);
			}
		}
		TArray<AActor*> floorList;  //场景中所有的地面
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("地面")), floorList);
		for (auto& floor : floorList)
		{
			if (floor->GetActorEnableCollision() == false)
			{
				ActorsToIgnores.Add(floor);
			}
		}
		TArray<AActor*> objList;  //场景中其他的模型
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("普通模型")), objList);
		for (auto& obj : objList)
		{
			ActorsToIgnores.Add(obj);
		}
		FHitResult OutHit;
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), mouseWorldPos, mouseWorldPos + mouseWorldDir * 500000, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnores, EDrawDebugTrace::Type::ForOneFrame, OutHit, true))
		{
			FVector impactPos = OutHit.ImpactPoint;
			if (selGroup)
			{//移动群组
				selGroup->MoveGroup(impactPos-selGroup->GetActorLocation());
				FVector oldAxis = selGroup->m_RotateAxis; //群组轴向随着群组移动也要变化
				FVector impactNormal = OutHit.ImpactNormal;
				FVector rotAxis = FVector::CrossProduct(oldAxis, impactNormal);
				float rotAngle = UKismetMathLibrary::DegAcos(FVector::DotProduct(oldAxis, impactNormal));
				FRotator deltaRot = UKismetMathLibrary::RotatorFromAxisAndAngle(rotAxis, rotAngle);
				selGroup->AddActorWorldRotation(deltaRot); //旋转 
				selGroup->m_RotateAxis = impactNormal;     //更新旋转轴
			}
			else if (selActor)
			{//移动单个模型
				selActor->SetActorLocation(impactPos);       //设置位置
				FVector impactNormal = OutHit.ImpactNormal;
				FVector modelAxis = FVector(0.0f, 0.0f, 1.0f);  //在max中建模时模型的轴向，max中朝着z正方向的（x向右，z向上，y向里），在ue中相当于朝着z正方向，因为ue4中坐标系是y向右，x向里，z向上
				FVector rightVec = FVector::CrossProduct(impactNormal, modelAxis);
				FVector upVec = impactNormal;
				if (rightVec.IsNearlyZero())
				{//模型放到了地上或者吊顶上
					float angle = FVector::DotProduct(impactNormal, modelAxis);
					if (angle >= 0.99999f)
					{//地面上
						rightVec = FVector(0.0f, 1.0f, 0.0f);
						upVec = FVector(0.0f, 0.0f, 1.0f);
					}
					if (angle <= -0.99999f)
					{//吊顶上
						rightVec = FVector(0.0f, 1.0f, 0.0f);
						upVec = FVector(0.0f, 0.0f, -1.0f);
					}
				}
				FMatrix rotationMatrix = FRotationMatrix::MakeFromYZ(rightVec, upVec);
				selActor->SetActorRotation(rotationMatrix.Rotator());  //设置轴向
				HandleAdsorb(selActor);  //处理吸附
				//设置可见性
				gameInstance->m_pScene->ModifyObjectFlagForFloors(selActor->ReturnModelType(), selActor, selActor->GetActorLocation());
			}
		}
	}
}

//二维三维切换 
void ABuildHouseCharacter::ChangeViewType(FString viewType)
{
	TravelType curTravelType = GetTravelType();
	if (viewType.Equals(TEXT("三维漫游")))
	{
		if (curTravelType == TravelType::TT_3D_FirstPerson)
		{
			return;  //已经是三维第一人称漫游了，直接返回
		}
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance && gameInstance->m_pScene && gameInstance->m_pScene->m_FloorList.size() != 0)
		{//已经存在三维的情况下才能进入三维模式
			SetTravelType(TravelType::TT_3D_FirstPerson);
		}
	}
	else if (viewType.Equals(TEXT("二维顶面")))
	{
		if (curTravelType == TravelType::TT_2D_Ortho)
		{
			return;  //已经是二维顶面了，直接返回
		}
		SetTravelType(TravelType::TT_2D_Ortho);
	}
	return;
}

//移动二维视图
void ABuildHouseCharacter::MoveView2D()
{
	int32 X = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
	int32 Y = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
	FVector ltPos, ltDir, rbPos, rbDir;
	GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(0, 0, ltPos, ltDir);
	GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(X, Y, rbPos, rbDir);
	float xCmPerPixel = fabsf(rbPos.X - ltPos.X) / X;
	float yCmPerPixel = fabsf(rbPos.Y - ltPos.Y) / Y;
	FVector2D mousePos;
	GetWorld()->GetFirstPlayerController()->GetMousePosition(mousePos.X, mousePos.Y);
	FVector2D mouseDelta = m_LeftMouseButtonDownPos - mousePos;
	float xMove = mouseDelta.X * xCmPerPixel;
	float yMove = mouseDelta.Y * yCmPerPixel;
	AddActorWorldOffset(FVector(xMove, yMove, 0));
	m_LeftMouseButtonDownPos = mousePos;
	return;
}

//设置二维中charactor的位置
void ABuildHouseCharacter::ResetView2D()
{
	//设置可见区域
	OrthoViewCameraComponent->OrthoWidth = 1920;
	SetActorLocation(FVector::ZeroVector);   //默认charactor位置在中间
	SpringArmComponent->SetRelativeLocation(FVector::ZeroVector);   //摇臂保持和charator的位置一致
	SpringArmComponent->TargetArmLength = gWallHeight;//springArm的长度要大于户型的墙高，否则吊顶会看不到，这里假设墙高是3m
	SpringArmComponent->SetRelativeRotation(FRotator(270.0f, 0.0f, 270.0f));  //相机要朝下
	//计算户型的中心点
	float minX = 10000000.0f, minY = 10000000.0f, maxX = -10000000.0f, maxY = -10000000.0f;
	TArray<AActor*> ceilingList;  //场景中所有的吊顶
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("吊顶")), ceilingList);
	for (auto& ceiling : ceilingList)
	{
		FBox boundLocal = ceiling->CalculateComponentsBoundingBoxInLocalSpace(true);
		FVector boundMin = UKismetMathLibrary::TransformLocation(ceiling->GetActorTransform(), boundLocal.Min);
		FVector boundMax = UKismetMathLibrary::TransformLocation(ceiling->GetActorTransform(), boundLocal.Max); //转化为世界坐标
		minX = boundMin.X < minX ? boundMin.X : minX;
		minY = boundMin.Y < minY ? boundMin.Y : minY;
		maxX = boundMax.X > maxX ? boundMax.X : maxX;
		maxY = boundMax.Y > maxY ? boundMax.Y : maxY;
	}
	if (ceilingList.Num() > 0)
	{//房间中有地面,相机放到中间位置
		FVector mid = FVector((minX + maxX) * 0.5f, (minY + maxY) * 0.5f, 0);
		AddActorWorldOffset(mid);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);  //更新相机
		//左右两侧各预留30%，上下各预留5%，然后尽可能将户型图放大
		int32 X = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
		int32 Y = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
		float frameWidth = X - 2 * X * 0.3f;   //为户型预留的尺寸
		float frameHeight = Y - 2 * Y * 0.09f;
		FVector2D ltScreen, rbScreen;
		bool transSuc = GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(minX, minY, 0), ltScreen);
		bool transSuc1 = GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(maxX, maxY, 0), rbScreen);
		float houseWidth = rbScreen.X - ltScreen.X;   //当前户型的尺寸
		float houseHeight = rbScreen.Y - ltScreen.Y;
		//按照为户型预留的尺寸，将户型进行缩放
		float scale = 1;
		if (houseWidth <= frameWidth && houseHeight < frameHeight)
		{//整个户型都在可见范围内,则计算最大缩放多少，能让户型既在可见范围，又能尽可能的放大
			float scale1 = (float)houseWidth / frameWidth;
			float scale2 = (float)houseHeight / frameHeight;
			scale = scale1 >= scale2 ? scale1 : scale2;
		}
		else if (houseWidth <= frameWidth && houseHeight > frameHeight)
		{//高度方向上不在可见范围，宽度方向上在可见范围内，则计算缩小多少，能让户型全部在可见范围内
			float scale1 = (float)houseHeight / frameHeight;
			scale = scale1;
		}
		else if (houseWidth > frameWidth && houseHeight <= frameHeight)
		{//高度方向上不在可见范围，宽度方向上在可见范围内，则计算缩小多少，能让户型全部在可见范围内
			float scale1 = (float)houseWidth / frameWidth;
			scale = scale1;
		}
		else if (houseWidth > frameWidth && houseHeight > frameHeight)
		{//宽度和高度方向上都不在可见范围内，则计算最小缩小多少，能让户型既在可见范围，又能尽可能的少的缩小
			float scale1 = (float)houseWidth / frameWidth;
			float scale2 = (float)houseHeight / frameHeight;
			scale = scale1 > scale2 ? scale1 : scale2;
		}
		OrthoViewCameraComponent->OrthoWidth*=scale; //这个值越大看到的范围越小
	}
	return;
}

//设置吊顶上模型的显示和隐藏
//判断方式：轴向是z负轴，并且距离吊顶在20cm以内
void ABuildHouseCharacter::HideCeilingModel(bool hide)
{
	TArray<AActor*> objList;  //场景中的模型
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("普通模型")), objList);
	for (auto& obj : objList)
	{
		FVector upVector;  //模型的轴向
		upVector = obj->GetActorUpVector();
		if (upVector.Equals(FVector::DownVector))
		{//模型轴向向下
			TArray<AActor*> ActorsToIgnore;  //忽略不检测的actor
			ActorsToIgnore.Add(obj);
			FHitResult OutHit;
			FVector start = obj->GetActorLocation();
			FVector end = start+20*FVector::UpVector;   //向上20cm
			if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHit, true))
			{
				if (OutHit.Actor.Get()->ActorHasTag(TEXT("吊顶")))
				{
					obj->SetActorHiddenInGame(hide);
					obj->SetActorEnableCollision(!hide);  //隐藏的模型不允许碰撞
				}
			}
		}
	}
	return;
}

//移动模型,二维
void ABuildHouseCharacter::MoveModel2D()
{
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	AModulBaseActor* selActor = gameInstance->ReturnSelectModulBaseActor();  //在移动的模型
	AHouseGroupActor* selGroup = gameInstance->ReturnSelectGroupActor();     //在移动的群组
	if (!(selActor || selGroup))
		return;
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		int32 X = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
		int32 Y = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
		FVector ltPos, ltDir, rbPos, rbDir;
		GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(0, 0, ltPos, ltDir);
		GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(X, Y, rbPos, rbDir);
		float xCmPerPixel = fabsf(rbPos.X - ltPos.X) / X;
		float yCmPerPixel = fabsf(rbPos.Y - ltPos.Y) / Y;
		FVector2D mousePos;
		GetWorld()->GetFirstPlayerController()->GetMousePosition(mousePos.X, mousePos.Y);
		FVector2D mouseDelta = mousePos - m_LeftMouseButtonDownPos;
		float xMove = mouseDelta.X * xCmPerPixel;
		float yMove = mouseDelta.Y * yCmPerPixel;
		if (selGroup)
		{//移动群组
			selGroup->MoveGroup(FVector(xMove, yMove, 0));
		}
		else if (selActor)
		{//移动单个模型
			selActor->AddActorWorldOffset(FVector(xMove, yMove, 0));
			//设置可见性
			gameInstance->m_pScene->ModifyObjectFlagForFloors(selActor->ReturnModelType(), selActor, selActor->GetActorLocation());
		}
		HandleAdsorb(selActor);  //处理吸附
		m_LeftMouseButtonDownPos = mousePos;
	}
	return;
}

//处理吸附
void ABuildHouseCharacter::HandleAdsorb(AModulBaseActor* selActor)
{
	return;
	if (selActor)
	{
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		//在移动模型之前做射线检测,避免重叠
		TArray<AActor*> ActorsToIgnore;  //忽略不检测的actor
		ActorsToIgnore.Add(selActor);    //正在移动的模型
		TArray<AActor*> ceilingList;  //场景中所有的吊顶
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("吊顶")), ceilingList);
		for (auto& ceiling : ceilingList)
		{
			if (ceiling->GetActorEnableCollision() == false)
			{
				ActorsToIgnore.Add(ceiling);
			}
		}
		TArray<AActor*> wallList;  //场景中所有的墙体
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("墙体")), wallList);
		for (auto& wall : wallList)
		{
			if (wall->GetActorEnableCollision() == false || gameInstance->GetWallAdsorb() == false)
			{
				ActorsToIgnore.Add(wall);
			}
		}
		TArray<AActor*> floorList;  //场景中所有的地面
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("地面")), floorList);
		for (auto& floor : floorList)
		{
			if (floor->GetActorEnableCollision() == false)
			{
				ActorsToIgnore.Add(floor);
			}
		}
		if (gameInstance->GetObjectAdsorb() == false)
		{//不吸附模型，则射线检测忽略模型
			TArray<AActor*> objList;  //场景中其他的模型
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("普通模型")), objList);
			for (auto& obj : objList)
			{
				ActorsToIgnore.Add(obj);
			}
		}
		TArray<FHitResult> OutHits;
		FBox localBox = selActor->CalculateActorBound();  //获得模型的包围盒
		FVector boxMin = UKismetMathLibrary::TransformLocation(selActor->GetActorTransform(), localBox.Min);
		FVector boxMax = UKismetMathLibrary::TransformLocation(selActor->GetActorTransform(), localBox.Max);
		FVector newPos = selActor->GetActorLocation();
		FVector currentPos = newPos;   //模型的新位置
		currentPos.Z = (boxMin.Z + boxMax.Z) * 0.5;
		FVector xPos = currentPos + FVector((boxMax.X - boxMin.X) * 0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), currentPos, xPos, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		FVector xPos1 = currentPos + FVector(0.0f, (boxMax.Y - boxMin.Y) * 0.5, 0.0f);
		FVector xPos2 = xPos1 + FVector((boxMax.X - boxMin.X) * 0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), xPos1, xPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		xPos1 = xPos1 + FVector(0.0f,0.0f, -(boxMax.Z - boxMin.Z) * 0.5);
		xPos2 = xPos1 + FVector((boxMax.X - boxMin.X) * 0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), xPos1, xPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		xPos1 = currentPos + FVector(0.0f, -(boxMax.Y - boxMin.Y) * 0.5, 0.0f);
		xPos2 = xPos1 + FVector((boxMax.X - boxMin.X) * 0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), xPos1, xPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		xPos1 = xPos1 + FVector(0.0f, 0.0f, -(boxMax.Z - boxMin.Z) * 0.5);
		xPos2 = xPos1 + FVector((boxMax.X - boxMin.X) * 0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), xPos1, xPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		xPos = currentPos + FVector((boxMax.X - boxMin.X) * -0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), currentPos, xPos, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		xPos1 = currentPos + FVector(0.0f, (boxMax.Y - boxMin.Y) * 0.5, 0.0f);
		xPos2 = xPos1 + FVector((boxMax.X - boxMin.X) * -0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), xPos1, xPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		xPos1 = xPos1 + FVector(0.0f, 0.0f, -(boxMax.Z - boxMin.Z) * 0.5);
		xPos2 = xPos1 + FVector((boxMax.X - boxMin.X) * -0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), xPos1, xPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		xPos1 = currentPos + FVector(0.0f, -(boxMax.Y - boxMin.Y) * 0.5, 0.0f);
		xPos2 = xPos1 + FVector((boxMax.X - boxMin.X) * -0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), xPos1, xPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		xPos1 = xPos1 + FVector(0.0f, 0.0f, -(boxMax.Z - boxMin.Z) * 0.5);
		xPos2 = xPos1 + FVector((boxMax.X - boxMin.X) * -0.5, 0.0f, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), xPos1, xPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (xPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		FVector yPos = currentPos + FVector(0.0f, (boxMax.Y - boxMin.Y) * 0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), currentPos, yPos, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		FVector yPos1 = currentPos + FVector((boxMax.X - boxMin.X) * 0.5, 0.0f, 0.0f);
		FVector yPos2 = yPos1 + FVector(0.0f, (boxMax.Y - boxMin.Y) * 0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), yPos1, yPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		yPos1 = yPos1 + FVector(0.0f, 0.0f, -(boxMax.Z - boxMin.Z) * 0.5);
		yPos2 = yPos1 + FVector(0.0f, (boxMax.Y - boxMin.Y) * 0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), yPos1, yPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		yPos1 = currentPos + FVector((boxMax.X - boxMin.X) * -0.5, 0.0f, 0.0f);
		yPos2 = yPos1 + FVector(0.0f, (boxMax.Y - boxMin.Y) * 0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), yPos1, yPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		yPos1 = yPos1 + FVector(0.0f, 0.0f, -(boxMax.Z - boxMin.Z) * 0.5);
		yPos2 = yPos1 + FVector(0.0f, (boxMax.Y - boxMin.Y) * 0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), yPos1, yPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		yPos = currentPos + FVector(0.0f, (boxMax.Y - boxMin.Y) * -0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), currentPos, yPos, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		yPos1 = currentPos + FVector((boxMax.X - boxMin.X) * 0.5, 0.0f, 0.0f);
		yPos2 = yPos1 + FVector(0.0f, (boxMax.Y - boxMin.Y) * -0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), yPos1, yPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		yPos1 = yPos1 + FVector(0.0f, 0.0f, -(boxMax.Z - boxMin.Z) * 0.5);
		yPos2 = yPos1 + FVector(0.0f, (boxMax.Y - boxMin.Y) * -0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), yPos1, yPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		yPos1 = currentPos + FVector((boxMax.X - boxMin.X) * -0.5, 0.0f, 0.0f);
		yPos2 = yPos1 + FVector(0.0f, (boxMax.Y - boxMin.Y) * -0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), yPos1, yPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		yPos1 = yPos1 + FVector(0.0f, 0.0f, (boxMax.Z - boxMin.Z) * -0.5);
		yPos2 = yPos1 + FVector(0.0f, (boxMax.Y - boxMin.Y) * -0.5, 0.0f);
		if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), yPos1, yPos2, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
		{
			if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
			{
				FVector hitPos = OutHits[0].ImpactPoint;
				newPos = newPos - (yPos2 - hitPos);
				currentPos.X = newPos.X;
				currentPos.Y = newPos.Y;
			}
		}
		if (GetTravelType() != TravelType::TT_2D_Ortho)
		{//二维视图下移动模型时不改变z坐标
			FVector zPos = currentPos + FVector(0.0f, 0.0f, (boxMax.Z - boxMin.Z) * 0.5);
			if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), currentPos, zPos, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
			{
				if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
				{
					FVector hitPos = OutHits[0].ImpactPoint;
					newPos = newPos - (zPos - hitPos);
					currentPos.X = newPos.X;
					currentPos.Y = newPos.Y;
				}
			}
			zPos = currentPos + FVector(0.0f, 0.0f, (boxMax.Z - boxMin.Z) * -0.5);
			if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), currentPos, zPos, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true))
			{
				if (OutHits.Num() > 0 && OutHits[0].Distance > 0.0001)
				{
					FVector hitPos = OutHits[0].ImpactPoint;
					newPos = newPos - (zPos - hitPos);
				}
			}
		}
		selActor->SetActorLocation(newPos);
	}
	return;
}

//删除模型 
//只有除了门窗之外的普通模型才能被删除
void ABuildHouseCharacter::DeleteModel()
{
	if (GetWorld())
	{
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if(gameInstance)
		{
			AModulBaseActor* selActor = gameInstance->ReturnSelectModulBaseActor();
			AHouseGroupActor* selGroup = gameInstance->ReturnSelectGroupActor();
			ABuildHouseGameModeBase* gameModeBase = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
			if (selGroup && gameInstance && gameInstance->m_pScene)
			{//删除群组
				gameInstance->m_pScene->DeleteGroup(selGroup);  //删除群组
				gameInstance->SetSelectModulBaseActor(nullptr);  //重置选中的模型
				gameInstance->SetSelectGroupActor(nullptr);  //重置选中的群组
				gameModeBase->AddProdOperationWidget(false);  //隐藏产品操作UI
				gameModeBase->m_EntryWidget->UpdateProdInfor(nullptr, nullptr);//刷新产品信息
			}
			else if (selActor && gameInstance)
			{//删除单个模型
				OBJECT_STYLE modelStyle =  selActor->ReturnModelType();
				if (modelStyle == OBJECT_STYLE::NORMAL || modelStyle == OBJECT_STYLE::LIGHT)
				{
					gameInstance->m_pScene->DelObjectFlagForFloors(modelStyle, selActor);  //修正可见性
					selActor->Destroy();
					gameInstance->SetSelectModulBaseActor(nullptr);  //重置选中的模型
					if (gameModeBase)
					{
						gameModeBase->m_EntryWidget->UpdateProdInfor(nullptr,nullptr);//刷新产品信息
						gameModeBase->AddGizmo(false); //隐藏坐标轴
						gameModeBase->AddProdOperationWidget(false);  //隐藏产品操作UI
					}
				}
			}
		}
	}
}

//设置操作类型
void ABuildHouseCharacter::SetOperationType(OperationType type)
{ 
	if (OperationType::OT_MakeGroup == type)
	{//打群组
		if (m_OperationType == type)
		{//连续两次点击群组开关，认为是关闭群组功能
			FinishGroupOperation();
		}
		else
		{
			ABuildHouseGameModeBase* gameModeBase = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
			if (gameModeBase)
			{//隐藏菜单
				gameModeBase->AddProdOperationWidget(false);
			}
			UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
			AModulBaseActor* lastSel = gameInstance->ReturnSelectModulBaseActor();
			if (lastSel)
			{
				lastSel->SetOutline(false);//取消上次选中物体的描边
				lastSel->SetSelComponent(nullptr);  //重置选中的组件
				gameInstance->SetSelectModulBaseActor(nullptr); //重置选中的模型
			}
			AHouseGroupActor* lastSelGroup = gameInstance->ReturnSelectGroupActor();
			if (lastSelGroup)
			{
				lastSelGroup->SetOutline(false);  //取消上次选中群组的描边
				gameInstance->SetSelectGroupActor(nullptr);  //重置选中的群组
			}
			//创建新的群组
			FTransform trans;
			FActorSpawnParameters aa;
			TSubclassOf<AActor>modulbaseInstance = LoadClass<AActor>(NULL, TEXT("Blueprint'/Game/Blueprint/BP_HouseGroup.BP_HouseGroup_C'"));
			AHouseGroupActor* groupTemp = Cast<AHouseGroupActor>(GetWorld()->SpawnActor(modulbaseInstance, &trans, aa));  //创建群组
			if (groupTemp && gameInstance)
			{
				groupTemp->SetGroupName(TEXT("自定义群组"));
#if WITH_EDITOR
				groupTemp->SetActorLabel(*(groupTemp->ReturnGroupName()));
#endif 
				gameInstance->m_pScene->m_GroupList.push_back(groupTemp);
			}
			//设置光标形状
			GetWorld()->GetFirstPlayerController()->CurrentMouseCursor = EMouseCursor::Hand;

		}
	}
	//设置操作类型
	m_OperationType = type;
} 

//完成群组操作
void ABuildHouseCharacter::FinishGroupOperation()
{
	//认为超过一个模型才能形成群组，判断创建的群组中是否超过一个模型，如果不超过一个模型，则将群组删除
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance && gameInstance->m_pScene && gameInstance->m_pScene->m_GroupList.size() > 0)
	{
		AHouseGroupActor* groupTemp = gameInstance->m_pScene->m_GroupList.back();
		if (groupTemp && groupTemp->m_GroupUnitList.Num()<2)
		{//删除群组
			gameInstance->m_pScene->DeleteGroup(groupTemp);
		}
	}
	//恢复光标形状
	GetWorld()->GetFirstPlayerController()->CurrentMouseCursor = EMouseCursor::Default;
	//恢复初始状态
	SetOperationType(OperationType::OT_Normal);
	return;
}

//将鼠标选择的模型添加到群组中 
void ABuildHouseCharacter::AddPickedThingsToGroup()
{
	if (GetOperationType() != OperationType::OT_MakeGroup)
		return;  //当前是创建群组状态
	FHitResult hitResult;
	if (GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, hitResult))
	{
		AModulBaseActor* underCursorActor = Cast<AModulBaseActor>(hitResult.Actor.Get());
		if (underCursorActor && (underCursorActor->ActorHasTag(TEXT("普通模型"))))
		{
			UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
			if (gameInstance && gameInstance->m_pScene && gameInstance->m_pScene->m_GroupList.size() > 0)
			{
				AHouseGroupActor* groupTemp = gameInstance->m_pScene->m_GroupList.back();
				groupTemp->AddGroupUnit(underCursorActor->ReturnModelName());
			}
		}
	}
}