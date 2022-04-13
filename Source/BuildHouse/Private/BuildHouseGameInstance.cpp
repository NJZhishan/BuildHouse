// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildHouseGameInstance.h"
#include "../Public/BuildHousePlayerController.h"
#include "../Public/ModulBaseActor.h"
#include "flyFile_w.h"
#include "../Public/BuildHouseCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

void UBuildHouseGameInstance::Init()
{
	m_sceneTextureManager = NewObject<UTextureManager>(this, TEXT("SceneTextureManager"));  //创建纹理
	SetSelectModulBaseActor(nullptr);   //默认没有模型被选中
	m_DownLoadFileTool = nullptr;       //下载文件的工具
	m_DownLoadFileTool = NewObject<UDownloadFile>(this, TEXT("DownloadFileTool"));       //用于下载文件
	SetWallAdsorb(false);     //默认墙体吸附
	SetObjectAdsorb(false);  //默认物体不吸附
	m_pScene = new CScene(); //当前的场景
}

//添加贴图，贴图来自本地文件夹中
//参数是绝对路径
void UBuildHouseGameInstance::AddPic(FString picName)
{
	if (m_sceneTextureManager)
	{
		m_sceneTextureManager->AddPic(picName);
	}
	return;
}

//创建纹理，纹理来自压缩文件中
////参数是纹理的名字，比如 奥米茄tex.bmp
bool UBuildHouseGameInstance::AddZipPic(const FString& texURL, void* picture)
{
	if (m_sceneTextureManager)
	{
		return m_sceneTextureManager->AddZipPic(texURL,picture);
	}
	return false;
}

//查找贴图
UTexture* UBuildHouseGameInstance::FindPic(FString picName)
{
	if (m_sceneTextureManager)
	{
		return m_sceneTextureManager->FindPic(picName);
	}
	return nullptr;
}

//设置被选中的模型
void UBuildHouseGameInstance::SetSelectModulBaseActor(AModulBaseActor* actor)
{
	m_SelectModulBase = actor;
	if(m_SelectModulBase)
		m_SelectModulBase->CalculateActorBound();  //计算包围盒
	return;
}

//返回被选中的模型
AModulBaseActor* UBuildHouseGameInstance::ReturnSelectModulBaseActor()
{
	return m_SelectModulBase;
}

//设置被选中的群组
void UBuildHouseGameInstance::SetSelectGroupActor(AHouseGroupActor* group)
{
	m_SelectGroup = group;
	return;
}

//返回被选中的群组
AHouseGroupActor* UBuildHouseGameInstance::ReturnSelectGroupActor()
{
	return m_SelectGroup;
}

//解析拖拽信息
bool UBuildHouseGameInstance::ParseDragInfor()
{
	if (m_sDragInfor.IsEmpty())
		return false;
	FString newUrl = m_sDragInfor;
	int nChar = newUrl.Find(TEXT("$["));
	if (nChar > 0)
	{
		newUrl = newUrl.Left(newUrl.Len() - 1);
		if (newUrl.FindLastChar('$', nChar))
		{
			newUrl = newUrl.Right(newUrl.Len() - nChar - 1);
			if (newUrl.FindChar(']', nChar))
			{
				newUrl = newUrl.Left(nChar);
				newUrl = newUrl.Right(newUrl.Len() - 1 - newUrl.Find(TEXT("[")));  //去掉两个中括号
				newUrl = TEXT("*#@") + newUrl;
				FDownloadInforStruct dInfo;
				dInfo.info.SetNum(30);
				int i = 0;
				while (newUrl.FindLastChar('*', nChar))
				{
					while (nChar >= 0)
					{
						if ((nChar = newUrl.Find(TEXT("*#@"),ESearchCase::IgnoreCase,ESearchDir::FromEnd)) >= 0)
						{
							FString szContent = newUrl.Right(newUrl.Len() - nChar - 3);
							if (i > 29)
							{
								newUrl = _T("");
								newUrl = _T("");
								break;
							}
							int jump = 0;
							if (i > 18)
							{
								jump = 2;
							}
							dInfo.info[i + jump] = szContent;
							switch (i)
							{
							case 9:
								dInfo.url = szContent;
								break;
							case 10:
								dInfo.type = szContent;
								break;
							case 18:
								dInfo.info[20] = szContent;
								break;
							case 15:
							{
								if ("1" == dInfo.type)
									dInfo.info[20] = szContent;
							}
							break;
							case 17:
							{
								if (szContent == "**")
									dInfo.info[17] = TEXT("254*254*254");
							}
							break;
							}
							newUrl = newUrl.Left(nChar);
							i++;
							break;
						}
						else
						{
							nChar--;
						}
					}
				}
				if ("4" == dInfo.type || "" == dInfo.type)
				{
				}
				else
				{
					if ("" != dInfo.info[21] && "0" != dInfo.info[21] && dInfo.info[3] != dInfo.info[21])
					{
						FString temp = dInfo.info[21];
						dInfo.info[21] = dInfo.info[3];
						dInfo.info[3] = temp;
					}
					else
					{
						dInfo.info[21] = dInfo.info[3];
					}
				}
				m_CurProdInfor = dInfo;
			}
		}
	}
	return true;
}

