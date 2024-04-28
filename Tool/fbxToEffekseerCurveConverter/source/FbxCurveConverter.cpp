#include "FbxCurveConverter.h"
#include <fstream>
#include <iostream>

FbxNurbsCurveData* FbxCurveConverter::SearchNurbsCurve(const FbxNodeInfo& _pNodeInfo)
{
	if (_pNodeInfo.mAttributeType == std::string("eNurbsCurve"))
	{
		return (FbxNurbsCurveData*)_pNodeInfo.mAttributeData;
	}
	else
	{
		for (int ci = 0; ci < _pNodeInfo.mChildCount; ci++)
		{
			return SearchNurbsCurve(*_pNodeInfo.mChilds[ci]);
		}
	}

	return nullptr;
}

bool FbxCurveConverter::Export(const std::string& file_path, const FbxInfo& _pFbxeInfo)
{
	FbxNurbsCurveData* pCurve = nullptr;

	for (int i = 0; i < _pFbxeInfo.mNodeCount; i++)
	{
		// search export data
		pCurve = SearchNurbsCurve(*_pFbxeInfo.mNodes[i]);

		if (pCurve != nullptr)
		{
			break;
		}
	}

	if (pCurve == nullptr)
	{
		std::cout << "Error : Curve is not found" << std::endl;

		return false;
	}

	//
	//   Start Export
	//

	// create export file
	std::ofstream ofs(file_path, std::ios::out | std::ios::binary);

	// export version
	int version = ConverterVersion;
	ofs.write((char*)&version, sizeof(int));

	// export controll point count
	ofs.write((char*)&pCurve->mControllPointCount, sizeof(int));

	// export controll point
	for (int i = 0; i < pCurve->mControllPointCount; i++)
	{
		ofs.write((char*)&pCurve->mControllPoints[i], sizeof(FbxVector4));
	}

	// export knot count
	ofs.write((char*)&pCurve->mKnotCount, sizeof(int));

	// export knot value
	for (int i = 0; i < pCurve->mKnotCount; i++)
	{
		ofs.write((char*)&pCurve->mKnotValues[i], sizeof(double));
	}

	// export order
	ofs.write((char*)&pCurve->mOrder, sizeof(int));

	// export step
	ofs.write((char*)&pCurve->mStep, sizeof(int));

	// export type
	int type = (int)pCurve->mType;
	ofs.write((char*)&type, sizeof(int));

	// export dimension
	int dimension = (int)pCurve->mDimension;
	ofs.write((char*)&dimension, sizeof(int));

	// close file
	ofs.close();

	//
	//   End Export
	//

	return true;
}
