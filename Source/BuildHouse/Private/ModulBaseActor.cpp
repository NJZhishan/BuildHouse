// Fill out your copyright notice in the Description page of Project Settings.


#include "ModulBaseActor.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "../Public/BuildHouseGameInstance.h"
#include "Windows/AllowWindowsPlatformTypes.h" //一般为Windows中的宏和UE4冲突所致,将可能会导致冲突的宏所在的hom文件包裹起来
#include "Windows/PreWindowsApi.h"
#include "Picture.h"
#include "flyfile_w.h"
#include "Windows/PostWindowsApi.h"            //一般为Windows中的宏和UE4冲突所致,将可能会导致冲突的宏所在的hom文件包裹起来
#include "Windows/HideWindowsPlatformTypes.h"
#include "../Public/WljMain.h"
#include "../Public/BuildHouseCharacter.h"
#include <vector>
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "../BuildHouseGameModeBase.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceConstant.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
AModulBaseActor::AModulBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));  //创建根节点
	RootComponent = MyRootComponent; 
	//模型包围盒
	BoundComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoundComponent"));       //包围盒
	static ConstructorHelpers::FObjectFinder<UStaticMesh> boundAsset(TEXT("StaticMesh'/Game/Model/SM_ModelBound.SM_ModelBound'"));
	if (boundAsset.Succeeded())
	{
		BoundComponent->SetStaticMesh(boundAsset.Object);
		BoundComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		BoundComponent->SetRelativeScale3D(FVector(0.0f));
	}
	BoundComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BoundComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);  //射线检测会检测不到这个模型
	BoundComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void AModulBaseActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AModulBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//创建geometry
void AModulBaseActor::CreateMesh(TArray<FVector>& vertices, TArray<int32>& Triangles, TArray<FVector>& normals, TArray<FVector2D>& UV0, TArray<FProcMeshTangent>& tangents, TArray<FLinearColor>& vertexColors,int id, bool bCollision/* = true*/)
{
	FString geoName = FString("Geo_") + FString::FromInt(id);
	UProceduralMeshComponent* geometryTemp = NewObject<UProceduralMeshComponent>(this, *geoName, RF_NoFlags, nullptr, false,nullptr);
	geometryTemp->RegisterComponent();  //动态创建compoent需要注册才能生效
	geometryTemp->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, bCollision);
	geometryTemp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponentList.Add(geometryTemp);
	auto a = geometryTemp->GetCollisionEnabled();
	CalculateActorBound();  //更新模型包围盒
	/*FTransform trans;
	UProceduralMeshComponent* geometryTemp = Cast<UProceduralMeshComponent>(AddComponentByClass(UProceduralMeshComponent::StaticClass(),true,trans,false));
	geometryTemp->AttachToComponent(ParentComponent, FAttachmentTransformRules::KeepRelativeTransform);
	geometryTemp->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	MeshComponentList.Add(geometryTemp);*/
}


//设置描边
void AModulBaseActor::SetOutline(bool outLine)
{
	ABuildHouseCharacter* charactor = Cast<ABuildHouseCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (charactor->GetTravelType() == TravelType::TT_2D_Ortho)
	{//二维中显示包围盒
		OBJECT_STYLE style= ReturnModelType();
		if(style == OBJECT_STYLE::NORMAL || style == OBJECT_STYLE::LIGHT)  //只显示门窗的包围盒
			BoundComponent->SetVisibility(outLine);
	}
	else
	{//三维中使用描边
		//组件描边
		for (int i = 0; i < MeshComponentList.Num(); i++)
		{
			MeshComponentList[i]->SetRenderCustomDepth(outLine);
		}
		//三维中不显示包围盒
		BoundComponent->SetVisibility(false);
	}
	//如果没有描边就肯定没有选中的组件
	if (outLine == false)
	{
		SetSelComponent(nullptr);
	}
}

//设置选中的组件
void AModulBaseActor::SetSelComponent(UProceduralMeshComponent* selComponent)
{
	//设置选中
	m_selProceduralMeshComponent = selComponent;
	return;
}

UProceduralMeshComponent* AModulBaseActor::ReturnSelComponent()
{
	return m_selProceduralMeshComponent;
}

//鼠标滑过actor
void AModulBaseActor::BeginCursorOver()
{
	if (GetWorld())
	{
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance)
		{
			AModulBaseActor* lastSel = gameInstance->ReturnSelectModulBaseActor();
			if (lastSel)
			{
				lastSel->SetOutline(false);//取消上次选中物体的描边
			}
			SetOutline(true);//将当前选中的物体描边
			gameInstance->SetSelectModulBaseActor(this); //保存选中的模型
		}
	}
	return;
}

//将纹理文件的后缀统一到bmp格式下
std::string AModulBaseActor::ChangeZipToBmp(std::string filename)
{
	//如果是压缩文件的话，要从中获得纹理图片的实际长宽
	//jiang gai 2017.6.29
	/*CString ExtName=filename.substr(filename.size()-3,3).c_str();
		ExtName.MakeUpper();*/
		//jiang gai 2015.8.12 这里还是改为获得压缩包中图像名称
	flyFile_w flyfile;
	if (flyfile.SetZipList(filename.c_str())
		&& !flyfile.RowFileName.empty())
	{
		//用压缩包中真正的纹理文件名更新map_name
		std::string newfile = flyfile.RowFileName.c_str();
		return newfile;
	}
	else
	{
		//不是压缩文件，应该是图片文件
		return filename;
	}
}

//导入材质资源
UMaterialInstance* AModulBaseActor::LoadMaterialResource(FString materialName)
{
	UMaterialInstance* matPtr = NULL;
	FString loadPath = TEXT("MaterialInstanceConstant'/Game/材质库/M_") + materialName + ".M_" + materialName + "'";
	matPtr = LoadObject<UMaterialInstanceConstant>(NULL, *loadPath);
	if (!matPtr)
	{
		FString aa(TEXT("MaterialInstanceConstant'/Game/材质库/M_乳胶漆.M_乳胶漆'")); //初始化时一定要加TEXT，否则FString乱码会导致载入含有中文的材质时失败
		matPtr = LoadObject<UMaterialInstanceConstant>(NULL, *aa);
	}
	return matPtr;
}