//下载完成
//参数：txtPath是绝对路径，比如d:/model_zy/5667.txt
void UBuildHouseGameInstance::OnDownloadEnd(FString txtPath)
{
	FString txtName;  //txt名字，比如5667.txt
	int index;
	DownloadType style;
	if (txtPath.FindLastChar('/', index))
	{
		txtName = txtPath.Right(txtPath.Len() - 1 - index);
	}
	//下载素材的类型
	for (int i = m_DownLoadList.Num() - 1; i >= 0; i--)
	{
		if (txtName == m_DownLoadList[i].url)
		{
			style = m_DownLoadList[i].style;
			break;
		}
	}
	//从下载序列中移除
	for (int i = m_DownLoadList.Num() - 1; i >= 0; i--)
	{
		if (txtName == m_DownLoadList[i].url)
		{
			m_DownLoadList.RemoveAt(i);
		}
	}
	if (style == DownloadType::DT_MODEL)
	{//模型替换	
		TArray<AActor*> modelList;  //模型列表，不要使用TObjectIterator，因为TObjectIterator遍历的是内存中所有的actor，但是actor调用destroy后不一定会马上从内存中移除
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AModulBaseActor::StaticClass(), modelList);
		for (auto & singleModel : modelList)
		{
			AModulBaseActor* modelActor = Cast<AModulBaseActor>(singleModel);
			if (modelActor && (txtPath.Find(modelActor->m_txtPath))>-1)
			{
				modelActor->CreateNewObject(txtPath);
			}
		}
	}
	else if (style == DownloadType::DT_PIC)
	{//材质替换
		TArray<AActor*> modelList;  //模型列表，不要使用TObjectIterator，因为TObjectIterator遍历的是内存中所有的actor，但是actor调用destroy后不一定会马上从内存中移除
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AModulBaseActor::StaticClass(), modelList);
		for (auto& singleModel : modelList)
		{
			AModulBaseActor* modelActor = Cast<AModulBaseActor>(singleModel);
			if(modelActor)
				modelActor->DownloadTextureFinished(txtPath);
		}
	}
	else if (style == DownloadType::DT_GROUP)
	{//组合替换
		TArray<AActor*> groupList;  //群组，不要使用TObjectIterator，因为TObjectIterator遍历的是内存中所有的actor，但是actor调用destroy后不一定会马上从内存中移除
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHouseGroupActor::StaticClass(), groupList);
		for (auto& singleGroup : groupList)
		{
			AHouseGroupActor* groupActor = Cast<AHouseGroupActor>(singleGroup);
			if (groupActor && (txtPath.Find(groupActor->m_txtPath)) > -1)
			{
				groupActor->ReadGroup(txtPath);
				//设置群组的位置
				groupActor->MoveGroup(groupActor->m_groupPosition - groupActor->GetActorLocation());
				//设置群组的轴向
				FVector oldAxis = groupActor->m_RotateAxis;
				FVector impactNormal = groupActor->m_groupNormal;
				FVector rotAxis = FVector::CrossProduct(oldAxis, impactNormal);
				float rotAngle = UKismetMathLibrary::DegAcos(FVector::DotProduct(oldAxis, impactNormal));
				FRotator deltaRot = UKismetMathLibrary::RotatorFromAxisAndAngle(rotAxis, rotAngle);
				groupActor->AddActorWorldRotation(deltaRot); //旋转 
				groupActor->m_RotateAxis = impactNormal;  //更新旋转轴
			}
		}
	}
	return;
}

