// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/CScene.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "../Public/BuildHouseCharacter.h"
#include "../Public/CollisionBFL.h"
#include "../Public/ReadFbx.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"
#include "Runtime/Engine/Classes/Engine/RectLight.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/RectLightComponent.h"

CScene::CScene()
{
	MyHouse = nullptr;
	MyWorldContex = nullptr;
	StyleFlag.clear();
	CStyleObjectFlag objectFlag;
	StyleFlag.insert(StyleFlag.begin(), 4, objectFlag);  //默认4类模型
	StyleList.clear();
	CStyle objectStyle;
	StyleList.insert(StyleList.begin(),4,objectStyle);   //默认4类模型
	FString projectDir = FPaths::ProjectDir();  //项目路径
	std::string projectDirStr = TCHAR_TO_ANSI(*projectDir);
	pInformation = new CObjectInScene((char*)projectDirStr.c_str());
}

CScene::~CScene()
{
	if (MyHouse)
	{
		delete MyHouse;
		MyHouse = nullptr;
	}
	MyWorldContex = nullptr;
	if (pInformation)
	{
		delete pInformation;
		pInformation = nullptr;
	}
}

//初始化场景
//删除场景中的模型，删除场景中的可见性标记
void CScene::InitScene()
{
	//删除放置在门和窗户处的rectlight
	TArray<AActor*> rectLightSpawnedList;
	UGameplayStatics::GetAllActorsOfClass(MyWorldContex, ARectLight::StaticClass(), rectLightSpawnedList);
	for (auto& spawnedActor : rectLightSpawnedList)
	{
		spawnedActor->Destroy();
	}
	//删除群组
	TArray<AActor*> GroupSpawnedList;
	UGameplayStatics::GetAllActorsOfClass(MyWorldContex, AHouseGroupActor::StaticClass(), GroupSpawnedList);
	for (auto& spawnedActor : GroupSpawnedList)
	{
		spawnedActor->Destroy();
	}
	//删除模型
	TArray<AActor*> ActorsSpawnedList; 
	UGameplayStatics::GetAllActorsOfClass(MyWorldContex, AModulBaseActor::StaticClass(), ActorsSpawnedList);
	for (auto &spawnedActor:ActorsSpawnedList)
	{
		spawnedActor->Destroy();
	}
	MyWorldContex = nullptr;
	//删除可见性标记以及保存的模型指针     
	m_CeilingList.clear();
	m_FloorList.clear();
	m_MergeWallfaceList.clear();
	StyleList.clear();
	m_FloorObjectFlagList.clear();
	m_floorVisibleIndexList.Empty();
	m_floorVisibleIndexList.Empty();
	FloorFlags.clear();
	CeilingFlags.clear();
	HouseWallFlag.clear();
	MergeWallFaceFlags.clear();
	StyleFlag.clear();
	m_GroupList.clear();
	if (MyHouse)
	{
		delete MyHouse;
		MyHouse = nullptr;
	}
}