//设置材质
void AModulBaseActor::CreateProceduralMeshMaterial(const FMaterialStruct* mat, UProceduralMeshComponent* mesh, int textureIndex)
{
	if (!mat || !mesh || MeshComponentList.Num() == 0 || textureIndex < 0)
		return;
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (!gameInstance)
		return;
	int meshIndex = MeshComponentList.Find(mesh);  //要修改的proceduralMesh的序号
	if (meshIndex < 0)
		return;
	FString materialName = mat->materialname;  //材质名称
	if (materialName.IsEmpty())
	{//如果材质名称是空的，默认给乳胶漆
		materialName = TEXT("乳胶漆");
	}
	UMaterialInstanceDynamic* materialInstance = nullptr;
	//查找是否已经创建了该材质
	std::map<int, TArray<int>>::iterator findIter = m_TextureAndProceduralMeshMatches.find(textureIndex);
	if (findIter != m_TextureAndProceduralMeshMatches.end())
	{//已经创建了该材质
		findIter->second.AddUnique(meshIndex);    //保存textureIndex和ProceduralMesh的对应关系
		int exitMeshIndex = findIter->second[0];  //已经使用了该材质的proceduralMesh的序号
		if (MeshComponentList.IsValidIndex(exitMeshIndex))
		{//是有效的proceduralMesh
			materialInstance = Cast<UMaterialInstanceDynamic>(MeshComponentList[exitMeshIndex]->GetMaterial(0));
		}
		mesh->SetMaterial(0, materialInstance);  //设置材质
	}
	else
	{//没有创建该材质
		TArray<int> tempList;
		tempList.Add(meshIndex);
		m_TextureAndProceduralMeshMatches.insert(std::pair<int, TArray<int>>(textureIndex, tempList));  //保存textureIndex和ProceduralMesh的对应关系
		m_TextureAndDynamicMaterialMatches.insert(std::pair<int, FMaterialStruct>(textureIndex, *mat)); //保存textureIndex和FMaterialStruct的对应关系
		//创建材质
		UMaterialInstance* matPtr = LoadMaterialResource(materialName);
		if (matPtr)
		{
			mesh->SetMaterial(0, matPtr);
		}
		materialInstance = mesh->CreateAndSetMaterialInstanceDynamic(0);
		ChangeProceduralMeshMaterial(mat, mesh);
	}
}

//获得meshIndex,tetureIndex,proceduralMesh
bool AModulBaseActor::ModifyMeshAndTextureIndex(UProceduralMeshComponent* &mesh, int& textureIndex, int& meshIndex)
{
	if (!mesh && textureIndex > -1)
	{//只指定了textureIndex,没有指定mesh，则根据textureIndex找到mesh
		std::map<int, TArray<int>>::iterator findIter = m_TextureAndProceduralMeshMatches.find(textureIndex);
		if (findIter != m_TextureAndProceduralMeshMatches.end())
		{
			int exitMeshIndex = findIter->second[0];  //已经使用了该材质的proceduralMesh的序号
			if (MeshComponentList.IsValidIndex(exitMeshIndex))
			{//是有效的proceduralMesh
				mesh = MeshComponentList[exitMeshIndex];
			}
		}
	}
	if (!mesh)
		return false;
	if (MeshComponentList.Find(mesh, meshIndex) == false)
	{//根据mesh确定meshIndex
		return false;
	}
	if (textureIndex < 0)
	{
		for (auto& findIter : m_TextureAndProceduralMeshMatches)
		{
			TArray<int>indexList = findIter.second;
			if (indexList.Find(meshIndex) > -1)
			{
				textureIndex = findIter.first;
				break;
			}
		}
	}
	if (mesh && textureIndex > -1 && meshIndex > -1)
		return true;
	return false;
}

//修改diffuse颜色
void AModulBaseActor::ModifyProceduralMeshColor(UProceduralMeshComponent* mesh, int textureIndex, FVector diffuseColor)
{
	int meshIndex = -1;
	bool bValid = ModifyMeshAndTextureIndex(mesh, textureIndex, meshIndex);  //获得有效的mesh、材质、mesh的索引
	if (!bValid)
		return;
	std::map<int, FMaterialStruct>::iterator materialIter = m_TextureAndDynamicMaterialMatches.find(textureIndex);
	materialIter->second.useDiffuseTex = false;        //不使用贴图
	materialIter->second.diffuseColor = diffuseColor;  //指定颜色
	materialIter->second.diffuseZipName = TEXT("");    //贴图压缩文件，比如7885.txt
	UMaterialInstanceDynamic* materialInstance = Cast<UMaterialInstanceDynamic>(mesh->GetMaterial(0));
	if (materialInstance)
	{//修改材质球中的参数
		materialInstance->SetScalarParameterValue(FName("UseDiffusePic"), materialIter->second.useDiffuseTex);
		materialInstance->SetVectorParameterValue(FName("DiffuseColor"), materialIter->second.diffuseColor);
	}
}

//修改uv
void AModulBaseActor::ModifyProceduralMeshUV(UProceduralMeshComponent* mesh, int textureIndex, FVector2D moveUV, FVector2D scaleUV, float rotateUV)
{
	int meshIndex = -1;
	bool bValid = ModifyMeshAndTextureIndex(mesh, textureIndex, meshIndex);  //获得有效的mesh、材质、mesh的索引
	if (!bValid)
		return;
	std::map<int, FMaterialStruct>::iterator materialIter = m_TextureAndDynamicMaterialMatches.find(textureIndex);
	materialIter->second.moveUV += moveUV;       
	materialIter->second.scaleUV += scaleUV;
	materialIter->second.rotateUV += rotateUV;
	UMaterialInstanceDynamic* materialInstance = Cast<UMaterialInstanceDynamic>(mesh->GetMaterial(0));
	if (materialInstance)
	{//修改材质球中的参数
		materialInstance->SetScalarParameterValue(FName("Offset_U"), moveUV.X);  //UV移动
		materialInstance->SetScalarParameterValue(FName("Offset_V"), moveUV.Y);
		materialInstance->SetScalarParameterValue(FName("Number_U"), scaleUV.Y); //UV缩放
		materialInstance->SetScalarParameterValue(FName("Number_V"), scaleUV.Y);
		materialInstance->SetScalarParameterValue(FName("Rotator"), rotateUV);  //UV旋转
	}
	return;
}

//修改材质球
void AModulBaseActor::ModifyProceduralMeshMaterial(UProceduralMeshComponent* mesh, int textureIndex, FString materialname, FVector specularColor/*=FVector::OneVector*/, bool changSpecularColor/*=false*/)
{
	int meshIndex = -1;
	bool bValid = ModifyMeshAndTextureIndex(mesh, textureIndex, meshIndex);  //获得有效的mesh、材质、mesh的索引
	if (!bValid)
		return;
	std::map<int, FMaterialStruct>::iterator materialIter = m_TextureAndDynamicMaterialMatches.find(textureIndex);
	materialIter->second.materialname = materialname;         //材质球名字
	if (changSpecularColor)
		materialIter->second.specularColor = specularColor;   //指定高光颜色
	ChangeProceduralMeshMaterial(&materialIter->second, mesh);//创建并修改材质
	return;
}