//准备下载
//参数：txt文件的绝对路径，比如d:/model_zy/5667.txt
void UBuildHouseGameInstance::OnDownloadBegin(FString txtPath, DownloadType style)
{
	if (m_DownLoadFileTool)
	{
		FString downloadURL(TEXT("http://sucaiku.yjia88.com:8888/upload/products_zip//"));
		int npos = -1;
		if (txtPath.FindLastChar('/', npos))
		{
			FString tetName = txtPath.Right(txtPath.Len() - npos - 1);
			//查询是否已经在下载文件
			for (int i = 0; i < m_DownLoadList.Num(); i++)
			{
				if (m_DownLoadList[i].url == tetName)
				{//已经在下载这个文件
					return;
				}
			}
			//插入下载序列
			FReCreateStruct recreat;
			recreat.url = tetName;
			recreat.style = style;
			m_DownLoadList.Add(recreat);
			//开始下载
			m_DownLoadFileTool->m_DownloadList.Add(downloadURL + tetName);
			if (m_DownLoadFileTool->m_DownloadList.Num() == 1)
			{
				m_DownLoadFileTool->DownloadFile(m_DownLoadFileTool->m_DownloadList[0]);  //下载模型
			}
		}
	}
	return;
}

//替换贴图
void UBuildHouseGameInstance::HandleReplaceTexture(FString texURL, FString matName, FVector specularColor)
{
	//判断文件是否在本地存在
	bool bExit = IsTheModelInLocal(texURL);
	//替换贴图
	m_SelectModulBase->ReplaceTexture(m_SelectModulBase->ReturnSelComponent(),-1,texURL, matName, specularColor);
	if (!bExit)
	{//文件在本地不存在
		OnDownloadBegin(texURL, DownloadType::DT_PIC);
	}
}

//放置模型
AModulBaseActor* UBuildHouseGameInstance::HandlePutModel(FString texURL, FDownloadInforStruct prodInfor,FVector location, FVector scale, FRotator rotate , int style/* = -1*/)
{
	//判断文件是否在本地存在
	bool bExit = IsTheModelInLocal(texURL);
	//创建模型
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
		modelbaseTemp->SetProdInfor(prodInfor);  //设置产品信息
		modelbaseTemp->SetModelName(prodInfor.info[11],style);  //模型名字
#if WITH_EDITOR
		modelbaseTemp->SetActorLabel(*(modelbaseTemp->ReturnModelName()));
#endif 
		if (bExit)
		{//模型文件在本地存在
			//指定模型的geometry和texture信息
			modelbaseTemp->CreateNewObject(texURL);
		}
		//设置模型的位置、缩放、旋转信息
		modelbaseTemp->SetActorLocation(location);  //位置
		modelbaseTemp->SetActorScale3D(scale);      //缩放
		modelbaseTemp->SetActorRotation(rotate);    //旋转
		//添加场景可见性
		m_pScene->AddObjectFlagForFloors(modelbaseTemp->ReturnModelType(), modelbaseTemp, modelbaseTemp->GetActorLocation());
	}
	if (!bExit)
	{//文件在本地不存在，则下载
		OnDownloadBegin(texURL, DownloadType::DT_MODEL);
	}
	return modelbaseTemp;
}

