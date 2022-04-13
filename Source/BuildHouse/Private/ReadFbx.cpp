// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadFbx.h"
#include "Engine/Engine.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Core/Public/HAL/FileManagerGeneric.h"
#include "../Public/ModulBaseActor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../Public/BuildHouseGameInstance.h"
#include "../Public/BuildHouseCharacter.h"

//读取fbx文件
void UReadFbx::ReadFbxFile()
{
	FString FBXFilePath = FindFBXFiles();  //获得要导入的fbx的绝对路径
	UBuildHouseGameInstance* gameInstance = nullptr;
	if (GetWorld())
	{//先将场景清空
		gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		gameInstance->m_pScene->InitScene();
	}
	if (!FBXFilePath.IsEmpty() && gameInstance)
	{
		FbxManager* lSdkManager = NULL;
		FbxScene* lScene = NULL;
		bool lResult = InitializeFbxSdkObjects(lSdkManager, lScene);  //初始化fbx sdk
		if (!lResult)
			return;
		FbxString lFilePath(TCHAR_TO_UTF8(*FBXFilePath));  //避免路径中包含中文时无法导入
		lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer()); //读入fbx文件，将fbx文件中的内容读到pScene中
		if (lResult)
		{
			ImportFbxSceneContent(lScene); //读取FBX的信息并往场景内导入模型
		}
		DestroyFbxSdkObjects(lSdkManager);  //释放fbx sdk
		//读取场景信息文件
		bool bReadScene = false;
		int npos = -1;
		if (FBXFilePath.FindLastChar('/', npos))
		{
			FString sceneFile = FBXFilePath.Left(npos) + FString("/sceneInfor.txt");
			bReadScene = gameInstance->m_pScene->ReadSceneFile(sceneFile, GetWorld());
			if(bReadScene == false)
			{//读取fbx文件失败，则清空scene并返回二维
				gameInstance->m_pScene->InitScene();
				ABuildHouseCharacter* charactor = Cast<ABuildHouseCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
				if (charactor)
				{
					charactor->SetTravelType(TravelType::TT_2D_Ortho);
				}
			}
		}
	}
	return;
}

//返回要打开的FBX文件的绝对路径
FString UReadFbx::FindFBXFiles()
{
	TArray<FString> OpenFilenames;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	bool bOpened = false;
	if (DesktopPlatform != NULL)
	{
		bOpened = DesktopPlatform->OpenFileDialog(
			NULL,
			TEXT("Import fbx file"),
			//*(FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_IMPORT)),
			TEXT(""),
			TEXT(""),
			TEXT("FBX document|*.fbx"),
			EFileDialogFlags::None,
			OpenFilenames);
	}
	if (bOpened)
	{
		FString ImportFilename = OpenFilenames[0];
		FString FileName = OpenFilenames[0];
		//FEditorDirectories::Get().SetLastDirectory(ELastDirectory::GENERIC_IMPORT, FPaths::GetPath(FileName)); 
		const FString FileExtension = FPaths::GetExtension(FileName);   //fbx文件的绝对路径
		const bool bIsFBX = FCString::Stricmp(*FileExtension, TEXT("FBX")) == 0;
		if (bIsFBX)
		{
			GetFbxFilePath(FileName);
			return FileName;
		}
	}
	return "";
}

//计算m_dxfFilePath
void UReadFbx::GetFbxFilePath(FString path)
{
	int npos = -1;
	bool bSuc = path.FindLastChar('/', npos);
	if (bSuc)
	{
		path = path.Left(npos);
		m_dxfFilePath = path + _T("/");
	}
	return;
}

//初始化fbx sdk
bool UReadFbx::InitializeFbxSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		return false;
	}
	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	//FbxString lPath = FbxGetApplicationDirectory();
	//pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "MyScene");
	if (!pScene)
	{
		return false;
	}
	return true;
}

//释放fbx sdk
void UReadFbx::DestroyFbxSdkObjects(FbxManager* pManager)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) 
		pManager->Destroy();
}

//读入fbx文件，将fbx文件中的内容读到pScene中
bool UReadFbx::LoadScene(FbxManager* pManager, FbxScene* pScene, const char* pFilename)
{
	bool lStatus;
	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");
	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	if (!lImportStatus)
	{
		return false;
	}
	// Import the scene.
	lStatus = lImporter->Import(pScene);
	// Destroy the importer.
	lImporter->Destroy();
	return lStatus;
}

