// Fill out your copyright notice in the Description page of Project Settings.


#include "HouseGroupActor.h"
#include "../Public/ModulBaseActor.h"
#include "../Public/BuildHouseGameInstance.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Windows/AllowWindowsPlatformTypes.h" //一般为Windows中的宏和UE4冲突所致,将可能会导致冲突的宏所在的hom文件包裹起来
#include "Windows/PreWindowsApi.h"
#include "Picture.h"
#include "flyfile_w.h"
#include "GeoMaths.h"
#include "RoomMaterialChange.h"
#include "Windows/PostWindowsApi.h"            //一般为Windows中的宏和UE4冲突所致,将可能会导致冲突的宏所在的hom文件包裹起来

// Sets default values
AHouseGroupActor::AHouseGroupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_PositionState = 3; //默认为任意放置
}

// Called when the game starts or when spawned
void AHouseGroupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHouseGroupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//设置产品信息
void AHouseGroupActor::SetProdInfor(FDownloadInforStruct infor)
{
	m_GroupInfor = infor;
	return;
}

//获得产品信息
FDownloadInforStruct AHouseGroupActor::ReturnProdInfor()
{
	return m_GroupInfor;
}

//设置group的名字
void AHouseGroupActor::SetGroupName(FString name)
{
	int styleIndex = -1, objectIndex = 0;  //物体类型，物体序号
	styleIndex = (int)OBJECT_STYLE::GROUP;
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		objectIndex = gameInstance->m_pScene->m_GroupList.size();
	}
	if (styleIndex > -1 && objectIndex > -1)
	{
		name = FString::FromInt(styleIndex) + TEXT("_") + FString::FromInt(objectIndex) + TEXT("_") + name;
	}
	//判断是否有同名群组
	TArray<AActor*> GroupActors;  //场景中的群组
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("群组")), GroupActors);
	int sameNum = 0;  //拥有相同名字的群组个数
	for (auto& actor : GroupActors)
	{
		AHouseGroupActor* groupActor = Cast<AHouseGroupActor>(actor);
		if (groupActor && groupActor->ReturnGroupName().Find(name) > -1)
		{
			sameNum++;
		}
	}
	if (sameNum > 0)
	{//存在同名模型
		name = name + TEXT("_") + FString::FromInt(sameNum);
	}
	m_GroupName = name;
	Tags.Add(FName(TEXT("群组")));
	Tags.Add(FName(*m_GroupName));
	return;
}

 //返回group的名字
FString AHouseGroupActor::ReturnGroupName()
{
	return m_GroupName;
}

//获得所有单元，包含子群组
void AHouseGroupActor::GetAllUnit(TArray<FString>& unitList)
{
	for (auto& unit : m_GroupUnitList)
	{
		FString unitName = unit.groupUnitName;
		int index = -1;
		if (unitName.FindChar('_', index))
		{
			FString typeStr = unitName.Left(index);
			int style = FCString::Atoi(*typeStr);
			if (style == (int)OBJECT_STYLE::GROUP)
			{//群组
				TArray<AActor*> subGroupList;  
				UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*unitName), subGroupList);
				for (auto& subGroup : subGroupList)
				{
					AHouseGroupActor* subGroupTemp = Cast<AHouseGroupActor>(subGroup);
					if (subGroupTemp)
					{
						unitList.AddUnique(subGroupTemp->ReturnGroupName());
						subGroupTemp->GetAllUnit(unitList);
					}
				}
			}
			else
			{//除了群组外的模型
				unitList.AddUnique(unitName);
			}
		}
	}
}

//描边
void AHouseGroupActor::SetOutline(bool outLine)
{
	//获得群组中的所有模型
	TArray<FString>unitList;
	GetAllUnit(unitList);
	for (auto& unit : unitList)
	{
		TArray<AActor*> modulActors;  
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*unit), modulActors);
		for (auto& modul : modulActors)
		{
			AModulBaseActor* modulActor = Cast<AModulBaseActor>(modul);
			if (modulActor)
			{
				modulActor->SetOutline(outLine);
			}
		}
	}
}