//修改材质
void AModulBaseActor::ChangeProceduralMeshMaterial(const FMaterialStruct* mat, UProceduralMeshComponent* mesh)
{
	if (!mat || !mesh)
		return;
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (!gameInstance)
		return;
	int meshIndex = MeshComponentList.Find(mesh);  //要修改的proceduralMesh的序号
	if (meshIndex < 0)
		return;
	//创建新材质
	UMaterialInstance* matPtr = LoadMaterialResource(mat->materialname);
	if (matPtr)
	{
		mesh->SetMaterial(0, matPtr);
	}
	//创建动态材质
	UMaterialInstanceDynamic* materialInstance = mesh->CreateAndSetMaterialInstanceDynamic(0);
	//修改所有使用了该材质的proceduralMesh
	for (auto& findIter : m_TextureAndProceduralMeshMatches)
	{
		TArray<int>indexList = findIter.second;
		if (indexList.Find(meshIndex) > -1)
		{
			for (auto& procedualIndex : indexList)
			{
				MeshComponentList[procedualIndex]->SetMaterial(0, materialInstance);
			}
			break;
		}
	}
	//修改参数
	if (materialInstance)
	{
		//关于diffuse
		materialInstance->SetScalarParameterValue(FName("UseDiffusePic"), mat->useDiffuseTex);
		if (!mat->useDiffuseTex)
		{//颜色
			materialInstance->SetVectorParameterValue(FName("DiffuseColor"), mat->diffuseColor);
		}
		else
		{//贴图
			UTexture* texPtr = gameInstance->FindPic(mat->diffuseTexName);
			if (texPtr)
			{
				materialInstance->SetTextureParameterValue(FName("DiffusePic"), texPtr);
			}
		}
		//关于normal
		materialInstance->SetScalarParameterValue(FName("NormalPower"), 0);  //凹凸权重
		if (mat->bumpTexName.IsEmpty() == false)
		{
			UTexture* texPtr = gameInstance->FindPic(mat->bumpTexName);
			if (texPtr)
			{
				materialInstance->SetTextureParameterValue(FName("NormalPic"), texPtr);  //凹凸贴图
			}
			materialInstance->SetScalarParameterValue(FName("NormalPower"), mat->bumpWeight / 10.0f);  //凹凸权重
		}
		//关于绒布
		if (mat->materialname.Find(TEXT("绒布")) > -1)
		{
			materialInstance->SetVectorParameterValue(FName("DiffuseColor2"), mat->specularColor);
		}
		//关于金属
		if (mat->materialname.Find(TEXT("金属")) > -1 || mat->materialname.Find(TEXT("不锈钢")) > -1 || mat->materialname.Find(TEXT("铝合金")) > -1 || mat->materialname.Find(TEXT("铜")) > -1)
		{
			materialInstance->SetVectorParameterValue(FName("ReflectColor"), mat->specularColor);
		}
		//关于UV
		materialInstance->SetScalarParameterValue(FName("Offset_U"), mat->moveUV.X);  //UV移动
		materialInstance->SetScalarParameterValue(FName("Offset_V"), mat->moveUV.Y);
		materialInstance->SetScalarParameterValue(FName("Number_U"), mat->scaleUV.Y); //UV缩放
		materialInstance->SetScalarParameterValue(FName("Number_V"), mat->scaleUV.Y);
		materialInstance->SetScalarParameterValue(FName("Rotator"),  mat->rotateUV);  //UV旋转
	}
}

//替换贴图
//参数fileName是贴图所在的txt文件的绝对路径，如 d:/model_zy/5667.txt
//参数matName是材质名字
//参数specularColor是高光颜色
void AModulBaseActor::ReplaceTexture(UProceduralMeshComponent* mesh, int textureIndex,FString& fileName, FString matName/* = TEXT("")*/, FVector specularColor/* = FVector::OneVector*/)
{
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (!gameInstance)
		return;
	int meshIndex = -1;
	bool bValid = ModifyMeshAndTextureIndex(mesh, textureIndex, meshIndex);  //获得有效的mesh、材质、mesh的索引
	if (!bValid)
		return;
	if (matName.IsEmpty() == false)
	{//替换材质球
		ModifyProceduralMeshMaterial(mesh, textureIndex, matName, specularColor, true);
	}
	std::map<int, FMaterialStruct>::iterator materialIter = m_TextureAndDynamicMaterialMatches.find(textureIndex);
	materialIter->second.useDiffuseTex = true;        //使用贴图
	FString relativeFile = fileName;
	int findIndex = -1;
	if (relativeFile.FindLastChar('/', findIndex))
	{
		relativeFile = relativeFile.Right(relativeFile.Len() - 1 - findIndex);
	}
	materialIter->second.diffuseZipName = relativeFile; //贴图压缩文件，比如7885.txt
	UMaterialInstanceDynamic* materialInstance = Cast<UMaterialInstanceDynamic>(mesh->GetMaterial(0));
	if (materialInstance)
	{//修改材质球中的参数
		materialInstance->SetScalarParameterValue(FName("UseDiffusePic"), materialIter->second.useDiffuseTex);//只要传入一个大于1的值就是使用贴图
	}
	bool bExit = gameInstance->IsTheModelInLocal(fileName);
	if (bExit)
	{//贴图在本地存在
		//创建图象
		CPicture* pPicture = NULL;
		std::string filenameBmp = TCHAR_TO_UTF8(*fileName);
		//针对压缩文件
		flyFile_w flyfile;
		filenameBmp = ChangeZipToBmp(filenameBmp);  //贴图的名字，比如 奥米茄tex.bmp
		if (flyfile.SetZipList(TCHAR_TO_ANSI(*fileName)))
		{
			pPicture = CreateImageFromZipFileWithAlpha(filenameBmp.c_str(), flyfile, 1);
			if (gameInstance)
			{
				bool bSuc = gameInstance->AddZipPic(UTF8_TO_TCHAR(filenameBmp.c_str()), (void*)pPicture);
				if (bSuc)
				{
					UTexture* texPtr = gameInstance->FindPic(UTF8_TO_TCHAR(filenameBmp.c_str()));
					if (texPtr)
					{
						materialInstance->SetTextureParameterValue(FName("DiffusePic"), texPtr);
					}
				}
			}
		}
	}
	else
	{//贴图在本地不存在,贴图下载完成后，会在m_TextureAndDynamicMaterialMatches中查找并进行下载替换
	}
	return;
}


