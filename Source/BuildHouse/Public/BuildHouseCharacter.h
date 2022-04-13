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
 * ��ǰ���ӽ�״̬������ο��ǿ���֣��ֱ���2D�������ӣ�3D�����˳����3D��һ�˳�����
 */
UENUM(BlueprintType)
enum class TravelType :uint8
{
	TT_2D_Ortho,//2D��������
	TT_3D_ThirdPerson,   //3D�����˳����
	TT_3D_FirstPerson   //3D��һ�˳�����
};

/**
 * ����״̬
 */
UENUM(BlueprintType)
enum class OperationType :uint8
{
	OT_Normal, //��ͨģʽ
	OT_Rotation3DView, //3ά��ת����
	OT_MoveModel,      //�ƶ�ģ��
	OT_CopyModel,      //����ģ��
	OT_MakeGroup,      //����Ⱥ��
	OT_MoveGroup       //�ƶ�Ⱥ��
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
	void BindOperationBinding(UInputComponent* PlayerInputComponent);  //�󶨶���ӳ��
	void ForwardMove(float axisValue); //ǰ���ƶ�������x��
	void LeftMove(float axisValue);    //�����ƶ�������y��
	void UpMove(float axisValue);      //�����ƶ�������z��
	void LeftRotation(float axisValue);               //������ת
	void UpRotation(float axisValue);                 //������ת
	void LeftMouseButtonUp();                         //����ɿ�
	void LeftMouseButtonDown();                       //�������
	void RightMouseButtonUp();                        //�Ҽ��ɿ�
	void SetTravelType(TravelType type);              //������������
	TravelType GetTravelType();                       //������������
	void SetSceneVisible();                           //���ó����е�ǽ����ߵ����Ƿ�ɼ�
	UFUNCTION(BlueprintCallable, Category = "BuildHouseCharacter")
	void SetOperationType(OperationType type); //���ò�������
	OperationType GetOperationType() { return m_OperationType; }  //���ز�������
	void MoveModel3D();                                //�ƶ�ģ�ͣ���ά 
	void MoveModel2D();                                //�ƶ�ģ��,��ά
	void MoveView2D();                                 //�ƶ���ά��ͼ
	void ChangeViewType(FString viewType);             //��ά��ά�л� 
	void ResetView2D();                                //���ö�ά��charactor��λ��
	void HideCeilingModel(bool hide);                  //���õ�����ģ�͵���ʾ������
	void HandleAdsorb(AModulBaseActor* selActor);       //��������
	void DeleteModel();                                 //ɾ��ģ�� 
	void ReturnCamera(FVector& cameraPos, FVector& cameraDir);  //���������λ�úͷ���
	void FinishGroupOperation();  //���Ⱥ�����
	void AddPickedThingsToGroup();  //�����ѡ���ģ����ӵ�Ⱥ���� 
public:
	FInputAxisKeyMapping m_ForwardMoveBinding;   //ǰ���ƶ�
	FInputAxisKeyMapping m_BackMoveBinding;
	FInputAxisKeyMapping m_ForwardMoveBinding2;
	FInputAxisKeyMapping m_LeftMoveBinding;      //�����ƶ�
	FInputAxisKeyMapping m_RightMoveBinding;
	FInputAxisKeyMapping m_UpMoveBinding;      //�����ƶ�
	FInputAxisKeyMapping m_DownMoveBinding;
	FInputAxisKeyMapping m_LeftRotateBinding;  //������ת
	FInputAxisKeyMapping m_UpRotateBinding;    //������ת
	FInputActionKeyMapping  m_LbnUpBinding;  //�������ɿ�
	FInputActionKeyMapping  m_LbnDownBinding;  //����������
	FInputActionKeyMapping  m_RbnUpBinding;  //����Ҽ��ɿ�
	FInputActionKeyMapping  m_DeleteBinding;  //delete��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildHouseCharacter")
	USpringArmComponent* SpringArmComponent;  //ҡ�۽ڵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildHouseCharacter")
	UCameraComponent* ThirdPersonCameraComponent;  //����ڵ㣬���ڵ����˳�����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildHouseCharacter")
	UCameraComponent* FirstPersonCameraComponent;  //����ڵ㣬���ڵ�һ�˳�����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildHouseCharacter")
	UCameraComponent* OrthoViewCameraComponent;  //����ڵ㣬����2D�����ӽ�
	UPROPERTY()
	TravelType m_TravelType;                   //��������
	UPROPERTY()
	FVector2D m_LeftMouseButtonDownPos;         //���������µ�λ��
	UPROPERTY()
	FVector2D m_LeftMouseButtonUpPos;            //�����������λ��
	UPROPERTY()
	OperationType m_OperationType;         //��������
	AMiniMapCamera* m_MiniMap;  //С��ͼ
};