//放置组合
void UBuildHouseGameInstance::HandlePutGroup(FString texURL, FDownloadInforStruct prodInfor, FVector location, FVector scale, FVector normal)
{
	//判断文件是否在本地存在
	bool bExit = IsTheModelInLocal(texURL);
	//创建组合
	FTransform trans;
	FActorSpawnParameters aa;
	TSubclassOf<AActor>groupInstance = LoadClass<AActor>(NULL, TEXT("Blueprint'/Game/Blueprint/BP_HouseGroup.BP_HouseGroup_C'"));
	AHouseGroupActor* groupTemp = Cast<AHouseGroupActor>(GetWorld()->SpawnActor(groupInstance, &trans, aa));
	//设置群组的位置、缩放、旋转信息
	groupTemp->SetActorLocation(location);  //位置
	groupTemp->SetActorScale3D(scale);      //缩放
	if (groupTemp)
	{
		//创建群组
		FString tempName = texURL;
		int index = -1;
		if (texURL.FindLastChar('/', index))
		{
			tempName = tempName.Right(tempName.Len() - 1 - index);
		}
		groupTemp->m_txtPath = tempName;  //txt文件的名字
		groupTemp->SetProdInfor(prodInfor);  //设置产品信息
		groupTemp->SetGroupName(prodInfor.info[11]);  //组合名字
#if WITH_EDITOR
		groupTemp->SetActorLabel(*(groupTemp->ReturnGroupName()));
#endif 
		if (bExit)
		{//组合文件在本地存在
			//指定模型的geometry和texture信息
			groupTemp->ReadGroup(texURL);
		}
		//保存到场景中
		m_pScene->m_GroupList.push_back(groupTemp);
		//设置群组的位置
		groupTemp->MoveGroup(location - groupTemp->GetActorLocation());
		//群组在本地不存在，临时记录群组的位置，保证群组下载完成后群组的位置是正确的
		if (!bExit)
		{
			groupTemp->m_groupPosition = location;
			groupTemp->m_groupNormal = normal;
		}
		else
		{
			//根据鼠标位置设置群组的轴向
			FVector oldAxis = groupTemp->m_RotateAxis;
			FVector impactNormal = normal;
			FVector rotAxis = FVector::CrossProduct(oldAxis, impactNormal);
			float rotAngle = UKismetMathLibrary::DegAcos(FVector::DotProduct(oldAxis, impactNormal));
			FRotator deltaRot = UKismetMathLibrary::RotatorFromAxisAndAngle(rotAxis, rotAngle);
			groupTemp->AddActorWorldRotation(deltaRot); //旋转 
			groupTemp->m_RotateAxis = impactNormal;  //更新旋转轴
		}
	}
	if (!bExit)
	{//文件在本地不存在，则下载
		OnDownloadBegin(texURL, DownloadType::DT_GROUP);
	}
	return;
}