//添加单元
void AHouseGroupActor::AddGroupUnit(FString name)
{
	if (name.IsEmpty())
		return;
	//获得群组中的所有模型
	TArray<FString>unitList;
	GetAllUnit(unitList);
	if (unitList.Find(name)<0)
	{//模型不在群组中
		//判断模型是否在其他群组中
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance)
		{
			bool bInOtherGroup = false;  //模型是否已经在其他群组中
			TArray<AActor*> modulActors;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*name), modulActors);
			for (auto single : modulActors)
			{
				AHouseGroupActor* parentGroup = gameInstance->m_pScene->GetInWhichGroup(single);
				if (parentGroup)
				{//模型已经在其他群组中
					FHouseGroupUnitStruct groupUnit;
					groupUnit.groupUnitName = parentGroup->ReturnGroupName();
					m_GroupUnitList.Add(groupUnit);
					if (m_GroupUnitList.Num() == 1)
					{
						parentGroup->ReComputeMinMax();
						SetActorLocation(parentGroup->GetActorLocation());
					}
					parentGroup->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
					bInOtherGroup = true;
				}
			}
			if (!bInOtherGroup)
			{
				//模型不属于任何群组
				FHouseGroupUnitStruct groupUnit;
				groupUnit.groupUnitName = name;
				m_GroupUnitList.Add(groupUnit);
				for (auto single : modulActors)
				{
					if (m_GroupUnitList.Num() == 1)
					{
						SetActorLocation(single->GetActorLocation());
					}
					single->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld,true));
				}
			}
			//调整群组中每个模型的位置，保证以群组的位置为中心点
			FVector oldCenter = GetActorLocation();
			ReComputeMinMax();  //更新群组位置
			FVector newCenter = GetActorLocation();
			FVector delta = oldCenter - newCenter;
			for (auto& unit : m_GroupUnitList)
			{
				TArray<AActor*> modulActorList;
				UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*unit.groupUnitName), modulActorList);
				for (auto& modul : modulActorList)
				{
					AModulBaseActor* subModel = Cast<AModulBaseActor>(modul);
					if (subModel)
					{//单个模型
						subModel->AddActorWorldOffset(delta);
					}
					else
					{
						AHouseGroupActor* subGroup = Cast<AHouseGroupActor>(modul);
						if (subGroup)
						{//群组
							subGroup->AddActorWorldOffset(delta);
							subGroup->ReComputeMinMax();
						}
					}
				}
			}
			//描边
			SetOutline(true);
			return;
		}
	}
}

/***********************************************
作者： jiang    创建时间 ： 2010.2.13
功能描述：以新轴对当前axislists进行调整：如果axis与序列中某元素相等，其num加1；否则，为axislists增加一个新元素
输入参数：axislists：当前的旋转轴序列；axis：新的轴
输出参数：无
返回参数说明：无
************************************************/
void AHouseGroupActor::AdjustAxisLists(std::vector<CAxisInfor>& axislists, FVector axis)
{
	unsigned long i;
	for (i = 0; i < axislists.size(); i++)
	{
		if ((axislists[i].axis.X == axis.X) && (axislists[i].axis.Y == axis.Y) && (axislists[i].axis.Z == axis.Z))
		{
			axislists[i].num++;
			return;
		}
	}

	CAxisInfor newaxis;
	newaxis.axis = axis;
	axislists.push_back(newaxis);
	return;
}

void AHouseGroupActor::GetAllAxis(std::vector<CAxisInfor>& axislists)
{
	//获得群组中的所有模型
	TArray<FString>unitList;
	GetAllUnit(unitList);
	for (auto& unit : unitList)
	{
		TArray<AActor*> modulActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*unit), modulActors);
		for (auto& modul : modulActors)
		{
			AModulBaseActor* subModel = Cast<AModulBaseActor>(modul);
			if (subModel)
			{//单个模型
				FVector axis = subModel->GetActorUpVector(); //模型轴向
				AdjustAxisLists(axislists, axis);//调整AxisLists
			}
			else
			{
				AHouseGroupActor* subGroup = Cast<AHouseGroupActor>(modul);
				if (subGroup)
				{//群组
					GetAllAxis(axislists);
				}
			}
		}
	}
	return;
}