////读取场景信息文件
//因为c++类中无法直接获得world，所以这里设置了一个变量worldContex,在函数调用时直接传入GetWorld()即可
bool CScene::ReadSceneFile(FString sceneFile, UWorld* worldContex)
{
	if (worldContex == nullptr)
		return false;
	MyWorldContex = worldContex;
	if (FPaths::FileExists(sceneFile))
	{
		FString ResultString;
		if (FFileHelper::LoadFileToString(ResultString, *sceneFile))
		{
			TSharedPtr<FJsonObject> rootObject = MakeShareable(new FJsonObject());
			TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(ResultString);
			if (FJsonSerializer::Deserialize(jsonReader, rootObject))
			{
				if (MyHouse)
				{
					delete MyHouse;
					MyHouse = nullptr;
				}
				MyHouse = new CHouse;  //整个墙体
				MyHouse->m_height = rootObject->GetNumberField("houseHeight");  //墙高
				auto wallInforList = rootObject->GetObjectField("wallInforList")->GetArrayField("wallInfor");
				for (auto &wallInfor : wallInforList)
				{
					auto wallInforParse = wallInfor.Get()->AsObject();
					CHouseWall houseWall;
					houseWall.wallName = wallInforParse->GetStringField("wallName");  //墙体名字
					FVector midPos;
					midPos.X = wallInforParse->GetNumberField("middleLineBeginX") * 100.0f;  //墙体中轴线
					midPos.Y = wallInforParse->GetNumberField("middleLineBeginZ") * 100.0f;
					midPos.Z = wallInforParse->GetNumberField("middleLineBeginY") * 100.0f;
					houseWall.middle_vertex.push_back(midPos);
					midPos.X = wallInforParse->GetNumberField("middleLineEndX") * 100.0f;  //墙体中轴线
					midPos.Y = wallInforParse->GetNumberField("middleLineEndZ") * 100.0f;
					midPos.Z = wallInforParse->GetNumberField("middleLineEndY") * 100.0f;
					houseWall.middle_vertex.push_back(midPos);
					auto beginLinkInfoList = wallInforParse->GetArrayField("beginLinkInfo"); //墙体前端连接信息
					for (auto& beginLink : beginLinkInfoList)
					{
						auto beginLinkParse = beginLink.Get()->AsObject();
						CWallIlnkInfo link;
						link.WallIndex = beginLinkParse->GetNumberField("wallIndex");
						link.PointIndex = beginLinkParse->GetNumberField("pointIndex");
						houseWall.BeginLinkInfo.push_back(link);
					}
					auto endLinkInfoList = wallInforParse->GetArrayField("endLinkInfo"); //墙体后端连接信息
					for (auto& endLink : endLinkInfoList)
					{
						auto endLinkParse = endLink.Get()->AsObject();
						CWallIlnkInfo link;
						link.WallIndex = endLinkParse->GetNumberField("wallIndex");
						link.PointIndex = endLinkParse->GetNumberField("pointIndex");
						houseWall.EndlinkInfo.push_back(link);
					}
					if (wallInforParse->HasTypedField<EJson::Array>("mergeList"))
					{
						auto mergeList = wallInforParse->GetArrayField("mergeList");
						for (auto& merge : mergeList)
						{
							houseWall.mergeList.push_back(merge.Get()->AsNumber());
						}
					}
					MyHouse->m_wall.push_back(houseWall);  //压入
				}
				auto floorVisibleList = rootObject->GetArrayField("floorVisibleList");
				for (auto& floorVisible : floorVisibleList)
				{
					auto floorVisibleParse = floorVisible.Get()->AsObject();
					CFloorObjectFlag floorObjectFlag;
					floorObjectFlag.FloorName = floorVisibleParse->GetStringField("floorName"); //房间名字
					floorObjectFlag.m_roomStyle = (ROOM)floorVisibleParse->GetIntegerField("roomStyle");       //房间类型
					auto CeilingFlagsList = floorVisibleParse->GetArrayField("CeilingFlags");       //吊顶可见性
					for (auto& ceilingFlag : CeilingFlagsList)
					{
						floorObjectFlag.OldCeilingFlags.push_back(ceilingFlag.Get()->AsBool());
					}
					auto OldHouseWallFlags = floorVisibleParse->GetArrayField("OldHouseWallFlag");       //墙体可见性
					for (auto& OldHouseWallFlag : OldHouseWallFlags)
					{
						floorObjectFlag.OldHouseWallFlag.push_back(OldHouseWallFlag.Get()->AsBool());
					}
					if (floorVisibleParse->HasTypedField<EJson::Array>("MergeWallFaceFlags"))//融合面可见性
					{
						auto MergeWallFaceFlagsTemp = floorVisibleParse->GetArrayField("MergeWallFaceFlags");       
						for (auto& MergeWallFaceFlag : MergeWallFaceFlagsTemp)
						{
							floorObjectFlag.OldMergeWallFaceFlags.push_back(MergeWallFaceFlag.Get()->AsBool());
						}
					}
					CStyleObjectFlag styleFlag;
					floorObjectFlag.OldStyleFlag.push_back(styleFlag);
					auto normalObjFlag = floorVisibleParse->GetArrayField("normalObjFlag");       //普通模型的可见性
					for (auto& normalObj : normalObjFlag)
					{
						floorObjectFlag.OldStyleFlag.back().ObjectFlag.push_back(normalObj.Get()->AsBool());
					}
					floorObjectFlag.OldStyleFlag.push_back(styleFlag);
					auto doorObjFlag = floorVisibleParse->GetArrayField("doorObjFlag");       //门的可见性
					for (auto& doorObj : doorObjFlag)
					{
						floorObjectFlag.OldStyleFlag.back().ObjectFlag.push_back(doorObj.Get()->AsBool());
					}
					floorObjectFlag.OldStyleFlag.push_back(styleFlag);
					auto windowObjFlag = floorVisibleParse->GetArrayField("windowObjFlag");       //窗的可见性
					for (auto& windowObj : windowObjFlag)
					{
						floorObjectFlag.OldStyleFlag.back().ObjectFlag.push_back(windowObj.Get()->AsBool());
					}
					floorObjectFlag.OldStyleFlag.push_back(styleFlag);
					auto lightObjFlag = floorVisibleParse->GetArrayField("lightObjFlag");       //灯的可见性
					for (auto& lightObj : lightObjFlag)
					{
						floorObjectFlag.OldStyleFlag.back().ObjectFlag.push_back(lightObj.Get()->AsBool());
					}
					floorObjectFlag.FloorIndex = m_FloorObjectFlagList.size();     //房间序号，认为输出sceneInfor.txt时是按照floor从0开始输出的
					m_FloorObjectFlagList.push_back(floorObjectFlag);  //压入
					FloorFlags.push_back(false);
				}
				bool InitSuc = InitSceneModelBase();  //填充墙顶地
				InitSceneLight(); //初始化场景灯光
				if(InitSuc)
					return Init3DFirstPersonCharator();  //初始化角色的位置
				return InitSuc;
			}
		}
		else
		{//加载不成功
			return false;
		}
	}
	else
	{//文件不存在
		return false;
	}
	return false;
}

//初始化场景中的modulbase
//根据从sceneInfor.txt中读入的信息填充CSnene中的墙顶地序列、模型序列
//根据fbx输出时的名字和输出房间信息时的名字做对应关系，是一一对应的关系
bool CScene::InitSceneModelBase()
{
	const UObject* worldContex = MyWorldContex;
	if (MyHouse == nullptr || worldContex== nullptr)
		return false;
	//填充墙体
	for (auto& singleHouseWall : MyHouse->m_wall)
	{
		FString wallName = singleHouseWall.wallName;
		if (wallName.IsEmpty())
			return false;
		TArray<AActor*> wallList;  //fbx导入的所有的墙体
		UGameplayStatics::GetAllActorsWithTag(worldContex, FName(*wallName), wallList);
		if (wallList.Num() != 1)
		{
			return false;
		}
		else
		{
			singleHouseWall.wallModelBase = Cast<AModulBaseActor>(wallList[0]);
			//转化为
		}
	}
	//填充地面、吊顶、融合面
	for (int i=0;i<(int)m_FloorObjectFlagList.size();i++)
	{
		FString floorName = TEXT("地面_") + FString::FromInt(i);
		TArray<AActor*> floorList; 
		UGameplayStatics::GetAllActorsWithTag(worldContex, FName(*floorName), floorList);
		if (floorList.Num() != 1)
		{
			return false;
		}
		else
		{
			m_FloorList.push_back(Cast<AModulBaseActor>(floorList[0]));
		}
		if (i == 0)
		{//填充吊顶、融合面
			for (int j = 0; j < (int)m_FloorObjectFlagList[i].OldCeilingFlags.size(); j++)
			{
				FString ceilingName = TEXT("吊顶_") + FString::FromInt(j);
				TArray<AActor*> ceilingList;
				UGameplayStatics::GetAllActorsWithTag(worldContex, FName(*ceilingName), ceilingList);
				if (ceilingList.Num() != 1)
				{
					return false;
				}
				else
				{
					m_CeilingList.push_back(Cast<AModulBaseActor>(ceilingList[0]));
				}
			}
			for (int j = 0; j < (int)m_FloorObjectFlagList[i].OldMergeWallFaceFlags.size(); j++)
			{
				FString mergeName = TEXT("融合面_") + FString::FromInt(j);
				TArray<AActor*> mergeFaceList;
				UGameplayStatics::GetAllActorsWithTag(worldContex, FName(*mergeName), mergeFaceList);
				if (mergeFaceList.Num() != 1)
				{
					return false;
				}
				else
				{
					m_MergeWallfaceList.push_back(Cast<AModulBaseActor>(mergeFaceList[0]));
				}
			}
			if (m_FloorObjectFlagList[i].OldHouseWallFlag.size() != MyHouse->m_wall.size())
				return false;  //确保墙体的个数是一致的
			//填充模型
			TArray<AActor*> ModelActorList;  //场景中除了墙顶地外所有的模型
			UGameplayStatics::GetAllActorsWithTag(MyWorldContex, FName(TEXT("普通模型")), ModelActorList);
			TArray<AModulBaseActor*>modelbaseActorList;
			for (int j = 0; j < (int)ModelActorList.Num(); j++)
			{
				AModulBaseActor* actor = Cast<AModulBaseActor>(ModelActorList[j]);
				if (actor)
				{
					modelbaseActorList.Add(actor);
				}
			}
			for (int j = 0; j < (int)m_FloorObjectFlagList[i].OldStyleFlag.size(); j++)
			{
				CStyle objectStyle;
				StyleList.push_back(objectStyle);  //压入模型
				for (int k = 0; k < (int)m_FloorObjectFlagList[i].OldStyleFlag[j].ObjectFlag.size(); k++)
				{
					FString name = FString::FromInt(j) + TEXT("_") + FString::FromInt(k) + TEXT("_");  //输出fbx文件时，模型的名字是这样命名的：类型_序号_产品名称
					bool bFind = false;  //fbx文件是否输出了对应的模型
					for (int m = modelbaseActorList.Num() - 1; m >= 0; m--)
					{
						FString modelName = modelbaseActorList[m]->ReturnModelName();
						int index = modelName.Find(name);
						if (index > -1)
						{
							StyleList.back().pObjectList.push_back(modelbaseActorList[m]);
							modelbaseActorList.RemoveAt(m);
							bFind = true;
							break;
						}
					}
					if (!bFind)
					{
						StyleList.back().pObjectList.push_back(nullptr);
					}
				}
			}
		}
	}
	return true;
}