//替换的贴图下载完成
//参数是txt绝对路径，比如d:/model_zy/5667.txt
void AModulBaseActor::DownloadTextureFinished(FString fileName)
{
	FString tempName = fileName; //贴图txt名字
	int index = -1;
	if (tempName.FindLastChar('/', index))
	{
		tempName = tempName.Right(tempName.Len() - 1 - index);
	}
	for (auto& single : m_TextureAndDynamicMaterialMatches)
	{
		if (single.second.diffuseZipName.Equals(tempName))
		{
			ReplaceTexture(nullptr, single.first, fileName);
		}
	}
	return;
}


std::string AModulBaseActor::UTF8_To_string(std::string str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}


std::string AModulBaseActor::string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);
	setlocale(LC_ALL, "Chinese-simplified");
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

//创建模型，参数是模型所在txt文件的绝对路径，比如 d:/model_zy/7885.txt
void AModulBaseActor::CreateNewObject(const FString& fileName)
{
	if (fileName.IsEmpty() || fileName.Find(TEXT(".txt")) == -1)
		return;
	flyFile_w flyfile;
	if (flyfile.SetZipList(TCHAR_TO_ANSI(*fileName)))
	{
		std::string fltFileName = flyfile.RowFileName;  //模型文件的名字，比如 zhuozi.thd
		//确认Name有效
		if ((int)fltFileName.size() < 3)
			return ;
		std::string Name = fltFileName.substr(fltFileName.length() - 3, 3);
		if (Name != ("flt") && Name != ("jia") && Name != ("sdk") && Name != ("zyj")&& Name != ("mod") && Name != ("thd"))
			return ;
		if (flyfile.open(fltFileName.c_str()))
		{
			CFileHeader FileHeader;                                 //文件与模型总信息
			std::vector<CGeomentry_Information> Gementries_Info;    //模型实体列表
			CTexture Texture;                                       //材质信息 
			//从zip对象中中提取数据
			//--------------------------以下为读文件操作------------------------
			//读取文件头
			flyfile.read((char*)&FileHeader.ID, sizeof(CFileHeader));
			//读取物体信息列表
			unsigned short i = 0;
			for (i = 0; i < FileHeader.FileInfo.group_number; i++)
			{
				CGeomentry_Information geom;          //一定要放这里

				flyfile.read((char*)&geom.ID, sizeof(unsigned short));                       //ID号
				flyfile.read((char*)&geom.Length, sizeof(unsigned long));                    //块长度
				flyfile.read((char*)&geom.Geomentry_name[0], 256);                           //名称
				flyfile.read((char*)&geom.Vertices.ID, sizeof(unsigned short));              //顶点块ID号                
				flyfile.read((char*)&geom.Vertices.Length, sizeof(unsigned long));           //顶点块长度           
				flyfile.read((char*)&geom.Vertices.numberOfVertices, sizeof(unsigned long)); //顶点数目

				unsigned long j = 0;
				for (j = 0; j < geom.Vertices.numberOfVertices; j++)
				{
					CVector3 temp1;
					CVMapCoords coords;

					flyfile.read((char*)&temp1, sizeof(CVector3));//顶点坐标
					geom.Vertices.VPoint.push_back(temp1);
					flyfile.read((char*)&temp1, sizeof(CVector3));//顶点法向量
					geom.Vertices.VNormals.push_back(temp1);
					geom.Vertices.VMaps.push_back(coords);
					//顶点的贴图坐标数
					flyfile.read((char*)&geom.Vertices.VMaps[j].numberOfMapCoords, sizeof(unsigned short));
					for (unsigned short k = 0; k < geom.Vertices.VMaps[j].numberOfMapCoords; k++)
					{
						//顶点贴图坐标信息
						CVertexMapCoords uvw;
						flyfile.read((char*)&uvw.MapIndex, sizeof(unsigned short));
						flyfile.read((char*)&uvw.MapCoords, sizeof(CVector2));
						geom.Vertices.VMaps[j].UVWCoords.push_back(uvw);
					}
					flyfile.read((char*)&temp1, sizeof(CVector3));//顶点颜色
					geom.Vertices.VColors.push_back(temp1);
				}
				flyfile.read((char*)&geom.Faces.ID, sizeof(unsigned short));          //面块ID号
				flyfile.read((char*)&geom.Faces.Length, sizeof(unsigned long));       //面块长度
				flyfile.read((char*)&geom.Faces.numberOfFaces, sizeof(unsigned long));//面数目
				for (j = 0; j < geom.Faces.numberOfFaces * 3; j++)                              //顶点序号
				{
					unsigned long index;
					flyfile.read((char*)&index, sizeof(unsigned long));
					geom.Faces.Vertices_Index.push_back(index);
				}
				flyfile.read((char*)&geom.Materials.ID, sizeof(unsigned short));               //材质块ID号
				flyfile.read((char*)&geom.Materials.Length, sizeof(unsigned long));            //材质块长度
				flyfile.read((char*)&geom.Materials.numberOfMaterials, sizeof(unsigned short));//材质数目
				for (j = 0; j < geom.Materials.numberOfMaterials; j++)             //材质信息
				{
					CMatAndFace matface;

					flyfile.read((char*)&matface.Materials_Index, sizeof(unsigned short));
					flyfile.read((char*)&matface.numberOfFaces, sizeof(unsigned long));
					for (unsigned long k = 0; k < matface.numberOfFaces; k++)
					{
						unsigned long index;
						flyfile.read((char*)&index, sizeof(unsigned long));
						matface.Face_Index.push_back(index);
					}
					geom.Materials.MaterialProp.push_back(matface);
				}

				flyfile.read((char*)&geom.Center_Point, sizeof(CVector3));
				flyfile.read((char*)&geom.min, sizeof(CVector3));
				flyfile.read((char*)&geom.max, sizeof(CVector3));
				flyfile.read((char*)&geom.radius, sizeof(float));
				Gementries_Info.push_back(geom);     //加入信息列表
			}

			//读取材质信息列表
			flyfile.read((char*)&Texture.ID, sizeof(unsigned short));        //ID号
			flyfile.read((char*)&Texture.Length, sizeof(unsigned long));     //块长度
			flyfile.read((char*)&Texture.map_number, sizeof(unsigned short));//材质数目
			for (i = 0; i < Texture.map_number; i++)               //材质信息列表
			{
				//jiang gai 2015.12.14 增加类型
				if (Name == ("flt") || Name == ("sdk") || Name == ("mod"))
				{
					//jiang gai 2014.9.1 老格式文件，先从文件中读取旧的CMapsOld信息，然后创建新的CMaps，然后压入到Texture中
					CMapsOld maps;
					CMaps Newmap;
					flyfile.read((char*)&maps, sizeof(CMapsOld));
					Newmap.CopyFromOld(maps);

					if (Newmap.texture_type == MAPS)
					{
						Newmap.texture_property.material.Diffuse = ONE_VECTOR3; //wqq 2018.8.22 如果表面是贴图，设置ambient和diffuse颜色为白色，避免影响最终的显示
						Newmap.texture_property.material.Ambient = ONE_VECTOR3;
					}

					Texture.VMaps.push_back(Newmap);
					Texture.m_regSizeList.push_back(ONE_VECTOR2);
					Texture.m_texClassList.push_back("");
					Texture.m_texPriceList.push_back("");
				}
				else if (Name == ("jia"))
				{
					//jiang gai 2014.9.1 新格式文件，直接读新版CMaps
					CMaps Newmap;
					Newmap.ReadFromFile(&flyfile);
					//flyfile.read((char *)&Newmap,sizeof(CMaps));
					if (Newmap.texture_type == MAPS)
					{
						Newmap.texture_property.material.Diffuse = ONE_VECTOR3; //wqq 2018.8.22 如果表面是贴图，设置ambient和diffuse颜色为白色，避免影响最终的显示
						Newmap.texture_property.material.Ambient = ONE_VECTOR3;
					}
					Texture.VMaps.push_back(Newmap);
					Texture.m_regSizeList.push_back(ONE_VECTOR2);
					Texture.m_texClassList.push_back("");
					Texture.m_texPriceList.push_back("");
				}
				else if (Name == ("zyj"))
				{
					//jiang gai 2015.9.24 虽然和上面一样，但是实际上是2,0版
					CMaps Newmap;
					Newmap.ReadFromFile(&flyfile);

					if (Newmap.texture_type == MAPS)
					{
						Newmap.texture_property.material.Diffuse = ONE_VECTOR3; //wqq 2018.8.22 如果表面是贴图，设置ambient和diffuse颜色为白色，避免影响最终的显示
						Newmap.texture_property.material.Ambient = ONE_VECTOR3;
					}
					Texture.VMaps.push_back(Newmap);
					Texture.m_regSizeList.push_back(ONE_VECTOR2);
					Texture.m_texClassList.push_back("");
					Texture.m_texPriceList.push_back("");
				}
				else if (Name == ("thd"))
				{
					//wqq 2016.8.22 
					CMaps Newmap;
					Newmap.ReadFromFile(&flyfile);
					if (Newmap.texture_type == MAPS)
					{
						Newmap.texture_property.material.Diffuse = ONE_VECTOR3; //wqq 2018.8.22 如果表面是贴图，设置ambient和diffuse颜色为白色，避免影响最终的显示
						Newmap.texture_property.material.Ambient = ONE_VECTOR3;
					}
					Texture.VMaps.push_back(Newmap);
					Texture.m_regSizeList.push_back(ONE_VECTOR2);
					Texture.m_texClassList.push_back("");
					Texture.m_texPriceList.push_back("");
				}
			}
			if (Gementries_Info.size() == 0)
				return;
			//创建贴图材质
			for (int m = 0; m < (int)Texture.VMaps.size(); m++)
			{
				if (Texture.VMaps[m].texture_type == MAPS)
				{
					std::string picFileName = Texture.VMaps[m].texture_property.map.FileName;  //使用贴图的名字，比如奥米茄tex.bmp
					CPicture* pPicture = CreateImageFromZipFileWithAlpha(picFileName.c_str(), flyfile, 1);
					UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
					if (gameInstance)
					{
						bool bSuc = gameInstance->AddZipPic(UTF8_TO_TCHAR(picFileName.c_str()), (void*)pPicture);
					}
				}
			}
			//创建凹凸贴图的材质
			for (int m = 0; m < (int)Texture.VMaps.size(); m++)
			{
				if (strcmp(Texture.VMaps[m].bump_property.map.FileName,"")!=0)
				{
					std::string picFileName = Texture.VMaps[m].bump_property.map.FileName;  //使用的凹凸贴图的名字，比如奥米茄tex.bmp
					CPicture* pPicture = CreateImageFromZipFileWithAlpha(picFileName.c_str(), flyfile, 1);
					UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
					if (gameInstance)
					{
						bool bSuc = gameInstance->AddZipPic(UTF8_TO_TCHAR(picFileName.c_str()), (void*)pPicture);
					}
				}
			}
			for (int m = 0; m < Gementries_Info.size(); m++)
			{
				CGeomentry_Information& geoTemp = Gementries_Info[m];
				TArray<FVector> vertices;
				TArray<int32> Triangles;
				TArray<FVector> normals;
				TArray<FVector2D> UV0;
				TArray<FProcMeshTangent> tangents;
				TArray<FLinearColor> vertexColors;
				for (unsigned long j = 0; j<geoTemp.Faces.Vertices_Index.size(); j++)
				{
					Triangles.Add(j);
					int32 pointIndex = geoTemp.Faces.Vertices_Index[j];
					vertices.Add(FVector(-geoTemp.Vertices.VPoint[pointIndex].z * 100, geoTemp.Vertices.VPoint[pointIndex].x * 100, geoTemp.Vertices.VPoint[pointIndex].y * 100));
					normals.Add(FVector(-geoTemp.Vertices.VNormals[pointIndex].z, geoTemp.Vertices.VNormals[pointIndex].x, geoTemp.Vertices.VNormals[pointIndex].y));
					if (pointIndex < geoTemp.Vertices.VMaps.size() && geoTemp.Vertices.VMaps[pointIndex].numberOfMapCoords == 1)
					{
						UV0.Add(FVector2D(geoTemp.Vertices.VMaps[pointIndex].UVWCoords[0].MapCoords.u, geoTemp.Vertices.VMaps[pointIndex].UVWCoords[0].MapCoords.v));
					}
					else
					{
						UV0.Add(FVector2D(0, 0));
					}
				}
				//创建模型
				CreateMesh(vertices, Triangles, normals, UV0, tangents, vertexColors, m);  
				//创建材质
				if (geoTemp.Materials.MaterialProp.size()>0 && geoTemp.Materials.MaterialProp[0].Materials_Index>-1 &&geoTemp.Materials.MaterialProp[0].Materials_Index < Texture.VMaps.size())
				{
					int materialDropIndex = 0;
					for (int n = 0; n < (int)geoTemp.Materials.MaterialProp.size(); n++)
					{//可能有多个MaterialProp，哪个对应的面多就用哪个材质
						materialDropIndex = geoTemp.Materials.MaterialProp[n].Face_Index.size()>= geoTemp.Materials.MaterialProp[0].Face_Index.size()?n:materialDropIndex;
					}
					CMaps& tempMap = Texture.VMaps[geoTemp.Materials.MaterialProp[materialDropIndex].Materials_Index];
					FMaterialStruct materialTemp;
					materialTemp.materialname = UTF8_TO_TCHAR(string_To_UTF8(std::string(tempMap.m_sEssMatirealID)).c_str());
					switch (tempMap.texture_type)
					{
						case MATERIALS:
						{
							materialTemp.useDiffuseTex = false;
							materialTemp.diffuseColor = FVector(tempMap.texture_property.material.Diffuse.x, tempMap.texture_property.material.Diffuse.y, tempMap.texture_property.material.Diffuse.z);
						}
						break;
					case MAPS:
						{
							materialTemp.useDiffuseTex = true;
							materialTemp.diffuseTexName = UTF8_TO_TCHAR(tempMap.texture_property.map.FileName);
						}
						break;
					}
					//凹凸
					materialTemp.bumpWeight = tempMap.bump_property.material.Bumpweight;
					materialTemp.bumpTexName = UTF8_TO_TCHAR(tempMap.bump_property.map.FileName);
					//specular颜色
					materialTemp.specularColor = FVector(tempMap.texture_property.material.SpecularColor.x, tempMap.texture_property.material.SpecularColor.y, tempMap.texture_property.material.SpecularColor.z);
					//设置材质
					CreateProceduralMeshMaterial(&materialTemp, MeshComponentList.Last(), geoTemp.Materials.MaterialProp[materialDropIndex].Materials_Index);
				}
			}
			ChangeMaterialBasedRoomMaterialChange(); //修改材质
			return ;
		}
	}
}