//计算群组的位置
//群组位置在组成群组的actor的位置底面中心点
void AHouseGroupActor::ReComputeMinMax()
{
	if (m_GroupUnitList.Num() == 0)
		return;
	m_MinCoord.X = m_MinCoord.Y = m_MinCoord.Z = 100000;
	m_MaxCoord.X = m_MaxCoord.Y = m_MaxCoord.Z = -100000;
	FVector regMin = m_MinCoord, regMax = m_MaxCoord;
	std::vector<CAxisInfor> AxisLists;
	//获得群组中的所有模型
	for (auto& unit : m_GroupUnitList)
	{
		TArray<AActor*> modulActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*unit.groupUnitName), modulActors);
		for (auto& modul : modulActors)
		{
			FVector boundMinTemp, boundMaxTemp;
			FVector boundMin = FVector::ZeroVector, boundMax = FVector::ZeroVector;
			AModulBaseActor* subModel = Cast<AModulBaseActor>(modul);
			if (subModel)
			{//单个模型
				FBox box = subModel->CalculateComponentsBoundingBoxInLocalSpace(true, false);
				boundMinTemp = UKismetMathLibrary::TransformLocation(subModel->GetActorTransform(), box.Min);
				boundMaxTemp = UKismetMathLibrary::TransformLocation(subModel->GetActorTransform(), box.Max); //转化为世界坐标
				if (subModel->MeshComponentList.Num() == 0)
				{//群组的模型在本地不存在，比如素材库中拖群组时
					boundMinTemp = boundMaxTemp = subModel->GetActorLocation();
				}
				FVector axis = UKismetMathLibrary::GetUpVector(subModel->GetActorRotation());  //模型轴向
				AdjustAxisLists(AxisLists, axis);//调整AxisLists
			}
			else
			{
				AHouseGroupActor* subGroup = Cast<AHouseGroupActor>(modul);
				if (subGroup)
				{//群组
					subGroup->ReComputeMinMax();
					boundMinTemp = subGroup->m_MinCoord;
					boundMaxTemp = subGroup->m_MaxCoord;
					subGroup->GetAllAxis(AxisLists);//群组轴向
				}
			}
			boundMin.X = boundMinTemp.X < boundMaxTemp.X ? boundMinTemp.X : boundMaxTemp.X;
			boundMin.Y = boundMinTemp.Y < boundMaxTemp.Y ? boundMinTemp.Y : boundMaxTemp.Y;
			boundMin.Z = boundMinTemp.Z < boundMaxTemp.Z ? boundMinTemp.Z : boundMaxTemp.Z;
			boundMax.X = boundMinTemp.X > boundMaxTemp.X ? boundMinTemp.X : boundMaxTemp.X;
			boundMax.Y = boundMinTemp.Y > boundMaxTemp.Y ? boundMinTemp.Y : boundMaxTemp.Y;
			boundMax.Z = boundMinTemp.Z > boundMaxTemp.Z ? boundMinTemp.Z : boundMaxTemp.Z;
			m_MinCoord.X = boundMin.X < m_MinCoord.X ? boundMin.X : m_MinCoord.X;
			m_MinCoord.Y = boundMin.Y < m_MinCoord.Y ? boundMin.Y : m_MinCoord.Y;
			m_MinCoord.Z = boundMin.Z < m_MinCoord.Z ? boundMin.Z : m_MinCoord.Z;
			m_MaxCoord.X = boundMax.X > m_MaxCoord.X ? boundMax.X : m_MaxCoord.X;
			m_MaxCoord.Y = boundMax.Y > m_MaxCoord.Y ? boundMax.Y : m_MaxCoord.Y;
			m_MaxCoord.Z = boundMax.Z > m_MaxCoord.Z ? boundMax.Z : m_MaxCoord.Z;
		}
	}
	//以最多的旋转轴作为群的旋转轴
	int index = -1, max = -1;
	for (int j = 0; j < AxisLists.size(); j++)
	{
		if (AxisLists[j].num > max)
		{
			max = AxisLists[j].num;
			index = j;
		}
	}
	if (-1 != index)
	{
		m_RotateAxis = AxisLists[index].axis;
	}
	for (int m = 0; m < AxisLists.size(); m++)
	{
		FVector axistemp = AxisLists.at(m).axis;
		FVector crossDir = FVector::CrossProduct(axistemp, FVector::ZAxisVector);
		if (crossDir.Size() < 1e-3)
		{
			m_RotateAxis = axistemp;
			//如果有对象是放在地面的，就设置为垂直向上
			m_PositionState = 4;
			break;
		}
	}
	//如果群组是放在地上或者轴是垂直向上的，轴不变
	if (m_PositionState == 0 || m_PositionState == 4)
		m_RotateAxis = FVector::ZAxisVector;
	//确定中心点坐标
	FVector m_Center = (m_MinCoord + m_MaxCoord) * 0.5;
	//修正中心点，使中心点在垂直轴的底面上
	if (fabs(m_RotateAxis.X) < 0.1f
		&& fabs(m_RotateAxis.Z) < 0.1f
		&& fabs(fabs(m_RotateAxis.Y) - 1.0f) < 0.1f)
	{///竖直方向上
		if (m_RotateAxis.Y > 0.0f)
		{
			//如果是y轴正向，中心点应该在底面
			m_Center.Y = m_MinCoord.Y /*+ 0.002f*/;
		}
		else
		{
			//如果是y轴负向，中心点应该在顶面
			m_Center.Y = m_MaxCoord.Y;
		}
	}
	if (fabs(m_RotateAxis.X) < 0.1f
		&& fabs(m_RotateAxis.Y) < 0.1f
		&& fabs(fabs(m_RotateAxis.Z) - 1.0f) < 0.1f)
	{///z方向
		if (m_RotateAxis.Z > 0.0f)
		{
			m_Center.Z = m_MinCoord.Z;
		}
		else
		{
			m_Center.Z = m_MaxCoord.Z;
		}
	}
	if (fabs(m_RotateAxis.Y) < 0.1f
		&& fabs(m_RotateAxis.Z) < 0.1f
		&& fabs(fabs(m_RotateAxis.X) - 1.0f) < 0.1f)
	{///x方向
		if (m_RotateAxis.X > 0.0f)
		{
			m_Center.X = m_MinCoord.X;
		}
		else
		{
			m_Center.X = m_MaxCoord.X;
		}
	}
	//设置群组的位置
	SetActorLocation(m_Center);
	return;
}