//从根结点递归遍历所有的结点，并读取
void UReadFbx::ImportFbxSceneContent(FbxScene* pScene)
{
	int i = 0;
	FbxNode* lNode = pScene->GetRootNode();  //获得fbx scene的根节点
	if (lNode)
	{
		for (i = 0; i < lNode->GetChildCount(); i++)
		{//依次读取所有的子节点
			FbxNode* sunNode = lNode->GetChild(i);  //获得子节点
			m_GeometryList.Empty();  //清空geometry序列
			ImportFbxNodeContent(sunNode);   //导入子节点
			//创建模型
			if (m_GeometryList.Num() != 0)
			{//FbxNode下面有geometry，则创建mesh
				CreateModelBase(sunNode);
			}
		}
	}
}

//获取子节点内数据信息，节点有各种属性，这里只需要对mesh节点做处理
void UReadFbx::ImportFbxNodeContent(FbxNode* pNode)
{
	FbxNodeAttribute::EType lAttributeType;
	if (pNode->GetNodeAttribute())   //FbxNode下面有FbxAttribute
	{
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());//获取节点属性类中包含的节点类型信息
		switch (lAttributeType) //根据节点类型对节点进行分类处理
		{
			case FbxNodeAttribute::eMesh: //只处理mesh节点，表示模型的一个geometry
			{
				m_GeometryTemp.InitGeometry();   //数据初始化
				ImportFbxMeshContent(pNode); //读取mesh节点中的模型几何形信息和相关的材质信息
				for (int i = 0; i < m_GeometryTemp.vertices.Num(); i += 3)
				{
					m_GeometryTemp.Triangles.Add(i);
					m_GeometryTemp.Triangles.Add(i + 1);
					m_GeometryTemp.Triangles.Add(i + 2);
				}
				if (m_GeometryTemp.normals.Num() == 0)
				{
					m_GeometryTemp.normals.Empty();
					for (int i = 0; i < m_GeometryTemp.vertices.Num(); i++)
						m_GeometryTemp.normals.Add(FVector(0, 0, -1));
				}
				if (m_GeometryTemp.tangents.Num() == 0)
					for (int i = 0; i < m_GeometryTemp.vertices.Num(); i++)
						m_GeometryTemp.tangents.Add(FProcMeshTangent(0, 1, 0));
				if (m_GeometryTemp.vertexColors.Num() == 0)
					for (int i = 0; i < m_GeometryTemp.vertices.Num(); i++)
						m_GeometryTemp.vertexColors.Add(FLinearColor(1.0, 1.0, 1.0, 1.0));
				//保存读取的mesh
				m_GeometryList.Add(m_GeometryTemp);
			}
			break;
			default:
				break;
		}
	}
	//继续读取该节点的其他子节点
	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		ImportFbxNodeContent(pNode->GetChild(i));
	}
}

//读取mesh节点中的模型几何形信息和相关的材质信息
void UReadFbx::ImportFbxMeshContent(FbxNode* pNode)
{
	FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();  //获取当前节点
	ImportNormals(lMesh);   //读取法向量
	ImportPolygons(lMesh);  //读取该模型节点中的几何信息
	ImportMaterial(lMesh);//读取该模型节点中的材质信息
}

 //读取法向量
void UReadFbx::ImportNormals(FbxMesh* pMesh)
{
	int i, lControlPointsCount = pMesh->GetControlPointsCount();  //获得控制点的个数
	FbxVector4* lControlPoints = pMesh->GetControlPoints();
	for (i = 0; i < lControlPointsCount; i++)
	{
		for (int j = 0; j < pMesh->GetElementNormalCount(); j++)
		{
			FbxGeometryElementNormal* leNormals = pMesh->GetElementNormal(j);
			if (leNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FbxVector4 Normal = leNormals->GetDirectArray().GetAt(i);
					m_GeometryTemp.temp_normals.Add(FVector(Normal[0], Normal[2], Normal[1]));
				}
			}
		}
	}
}