//初始化三维中第一人称漫游时charator的位置
//将相机放到场景中第一个房间的中心位置，并将所有的房间设置为可见
bool CScene::Init3DFirstPersonCharator()
{
	if (FloorFlags.size()==0)
		return false;
	ABuildHouseCharacter* charactor = Cast<ABuildHouseCharacter>(MyWorldContex->GetFirstPlayerController()->GetCharacter());
	if (charactor == nullptr)
		return false;
	//将所有的房间都设置为可见
	for (int i = 0; i < m_FloorList.size(); i++)
	{
		m_floorVisibleIndexList.AddUnique(i);
		//设置视点位置在第一个房间的中心位置
		if (i == 0)
		{
			charactor->SetActorLocation(m_FloorList[i]->GetActorLocation());
		}
	}
	charactor->SetTravelType(TravelType::TT_3D_FirstPerson);  //设置为第一人称漫游
	//更新可见性
	UpdateRoomVisiblity(m_floorVisibleIndexList);
	//获得视线位置和方向
	FVector cameraPos, cameraDir;
	charactor->ReturnCamera(cameraPos, cameraDir);
	//根据视点位置调整可见性
	ModifyVisibility(cameraPos, cameraDir);
	return true;
}

//清空可见性
void CScene::ClearVisibility()
{
	//墙体
	HouseWallFlag.clear();
	HouseWallFlag.insert(HouseWallFlag.begin(), MyHouse->m_wall.size(), false);
	for (auto& wall : MyHouse->m_wall)
	{
		wall.wallModelBase->SetModulBaseActorShow(false);
	}
	//吊顶
	CeilingFlags.clear();
	CeilingFlags.insert(CeilingFlags.begin(), m_CeilingList.size(), false);
	for (auto &ceiling : m_CeilingList)
	{
		ceiling->SetModulBaseActorShow(false);
	}
	//地面
	FloorFlags.clear();
	FloorFlags.insert(FloorFlags.begin(), m_FloorList.size(), false);
	for (auto& floor : m_FloorList)
	{
		floor->SetModulBaseActorShow(false);
	}
	//融合面
	MergeWallFaceFlags.clear();
	MergeWallFaceFlags.insert(MergeWallFaceFlags.begin(), m_MergeWallfaceList.size(), false);
	for (auto& mergeFace : m_MergeWallfaceList)
	{
		mergeFace->SetModulBaseActorShow(false);
	}
	//所有模型
	StyleFlag.clear();
	CStyleObjectFlag objectFlag;
	StyleFlag.insert(StyleFlag.begin(), 4, objectFlag);
	for (int i = 0; i < (int)StyleList.size(); i++)
	{
		StyleFlag[i].ObjectFlag.insert(StyleFlag[i].ObjectFlag.begin(), StyleList[i].pObjectList.size(), false);
		for (int j = 0; j < (int)StyleList[i].pObjectList.size(); j++)
		{
			if (StyleList[i].pObjectList[j])
			{
				StyleList[i].pObjectList[j]->SetModulBaseActorShow(false);
			}
		}
	}
	return;
}