//移动群组
void AHouseGroupActor::MoveGroup(FVector delta)
{
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (!gameInstance)
		return;
	//移动群组
	AddActorWorldOffset(delta);
	//获得群组中的所有模型
	TArray<FString>unitList;
	GetAllUnit(unitList);
	//重置群组中的所有模型的可见性
	for (auto& unit : unitList)
	{
		TArray<AActor*> modulActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*unit), modulActors);
		for (auto& modul : modulActors)
		{
			AModulBaseActor* model = Cast<AModulBaseActor>(modul);
			if (model)
			{
				gameInstance->m_pScene->ModifyObjectFlagForFloors(model->ReturnModelType(), model, model->GetActorLocation());
			}
		}
	}
	return;
}

//旋转群组:绕着旋转轴旋转
void AHouseGroupActor::RotateGroup(float newAngle)
{
	float deltaAngle = newAngle - m_RotateAngle;
	FRotator deltaRot = UKismetMathLibrary::RotatorFromAxisAndAngle(m_RotateAxis, deltaAngle);
	AddActorWorldRotation(deltaRot);
	m_RotateAngle = newAngle;
}

//拆分群组
void AHouseGroupActor::SplitGroup()
{
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance && gameInstance->m_pScene)
	{
		for (int j = 0; j < (int)gameInstance->m_pScene->m_GroupList.size(); j++)
		{
			if (gameInstance->m_pScene->m_GroupList[j] == this)
			{
				gameInstance->m_pScene->m_GroupList.erase(gameInstance->m_pScene->m_GroupList.begin()+j);
				Destroy();
				return;
			}
		}
	}
}

//创建群组，参数是群组所在txt文件的绝对路径
void AHouseGroupActor::ReadGroup(const FString& fileName)
{
	if (fileName.IsEmpty() || fileName.Find(TEXT(".txt")) == -1)
		return;
	flyFile_w flyfile;
	if (!flyfile.SetZipList(TCHAR_TO_ANSI(*fileName)))
		return;
	std::string fname = TCHAR_TO_UTF8(*fileName);;
	fname = fname.substr(0, fname.rfind('.')) + ".grp";
	flyfile.open(fname.substr(fname.rfind('\\') + 1, fname.length()).c_str());
	std::string groupFile(fname.substr(0, fname.rfind('.')));
	//从压缩包中读出群组信息生成本地群组文件
	char newGroupFile[512];
	bool bWriteSuc = flyfile.FindAndWriteFile(groupFile.c_str(), newGroupFile,".grp");
	flyfile.close();
	if (bWriteSuc == false)
		return;
	//打开新创建的grp文件，读入数据
	std::ifstream ifile;
	setlocale(LC_ALL, "Chinese-simplified");
	ifile.open(newGroupFile, std::ios::in | std::ios::binary);
	setlocale(LC_ALL, "C");
	if (ifile.fail())
	{
		ifile.close();
	}
	int version;
	ifile.read((char*)&version, sizeof(int));
	switch (version)
	{
	case 1:
	{
		ReadGroup_1(ifile);
	}
	break;
	case 2:
	{
		ReadGroup_2(ifile);
	}
	break;
	case 3:
	{
		ReadGroup_3(ifile);
	}
	break;
	case 4:
	{
		ReadGroup_4(ifile);
	}
	break;
	case 5:
	{
		ReadGroup_5(ifile);
	}
	break;
	case 6:
	{
		ReadGroup_6(ifile);
	}
	break;
	case 7:
	{
		ReadGroup_7(ifile);
	}
	break;
	default:
		break;
	}
	FVector tempAxis;
	ifile.read((char*)&tempAxis, sizeof(CVector3));
	m_RotateAxis.X = tempAxis.X;
	m_RotateAxis.Y = tempAxis.Z;
	m_RotateAxis.Z = tempAxis.Y;
	ifile.close();
	FString deleteFile = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(newGroupFile)).c_str());
	IFileManager::Get().Delete(*deleteFile);//删除
}

