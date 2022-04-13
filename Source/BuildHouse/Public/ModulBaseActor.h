// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "ReadFbx.h"
#include <map>
#include "DownloadFile.h"
#include "RoomMaterialChange.h"
#include "ModulBaseActor.generated.h"

/**
 * ��ת����
 */
UENUM(BlueprintType)
enum class RotationType :uint8
{
	RT_Pitch,
	RT_Yaw,
	RT_Roll
};

/**
 * ��������
 */
UENUM(BlueprintType)
enum class OBJECT_STYLE :uint8
{
	NORMAL,
	DOOR,
	WINDOW,
	LIGHT,
	WALL,
	CEILING,
	FLOOR,
	GROUP,
	UNDIFINE
};


UCLASS(Blueprintable)
class BUILDHOUSE_API AModulBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AModulBaseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UFUNCTION(BlueprintCallable)
	void BeginCursorOver();  //������actorʱ����Ӧ����
	UFUNCTION(BlueprintCallable)
	FBox CalculateActorBound();  //����ģ�Ͱ�Χ��(�ֲ�����ϵ)
	UFUNCTION(BlueprintCallable)
	void CopyModelBase();  //����ģ��
	UFUNCTION(BlueprintCallable)
	void MoveModelBase();  //�ƶ�ģ��
	UFUNCTION(BlueprintCallable)
	void LeftMirrorModelBase();  //���Ҿ���ģ��
	UFUNCTION(BlueprintCallable)
	void FrontMirrorModelBase();  //ǰ����ģ��
public:
	void CreateMesh(TArray<FVector>& vertices, TArray<int32>& Triangles, TArray<FVector>& normals, TArray<FVector2D>& UV0, TArray<FProcMeshTangent>& tangents, TArray<FLinearColor>& vertexColors, int id, bool bCollision = true); //����geometry
	UMaterialInstance* LoadMaterialResource(FString materialName);  //���������Դ
	void CreateProceduralMeshMaterial(const FMaterialStruct* mat, UProceduralMeshComponent* mesh, int textureIndex); //ΪproceduralMesh��������
	bool ModifyMeshAndTextureIndex(UProceduralMeshComponent* &mesh, int& textureIndex, int& meshIndex);     //���meshIndex,tetureIndex,proceduralMesh
	void ModifyProceduralMeshUV(UProceduralMeshComponent* mesh, int textureIndex, FVector2D moveUV, FVector2D scaleUV, float rotateUV);   //�޸�uv
	void ModifyProceduralMeshColor(UProceduralMeshComponent* mesh, int textureIndex, FVector diffuseColor);       //�޸�diffuse��ɫ
	void ModifyProceduralMeshMaterial(UProceduralMeshComponent* mesh, int textureIndex, FString materialname,FVector specularColor =FVector::OneVector,bool changSpecularColor = false);    //�޸Ĳ���
	void ReplaceTexture(UProceduralMeshComponent* mesh, int textureIndex, FString& fileName, FString matName = TEXT(""), FVector specularColor = FVector::OneVector);  //�滻��ͼ����������ͼ����txt�ļ��ľ���·��
	void SetOutline(bool outLine);   //�������
	void SetSelComponent(UProceduralMeshComponent* selComponent);  //����ѡ�е����
	UProceduralMeshComponent* ReturnSelComponent();  //����ѡ�е����
	void DownloadTextureFinished(FString fileName);  //�滻����ͼ�������
	void CreateNewObject(const FString &fileName); //����ģ�ͣ�������ģ������txt�ļ��ľ���·��
	void SetModelName(FString name, int style = -1);  //����ģ�͵�����
	FString ReturnModelName();//����ģ�͵�����
	void SetProdInfor(FDownloadInforStruct infor);  //���ò�Ʒ��Ϣ
	FDownloadInforStruct ReturnProdInfor();         //��ò�Ʒ��Ϣ
	void RotateModel(RotationType rotType, float delta);//��תģ��
	void RotateModel(float newAngle);              //��תģ��
	OBJECT_STYLE ReturnModelType();
	void SetModulBaseActorShow(bool show);       //����ģ����ʾ��������
	bool GetModulBaseActorShow();       //����ģ����ʾ��������
	void ChangeMaterialBasedRoomMaterialChange();   //����RoomMaterialChange�޸Ĳ���
private:
	std::string ChangeZipToBmp(std::string filename);
	void RefreshActorBoundBox(FVector min, FVector max);  //����ģ�Ͱ�Χ��
	void ChangeProceduralMeshMaterial(const FMaterialStruct* mat, UProceduralMeshComponent* mesh);  //�޸Ĳ���
public:
	UPROPERTY()
	FString m_txtPath;    //ģ��ʹ��txt�����֣�����5667.txt
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modelbase")
	USceneComponent* MyRootComponent;  //���ڵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modelbase")
	UStaticMeshComponent* BoundComponent;        //��Χ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modelbase")
	TArray<UProceduralMeshComponent*> MeshComponentList;  //geometry�ڵ�����
	std::vector<CRoomMaterialChange*>m_materialChangeList;    //ģ�ͱ��ز�����ʱ���ñ�����ʱ��������޸Ķ���
private:
	UPROPERTY()
	UProceduralMeshComponent* m_selProceduralMeshComponent;            //��ǰѡ�е����
	std::map<int, TArray<int>>m_TextureAndProceduralMeshMatches;       //texture��ProceduralMesh�Ķ�Ӧ��ϵ����һ��������ʾtexture����ţ��ڶ���������ʾ����ʹ����texture��ProceduralMesh��
	std::map<int, FMaterialStruct>m_TextureAndDynamicMaterialMatches;  //texture�Ͳ�����Ķ�Ӧ��ϵ����һ��������ʾtexture����ţ��ڶ���������ʾ������
	UPROPERTY()
	FString m_ModelName;  //��Ʒ���ƣ����磬A�������ƣ�Ϊ��ÿ��actor��������Ψһ�ģ���������:styleIndex_ObjectIndex_��Ʒ����_���
	FDownloadInforStruct m_prodInfor;   //��Ʒ��Ϣ
public:
	static std::string string_To_UTF8(const std::string& str);
	static std::string UTF8_To_string(std::string str);
};