//更新可见性
void CScene::UpdateRoomVisiblity(TArray<int>floorVisible)
{
	//清空可见性
	ClearVisibility();
	//设置每个房间中的墙顶地可见性以及整体的地面可见性
	for (int i = 0; i < (int)m_FloorObjectFlagList.size(); i++)
	{
		int bFind= m_floorVisibleIndexList.Find(i);
		if (bFind < 0)
		{//房间不可见
			FloorFlags[i] = false;  //设置房间可见性
			m_FloorList[i]->SetModulBaseActorShow(false);
		}
		else
		{//房间可见
			FloorFlags[i] = true;  //设置房间可见性
			m_FloorList[i]->SetModulBaseActorShow(true);
		}
	}
	//设置整体的可见性
	for (int i = 0; i < (int)HouseWallFlag.size(); i++)
	{//墙
		bool bVisible = false;
		for (int j = 0; j < (int)m_FloorObjectFlagList.size(); j++)
		{
			if (FloorFlags[j] == false)
				continue;
			bVisible |= m_FloorObjectFlagList[j].OldHouseWallFlag[i];
			if (bVisible)
				break;
		}
		HouseWallFlag[i] = bVisible;
		MyHouse->m_wall[i].wallModelBase->SetModulBaseActorShow(bVisible);
	}
	for (int i = 0; i < (int)CeilingFlags.size(); i++)
	{//顶
		bool bVisible = false;
		for (int j = 0; j < (int)m_FloorObjectFlagList.size(); j++)
		{
			if (FloorFlags[j] == false)
				continue;
			bVisible |= m_FloorObjectFlagList[j].OldCeilingFlags[i];
			if (bVisible)
				break;
		}
		CeilingFlags[i] = bVisible;
		m_CeilingList[i]->SetModulBaseActorShow(bVisible);
	}
	for (int i = 0; i < (int)MergeWallFaceFlags.size(); i++)
	{//融合面
		bool bVisible = false;
		for (int j = 0; j < (int)m_FloorObjectFlagList.size(); j++)
		{
			if (FloorFlags[j] == false)
				continue;
			bVisible |= m_FloorObjectFlagList[j].OldMergeWallFaceFlags[i];
			if (bVisible)
				break;
		}
		MergeWallFaceFlags[i] = bVisible;
		m_MergeWallfaceList[i]->SetModulBaseActorShow(bVisible);
	}
	for (int i = 0; i < (int)StyleFlag.size(); i++)
	{//模型
		for (int j = 0; j < (int)StyleFlag[i].ObjectFlag.size(); j++)
		{
			bool bVisible = false;
			for (int k = 0; k < (int)m_FloorObjectFlagList.size(); k++)
			{
				if (FloorFlags[k] == false)
					continue;
				bVisible |= m_FloorObjectFlagList[k].OldStyleFlag[i].ObjectFlag[j];
				if (bVisible)
					break;
			}
			StyleFlag[i].ObjectFlag[j] = bVisible;
			if (StyleList[i].pObjectList[j])
			{
				StyleList[i].pObjectList[j]->SetModulBaseActorShow(bVisible);
			}
		}
	}
}