void  AHouseGroupActor::ReadGroup_7(std::ifstream& file)
{
	ReadGroup_6(file);
}

void  AHouseGroupActor::ReadGroup_6(std::ifstream& file)
{
	ReadGroup_5(file);
	file.read((char*)&m_RotateAngle, sizeof(float));
}

void  AHouseGroupActor::ReadGroup_5(std::ifstream& file)
{
	ReadGroup_4(file);
	CGeoMaths math;
	m_GroupInfor.info[20] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());  //服务器标记
}

void  AHouseGroupActor::ReadGroup_4(std::ifstream& file)
{//CMyView::ConvertDownLoadInfoToCellInfo
	ReadGroup_3(file);
	CGeoMaths math;
	m_GroupInfor.info[0] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str()); 
	m_GroupInfor.info[1] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[3] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[4] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[5] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[6] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[7] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[8] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[9] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[10] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[11] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[12] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[13] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[14] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[2] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[17] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[15] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.info[16] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	FString positionState = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_PositionState = FCString::Atoi(*positionState);
}

void  AHouseGroupActor::ReadGroup_3(std::ifstream& file)
{
	ReadGroup_2(file);
	file.read((char*)&m_PositionState, sizeof(int));
}

void AHouseGroupActor::ReadGroup_2(std::ifstream& file)
{//CMyView::ConvertDownLoadInfoToCellInfo
	ReadGroup_1(file);
	CGeoMaths math;
	m_GroupInfor.type = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	m_GroupInfor.url = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
	std::string ImageName = math.ReadString(file);
	m_GroupInfor.info[2] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(math.ReadString(file))).c_str());
}