//设置模型的名字
void AModulBaseActor::SetModelName(FString name, int style/* = -1*/)
{
	if (m_prodInfor.url.IsEmpty() == false)
	{
		int styleIndex=style, objectIndex=-1;  //物体类型，物体序号
		if (m_prodInfor.info[14] == TEXT("装饰品")
			|| m_prodInfor.info[14] == TEXT("家具")
			|| m_prodInfor.info[14] == TEXT("开关面板")
			|| m_prodInfor.info[14] == TEXT("电器")
			|| m_prodInfor.info[14] == TEXT("五金构件")
			|| m_prodInfor.info[14] == TEXT("装饰构件")
			|| m_prodInfor.info[14] == TEXT("布艺")
			|| m_prodInfor.info[14] == TEXT("卫生间")
			|| m_prodInfor.info[14] == TEXT("厨房"))
		{
			styleIndex = (int)OBJECT_STYLE::NORMAL;
		}
		else if (m_prodInfor.info[14] == TEXT("灯具"))
		{
			styleIndex = (int)OBJECT_STYLE::LIGHT;
		}
		if (styleIndex > -1 && GetWorld())
		{
			UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
			if (gameInstance)
			{
				if (styleIndex < gameInstance->m_pScene->StyleList.size())
				{
					objectIndex = gameInstance->m_pScene->StyleList[styleIndex].pObjectList.size();
				}
			}
		}
		if (styleIndex > -1 && objectIndex > -1)
		{
			name = FString::FromInt(styleIndex) + TEXT("_") + FString::FromInt(objectIndex) + TEXT("_") + name;
		}
	}
	//判断是否有同名模型
	TArray<AActor*> NormalActors;  //场景中的actor
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("普通模型")), NormalActors);
	int sameNum = 0;  //拥有相同名字的模型个数
	for (auto& actor : NormalActors)
	{
		AModulBaseActor* modulbaseActor = Cast<AModulBaseActor>(actor);
		if (modulbaseActor && modulbaseActor->ReturnModelName().Find(name)>-1)
		{
			sameNum++;
		}
	}
	if (sameNum > 0)
	{//存在同名模型
		name = name + TEXT("_") + FString::FromInt(sameNum);
	}
	if (name.Equals(TEXT("0_5_zsp003")))
	{
		int a = 0;
	}
	m_ModelName = name;
	if (name.Find(TEXT("墙体_")) > -1 || name.Find(TEXT("融合面_")) > -1)
	{//是墙体，添加tag
		Tags.Add(FName(TEXT("墙体")));
	}
	else if (name.Find(TEXT("吊顶_")) > -1)
	{//是吊顶，添加tag
		Tags.Add(FName(TEXT("吊顶")));
	}
	else if (name.Find(TEXT("地面_")) > -1)
	{//是地面，添加tag
		Tags.Add(FName(TEXT("地面")));	
	}
	else
	{
		Tags.Add(FName(TEXT("普通模型")));
	}
	Tags.Add(FName(*m_ModelName));
	return;
}