void CScene::ModifyVisibility(const FVector& cameraPos, const FVector& cameraDir)
{
	if (MyWorldContex == nullptr)
		return;
	//恢复墙顶地模型的碰撞和可见性
	for (int i = 0; i < (int)HouseWallFlag.size(); i++)
	{
		if (HouseWallFlag[i])
		{
			MyHouse->m_wall[i].wallModelBase->SetModulBaseActorShow(true);
		}
	}
	for (int i = 0; i < (int)CeilingFlags.size(); i++)
	{
		if (CeilingFlags[i])
		{
			m_CeilingList[i]->SetModulBaseActorShow(true);
		}
	}
	for (int i = 0; i < (int)FloorFlags.size(); i++)
	{
		if (FloorFlags[i])
		{
			m_FloorList[i]->SetModulBaseActorShow(true);
		}
	}
	for (int i = 0; i < (int)MergeWallFaceFlags.size(); i++)
	{
		if (MergeWallFaceFlags[i])
		{
			m_MergeWallfaceList[i]->SetModulBaseActorShow(true);
		}
	}
	for (int i = 0; i < (int)StyleFlag.size(); i++)
	{//模型
		for (int j = 0; j < (int)StyleFlag[i].ObjectFlag.size(); j++)
		{
			if (StyleFlag[i].ObjectFlag[j] && StyleList[i].pObjectList[j])
			{
				StyleList[i].pObjectList[j]->SetModulBaseActorShow(true);
			}
		}
	}
	//判断视点在哪个房间
	int atFloorIndex = -1;  //视点所在的房间
	int atCeilingIndex = -1; //视点在哪个房间的上面
	int atWallIndex = -1;   //视点在墙体的内部，并且墙体属于m_floorVisibleIndexList
	//向下做射线检测，如果得到的是floor，就得到视点在哪个房间
	FVector traceBegin = cameraPos;
	FVector traceDir = FVector(0.0f, 0.0f, -1.0f);
	FVector traceEnd = traceBegin + traceDir * 50000;
	TArray<AActor*> ActorsToIgnore;  //忽略不检测的actor
	UGameplayStatics::GetAllActorsWithTag(MyWorldContex, FName(TEXT("普通模型")), ActorsToIgnore);
	for (int i = 0; i < (int)MergeWallFaceFlags.size(); i++)
	{//融合面也不检测了，只检测墙体就可以了
		if (MergeWallFaceFlags[i])
		{
			ActorsToIgnore.Add(m_MergeWallfaceList[i]);
		}
	}
	FHitResult OutHit;
	bool bTrace = UKismetSystemLibrary::LineTraceSingle(MyWorldContex, traceBegin, traceEnd, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHit, true);
	if (bTrace)
	{
		AModulBaseActor* floor = Cast<AModulBaseActor>(OutHit.Actor.Get());
		if (floor->ActorHasTag(TEXT("地面")))
		{
			for (int i = 0; i < (int)m_FloorList.size(); i++)
			{
				if (m_FloorList[i] == floor)
				{
					atFloorIndex = i;
				}
			}
		}
		if (floor->ActorHasTag(TEXT("吊顶")))
		{
			for (int i = 0; i < (int)m_CeilingList.size(); i++)
			{
				if (m_CeilingList[i] == floor)
				{
					atCeilingIndex = i;
				}
			}
		}
	}
	if (atFloorIndex < 0)
	{//向上做射线检测，如果得到是上墙面，则拥有墙体的房间都可见,因为此时视点在墙体内部
		traceDir = FVector(0.0f, 0.0f, 1.0f);
		traceEnd = traceBegin + traceDir * 50000;
		bTrace = UKismetSystemLibrary::LineTraceSingle(MyWorldContex, traceBegin, traceEnd, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHit, true);
		if (bTrace)
		{
			int wallIndex = -1;
			AModulBaseActor* wall = Cast<AModulBaseActor>(OutHit.Actor.Get());
			if (wall->ActorHasTag(TEXT("墙体"))&& wall->ReturnModelName().Find(TEXT("墙体"))>-1)
			{
				for (int i = 0; i < (int)MyHouse->m_wall.size(); i++)
				{
					if (MyHouse->m_wall[i].wallModelBase == wall)
					{
						wallIndex = i;
						break;
					}
				}
			}
			if (wallIndex > -1)
			{
				std::vector<int>wallBelongRoom;  //墙体属于的房间
				for (int i = 0; i < (int)m_floorVisibleIndexList.Num(); i++)
				{
					if (m_FloorObjectFlagList[m_floorVisibleIndexList[i]].OldHouseWallFlag[wallIndex])
					{
						wallBelongRoom.push_back(m_floorVisibleIndexList[i]);
					}
				}
				if (wallBelongRoom.size() != 0)
				{
					atWallIndex = wallIndex;
				}
			}
		}
	}
	if (m_floorVisibleIndexList.Find(atFloorIndex)>-1)
	{//视点在可见房间的内部，则可见房间的所有东西都可见
		
	}
	else if (atWallIndex > -1 && HouseWallFlag[atWallIndex])
	{//视点在可见房间的某个墙体的内部，则除了这个墙体外，其他的都可见
		MyHouse->m_wall[atWallIndex].wallModelBase->SetModulBaseActorShow(false);
		for (int i = 0; i < (int)MyHouse->m_wall[atWallIndex].mergeList.size(); i++)
		{
			int mergeIndex = MyHouse->m_wall[atWallIndex].mergeList[i];
			if (MergeWallFaceFlags[mergeIndex])
			{
				m_MergeWallfaceList[mergeIndex]->SetModulBaseActorShow(false);
			}
		}
	}
	else if (atCeilingIndex>-1)
	{//视点在某个房间的上面，将所有可见房间的吊顶都隐藏
		for (int i = 0; i < (int)CeilingFlags.size(); i++)
		{
			if (CeilingFlags[i])
			{
				m_CeilingList[i]->SetModulBaseActorShow(false);
			}
		}
	}
	else
	{//视点不在任何房间，计算距离哪个墙体最近，这里不能用射线检测的方式，因为墙上有洞的情况会判断不准确
		FHitResult CameraDirOutHit;
		traceDir = cameraDir;
		traceEnd = traceBegin + traceDir * 50000;
		float MinDis = 1000000;
		int MinIndex = -1;
		for (int i = 0; i < (int)HouseWallFlag.size(); i++)
		{
			if (HouseWallFlag[i] && MyHouse->m_wall[i].middle_vertex.size()==2)
			{
				FVector begin, end, crossPoint;
				float dis, t1, t2;
				begin = MyHouse->m_wall[i].middle_vertex[0];
				end = MyHouse->m_wall[i].middle_vertex[1];
				UCollisionBFL::GetT_CrossPointonTwoLinesOnXOZ(traceBegin, traceEnd, begin, end, t1, t2);
				UCollisionBFL::CrossPointonTwoLinesOnXOZ(traceBegin, traceEnd, begin, end, crossPoint);
				dis = FVector(traceBegin-crossPoint).Size();
				if ((t1 < 0) || (t2 < 0) || (t2 > 1.0))//与视线相交于视点的后面或交点在墙体的外面
					continue;
				if (dis < MinDis)
				{
					MinDis = dis;
					MinIndex = i;
				}
			}
		}
		if (MinIndex < 0)
			return;
		std::vector<bool>wallFlagTemp = HouseWallFlag;
		FVector b1 = MyHouse->m_wall[MinIndex].middle_vertex[0], e1 = MyHouse->m_wall[MinIndex].middle_vertex[1];
		wallFlagTemp[MinIndex] = false;   //视线最近的墙体不可见
		MyHouse->ADJustWallVisible(b1, e1, MyHouse->m_wall[MinIndex].EndlinkInfo, wallFlagTemp);
		MyHouse->ADJustWallVisible(b1, e1, MyHouse->m_wall[MinIndex].BeginLinkInfo, wallFlagTemp);
		FVector wallDir = b1-e1;  //墙体的方向
		wallDir.Normalize();
		float angle = fabs(FVector::DotProduct(wallDir, traceDir));
		if (angle < 0.35)
		{//夹角在75度以上，基本垂直，所以只要该墙不可见即可

		}
		else
		{
			FVector toBegin = traceBegin - MyHouse->m_wall[MinIndex].middle_vertex[0];
			float toBeginDis = toBegin.Size();
			FVector toEnd = traceBegin - MyHouse->m_wall[MinIndex].middle_vertex[1];
			float toEndDis = toEnd.Size();
			if (toBeginDis < toEndDis)
			{//相机在墙体起点一侧
				for (int j = 0; j < (int)MyHouse->m_wall[MinIndex].BeginLinkInfo.size(); j++)
				{
					int linkWallIndex = MyHouse->m_wall[MinIndex].BeginLinkInfo[j].WallIndex;
					if (linkWallIndex > -1)
					{
						wallFlagTemp[linkWallIndex] = false;
						b1 = MyHouse->m_wall[linkWallIndex].middle_vertex[0];
						e1 = MyHouse->m_wall[linkWallIndex].middle_vertex[1];
						MyHouse->ADJustWallVisible(b1, e1, MyHouse->m_wall[linkWallIndex].EndlinkInfo, wallFlagTemp);
						MyHouse->ADJustWallVisible(b1, e1, MyHouse->m_wall[linkWallIndex].BeginLinkInfo, wallFlagTemp);
					}
				}
			}
			else
			{//相机在墙体终点一侧
				for (int j = 0; j < (int)MyHouse->m_wall[MinIndex].EndlinkInfo.size(); j++)
				{
					int linkWallIndex = MyHouse->m_wall[MinIndex].EndlinkInfo[j].WallIndex;
					if (linkWallIndex > -1)
					{
						wallFlagTemp[linkWallIndex] = false;
						b1 = MyHouse->m_wall[linkWallIndex].middle_vertex[0];
						e1 = MyHouse->m_wall[linkWallIndex].middle_vertex[1];
						MyHouse->ADJustWallVisible(b1, e1, MyHouse->m_wall[linkWallIndex].EndlinkInfo, wallFlagTemp);
						MyHouse->ADJustWallVisible(b1, e1, MyHouse->m_wall[linkWallIndex].BeginLinkInfo, wallFlagTemp);
					}
				}
			}
		}
		for (int i = 0; i < (int)HouseWallFlag.size(); i++)
		{
			if (HouseWallFlag[i] && wallFlagTemp[i] == false)
			{//墙体重置了可见性
				MyHouse->m_wall[i].wallModelBase->SetModulBaseActorShow(false);
				for (int j = 0; j < (int)MyHouse->m_wall[i].mergeList.size(); j++)
				{//墙体对应的融合面
					int mergeIndex = MyHouse->m_wall[i].mergeList[j];
					if (MergeWallFaceFlags[mergeIndex])
					{
						m_MergeWallfaceList[mergeIndex]->SetModulBaseActorShow(false);
					}
				}
			}
		}
	}
	for (int i = 0; i < (int)StyleFlag.size(); i++)
	{//模型
		for (int j = 0; j < (int)StyleFlag[i].ObjectFlag.size(); j++)
		{
			if (StyleFlag[i].ObjectFlag[j] && StyleList[i].pObjectList[j])
			{
				FVector actorLocation = StyleList[i].pObjectList[j]->GetActorLocation();  //获得模型位置
				int wallIndex = -1;
				FVector wallPos;
				float dis = MyHouse->GetMinDistanceToPoint(actorLocation, wallIndex, wallPos);  //计算距离模型最近的墙体
				if(dis<30.0f && wallIndex>-1 && wallIndex<HouseWallFlag.size())
				{
					if (HouseWallFlag[wallIndex] && MyHouse->m_wall[wallIndex].wallModelBase->GetModulBaseActorShow()==false)
					{//距离模型最近的墙体（30cm以内）被隐藏，则模型也被隐藏
						StyleList[i].pObjectList[j]->SetModulBaseActorShow(false);
					}
				}
			}
		}
	}
}

