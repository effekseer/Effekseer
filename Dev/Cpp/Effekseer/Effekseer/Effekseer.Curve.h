
#ifndef __EFFEKSEER_CURVE_H__
#define __EFFEKSEER_CURVE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Resource.h"
#include "Effekseer.Vector3D.h"

#include <cmath>
#include <limits>
#include <vector>

namespace Effekseer
{

class dVector4
{
public:
	double X, Y, Z, W;

public:
	dVector4(double x = 0, double y = 0, double z = 0, double w = 0)
		: X(x)
		, Y(y)
		, Z(z)
		, W(w)
	{
	}
};

/**
@brief
\~English	Curve class
\~Japanese	カーブクラス
*/
class Curve : public Resource
{
	friend class CurveLoader;

public:
	static const int32_t Version = 1;

private:
	int controlPointCount_;
	std::vector<dVector4> controlPoints_;

	int knotCount_;
	std::vector<double> knotValues_;

	int order_;
	int step_;
	int type_;
	int dimension_;

	float length_;

private:
	/**
	 * CalcBSplineBasisFunc : B-スプライン基底関数の計算
	 * 
	 * const vector<double>& knot : ノット列
	 * unsigned int j : ノット列の開始番号
	 * unsigned int p : 次数
	 * double t : 計算対象の独立変数
	 * 
	 * ノット列は昇順である必要があるが、そのチェックは行わない
	 * 
	 * 戻り値 : 計算結果
	 */
	double CalcBSplineBasisFunc(const std::vector<double>& knot, unsigned int j, unsigned int p, double t)
	{
		if (knot.size() == 0)
			return std::numeric_limits<double>::quiet_NaN();

		// ノット列のデータ長が充分でない場合は nan を返す
		unsigned int m = static_cast<unsigned int>(knot.size()) - 1;
		if (m < j + p + 1)
			return std::numeric_limits<double>::quiet_NaN();

		// 正値をとる範囲外ならゼロを返す
		if ((t < knot[j]) || (t > knot[j + p + 1]))
			return (0);
		// p = 0 かつ knot[j] <= t <= knot[j + p + 1] なら 1 を返す
		if (p == 0)
			return (1);
		// p = 1 の場合、三角の頂点の値は特別扱い
		if (p == 1 && t == knot[j + 1])
			return (1);

		// 漸化式の計算
		double d1 = (knot[j + p] == knot[j]) ? 0 : (t - knot[j]) * CalcBSplineBasisFunc(knot, j, p - 1, t) / (knot[j + p] - knot[j]);
		double d2 = (knot[j + p + 1] == knot[j + 1]) ? 0 : (knot[j + p + 1] - t) * CalcBSplineBasisFunc(knot, j + 1, p - 1, t) / (knot[j + p + 1] - knot[j + 1]);

		return (d1 + d2);
	}

public:
	Curve()
	{
	}

	Curve(const void* data, int32_t size)
	{
		uint8_t* pData = new uint8_t[size];
		memcpy(pData, data, size);

		uint8_t* p = (uint8_t*)pData;

		// load converter version
		int converter_version = 0;
		memcpy(&converter_version, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load controll point count
		memcpy(&controlPointCount_, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load controll points
		for (int i = 0; i < controlPointCount_; i++)
		{
			dVector4 value;
			memcpy(&value, p, sizeof(dVector4));
			p += sizeof(dVector4);
			controlPoints_.push_back(value);
		}

		// load knot count
		memcpy(&knotCount_, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load knot values
		for (int i = 0; i < knotCount_; i++)
		{
			double value;
			memcpy(&value, p, sizeof(double));
			p += sizeof(double);
			knotValues_.push_back(value);
		}

		// load order
		memcpy(&order_, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load step
		memcpy(&step_, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load type
		memcpy(&type_, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load dimension
		memcpy(&dimension_, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// calc curve length
		length_ = 0;

		for (int i = 1; i < controlPointCount_; i++)
		{
			dVector4 p0 = controlPoints_[i - 1];
			dVector4 p1 = controlPoints_[i];

			float len = Vector3D::Length(Vector3D((float)p1.X, (float)p1.Y, (float)p1.Z) - Vector3D((float)p0.X, (float)p0.Y, (float)p0.Z));
			length_ += len;
		}

		ES_SAFE_DELETE_ARRAY(pData);
	}

	~Curve()
	{
	}

	Vector3D CalcuratePoint(float t, float magnification)
	{
		if (t == 0.0f && controlPoints_.size() > 0)
		{
			return {
				static_cast<float>(controlPoints_[0].X * magnification),
				static_cast<float>(controlPoints_[0].Y * magnification),
				static_cast<float>(controlPoints_[0].Z * magnification)};
		}

		int p = order_; // 次数

		std::vector<double> bs(controlPointCount_); // B-Spline 基底関数の計算結果(重み値を積算)

		// ノット列の要素を +1 する
		auto knot = knotValues_;
		knot.push_back(knotValues_[knotValues_.size() - 1] + 1);

		float t_rate = float(knot.back() - 1);

		double wSum = 0; // bs の合計
		for (int j = 0; j < controlPointCount_; ++j)
		{
			bs[j] = controlPoints_[j].W * CalcBSplineBasisFunc(knot, j, p, t * (t_rate));

			if (!std::isnan(bs[j]))
			{
				wSum += bs[j];
			}
		}

		Vector3D ans(0, 0, 0); // 計算結果
		for (int j = 0; j < controlPointCount_; ++j)
		{
			Vector3D d;
			d.X = (float)controlPoints_[j].X * magnification * (float)bs[j] / (float)wSum;
			d.Y = (float)controlPoints_[j].Y * magnification * (float)bs[j] / (float)wSum;
			d.Z = (float)controlPoints_[j].Z * magnification * (float)bs[j] / (float)wSum;
			if (!std::isnan(d.X) && !std::isnan(d.Y) && !std::isnan(d.Z))
			{
				ans += d;
			}
		}

		return ans;
	}

	//
	//  Getter
	//
	int GetControllPointCount()
	{
		return controlPointCount_;
	}
	dVector4 GetControllPoint(int index)
	{
		return controlPoints_[index];
	}

	int GetKnotCount()
	{
		return knotCount_;
	}
	double GetKnotValue(int index)
	{
		return knotValues_[index];
	}

	int GetOrder()
	{
		return order_;
	}
	int GetStep()
	{
		return step_;
	}
	int GetType()
	{
		return type_;
	}
	int GetDimension()
	{
		return dimension_;
	}

	float GetLength()
	{
		return length_;
	}

}; // end class

} // end namespace Effekseer

#endif // __EFFEKSEER_CURVE_H__