//读取该模型节点中的几何信息
void UReadFbx::ImportPolygons(FbxMesh* pMesh)
{
	int i, j, lPolygonCount = pMesh->GetPolygonCount();  //获取构成该模型的几何面的数量
	FbxVector4* lControlPoints = pMesh->GetControlPoints(); //坐标点的数组指针
	int vertexId = 0;
	for (i = 0; i < lPolygonCount; i++)
	{
		int l;
		int lPolygonSize = pMesh->GetPolygonSize(i);//获取每个多边形面中点的数量
		for (j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j); //第i个多边形中的第j个坐标点
			m_GeometryTemp.vertices.Add(FVector(lControlPoints[lControlPointIndex][0]*100, lControlPoints[lControlPointIndex][2] * 100, lControlPoints[lControlPointIndex][1] * 100));
			if (m_GeometryTemp.temp_normals.Num() != 0)
			{
				m_GeometryTemp.normals.Add(m_GeometryTemp.temp_normals[lControlPointIndex]);
			}
			for (l = 0; l < pMesh->GetElementUVCount(); ++l)
			{
				FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);
				FbxVector2 UV;
				switch (leUV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:   //UV坐标以控制点方式分布
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						UV = leUV->GetDirectArray().GetAt(lControlPointIndex);
						m_GeometryTemp.UV0.Add(FVector2D(UV[0], UV[1]));
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						//Display2DVector(header, leUV->GetDirectArray().GetAt(id));
						UV = leUV->GetDirectArray().GetAt(id);
						m_GeometryTemp.UV0.Add(FVector2D(UV[0], UV[1]));
					}
					break;
					default:
						break;
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:  //根据多边形的方式分布
				{
					int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						//Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
						UV = leUV->GetDirectArray().GetAt(lTextureUVIndex);
						m_GeometryTemp.UV0.Add(FVector2D(UV[0], UV[1]));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}
			for (l = 0; l < pMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);
				FbxVector4 Normal;
				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						//Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
						Normal = leNormal->GetDirectArray().GetAt(vertexId);
						m_GeometryTemp.normals.Add(FVector(Normal[0], Normal[2], Normal[1]));
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(vertexId);
						//Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
						Normal = leNormal->GetDirectArray().GetAt(id);
						m_GeometryTemp.normals.Add(FVector(Normal[0], Normal[2], Normal[1]));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for (l = 0; l < pMesh->GetElementTangentCount(); ++l)
			{
				FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(l);
				if (leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leTangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						//Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));
						FbxVector4 TangentTemp = leTangent->GetDirectArray().GetAt(vertexId);
						m_GeometryTemp.tangents.Add(FProcMeshTangent(TangentTemp[0], TangentTemp[2], TangentTemp[1]));
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leTangent->GetIndexArray().GetAt(vertexId);
						//Display3DVector(header, leTangent->GetDirectArray().GetAt(id));
						FbxVector4 TangentTemp = leTangent->GetDirectArray().GetAt(id);
						m_GeometryTemp.tangents.Add(FProcMeshTangent(TangentTemp[0], TangentTemp[2], TangentTemp[1]));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			vertexId++;
		} 
	}
}

//读取材质名称
void UReadFbx::ImportMaterial(FbxGeometry* pGeometry)
{
	int lMaterialCount = 0;//材质个数
	FbxNode* lNode = NULL;
	if (pGeometry) 
	{
		lNode = pGeometry->GetNode();
		if (lNode)
			lMaterialCount = lNode->GetMaterialCount();  
	}
	if (lMaterialCount > 0)
	{
		for (int lCount = 0; lCount < lMaterialCount; lCount++)
		{
			FbxSurfaceMaterial* lMaterial = lNode->GetMaterial(lCount);
			m_GeometryTemp.material.materialname = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(lMaterial->GetName()).c_str());  //材质名称 //材质名字是ascii编码，因为如果使用utf8编码，先登录素材库再输出fbx文件，软件会崩溃
			if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
			{//因为三维软件输出fbx文件的时候，输出的是这种材质
				FbxPropertyT<FbxDouble3> lKFbxDouble3 = ((FbxSurfacePhong*)lMaterial)->Diffuse;
				m_GeometryTemp.material.diffuseColor = FVector(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);  //diffuse颜色
				m_GeometryTemp.material.useDiffuseTex = false;  //直接使用diffuse颜色
				int diffusePicCount = ((FbxSurfacePhong*)lMaterial)->Diffuse.GetSrcObjectCount<FbxFileTexture>();  //使用的diffuse贴图的个数
				if (diffusePicCount > 0)
				{
					FbxFileTexture* lTex = ((FbxSurfacePhong*)lMaterial)->Diffuse.GetSrcObject<FbxFileTexture>(0);
					m_GeometryTemp.material.diffuseTexName = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(lTex->GetFileName()).c_str());  //diffuse贴图的名字
					if (m_GeometryTemp.material.diffuseTexName != "")
					{
						m_GeometryTemp.material.useDiffuseTex = true;  //使用diffuse贴图
						//保存贴图
						if (GetWorld())
						{
							UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
							if (gameInstance)
							{
								gameInstance->AddPic(m_dxfFilePath+m_GeometryTemp.material.diffuseTexName);
							}
						}
						break;  //每个diffuse贴图应该只有一个
					}
				}
			}
		}
	}
}