//在房中删除物体或灯具，在所有的floor中设置关联标记
void CScene::DelObjectFlagForFloors(OBJECT_STYLE styleindex, AModulBaseActor* delActor)
{
	if ((int)styleindex < 0 || (int)styleindex>3 || delActor == nullptr)
		return;  //检查有效性
	int objectIndex = -1;  //物体序号
	for (int i = 0; i < (int)StyleList[(int)styleindex].pObjectList.size(); i++)
	{
		if (StyleList[(int)styleindex].pObjectList[i] == delActor)
		{
			objectIndex = i;
			break;
		}
	}
	if (objectIndex < 0)
		return ;
	//删除每个房间中待删除模型的可见性
	for (int i = 0; i < (int)m_FloorObjectFlagList.size(); i++)
	{
		m_FloorObjectFlagList[i].OldStyleFlag[(int)styleindex].ObjectFlag.erase(m_FloorObjectFlagList[i].OldStyleFlag[(int)styleindex].ObjectFlag.begin() + objectIndex);
	}
	//删除scene中模型可见性
	StyleFlag[(int)styleindex].ObjectFlag.erase(StyleFlag[(int)styleindex].ObjectFlag.begin() + objectIndex);
	//删除scene中模型
	StyleList[(int)styleindex].pObjectList[objectIndex] = nullptr;
	StyleList[(int)styleindex].pObjectList.erase(StyleList[(int)styleindex].pObjectList.begin() + objectIndex);
	return;
}

//点在哪个房间
int CScene::InWhichRoom(FVector pos)
{
	//获得物体属于的房间，分别向上和向下做射线检测
	FVector traceBegin = pos;
	FVector traceDir = FVector(0.0f, 0.0f, -1.0f);
	FVector traceEnd = traceBegin + traceDir * 50000;
	TArray<AActor*> ActorsToIgnore;  //忽略不检测的actor
	UGameplayStatics::GetAllActorsWithTag(MyWorldContex, FName(TEXT("普通模型")), ActorsToIgnore);
	for (int i = 0; i < (int)MergeWallFaceFlags.size(); i++)
	{//融合面也不检测了，只检测墙体就可以了
		if (MergeWallFaceFlags[i])
		{
			ActorsToIgnore.Add(m_MergeWallfaceList[i]);
		}
	}
	for (int i = 0; i < (int)m_CeilingList.size(); i++)
	{//吊顶不需要检测
		if (m_CeilingList[i])
		{
			ActorsToIgnore.Add(m_CeilingList[i]);
		}
	}
	int floorIndex = -1;  //模型属于的房间
	TArray<FHitResult> OutHits;
	bool bTrace = UKismetSystemLibrary::LineTraceMulti(MyWorldContex, traceBegin, traceEnd, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true);
	if (bTrace)
	{
		for (auto hit : OutHits)
		{
			AModulBaseActor* hitActor = Cast<AModulBaseActor>(hit.Actor.Get());
			if (hitActor && hitActor->ActorHasTag(TEXT("地面")))
			{
				for (int i = 0; i < (int)m_FloorList.size(); i++)
				{
					if (m_FloorList[i] == hitActor)
					{
						return i;
					}
				}
			}
		}
	}
	//向下做射线检测没有碰撞到地面，再向上做射线检测
	traceDir = FVector(0.0f, 0.0f, 1.0f);
	traceEnd = traceBegin + traceDir * 50000;
	bTrace = UKismetSystemLibrary::LineTraceMulti(MyWorldContex, traceBegin, traceEnd, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHits, true);
	if (bTrace)
	{
		int wallIndex = -1;
		for (auto& hit : OutHits)
		{
			if (wallIndex > -1)
				break;
			AModulBaseActor* wall = Cast<AModulBaseActor>(hit.Actor.Get());
			if (wall && wall->ActorHasTag(TEXT("墙体")) && wall->ReturnModelName().Find(TEXT("墙体")) > -1)
			{
				for (int i = 0; i < (int)MyHouse->m_wall.size(); i++)
				{
					if (MyHouse->m_wall[i].wallModelBase == wall)
					{
						wallIndex = i;
						break;
					}
				}
			}
		}
		if (wallIndex > -1)
		{
			std::vector<int>wallBelongRoom;  //墙体属于的房间
			for (int i = 0; i < (int)m_floorVisibleIndexList.Num(); i++)
			{
				if (m_FloorObjectFlagList[m_floorVisibleIndexList[i]].OldHouseWallFlag[wallIndex])
				{
					wallBelongRoom.push_back(m_floorVisibleIndexList[i]);
				}
			}
			if (wallBelongRoom.size() != 0)
			{
				return wallBelongRoom[0];
			}
		}
	}
	return -1;
}

