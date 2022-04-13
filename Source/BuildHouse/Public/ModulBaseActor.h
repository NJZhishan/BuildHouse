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
 * 旋转类型
 */
UENUM(BlueprintType)
enum class RotationType :uint8
{
	RT_Pitch,
	RT_Yaw,
	RT_Roll
};

/**
 * 对象类型
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
	void BeginCursorOver();  //鼠标进入actor时的响应函数
	UFUNCTION(BlueprintCallable)
	FBox CalculateActorBound();  //计算模型包围盒(局部坐标系)
	UFUNCTION(BlueprintCallable)
	void CopyModelBase();  //复制模型
	UFUNCTION(BlueprintCallable)
	void MoveModelBase();  //移动模型
	UFUNCTION(BlueprintCallable)
	void LeftMirrorModelBase();  //左右镜像模型
	UFUNCTION(BlueprintCallable)
	void FrontMirrorModelBase();  //前后镜像模型
public:
	void CreateMesh(TArray<FVector>& vertices, TArray<int32>& Triangles, TArray<FVector>& normals, TArray<FVector2D>& UV0, TArray<FProcMeshTangent>& tangents, TArray<FLinearColor>& vertexColors, int id, bool bCollision = true); //创建geometry
	UMaterialInstance* LoadMaterialResource(FString materialName);  //导入材质资源
	void CreateProceduralMeshMaterial(const FMaterialStruct* mat, UProceduralMeshComponent* mesh, int textureIndex); //为proceduralMesh创建材质
	bool ModifyMeshAndTextureIndex(UProceduralMeshComponent* &mesh, int& textureIndex, int& meshIndex);     //获得meshIndex,tetureIndex,proceduralMesh
	void ModifyProceduralMeshUV(UProceduralMeshComponent* mesh, int textureIndex, FVector2D moveUV, FVector2D scaleUV, float rotateUV);   //修改uv
	void ModifyProceduralMeshColor(UProceduralMeshComponent* mesh, int textureIndex, FVector diffuseColor);       //修改diffuse颜色
	void ModifyProceduralMeshMaterial(UProceduralMeshComponent* mesh, int textureIndex, FString materialname,FVector specularColor =FVector::OneVector,bool changSpecularColor = false);    //修改材质
	void ReplaceTexture(UProceduralMeshComponent* mesh, int textureIndex, FString& fileName, FString matName = TEXT(""), FVector specularColor = FVector::OneVector);  //替换贴图，参数是贴图所在txt文件的绝对路径
	void SetOutline(bool outLine);   //设置描边
	void SetSelComponent(UProceduralMeshComponent* selComponent);  //设置选中的组件
	UProceduralMeshComponent* ReturnSelComponent();  //返回选中的组件
	void DownloadTextureFinished(FString fileName);  //替换的贴图下载完成
	void CreateNewObject(const FString &fileName); //创建模型，参数是模型所在txt文件的绝对路径
	void SetModelName(FString name, int style = -1);  //设置模型的名字
	FString ReturnModelName();//返回模型的名字
	void SetProdInfor(FDownloadInforStruct infor);  //设置产品信息
	FDownloadInforStruct ReturnProdInfor();         //获得产品信息
	void RotateModel(RotationType rotType, float delta);//旋转模型
	void RotateModel(float newAngle);              //旋转模型
	OBJECT_STYLE ReturnModelType();
	void SetModulBaseActorShow(bool show);       //设置模型显示或者隐藏
	bool GetModulBaseActorShow();       //返回模型显示或者隐藏
	void ChangeMaterialBasedRoomMaterialChange();   //根据RoomMaterialChange修改材质
private:
	std::string ChangeZipToBmp(std::string filename);
	void RefreshActorBoundBox(FVector min, FVector max);  //更新模型包围盒
	void ChangeProceduralMeshMaterial(const FMaterialStruct* mat, UProceduralMeshComponent* mesh);  //修改材质
public:
	UPROPERTY()
	FString m_txtPath;    //模型使用txt的名字，比如5667.txt
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modelbase")
	USceneComponent* MyRootComponent;  //根节点
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modelbase")
	UStaticMeshComponent* BoundComponent;        //包围盒
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modelbase")
	TArray<UProceduralMeshComponent*> MeshComponentList;  //geometry节点序列
	std::vector<CRoomMaterialChange*>m_materialChangeList;    //模型本地不存在时，该变量临时保存材质修改对象
private:
	UPROPERTY()
	UProceduralMeshComponent* m_selProceduralMeshComponent;            //当前选中的组件
	std::map<int, TArray<int>>m_TextureAndProceduralMeshMatches;       //texture和ProceduralMesh的对应关系；第一个参数表示texture的序号，第二个参数表示所有使用了texture的ProceduralMesh；
	std::map<int, FMaterialStruct>m_TextureAndDynamicMaterialMatches;  //texture和材质球的对应关系；第一个参数表示texture的序号，第二个参数表示材质球
	UPROPERTY()
	FString m_ModelName;  //产品名称，比如，A客厅吊灯，为了每个actor的名字是唯一的，设置名字:styleIndex_ObjectIndex_产品名称_编号
	FDownloadInforStruct m_prodInfor;   //产品信息
public:
	static std::string string_To_UTF8(const std::string& str);
	static std::string UTF8_To_string(std::string str);
};