void AHouseGroupActor::ReadGroup_1(std::ifstream& file)
{
	int num = 0;
	m_GroupUnitList.Empty();
	file.read((char*)&num, sizeof(int));   //组成群组的单元个数
	for (int i = 0; i < num; i++)
	{
		//读入每个组合元素
		//CSelObject tempObj(-2, -2, ZERO_VECTOR3);
		int styleIndex = -1;
		file.read((char*)&styleIndex, sizeof(int));  //类型
		switch (styleIndex)
		{
		case 0:
		case 3:
		{
			//如果是物体或灯
			char temp[512];
			file.read((char*)temp, 512);
			//读取该对象所对应的propert属性，用于创建这个新对象
			UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
			if (gameInstance == nullptr)
				return;
			CPropertyAndEvent regionProp = gameInstance->m_pScene->pInformation->ReturnPropertyAndEvent(styleIndex);
			regionProp.ReadFromFileReferScript(file);
			//创建新对象
			std::string fileName(temp);
			fileName = fileName.substr(fileName.rfind('\\') + 1, fileName.length());
			//获得产品信息
			FDownloadInforStruct prodInfor;
			
			prodInfor.url = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(std::string(fileName.substr(0, fileName.rfind('.')))).c_str());  //id
			prodInfor.type = TEXT("0");  //产品类型
			char prodBrand[256];
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("品牌").c_str(), "charcluster", prodBrand);
			prodInfor.info[0] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(prodBrand).c_str());   //品牌
			
			memset(prodBrand, 0, sizeof(prodBrand));
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("型号").c_str(), "charcluster", prodBrand);
			prodInfor.info[1] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(prodBrand).c_str());   //型号

			memset(prodBrand, 0, sizeof(prodBrand));
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("价格").c_str(), "float", prodBrand);
			prodInfor.info[2] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(prodBrand).c_str());   //价格

			float length =0, width=0, height=0;
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("长度").c_str(), "float", &length);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("宽度").c_str(), "float", &width);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("高度").c_str(), "float", &height);
			FString sizeStr = FString::FromInt((int)(length*1000))+ TEXT("*") + FString::FromInt((int)(width * 1000)) + TEXT("*") + FString::FromInt((int)(height * 1000));
			prodInfor.info[3] = sizeStr;   //尺寸

			memset(prodBrand, 0, sizeof(prodBrand));
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("类型").c_str(), "charcluster", prodBrand);
			prodInfor.info[6] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(prodBrand).c_str());   //类型

			memset(prodBrand, 0, sizeof(prodBrand));
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("产品名称").c_str(), "charcluster", prodBrand);
			prodInfor.info[11] = UTF8_TO_TCHAR(AModulBaseActor::string_To_UTF8(prodBrand).c_str());   //产品名称

			//镜像操作没处理

			FVector location, scale, rotatAngle,axis;  //位置、缩放、旋转、轴
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("位置X").c_str(), "float", &location.X);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("位置Y").c_str(), "float", &location.Z);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("位置Z").c_str(), "float", &location.Y);
			location *= 100;  //单位转化
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("缩放X").c_str(), "float", &scale.X);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("缩放Y").c_str(), "float", &scale.Z);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("缩放Z").c_str(), "float", &scale.Y);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("旋转X").c_str(), "float", &rotatAngle.X);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("旋转Y").c_str(), "float", &rotatAngle.Y);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("旋转Z").c_str(), "float", &rotatAngle.Z);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("轴X").c_str(), "float", &axis.X);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("轴Z").c_str(), "float", &axis.Y);
			regionProp.GetPropertyRealValue(AModulBaseActor::UTF8_To_string("轴Y").c_str(), "float", &axis.Z);
			FVector modelAxis = FVector(0.0f, 0.0f, 1.0f);  //在max中建模时模型的轴向，max中朝着z正方向的（x向右，z向上，y向里），在ue中相当于朝着z正方向，因为ue4中坐标系是y向右，x向里，z向上
			FVector rightVec = FVector::CrossProduct(axis, modelAxis);
			FVector upVec = axis;
			if (rightVec.IsNearlyZero())
			{//模型放到了地上或者吊顶上
				float angle = FVector::DotProduct(axis, modelAxis);
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
				rotatAngle.Y += 270;//再加270是因为软件中群组的朝向是朝着z轴，正好对应者ue4中的y轴，只有旋转270度，模型的才能朝向y轴
			}
			FMatrix rotationMatrix = FRotationMatrix::MakeFromYZ(rightVec, upVec);
			FRotator putRotate = rotationMatrix.Rotator();  //旋转
			//创建模型
			AModulBaseActor* createModel = gameInstance->HandlePutModel(UTF8_TO_TCHAR(fileName.c_str()), prodInfor, location, scale, putRotate,styleIndex);
			//旋转模型
			FRotator deltaRot = UKismetMathLibrary::RotatorFromAxisAndAngle(upVec, rotatAngle.Y);
			createModel->AddActorLocalRotation(deltaRot); 
			if (createModel)
			{
				AddGroupUnit(createModel->ReturnModelName());
			}
			//替换新对象表面材质
			int num2 = 0;
			file.read((char*)&num2, sizeof(int));
			for (int j = 0; j < num2; j++)
			{
				CRoomMaterialChange *mat = new CRoomMaterialChange();
				mat->ReadFromFile(file);
				//保存材质
				createModel->m_materialChangeList.push_back(mat);
			}
			if (createModel->MeshComponentList.Num() != 0)
			{//模型文件在本地存在，材质替换
				createModel->ChangeMaterialBasedRoomMaterialChange();
			}
		}
		break;
		case -7:
		{//几何元素没处理
		}
		break;
		case -9:
		{
			//创建组合
			FTransform trans;
			FActorSpawnParameters aa;
			TSubclassOf<AActor>groupInstance = LoadClass<AActor>(NULL, TEXT("Blueprint'/Game/Blueprint/BP_HouseGroup.BP_HouseGroup_C'"));
			AHouseGroupActor* subGroup = Cast<AHouseGroupActor>(GetWorld()->SpawnActor(groupInstance, &trans, aa));
			subGroup->SetGroupName(TEXT("自定义群组"));
#if WITH_EDITOR
			subGroup->SetActorLabel(*(subGroup->ReturnGroupName()));
#endif 
			UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
			//子群组保存到场景中
			gameInstance->m_pScene->m_GroupList.push_back(subGroup);
			subGroup->ReadGroup_1(file);
			//添加子群组
			AddGroupUnit(subGroup->ReturnGroupName());
		}
		break;
		default:
			break;
		}
	}
}