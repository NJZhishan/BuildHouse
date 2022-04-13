// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerInput.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "ModulBaseActor.h"
#include "MiniMapCamera.h"
#include "BuildHouseCharacter.generated.h"

/**
 * 当前的视角状态，这里参考是酷家乐，分别是2D正交俯视，3D第三人称鸟瞰，3D第一人称漫游
 */
UENUM(BlueprintType)
enum class TravelType :uint8
{
	TT_2D_Ortho,//2D正交俯视
	TT_3D_ThirdPerson,   //3D第三人称鸟瞰
	TT_3D_FirstPerson   //3D第一人称漫游
};

/**
 * 操作状态
 */
UENUM(BlueprintType)
enum class OperationType :uint8
{
	OT_Normal, //普通模式
	OT_Rotation3DView, //3维旋转场景
	OT_MoveModel,      //移动模型
	OT_CopyModel,      //复制模型
	OT_MakeGroup,      //创建群组
	OT_MoveGroup       //移动群组
};

UCLASS(Blueprintable)
class BUILDHOUSE_API ABuildHouseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABuildHouseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void BindOperationBinding(UInputComponent* PlayerInputComponent);  //绑定动作映射
	void ForwardMove(float axisValue); //前后移动，沿着x轴
	void LeftMove(float axisValue);    //左右移动，沿着y轴
	void UpMove(float axisValue);      //上下移动，沿着z轴
	void LeftRotation(float axisValue);               //左右旋转
	void UpRotation(float axisValue);                 //上下旋转
	void LeftMouseButtonUp();                         //左键松开
	void LeftMouseButtonDown();                       //左键按下
	void RightMouseButtonUp();                        //右键松开
	void SetTravelType(TravelType type);              //设置漫游类型
	TravelType GetTravelType();                       //返回漫游类型
	void SetSceneVisible();                           //设置场景中的墙体或者吊顶是否可见
	UFUNCTION(BlueprintCallable, Category = "BuildHouseCharacter")
	void SetOperationType(OperationType type); //设置操作类型
	OperationType GetOperationType() { return m_OperationType; }  //返回操作类型
	void MoveModel3D();                                //移动模型，三维 
	void MoveModel2D();                                //移动模型,二维
	void MoveView2D();                                 //移动二维视图
	void ChangeViewType(FString viewType);             //二维三维切换 
	void ResetView2D();                                //设置二维中charactor的位置
	void HideCeilingModel(bool hide);                  //设置吊顶上模型的显示和隐藏
	void HandleAdsorb(AModulBaseActor* selActor);       //处理吸附
	void DeleteModel();                                 //删除模型 
	void ReturnCamera(FVector& cameraPos, FVector& cameraDir);  //返回相机的位置和方向
	void FinishGroupOperation();  //完成群组操作
	void AddPickedThingsToGroup();  //将鼠标选择的模型添加到群组中 
public:
	FInputAxisKeyMapping m_ForwardMoveBinding;   //前后移动
	FInputAxisKeyMapping m_BackMoveBinding;
	FInputAxisKeyMapping m_ForwardMoveBinding2;
	FInputAxisKeyMapping m_LeftMoveBinding;      //左右移动
	FInputAxisKeyMapping m_RightMoveBinding;
	FInputAxisKeyMapping m_UpMoveBinding;      //上下移动
	FInputAxisKeyMapping m_DownMoveBinding;
	FInputAxisKeyMapping m_LeftRotateBinding;  //左右旋转
	FInputAxisKeyMapping m_UpRotateBinding;    //上下旋转
	FInputActionKeyMapping  m_LbnUpBinding;  //鼠标左键松开
	FInputActionKeyMapping  m_LbnDownBinding;  //鼠标左键按下
	FInputActionKeyMapping  m_RbnUpBinding;  //鼠标右键松开
	FInputActionKeyMapping  m_DeleteBinding;  //delete键

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildHouseCharacter")
	USpringArmComponent* SpringArmComponent;  //摇臂节点
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildHouseCharacter")
	UCameraComponent* ThirdPersonCameraComponent;  //相机节点，用于第三人称漫游
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildHouseCharacter")
	UCameraComponent* FirstPersonCameraComponent;  //相机节点，用于第一人称漫游
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildHouseCharacter")
	UCameraComponent* OrthoViewCameraComponent;  //相机节点，用于2D正交视角
	UPROPERTY()
	TravelType m_TravelType;                   //漫游类型
	UPROPERTY()
	FVector2D m_LeftMouseButtonDownPos;         //鼠标左键按下的位置
	UPROPERTY()
	FVector2D m_LeftMouseButtonUpPos;            //鼠标左键弹起的位置
	UPROPERTY()
	OperationType m_OperationType;         //操作类型
	AMiniMapCamera* m_MiniMap;  //小地图
};