//在房中放了物体或灯具，在所有的floor中设置关联标记
void CScene::AddObjectFlagForFloors(OBJECT_STYLE styleindex, AModulBaseActor* newSpawnActor, FVector pos)
{
	if ((int)styleindex < 0 || (int)styleindex>3 || newSpawnActor == nullptr)
		return;  //检查有效性
	int roomIndex = InWhichRoom(pos); //获得模型属于的房间
	//设置每个房间中关于模型的可见性
	for (int i = 0; i < (int)m_FloorObjectFlagList.size(); i++)
	{
		if (i == roomIndex)
		{//模型放到了某个房间的内部
			m_FloorObjectFlagList[i].OldStyleFlag[(int)styleindex].ObjectFlag.push_back(true);
		}
		else
		{
			m_FloorObjectFlagList[i].OldStyleFlag[(int)styleindex].ObjectFlag.push_back(false);
		}
	}
	//设置scene中关于这个模型的可见性
	if (roomIndex>-1 && FloorFlags[roomIndex])
	{//模型所在的房间可见
		StyleFlag[(int)styleindex].ObjectFlag.push_back(true);
	}
	else
	{//模型所在的房间不可见
		StyleFlag[(int)styleindex].ObjectFlag.push_back(false);
	}
	//添加到模型序列中
	StyleList[(int)styleindex].pObjectList.push_back(newSpawnActor);
}

//在房中移动物体或灯具，在所有的floor中设置关联标记 jiang10.18
void CScene::ModifyObjectFlagForFloors(OBJECT_STYLE styleindex, AModulBaseActor* movedActor, FVector pos)
{
	if ((int)styleindex < 0 || (int)styleindex>3 || movedActor == nullptr)
		return;  //检查有效性
	int objectIndex = -1;  //物体序号
	for (int i = 0; i < (int)StyleList[(int)styleindex].pObjectList.size(); i++)
	{
		if (StyleList[(int)styleindex].pObjectList[i] == movedActor)
		{
			objectIndex = i;
			break;
		}
	}
	if (objectIndex < 0)
		return;
	int roomIndex = InWhichRoom(movedActor->GetActorLocation());  //模型属于的房间
	//修正每个房间中正在移动模型的可见性
	for (int i = 0; i < (int)m_FloorObjectFlagList.size(); i++)
	{
		if (roomIndex == i)
		{
			m_FloorObjectFlagList[i].OldStyleFlag[(int)styleindex].ObjectFlag[objectIndex] = true;
		}
		else
		{
			m_FloorObjectFlagList[i].OldStyleFlag[(int)styleindex].ObjectFlag[objectIndex] = false;
		}
	}
	//修改scene中模型可见性
	if (roomIndex > -1 && FloorFlags[roomIndex])
	{//模型移动到的房间是可见的
		StyleFlag[(int)styleindex].ObjectFlag[objectIndex] = true;
	}
	else
	{
		StyleFlag[(int)styleindex].ObjectFlag[objectIndex] = false;
	}
	return;
}

//判断在哪个群组中
AHouseGroupActor* CScene::GetInWhichGroup(AActor* model)
{
	AHouseGroupActor* retGroup = nullptr;
	if (model)
	{
		AActor* parentGroup = model->GetAttachParentActor();//第一级
		if (parentGroup)
		{
			retGroup = Cast<AHouseGroupActor>(parentGroup);
			if (retGroup)
			{
				AHouseGroupActor* parentUp = retGroup;
				while (1)
				{
					AActor* temp = parentUp->GetAttachParentActor();
					parentUp = Cast<AHouseGroupActor>(temp);
					if (parentUp)
					{//还有上一级
						retGroup = parentUp;
					}
					else
					{//不存在上一级
						break;
					}
				}
			}
		}
	}
	return retGroup;
}

//删除群组
void CScene::DeleteGroup(AHouseGroupActor* delGroup)
{
	if (!delGroup)
		return;
	//获得群组中的所有模型和子群组
	TArray<FString>unitList;
	delGroup->GetAllUnit(unitList);
	TArray<AModulBaseActor*>delModelbaseList;  //待删除的群组中的单个模型
	TArray<AHouseGroupActor*>delGroupList;     //待删除的群组中的子群组
	for (auto& unit : unitList)
	{
		TArray<AActor*> modulActors;
		UGameplayStatics::GetAllActorsWithTag(MyWorldContex, FName(*unit), modulActors);
		for (auto& modul : modulActors)
		{
			AModulBaseActor* subModel = Cast<AModulBaseActor>(modul);
			if (subModel)
			{//群组中的单个模型
				delModelbaseList.AddUnique(subModel);
			}
			else
			{
				AHouseGroupActor* subGroup = Cast<AHouseGroupActor>(modul);
				if (subGroup)
				{//群组中的子群组
					delGroupList.AddUnique(subGroup);
				}
			}
		}
	}
	//删除子单个模型
	for (auto& delModel : delModelbaseList)
	{
		DelObjectFlagForFloors(delModel->ReturnModelType(), delModel);  //修正可见性
		delModel->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);  //从群组中移除
		delModel->Destroy();
	}
	//删除子群组
	for (auto& singleGroup : delGroupList)
	{
		for (int j = m_GroupList.size() - 1; j >= 0; j--)
		{
			if (m_GroupList[j] == singleGroup)
			{
				m_GroupList.erase(m_GroupList.begin() + j);
				singleGroup->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);  //从群组中移除
				singleGroup->Destroy();
				break;
			}
		}
	}
	//删除群组本身
	for (int j = m_GroupList.size() - 1; j >= 0; j--)
	{
		if (m_GroupList[j] == delGroup)
		{
			m_GroupList.erase(m_GroupList.begin() + j);
			delGroup->Destroy();
			break;
		}
	}
	return;
}