//返回物体类型
OBJECT_STYLE AModulBaseActor::ReturnModelType()
{
	if (ActorHasTag(TEXT("普通模型")))
	{
		FString modelName = ReturnModelName();
		int index = -1;
		if (modelName.FindChar('_', index))
		{
			FString modelType = modelName.Left(index);
			int style = FCString::Atoi(*modelType);
			if (style == 0)
			{
				return OBJECT_STYLE::NORMAL;
			}
			else if (style == 1)
			{
				return OBJECT_STYLE::DOOR;
			}
			else if (style == 2)
			{
				return OBJECT_STYLE::WINDOW;
			}
			else if (style == 3)
			{
				return OBJECT_STYLE::LIGHT;
			}
		}
	}
	else if (ActorHasTag(TEXT("地面")))
	{
		return OBJECT_STYLE::FLOOR;
	}
	else if (ActorHasTag(TEXT("吊顶")))
	{
		return OBJECT_STYLE::CEILING;
	}
	else if (ActorHasTag(TEXT("墙体")))
	{
		return OBJECT_STYLE::WALL;
	}
	return OBJECT_STYLE::UNDIFINE;
}

//返回模型名字
FString AModulBaseActor::ReturnModelName()
{
	return m_ModelName;
}

//计算模型包围盒
FBox AModulBaseActor::CalculateActorBound()
{
	FBox box = CalculateComponentsBoundingBoxInLocalSpace(true, false);
	FVector min = box.Min;
	FVector max = box.Max;
	//更新包围盒，包围盒是建模得到的，建模的尺寸是100*100*100cm
	if (BoundComponent)
	{
		BoundComponent->SetRelativeLocation(FVector((max.X + min.X) * 0.5f, (max.Y + min.Y) * 0.5f, 0.0f));
		BoundComponent->SetRelativeScale3D(FVector((max.X - min.X) /1.0f*0.01, (max.Y - min.Y) / 1.0f * 0.01, (max.Z - min.Z) / 1.0f * 0.01));  
	}
	return box;
}