//获得模型的位置、旋转、缩放
FTransform UReadFbx::ImportTransForm(FbxObject* pObject)
{
	FTransform trans;
	// Display all the properties
	int i, lCount = 0;
	FbxProperty lProperty = pObject->GetFirstProperty();
	while (lProperty.IsValid())
	{
		lCount++;
		lProperty = pObject->GetNextProperty(lProperty);
	}
	if (lCount == 0)
		return trans; // there are no properties to display
	i = 0;
	lProperty = pObject->GetFirstProperty();
	while (lProperty.IsValid())
	{
		FbxDouble3 lDefault;

		if (i == 66)  //获取物体位置数据
		{
			lDefault = lProperty.Get<FbxDouble3>();
			trans.SetTranslation(FVector(lDefault[0], lDefault[2], lDefault[1])*100);
		}
		if (i == 67)
		{
			lDefault = lProperty.Get<FbxDouble3>(); //获取物体旋转数据
			trans.SetRotation(FQuat(FRotator(lDefault[0], -lDefault[1], lDefault[2])));
		}
		if (i == 68)
		{
			lDefault = lProperty.Get<FbxDouble3>(); //获取物体缩放数据
			trans.SetScale3D(FVector(lDefault[0], lDefault[2], lDefault[1]));
		}
		i++;
		lProperty = pObject->GetNextProperty(lProperty);
	}
	return trans;
}

//创建模型
void UReadFbx::CreateModelBase(FbxNode* node)
{
	if (!GetWorld() || !node)
		return;
	if (m_GeometryList.Num() == 0)
		return;
	FString modelName = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(node->GetName()).c_str());
	FTransform trans = ImportTransForm(node);  //获得变换矩阵
	//对于墙顶地要转化为相对于中心的局部坐标
	if (modelName.Find(TEXT("墙体"))>-1 || modelName.Find(TEXT("融合面"))> -1 || modelName.Find(TEXT("吊顶"))> -1 || modelName.Find(TEXT("地面"))> -1)
	{
		FVector Center = FVector(0.0f,0.0f,0.0f);  //底面中心点
		float minZ = 100000.0f;
		int num = 0;
		for (int i = 0; i < (int)m_GeometryList.Num(); i++)
		{
			for (int j = 0; j < (int)m_GeometryList[i].vertices.Num(); j++)
			{
				Center += m_GeometryList[i].vertices[j];
				num += 1;
				minZ = m_GeometryList[i].vertices[j].Z < minZ ? m_GeometryList[i].vertices[j].Z : minZ;
			}
		}
		Center /= num;
		Center.Z = minZ;
		//转化为相对坐标
		for (int i = 0; i < (int)m_GeometryList.Num(); i++)
		{
			for (int j = 0; j < (int)m_GeometryList[i].vertices.Num(); j++)
			{
				m_GeometryList[i].vertices[j] -= Center;
			}
		}
		//设置位置
		trans.SetLocation(Center/**100*/);
	}
	FActorSpawnParameters aa;
	TSubclassOf<AActor>modulbaseInstance = LoadClass<AActor>(NULL, TEXT("Blueprint'/Game/Blueprint/BP_ModulBaseActor.BP_ModulBaseActor_C'"));
	AModulBaseActor* modelbaseTemp = Cast<AModulBaseActor>(GetWorld()->SpawnActor(modulbaseInstance,&trans, aa));  //创建模型
	if (!modelbaseTemp)
		return;
	modelbaseTemp->SetModelName(modelName);  //模型名字
#if WITH_EDITOR
	modelbaseTemp->SetActorLabel(*modelName);    //设置模型显示的名字,这个函数只有在编辑器模式下才能运行
#endif 
	//创建geometry
	for (int i = 0; i < (int)m_GeometryList.Num(); i++)
	{
		if (m_GeometryList[i].vertices.Num() == 0)
			continue;   //没有顶点则不创建
		bool bCollision = true;
		modelbaseTemp->CreateMesh(m_GeometryList[i].vertices, m_GeometryList[i].Triangles, m_GeometryList[i].normals, m_GeometryList[i].UV0, m_GeometryList[i].tangents, m_GeometryList[i].vertexColors, i, bCollision);
		modelbaseTemp->CreateProceduralMeshMaterial(&m_GeometryList[i].material, modelbaseTemp->MeshComponentList.Last(),i);
	}
}