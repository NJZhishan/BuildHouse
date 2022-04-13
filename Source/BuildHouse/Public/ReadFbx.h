// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../../ThirdParty/fbxSDK/include/fbxsdk.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"

#include "ReadFbx.generated.h"



/**
 *��ʾ������Ϣ
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FMaterialStruct
{
	GENERATED_USTRUCT_BODY()
public:
	FString materialname;   //��������
	bool useDiffuseTex;     //diffus�Ƿ�����ͼ 
	FVector diffuseColor;   //diffuse��ɫ
	FString diffuseTexName; //diffuse��ͼ�����ƣ�����û��ʹ�þ���·����Ĭ����ͼ��fbx�ļ���һ���ļ�����
	FString diffuseZipName; //diffuse��ͼ��zip�ļ�������7889.txt
	float bumpWeight;       //��͹��Ȩ��
	FString bumpTexName;    //��͹��ͼ�����ƣ�����û��ʹ�þ���·����Ĭ����ͼ��fbx�ļ���һ���ļ�����
	FVector specularColor;  //specular��ɫ
	FVector2D moveUV;       //uv��ƫ��
	FVector2D scaleUV;      //uv������
	float rotateUV;         //uv����ת
public:
	FMaterialStruct() { InitMaterial(); }
	void InitMaterial()
	{//��ʼ��
		materialname = "";
		useDiffuseTex = false; //Ĭ��û��ʹ����ͼ
		diffuseTexName = "";
		diffuseZipName = "";
		bumpTexName = "";     //Ĭ��û�а�͹
		diffuseColor = FVector{ 1.0f,1.0f,1.0f };  //Ĭ�ϰ�ɫ
		specularColor = FVector{ 1.0f,1.0f,1.0f };  //Ĭ�ϰ�ɫ
		moveUV = FVector2D::ZeroVector;   //Ĭ��UVû���ƶ�
		scaleUV = FVector2D::UnitVector;  //Ĭ��uvû������
		rotateUV = 0.0f;   //Ĭ��uvû����ת
	}
};

/**
 *��ʾgeometry����Ϣ
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FGeometryStruct
{
	GENERATED_USTRUCT_BODY()
public:
	TArray<FVector> temp_normals;//������
	TArray<FVector> vertices; //ģ���е������
	TArray<int32> Triangles;//ģ���е��Ӧ��������Ƭ��ı�ţ����ȱ�Ϊ3�ı���
	TArray<FVector> normals;//ģ����ÿ�����Ӧ�ķ��������꣬�͵��������鳤��һ��
	TArray<FVector2D> UV0;//ģ����ÿ�����Ӧ��UV���꣬�͵��������鳤��һ��
	TArray<FProcMeshTangent> tangents;//ģ����ÿ�����Ӧ�����������꣬�͵��������鳤��һ��
	TArray<FLinearColor> vertexColors;//ÿ�������ɫ���͵��������鳤��һ��
	FMaterialStruct material;   //����
public:
	void InitGeometry()
	{//��ʼ��
		vertices.Empty();
		Triangles.Empty();
		normals.Empty();
		temp_normals.Empty();
		UV0.Empty();
		tangents.Empty();
		vertexColors.Empty();
		material.InitMaterial();  //���ʳ�ʼ��
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
	void ReadFbxFile();   //��ȡfbx�ļ�

private:
	FString FindFBXFiles();//����Ҫ�򿪵�FBX�ļ��ľ���·��
	void GetFbxFilePath(FString path); //����m_dxfFilePath
	bool InitializeFbxSdkObjects(FbxManager*& pManager, FbxScene*& pScene);  //��ʼ��fbx sdk
	void DestroyFbxSdkObjects(FbxManager* pManager);       //�ͷ�fbx sdk
	bool LoadScene(FbxManager* pManager, FbxScene* pScene, const char* pFilename);//����fbx�ļ�����fbx�ļ��е����ݶ���pScene��
	void ImportFbxSceneContent(FbxScene* pScene);  //�Ӹ����ݹ�������еĽ�㣬����ȡ
	void ImportFbxNodeContent(FbxNode* pNode);//��ȡ�ӽڵ���������Ϣ���ڵ��и������ԣ�����ֻ��Ҫ��mesh�ڵ�������
	void ImportFbxMeshContent(FbxNode* pNode);//��ȡmesh�ڵ��е�ģ�ͼ�������Ϣ����صĲ�����Ϣ
	void ImportNormals(FbxMesh* pMesh);  //��ȡ������
	void ImportPolygons(FbxMesh* pMesh);	//��ȡ������Ϣ:�㡢uv���ꡢ
	void ImportMaterial(FbxGeometry* pGeometry);//��ȡ������Ϣ
	FTransform ImportTransForm(FbxObject* pObject);//���ģ�͵�λ�á���ת������

	void CreateModelBase(FbxNode *node);  //����ģ��
private:
	FString m_dxfFilePath;           //��ʱ���������ڱ���dxf�ļ���·�������ֵ������ͼ���ֵ�����ͼ�ľ���·�� 
	FGeometryStruct m_GeometryTemp;  //��ʱ���������ڱ����ȡ��fbxMesh��Ϣ
	TArray<FGeometryStruct> m_GeometryList;  //��ʱ��������ʾ�ɶ��geometry��ɵ�һ��ģ��
};