//更新模型包围盒
void AModulBaseActor::RefreshActorBoundBox(FVector min, FVector max)
{
	TArray<FVector>vertexList;  //包围盒一共是8个顶点
	TArray<FVector>allVertexList; //组成三角面的所有顶点
	TArray<int>triangeList;      //组成三角形的顶点索引
	//压入8个顶点
	vertexList.Add(min);
	vertexList.Add(FVector(min.X, max.Y, min.Z));
	vertexList.Add(FVector(max.X, max.Y, min.Z));
	vertexList.Add(FVector(max.X, min.Y, min.Z));
	vertexList.Add(FVector(min.X, min.Y, max.Z));
	vertexList.Add(FVector(min.X, max.Y, max.Z));
	vertexList.Add(max);
	vertexList.Add(FVector(max.X, min.Y, max.Z));
	//压入组成三角面的所有的顶点
	allVertexList.Add(vertexList[0]);
	allVertexList.Add(vertexList[1]);
	allVertexList.Add(vertexList[2]);
	allVertexList.Add(vertexList[0]);
	allVertexList.Add(vertexList[2]);
	allVertexList.Add(vertexList[3]);

	allVertexList.Add(vertexList[0]);
	allVertexList.Add(vertexList[7]);
	allVertexList.Add(vertexList[3]);
	allVertexList.Add(vertexList[0]);
	allVertexList.Add(vertexList[4]);
	allVertexList.Add(vertexList[7]);

	allVertexList.Add(vertexList[0]);
	allVertexList.Add(vertexList[1]);
	allVertexList.Add(vertexList[5]);
	allVertexList.Add(vertexList[0]);
	allVertexList.Add(vertexList[5]);
	allVertexList.Add(vertexList[4]);

	allVertexList.Add(vertexList[6]);
	allVertexList.Add(vertexList[4]);
	allVertexList.Add(vertexList[5]);
	allVertexList.Add(vertexList[6]);
	allVertexList.Add(vertexList[7]);
	allVertexList.Add(vertexList[4]);

	allVertexList.Add(vertexList[6]);
	allVertexList.Add(vertexList[5]);
	allVertexList.Add(vertexList[1]);
	allVertexList.Add(vertexList[6]);
	allVertexList.Add(vertexList[1]);
	allVertexList.Add(vertexList[2]);

	allVertexList.Add(vertexList[6]);
	allVertexList.Add(vertexList[3]);
	allVertexList.Add(vertexList[2]);
	allVertexList.Add(vertexList[6]);
	allVertexList.Add(vertexList[7]);
	allVertexList.Add(vertexList[3]);
	//计算顶点索引
	for (int i = 0; i < 36; i++)
		triangeList.Add(i);
	//更新包围盒组件
	TArray<FVector> Normals;
	TArray<FVector2D> UV0; 
	TArray<FLinearColor>VertexColors; 
	TArray<FProcMeshTangent>Tangents;
	//MyRootComponent->CreateMeshSection_LinearColor(0, allVertexList, triangeList, Normals, UV0, VertexColors, Tangents, true);  //指定顶点
	//MyRootComponent->AddCollisionConvexMesh(vertexList);   //添加碰撞点，否则无法和其他的actor产生碰撞
}

//设置产品信息
void AModulBaseActor::SetProdInfor(FDownloadInforStruct infor)
{
	m_prodInfor = infor;
	return;
}

//获得产品信息
FDownloadInforStruct AModulBaseActor::ReturnProdInfor()
{
	return m_prodInfor;
}

//移动模型
void AModulBaseActor::MoveModelBase()
{
	if (ActorHasTag(TEXT("普通模型")) == false)
		return;
	ABuildHouseCharacter* charator = Cast<ABuildHouseCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (charator)
	{
		charator->SetOperationType(OperationType::OT_MoveModel);
	}
}

//复制模型
void AModulBaseActor::CopyModelBase()
{
	if (ActorHasTag(TEXT("普通模型")) == false)
		return;
	if (m_prodInfor.url.IsEmpty())
		return;  //只复制素材库中拖下来的模型
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance == nullptr)
		return;
	//判断文件是否在本地存在
	FString texURL = m_txtPath;
	bool bExit = gameInstance->IsTheModelInLocal(texURL);
	if (bExit == false)
		return ;  //模型在本地不存在时不复制
	////在相同位置创建模型
	FTransform trans;
	FActorSpawnParameters aa;
	TSubclassOf<AActor>modulbaseInstance = LoadClass<AActor>(NULL, TEXT("Blueprint'/Game/Blueprint/BP_ModulBaseActor.BP_ModulBaseActor_C'"));
	AModulBaseActor* modelbaseTemp = Cast<AModulBaseActor>(GetWorld()->SpawnActor(modulbaseInstance, &trans, aa));  //创建模型
	if (modelbaseTemp)
	{
		FString tempName = texURL;
		int index = -1;
		if (texURL.FindLastChar('/', index))
		{
			tempName = tempName.Right(tempName.Len() - 1 - index);
		}
		modelbaseTemp->m_txtPath = tempName;  //txt文件的名字
		modelbaseTemp->SetProdInfor(this->m_prodInfor);  //设置产品信息
		modelbaseTemp->SetModelName(this->m_prodInfor.info[11]);  //模型名字
#if WITH_EDITOR
		modelbaseTemp->SetActorLabel(*(modelbaseTemp->ReturnModelName()));
#endif 
		//指定模型的geometry和texture信息
		modelbaseTemp->CreateNewObject(texURL);
		//设置模型的位置、缩放、旋转信息
		modelbaseTemp->SetActorTransform(this->GetActorTransform());
		//计算包围盒
		CalculateActorBound();
		//材质和被复制的模型一致
		for (int i = 0;i<(int)MeshComponentList.Num();i++)
		{
			if (i < modelbaseTemp->MeshComponentList.Num())
			{
				UMaterialInstanceDynamic* materialInstance = Cast<UMaterialInstanceDynamic>(MeshComponentList[i]->GetMaterial(0));
				modelbaseTemp->MeshComponentList[i]->SetMaterial(0, materialInstance);
			}
		}
		//将被复制的模型设置为选中
		this->SetOutline(false);//取消上次选中物体的描边
		this->SetSelComponent(nullptr);  //重置选中的组件
		modelbaseTemp->SetOutline(true);//将当前选中的物体描边
		gameInstance->SetSelectModulBaseActor(modelbaseTemp); //保存选中的模型
		//将当前的状态设置为复制模型
		ABuildHouseCharacter* charator = Cast<ABuildHouseCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
		charator->SetOperationType(OperationType::OT_CopyModel);
		//添加场景可见性
		gameInstance->m_pScene->AddObjectFlagForFloors(modelbaseTemp->ReturnModelType(), modelbaseTemp, modelbaseTemp->GetActorLocation());
	}
}

