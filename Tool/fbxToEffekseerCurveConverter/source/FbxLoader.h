#pragma once

#define FBXSDK_SHARED
#include <fbxsdk.h>
#include <vector>
#include <string>
#include <memory>

struct FbxNurbsCurveData
{
	int mControllPointCount;
	std::vector<FbxVector4> mControllPoints;

	int mKnotCount;
	std::vector<double> mKnotValues;

	int mOrder;
	int mStep;
	FbxNurbsCurve::EType mType;
	FbxNurbsCurve::EDimension mDimension;
};

struct FbxNodeInfo
{
	std::string mName;

	int mChildCount;
	std::vector<FbxNodeInfo*> mChilds;

	int mAttributeID;
	std::string mAttributeType;

	void* mAttributeData = nullptr;

	~FbxNodeInfo()
	{
		for (auto it : mChilds)
		{
			delete it;
		}

		if (mAttributeData != nullptr)
		{
			delete mAttributeData;
			mAttributeData = nullptr;
		}
	}
};

struct FbxInfo
{
	int mNodeCount;
	std::vector<std::shared_ptr<FbxNodeInfo>> mNodes;
};

class FbxLoader
{
private:
	static void LoadNodeInfo(FbxNode* _pNode, FbxNodeInfo* _pNodeInfo);
	static void LoadAttribute(FbxNode* _pNode, FbxNodeInfo* _pNodeInfo);

	static void LoadNurbsCurve(FbxNode* _pNode, FbxNodeInfo* _pNodeInfo);

public:
	static std::shared_ptr<FbxInfo> Imoport(const std::string& file_path);
};