//处理拖拽完成
void UBuildHouseGameInstance::HandleDragOver()
{
	//射线检测拾取模型
	ABuildHousePlayerController* playerController = ABuildHousePlayerController::GetPlayerController();
	if (playerController)
	{
		playerController->LeftMouseBtnUp();
	}
	else
		return;
	//解析拖拽信息
	bool bParseSuc = ParseDragInfor(); 
	if (!bParseSuc)
		return;
	if (m_CurProdInfor.info.Num() == 0)
		return;
	//根据拖拽信息决定下面要做什么
	if (m_CurProdInfor.info[14] == TEXT("材质贴图") && ReturnSelectModulBaseActor())
	{//拖的是材质，必须选择了某个actor
		//解析specularColor
		FString specularStr = m_CurProdInfor.info[17];
		FVector specularColor = FVector::OneVector;
		int npos = -1;
		if (specularStr.FindChar('*', npos))
		{
			FString rStr = specularStr.Left(npos);
			specularColor.X = FCString::Atof(*rStr)/255.0f;
			specularStr = specularStr.Right(specularStr.Len() - 1 - npos);
			if (specularStr.FindChar('*', npos))
			{
				FString gStr = specularStr.Left(npos);
				specularColor.Y = FCString::Atof(*gStr) / 255.0f;
				specularStr = specularStr.Right(specularStr.Len() - 1 - npos);
				specularColor.Z = FCString::Atof(*specularStr) / 255.0f;
			}
		}
		HandleReplaceTexture(m_CurProdInfor.url, m_CurProdInfor.info[15], specularColor);  //替换贴图
	}
	else if (m_CurProdInfor.info[14] == TEXT("装饰品")
		|| m_CurProdInfor.info[14] == TEXT("家具")
		|| m_CurProdInfor.info[14] == TEXT("开关面板")
		|| m_CurProdInfor.info[14] == TEXT("电器")
		|| m_CurProdInfor.info[14] == TEXT("五金构件")
		|| m_CurProdInfor.info[14] == TEXT("装饰构件")
		|| m_CurProdInfor.info[14] == TEXT("布艺")
		|| m_CurProdInfor.info[14] == TEXT("卫生间")
		|| m_CurProdInfor.info[14] == TEXT("厨房")
		|| m_CurProdInfor.info[14] == TEXT("灯具"))
	{//拖的是模型
		FHitResult hitReult;
		playerController->ReturnHitResult(hitReult);
		FVector location =  hitReult.ImpactPoint;  //位置
		FVector scale = FVector::OneVector;        //缩放，因为在导入模型的时候已经将单位从m转化为了cm，所以这里不需要缩放
		FVector modelAxis = FVector(0.0f, 0.0f, 1.0f);  //在max中建模时模型的轴向，max中朝着z正方向的（x向右，z向上，y向里），在ue中相当于朝着z正方向，因为ue4中坐标系是y向右，x向里，z向上
		FVector rightVec = FVector::CrossProduct(hitReult.ImpactNormal, modelAxis);
		FVector upVec = hitReult.ImpactNormal;
		if (rightVec.IsNearlyZero())
		{//模型放到了地上或者吊顶上
			float angle = FVector::DotProduct(hitReult.ImpactNormal, modelAxis);
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
		}
		FMatrix rotationMatrix = FRotationMatrix::MakeFromYZ(rightVec, upVec);
		FRotator rotate = rotationMatrix.Rotator();  //旋转
		HandlePutModel(m_CurProdInfor.url, m_CurProdInfor,location,scale,rotate);
	}
	else if (m_CurProdInfor.info[14] == TEXT("组合"))
	{//拖的是组合
		FHitResult hitReult;
		playerController->ReturnHitResult(hitReult);
		FVector location = hitReult.ImpactPoint;  //位置
		HandlePutGroup(m_CurProdInfor.url, m_CurProdInfor, location, FVector::OneVector, hitReult.ImpactNormal);
	}
	//放置完成后清空
	m_CurProdInfor.info.Empty();
}

//只处理了两种情况，一种参数只有id，比如5667；一种是绝对路径，比如d:/model_zy/5667.txt
bool UBuildHouseGameInstance::IsTheModelInLocal(FString& fileName, bool changeExt)
{
	if (changeExt)
	{
		int pos = -1;
		FString projectDir = FPaths::ProjectDir();  //项目路径
		if(fileName.Find(projectDir) == -1)
		{//相对路径
			fileName = projectDir + TEXT("model_zy/") + fileName;
			if (fileName.Find(TEXT(".txt")) == -1)
			{
				fileName = fileName + TEXT(".txt");
			}
		}
		else
		{//绝对路径
			if (fileName.Find(TEXT(".txt")) == -1)
				return false;
		}
	}
	if (FPaths::FileExists(fileName))
	{
		flyFile_w flyFile;
		if (!flyFile.SetZipList(TCHAR_TO_ANSI(*fileName)))
		{//无法解开压缩文件
			IFileManager::Get().Delete(*fileName);//删除该无效模型
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

//移动被选中的模型或者群组
void UBuildHouseGameInstance::MoveSelThing()
{
	AHouseGroupActor* groupTemp = ReturnSelectGroupActor();
	if (groupTemp)
	{//移动群组
		ABuildHouseCharacter* charator = Cast<ABuildHouseCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
		if (charator)
		{
			charator->SetOperationType(OperationType::OT_MoveGroup);
		}
		return;
	}
	AModulBaseActor* modulbase = ReturnSelectModulBaseActor();
	if (modulbase)
	{//移动单个模型
		modulbase->MoveModelBase();
		return;
	}
	return;
}