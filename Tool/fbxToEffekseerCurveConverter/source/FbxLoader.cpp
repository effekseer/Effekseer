#include "FbxLoader.h"

void FbxLoader::LoadNodeInfo(FbxNode* _pNode, FbxNodeInfo* _pNodeInfo)
{
	// Load Name
	_pNodeInfo->mName = _pNode->GetName();

	// Load Attribute
	LoadAttribute(_pNode, _pNodeInfo);

	// Load Child
	_pNodeInfo->mChildCount = _pNode->GetChildCount();

	for (int i = 0; i < _pNodeInfo->mChildCount; i++)
	{
		FbxNodeInfo* lNode = new FbxNodeInfo();
		_pNodeInfo->mChilds.push_back(lNode);

		LoadNodeInfo(_pNode->GetChild(i), lNode);
	}
}

void FbxLoader::LoadAttribute(FbxNode* _pNode, FbxNodeInfo* _pNodeInfo)
{
	if (_pNode->GetNodeAttribute())
	{
		FbxNodeAttribute* lAttribute = _pNode->GetNodeAttribute();
		_pNodeInfo->mAttributeID = lAttribute->GetUniqueID();

		switch (lAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eUnknown:
			_pNodeInfo->mAttributeType = "eUnknown";
			break;
		case FbxNodeAttribute::eNull:
			_pNodeInfo->mAttributeType = "eNull";
			break;
		case FbxNodeAttribute::eMarker:
			_pNodeInfo->mAttributeType = "eMarker";
			break;
		case FbxNodeAttribute::eSkeleton:
			_pNodeInfo->mAttributeType = "eSkeleton";
			break;
		case FbxNodeAttribute::eMesh:
			_pNodeInfo->mAttributeType = "eMesh";
			break;
		case FbxNodeAttribute::eNurbs:
			_pNodeInfo->mAttributeType = "eNurbs";
			break;
		case FbxNodeAttribute::ePatch:
			_pNodeInfo->mAttributeType = "ePatch";
			break;
		case FbxNodeAttribute::eCamera:
			_pNodeInfo->mAttributeType = "eCamera";
			break;
		case FbxNodeAttribute::eCameraStereo:
			_pNodeInfo->mAttributeType = "eCameraStereo";
			break;
		case FbxNodeAttribute::eCameraSwitcher:
			_pNodeInfo->mAttributeType = "eCameraSwitcher";
			break;
		case FbxNodeAttribute::eLight:
			_pNodeInfo->mAttributeType = "eLight";
			break;
		case FbxNodeAttribute::eOpticalReference:
			_pNodeInfo->mAttributeType = "eOpticalReference";
			break;
		case FbxNodeAttribute::eOpticalMarker:
			_pNodeInfo->mAttributeType = "eOpticalMarker";
			break;
		case FbxNodeAttribute::eNurbsCurve:
			_pNodeInfo->mAttributeType = "eNurbsCurve";
			LoadNurbsCurve(_pNode, _pNodeInfo);
			break;
		case FbxNodeAttribute::eTrimNurbsSurface:
			_pNodeInfo->mAttributeType = "eTrimNurbsSurface";
			break;
		case FbxNodeAttribute::eBoundary:
			_pNodeInfo->mAttributeType = "eBoundary";
			break;
		case FbxNodeAttribute::eNurbsSurface:
			_pNodeInfo->mAttributeType = "eNurbsSurface";
			break;
		case FbxNodeAttribute::eShape:
			_pNodeInfo->mAttributeType = "eShape";
			break;
		case FbxNodeAttribute::eLODGroup:
			_pNodeInfo->mAttributeType = "eLODGroup";
			break;
		case FbxNodeAttribute::eSubDiv:
			_pNodeInfo->mAttributeType = "eSubDiv";
			break;
		case FbxNodeAttribute::eCachedEffect:
			_pNodeInfo->mAttributeType = "eCachedEffect";
			break;
		case FbxNodeAttribute::eLine:
			_pNodeInfo->mAttributeType = "eLine";
			break;
		}
	}
	else
	{
		_pNodeInfo->mAttributeID = -1;
		_pNodeInfo->mAttributeType = "eNull";
	}
}

void FbxLoader::LoadNurbsCurve(FbxNode* _pNode, FbxNodeInfo* _pNodeInfo)
{
	_pNodeInfo->mAttributeData = new FbxNurbsCurveData();
	FbxNurbsCurveData* pNurbsCurve = (FbxNurbsCurveData*)_pNodeInfo->mAttributeData;

	auto pFbxNurbs = _pNode->GetNurbsCurve();

	// Get Controll Point
	pNurbsCurve->mControllPointCount = pFbxNurbs->GetControlPointsCount();
	for (int i = 0; i < pNurbsCurve->mControllPointCount; i++)
	{
		pNurbsCurve->mControllPoints.push_back(pFbxNurbs->GetControlPointAt(i));
	}

	// Get Knot
	pNurbsCurve->mKnotCount = pFbxNurbs->GetKnotCount();
	double* lKnot = pFbxNurbs->GetKnotVector();
	for (int i = 0; i < pNurbsCurve->mKnotCount; i++)
	{
		pNurbsCurve->mKnotValues.push_back(lKnot[i]);
	}

	// Get Order
	pNurbsCurve->mOrder = pFbxNurbs->GetOrder();

	// Get Step
	pNurbsCurve->mStep = pFbxNurbs->GetStep();

	// Get Type
	pNurbsCurve->mType = pFbxNurbs->GetType();

	// Get Dimension
	pNurbsCurve->mDimension = pFbxNurbs->GetDimension();
}

std::shared_ptr<FbxInfo> FbxLoader::Imoport(const std::string& file_path)
{
	// Import Scene
	FbxManager* pManager = FbxManager::Create();
	FbxIOSettings* pIOS = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(pIOS);

	FbxImporter* pImpoter = FbxImporter::Create(pManager, "");
	if (!pImpoter->Initialize(file_path.c_str(), -1, pManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", pImpoter->GetStatus().GetErrorString());
		return nullptr;
	}

	FbxScene* pScene = FbxScene::Create(pManager, "myScene");
	pImpoter->Import(pScene);
	pImpoter->Destroy();

	auto fbx_info = std::make_shared<FbxInfo>();

	fbx_info->mNodeCount = pScene->GetNodeCount();

	for (int i = 0; i < fbx_info->mNodeCount; i++)
	{
		auto node = std::make_shared<FbxNodeInfo>();
		fbx_info->mNodes.push_back(node);

		LoadNodeInfo(pScene->GetNode(i), node.get());
	}

	pManager->Destroy();

	return fbx_info;
}
