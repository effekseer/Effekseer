
#ifndef __EFFEKSEER_CURVELOADER_H__
#define __EFFEKSEER_CURVELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

#include <memory>
#include "Effekseer.DefaultFile.h"
#include "Effekseer.File.h"
#include "Effekseer.Curve.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
@brief	カーブ読み込み破棄関数指定クラス
*/
class CurveLoader
{
public:
	/**
	@brief	コンストラクタ
	*/
	CurveLoader()
	{
	}

	/**
	@brief	デストラクタ
	*/
	virtual ~CurveLoader()
	{
	}

	/**
	@brief	Nカーブを読み込む。
	@param	path	[in]	読み込み元パス
	@return	カーブのポインタ
	@note
	カーブを読み込む。
	::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual void* Load(const EFK_CHAR* path)
	{
		::Effekseer::DefaultFileInterface fileInterface;
		std::unique_ptr<::Effekseer::FileReader>reader(fileInterface.OpenRead(path));
		if (reader.get() == NULL)
		{
			return nullptr;
		}

		Effekseer::Curve* curve = new Effekseer::Curve();

		// load converter version
		int converter_version = 0;
		reader->Read(&converter_version, sizeof(int));

		// load controll point count
		reader->Read(&curve->mControllPointCount, sizeof(int));

		// load controll points
		for (int i = 0; i < curve->mControllPointCount; i++)
		{
			dVector4 value;
			reader->Read(&value, sizeof(dVector4));
			curve->mControllPoint.push_back(value);
		}

		// load knot count
		reader->Read(&curve->mKnotCount, sizeof(int));

		// load knot values
		for (int i = 0; i < curve->mKnotCount; i++)
		{
			double value;
			reader->Read(&value, sizeof(double));
			curve->mKnotValue.push_back(value);
		}

		// load order
		reader->Read(&curve->mOrder, sizeof(int));

		// load step
		reader->Read(&curve->mStep, sizeof(int));

		// load type
		reader->Read(&curve->mType, sizeof(int));

		// load dimension
		reader->Read(&curve->mDimension, sizeof(int));

		// calc curve length
		curve->mLength = 0;

		for (int i = 1; i < curve->mControllPointCount; i++)
		{
			dVector4 p0 = curve->mControllPoint[i - 1];
			dVector4 p1 = curve->mControllPoint[i];

			float len = Vector3D::Length(Vector3D((float)p1.X, (float)p1.Y, (float)p1.Z) - Vector3D((float)p0.X, (float)p0.Y, (float)p0.Z));
			curve->mLength += len;
		}

		return static_cast<void*>(curve);
	}

	/**
	@brief	カーブを破棄する。
	@param	data	[in]	カーブ
	@note
	カーブを破棄する。
	::Effekseer::Effectのインスタンスが破棄された時に使用される。
	*/
	virtual void Unload(void* data)
	{
		if (data != NULL)
		{
			Curve* curve = (Curve*)data;
			ES_SAFE_DELETE(curve);
		}
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
  //----------------------------------------------------------------------------------
  //
  //----------------------------------------------------------------------------------
#endif // __EFFEKSEER_MODELLOADER_H__