//初始化场景灯光
//场景中的灯光一部分是directLight  skyLight、后期处理，这三个是固定值，不需要修改
//另一部分是窗户和移门上的rectLight，放置在通往户外的窗户上，或者通往客餐厅的移门或者窗户上；
void CScene::InitSceneLight()
{
	if (StyleList.size() < 4 || (StyleList[1].pObjectList.size() + StyleList[2].pObjectList.size() == 0))
		return;  //没有门窗，则不放置rectLight
	float minXHouse = 10000000.0f, minYHouse = 10000000.0f, maxXHouse = -10000000.0f, maxYHouse = -10000000.0f;
	for (int floorIndex = 0; floorIndex < (int)m_FloorList.size(); floorIndex++)
	{
		AModulBaseActor* floor = m_FloorList[floorIndex];
		FBox boundLocal = floor->CalculateComponentsBoundingBoxInLocalSpace(true);
		FVector boundMin = UKismetMathLibrary::TransformLocation(floor->GetActorTransform(), boundLocal.Min);
		FVector boundMax = UKismetMathLibrary::TransformLocation(floor->GetActorTransform(), boundLocal.Max); //转化为世界坐标
		minXHouse = boundMin.X < minXHouse ? boundMin.X : minXHouse;
		minYHouse = boundMin.Y < minYHouse ? boundMin.Y : minYHouse;
		maxXHouse = boundMax.X > maxXHouse ? boundMax.X : maxXHouse;
		maxYHouse = boundMax.Y > maxYHouse ? boundMax.Y : maxYHouse;
	}
	float houseSize = fabs(maxXHouse - minXHouse) > fabs(maxYHouse - minYHouse) ? fabs(maxXHouse - minXHouse) : fabs(maxYHouse - minYHouse);  //整个户型的最大尺寸
	//遍历场景中所有的门窗
	for (int i = 0; i < (int)StyleList.size(); i++)
	{
		if (i != 1 && i != 2)
			continue;   //只在门窗的位置放置面光
		for (int j = 0; j < (int)StyleList[i].pObjectList.size(); j++)
		{
			AModulBaseActor* pObj = StyleList[i].pObjectList[j];
			FVector faceNormal = UKismetMathLibrary::GetRightVector(pObj->GetActorRotation());  //窗户或者门的朝向
			FVector reverseFaceNormal = -1 * faceNormal;   //门窗朝向的反方向
			FBox box = pObj->CalculateComponentsBoundingBoxInLocalSpace(true, false);
			FVector boundMin = UKismetMathLibrary::TransformLocation(pObj->GetActorTransform(), box.Min);
			FVector boundMax = UKismetMathLibrary::TransformLocation(pObj->GetActorTransform(), box.Max); //门窗的包围盒
			FVector boundMid = (boundMin + boundMax) * 0.5f;   //门窗的中心点
			float height = fabs(boundMax.Z - boundMin.Z);//门窗的高度
			float a = fabs(boundMax.X - boundMin.X);
			float b = fabs(boundMax.Y - boundMin.Y);
			float width = a > b ? a : b;  //门窗的宽度
			float thick = a < b ? a : b; //门窗的厚度
			FVector extendPt1 = boundMid + faceNormal * (thick + 5);  //从门窗中点分别延伸得到两个点
			FVector extendPt2 = boundMid + reverseFaceNormal * (thick + 5);
			int extendRoom1 = InWhichRoom(extendPt1); //延伸点所在的房间
			int extendRoom2 = InWhichRoom(extendPt2);
			FVector rectLightNormal = FVector::ZeroVector;   //rectlight的朝向
			bool bPutLight = false;  //是否放置灯光
			if (extendRoom1<0 && extendRoom2>-1)
			{
				if (i == 2)
				{//通往户外的窗户
					rectLightNormal = reverseFaceNormal;
					bPutLight = true;
				}
			}
			else if(extendRoom1>-1 && extendRoom2<0)
			{
				if (i == 2)
				{//通往户外的窗户
					rectLightNormal = faceNormal;
					bPutLight = true;
				}
			}
			else if (extendRoom1>-1 && extendRoom2>-1)
			{//两个房间之前的门窗
				ROOM extendRoomStyle1 = m_FloorObjectFlagList[extendRoom1].m_roomStyle;
				ROOM extendRoomStyle2 = m_FloorObjectFlagList[extendRoom2].m_roomStyle;
				if (extendRoomStyle1 == ROOM::RM_BALCONY && extendRoomStyle2 != ROOM::RM_BALCONY)
				{//通往阳台的门窗
					rectLightNormal = reverseFaceNormal;
					bPutLight = true;
				}
				if (extendRoomStyle2 == ROOM::RM_BALCONY && extendRoomStyle1 != ROOM::RM_BALCONY)
				{//通往阳台的门窗
					rectLightNormal = faceNormal;
					bPutLight = true;
				}
			}
			if (bPutLight)
			{//放置rectlight
				FTransform trans;
				ARectLight* rectLightTemp = MyWorldContex->SpawnActor<ARectLight>(ARectLight::StaticClass(), trans);
				if (rectLightTemp)
				{
					rectLightTemp->SetMobility(EComponentMobility::Movable);  //设置为可移动光源
					FVector oldAxis = FVector::XAxisVector;  //引擎中的rectLight默认是朝向x轴发光的
					FVector impactNormal = rectLightNormal;
					FVector rotAxis = FVector::CrossProduct(oldAxis, impactNormal);
					float rotAngle = UKismetMathLibrary::DegAcos(FVector::DotProduct(oldAxis, impactNormal));
					FRotator deltaRot = UKismetMathLibrary::RotatorFromAxisAndAngle(rotAxis, rotAngle);
					rectLightTemp->AddActorWorldRotation(deltaRot); //旋转 
					rectLightTemp->SetActorLocation(boundMid);      //位置
					rectLightTemp->SetCastShadows(true);            //投射阴影
					rectLightTemp->RectLightComponent->SetIntensityUnits(ELightUnits::Candelas); //亮度单位
					if(i == 1)
						rectLightTemp->RectLightComponent->SetIntensity(3);  //亮度
					else 
						rectLightTemp->RectLightComponent->SetIntensity(8);
					rectLightTemp->RectLightComponent->SetSourceWidth(width);  //宽度
					rectLightTemp->RectLightComponent->SetSourceHeight(height);//高度
					rectLightTemp->RectLightComponent->SetBarnDoorLength(0);   //挡板
					rectLightTemp->RectLightComponent->SetAttenuationRadius(houseSize); //衰减半径
				}
			}
		}
	}
}