//左右镜像模型
void AModulBaseActor::LeftMirrorModelBase()
{
	if (ActorHasTag(TEXT("普通模型")) == false)
		return;
	FVector scale = GetActorScale3D();
	SetActorScale3D(FVector(scale.X,scale.Y*-1,scale.Z));
	return;
}

//前后镜像模型
void AModulBaseActor::FrontMirrorModelBase()
{
	if (ActorHasTag(TEXT("普通模型")) == false)
		return;
	FVector scale = GetActorScale3D();
	SetActorScale3D(FVector(-1*scale.X, scale.Y, scale.Z));
	return;
}

//旋转模型
void AModulBaseActor::RotateModel(RotationType rotType, float delta)
{
	if (ActorHasTag(TEXT("普通模型")) == false)
		return;  //墙顶地等不能旋转
	FTransform actorTransform = GetActorTransform();
	FRotator actorWorldRotator = GetActorRotation();
	FRotator actorLocalRotator = UKismetMathLibrary::InverseTransformRotation(actorTransform,actorWorldRotator);  //转化到局部坐标系
	FVector rotAxis;
	switch (rotType)
	{
	case RotationType::RT_Pitch:
		rotAxis = UKismetMathLibrary::GetRightVector(actorLocalRotator);
		break;
	case RotationType::RT_Yaw:
		rotAxis = UKismetMathLibrary::GetUpVector(actorLocalRotator);
		break;
	case RotationType::RT_Roll:
		rotAxis = UKismetMathLibrary::GetForwardVector(actorLocalRotator);
		break;
	default:
		return;
	}
	FRotator rotLocal = UKismetMathLibrary::RotatorFromAxisAndAngle(rotAxis, delta);  //绕着轴旋转增量
	FRotator rotWorld = UKismetMathLibrary::TransformRotation(actorTransform, rotLocal);//转化为世界坐标
	SetActorRotation(rotWorld);
	//更新坐标轴
	ABuildHouseGameModeBase* gameModeBase = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
	if (gameModeBase)
	{
		gameModeBase->AddGizmo(true,GetActorLocation(),GetActorRotation());
		gameModeBase->AddProdOperationWidget(false);  //隐藏产品操作UI
	}
}

//旋转模型
//只沿着轴向旋转,默认建模时模型的轴向都是向上的
void AModulBaseActor::RotateModel(float newAngle)
{
	if (ActorHasTag(TEXT("普通模型")) == false)
		return;  //墙顶地等不能旋转
	for (int i = 0; i < (int)MeshComponentList.Num(); i++)
	{
		MeshComponentList[i]->SetRelativeRotation(FRotator(0,newAngle,0));
	}
	//更新坐标轴
	ABuildHouseGameModeBase* gameModeBase = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
	if (gameModeBase)
	{
		gameModeBase->AddGizmo(true, GetActorLocation(), GetActorRotation());
		gameModeBase->AddProdOperationWidget(false);  //隐藏产品操作UI
	}
}

//设置模型显示或者隐藏
void AModulBaseActor::SetModulBaseActorShow(bool show)
{
	if (show)
	{//显示
		for (auto& procedural : MeshComponentList)
		{
			procedural->SetVisibleInSceneCaptureOnly(false);
			SetActorEnableCollision(true);
		}
	}
	else
	{//隐藏
		for (auto& procedural : MeshComponentList)
		{
			procedural->SetVisibleInSceneCaptureOnly(true);
			SetActorEnableCollision(false);
		}
	}
	return;
}

//返回模型显示或者隐藏
bool AModulBaseActor::GetModulBaseActorShow()
{
	for (auto& procedural : MeshComponentList)
	{
		return !procedural->bVisibleInSceneCaptureOnly;
	}
	return false;
}

//根据RoomMaterialChange修改材质
void AModulBaseActor::ChangeMaterialBasedRoomMaterialChange()
{
	for (auto mat : m_materialChangeList)
	{
		FString changDes = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(mat->m_TexName).c_str());
		if (changDes == TEXT("更改纹理UV"))
		{
			int geoIndex = mat->m_TexUVChange.m_GeometryIndex;
			if (MeshComponentList.IsValidIndex(geoIndex))
			{
				ModifyProceduralMeshUV(MeshComponentList[geoIndex], -1, FVector2D(-mat->m_TexUVChange.m_Translation.x * 100, -mat->m_TexUVChange.m_Translation.y * 100),
					FVector2D(mat->m_TexUVChange.m_Scale.x, mat->m_TexUVChange.m_Scale.y), mat->m_TexUVChange.m_fRotation);
			}
		}
		else if (changDes == TEXT("更改渲染材质"))
		{
			FString materialname = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(mat->m_ErMaterialID)).c_str());
			ModifyProceduralMeshMaterial(nullptr, mat->m_iTextureIndex, materialname);
		}
		else if (mat->m_bTexture)
		{//修改贴图
			std::string txtName(mat->m_TexName);
			if (-1 != txtName.rfind("\\"))
			{//有的记录有路径，有的没有，不管有没有，这里都变成没有
				txtName = txtName.substr(txtName.rfind("\\") + 1);
			}
			if (-1 != txtName.rfind(".txt"))
			{//有的记录只有id，有的有后缀，统一改成有后缀
				txtName = txtName.substr(0, txtName.rfind(".txt"));
				txtName += ".txt";
			}
			FString replaceFile = UTF8_TO_TCHAR(txtName.c_str());
			FString materialname = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(mat->m_ErMaterialID)).c_str());
			ReplaceTexture(nullptr, mat->m_iTextureIndex, replaceFile, materialname);
		}
		else
		{//修改颜色
			ModifyProceduralMeshColor(nullptr, mat->m_iTextureIndex, FVector(mat->m_Color.x, mat->m_Color.y, mat->m_Color.z));
		}
	}
	//清空内存
	for (int i = m_materialChangeList.size() - 1; i >= 0; i--)
	{
		delete m_materialChangeList[i];
		m_materialChangeList[i] = nullptr;
	}
	m_materialChangeList.clear();
}