// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../../ThirdParty/fbxSDK/include/fbxsdk.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"

#include "ReadFbx.generated.h"



/**
 *表示材质信息
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FMaterialStruct
{
	GENERATED_USTRUCT_BODY()
public:
	FString materialname;   //材质名称
	bool useDiffuseTex;     //diffus是否是贴图 
	FVector diffuseColor;   //diffuse颜色
	FString diffuseTexName; //diffuse贴图的名称，这里没有使用绝对路径，默认贴图和fbx文件在一个文件夹下
	FString diffuseZipName; //diffuse贴图的zip文件，比如7889.txt
	float bumpWeight;       //凹凸的权重
	FString bumpTexName;    //凹凸贴图的名称，这里没有使用绝对路径，默认贴图和fbx文件在一个文件夹下
	FVector specularColor;  //specular颜色
	FVector2D moveUV;       //uv的偏移
	FVector2D scaleUV;      //uv的缩放
	float rotateUV;         //uv的旋转
public:
	FMaterialStruct() { InitMaterial(); }
	void InitMaterial()
	{//初始化
		materialname = "";
		useDiffuseTex = false; //默认没有使用贴图
		diffuseTexName = "";
		diffuseZipName = "";
		bumpTexName = "";     //默认没有凹凸
		diffuseColor = FVector{ 1.0f,1.0f,1.0f };  //默认白色
		specularColor = FVector{ 1.0f,1.0f,1.0f };  //默认白色
		moveUV = FVector2D::ZeroVector;   //默认UV没有移动
		scaleUV = FVector2D::UnitVector;  //默认uv没有缩放
		rotateUV = 0.0f;   //默认uv没有旋转
	}
};

/**
 *表示geometry的信息
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FGeometryStruct
{
	GENERATED_USTRUCT_BODY()
public:
	TArray<FVector> temp_normals;//法向量
	TArray<FVector> vertices; //模型中点的坐标
	TArray<int32> Triangles;//模型中点对应的三角面片点的标号，长度必为3的倍数
	TArray<FVector> normals;//模型中每个点对应的法向量坐标，和点坐标数组长度一致
	TArray<FVector2D> UV0;//模型中每个点对应的UV坐标，和点坐标数组长度一致
	TArray<FProcMeshTangent> tangents;//模型中每个点对应的切向量坐标，和点坐标数组长度一致
	TArray<FLinearColor> vertexColors;//每个点的颜色，和点坐标数组长度一致
	FMaterialStruct material;   //材质
public:
	void InitGeometry()
	{//初始化
		vertices.Empty();
		Triangles.Empty();
		normals.Empty();
		temp_normals.Empty();
		UV0.Empty();
		tangents.Empty();
		vertexColors.Empty();
		material.InitMaterial();  //材质初始化
	}
};

/**
 * 
 */
UCLASS()
class BUILDHOUSE_API UReadFbx : public UObject
{
	GENERATED_BODY()

public:
	void ReadFbxFile();   //读取fbx文件

private:
	FString FindFBXFiles();//返回要打开的FBX文件的绝对路径
	void GetFbxFilePath(FString path); //计算m_dxfFilePath
	bool InitializeFbxSdkObjects(FbxManager*& pManager, FbxScene*& pScene);  //初始化fbx sdk
	void DestroyFbxSdkObjects(FbxManager* pManager);       //释放fbx sdk
	bool LoadScene(FbxManager* pManager, FbxScene* pScene, const char* pFilename);//读入fbx文件，将fbx文件中的内容读到pScene中
	void ImportFbxSceneContent(FbxScene* pScene);  //从根结点递归遍历所有的结点，并读取
	void ImportFbxNodeContent(FbxNode* pNode);//获取子节点内数据信息，节点有各种属性，这里只需要对mesh节点做处理
	void ImportFbxMeshContent(FbxNode* pNode);//读取mesh节点中的模型几何形信息和相关的材质信息
	void ImportNormals(FbxMesh* pMesh);  //读取法向量
	void ImportPolygons(FbxMesh* pMesh);	//读取几何信息:点、uv坐标、
	void ImportMaterial(FbxGeometry* pGeometry);//读取材质信息
	FTransform ImportTransForm(FbxObject* pObject);//获得模型的位置、旋转、缩放

	void CreateModelBase(FbxNode *node);  //创建模型
private:
	FString m_dxfFilePath;           //临时变量，用于保存dxf文件的路径，这个值加上贴图名字等于贴图的绝对路径 
	FGeometryStruct m_GeometryTemp;  //临时变量，用于保存读取的fbxMesh信息
	TArray<FGeometryStruct> m_GeometryList;  //临时变量，表示由多个geometry组成的一个模型
};
