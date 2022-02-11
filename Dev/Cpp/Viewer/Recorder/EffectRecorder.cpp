#include "EffectRecorder.h"
#include "RecorderCallback.h"

#ifdef _WIN32
#include "../Graphics/Platform/DX11/efk.GraphicsDX11.h"
#include "Windows/RecorderCallbackH264.h"
#endif

#include "../GUI/RenderImage.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "AVIExporter.h"
#include "GifHelper.h"
#include "PNGHelper.h"

#include <sstream>

namespace Effekseer
{
namespace Tool
{

void GenerateExportedImageWithBlendAndAdd(std::vector<Effekseer::Color>& pixelsBlend,
										  std::vector<Effekseer::Color>& pixelsAdd,
										  std::vector<std::vector<Effekseer::Color>>& pixels)
{
	assert(pixels.size() == 9);

	auto f2b = [](float v) -> uint8_t {
		auto v_ = v * 255;
		if (v_ > 255)
			v_ = 255;
		if (v_ < 0)
			v_ = 0;
		return static_cast<uint8_t>(v_);
	};

	auto b2f = [](uint8_t v) -> float {
		return static_cast<float>(v) / 255.0f;
	};

	pixelsAdd.resize(pixels[0].size());
	pixelsBlend.resize(pixels[0].size());

	for (auto i = 0; i < pixels[0].size(); i++)
	{
		float colors[4][9];

		for (auto j = 0; j < 9; j++)
		{
			colors[0][j] = pixels[j][i].R / 255.0f;
			colors[1][j] = pixels[j][i].G / 255.0f;
			colors[2][j] = pixels[j][i].B / 255.0f;
			colors[3][j] = pixels[j][i].A / 255.0f;

			colors[0][j] *= colors[3][j];
			colors[1][j] *= colors[3][j];
			colors[2][j] *= colors[3][j];
		}

		std::array<float, 3> gradients;
		gradients.fill(0.0f);

		for (auto c = 0; c < 3; c++)
		{
			bool found = false;

			for (auto j = 0; j < 9; j++)
			{
				if (colors[c][j] >= 1.0f)
				{
					gradients[c] = (colors[c][j] - colors[c][0]) / ((j + 1) / 9.0f);
					found = true;
					break;
				}
			}

			if (!found)
			{
				gradients[c] = (colors[c][8] - colors[c][0]);
			}
		}

		float blendAlpha = (3.0f - (gradients[0] + gradients[1] + gradients[2])) / 3.0f;

		pixelsBlend[i].R = 0;
		pixelsBlend[i].G = 0;
		pixelsBlend[i].B = 0;
		pixelsBlend[i].A = static_cast<uint8_t>(Effekseer::Clamp(blendAlpha * 255.0f, 255.0f, 0.0f));

		pixelsAdd[i].R = static_cast<uint8_t>(Effekseer::Clamp(colors[0][0] * 255.0f, 255.0f, 0.0f));
		pixelsAdd[i].G = static_cast<uint8_t>(Effekseer::Clamp(colors[1][0] * 255.0f, 255.0f, 0.0f));
		pixelsAdd[i].B = static_cast<uint8_t>(Effekseer::Clamp(colors[2][0] * 255.0f, 255.0f, 0.0f));
		pixelsAdd[i].A = 255;
	}
}

class RecorderCallbackSprite : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	Effekseer::Tool::Vector2I imageSize_;

public:
	RecorderCallbackSprite(RecordingParameter& recordingParameter, Effekseer::Tool::Vector2I imageSize)
		: recordingParameter_(recordingParameter)
		, imageSize_(imageSize)
	{
	}

	virtual ~RecorderCallbackSprite() = default;

	bool OnBeginRecord() override
	{
		return true;
	}

	void OnEndRecord() override
	{
	}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override
	{
		char16_t path_[260];
		auto pathWithoutExt = recordingParameter_.GetPath();
		auto ext = recordingParameter_.GetExt();

		char pathWOE[256];
		char ext_[256];
		//char path8_dst[256];
		Effekseer::ConvertUtf16ToUtf8(pathWOE, 256, pathWithoutExt);
		Effekseer::ConvertUtf16ToUtf8(ext_, 256, ext);

		auto ss = std::stringstream();
		ss << pathWOE << "." << std::to_string(index) << ext_;

		Effekseer::ConvertUtf8ToUtf16(path_, 260, ss.str().c_str());

		spdlog::trace("RecorderCallbackSprite : {}", ss.str().c_str());

		efk::PNGHelper pngHelper;
		pngHelper.Save((char16_t*)path_, imageSize_.X, imageSize_.Y, pixels.data());
	}
};

class RecorderCallbackSpriteSheet : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	Effekseer::Tool::Vector2I imageSize_;
	int yCount = 0;
	std::vector<Effekseer::Color> pixels_out;

public:
	RecorderCallbackSpriteSheet(RecordingParameter& recordingParameter, Effekseer::Tool::Vector2I imageSize)
		: recordingParameter_(recordingParameter)
		, imageSize_(imageSize)
	{
	}

	virtual ~RecorderCallbackSpriteSheet() = default;

	bool OnBeginRecord() override
	{
		yCount = recordingParameter_.Count / recordingParameter_.HorizontalCount;
		if (recordingParameter_.Count > recordingParameter_.HorizontalCount * yCount)
			yCount++;

		pixels_out.resize((imageSize_.X * recordingParameter_.HorizontalCount) * (imageSize_.Y * yCount));

		if (recordingParameter_.Transparence == TransparenceType::None)
		{
			for (auto& p : pixels_out)
			{
				p = Effekseer::Color(0, 0, 0, 255);
			}
		}
		else

		{
			for (auto& p : pixels_out)
			{
				p = Effekseer::Color(0, 0, 0, 0);
			}
		}

		return true;
	}

	void OnEndRecord() override
	{
		auto path = std::u16string(recordingParameter_.GetPath()) + std::u16string(recordingParameter_.GetExt());

		efk::PNGHelper pngHelper;
		pngHelper.Save(
			path.c_str(), imageSize_.X * recordingParameter_.HorizontalCount, imageSize_.Y * yCount, pixels_out.data());
	}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override
	{
		auto x = index % recordingParameter_.HorizontalCount;
		auto y = index / recordingParameter_.HorizontalCount;

		for (int32_t y_ = 0; y_ < imageSize_.Y; y_++)
		{
			for (int32_t x_ = 0; x_ < imageSize_.X; x_++)
			{
				pixels_out[x * imageSize_.X + x_ +
						   (imageSize_.X * recordingParameter_.HorizontalCount) * (imageSize_.Y * y + y_)] =
					pixels[x_ + y_ * imageSize_.X];
			}
		}
	}
};

class RecorderCallbackGif : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	Effekseer::Tool::Vector2I imageSize_;
	efk::GifHelper helper;

public:
	RecorderCallbackGif(RecordingParameter& recordingParameter, Effekseer::Tool::Vector2I imageSize)
		: recordingParameter_(recordingParameter)
		, imageSize_(imageSize)
	{
	}

	virtual ~RecorderCallbackGif() = default;

	bool OnBeginRecord() override
	{
		auto path = std::u16string(recordingParameter_.GetPath()) + std::u16string(recordingParameter_.GetExt());

		helper.Initialize(path.c_str(), imageSize_.X, imageSize_.Y, recordingParameter_.Freq);
		return true;
	}

	void OnEndRecord() override
	{
	}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override
	{
		helper.AddImage(pixels);
	}
};

class RecorderCallbackAvi : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	Effekseer::Tool::Vector2I imageSize_;
	efk::AVIExporter exporter;
	std::vector<uint8_t> d;
	FILE* fp = nullptr;

public:
	RecorderCallbackAvi(RecordingParameter& recordingParameter, Effekseer::Tool::Vector2I imageSize)
		: recordingParameter_(recordingParameter)
		, imageSize_(imageSize)
	{
	}

	virtual ~RecorderCallbackAvi() = default;

	bool OnBeginRecord() override
	{
		auto path = std::u16string(recordingParameter_.GetPath()) + std::u16string(recordingParameter_.GetExt());
		char path8[256];
		Effekseer::ConvertUtf16ToUtf8(path8, 256, path.c_str());

#ifdef _WIN32
		_wfopen_s(&fp, (const wchar_t*)path.c_str(), L"wb");
#else
		fp = fopen(path8, "wb");
#endif

		if (fp == nullptr)
			return false;

		exporter.Initialize(
			imageSize_.X, imageSize_.Y, (int32_t)(60.0f / (float)recordingParameter_.Freq), recordingParameter_.Count);

		exporter.BeginToExportAVI(d);
		fwrite(d.data(), 1, d.size(), fp);

		return true;
	}

	void OnEndRecord() override
	{
		exporter.FinishToExportAVI(d);
		fwrite(d.data(), 1, d.size(), fp);
		fclose(fp);
	}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override
	{
		exporter.ExportFrame(d, pixels);
		fwrite(d.data(), 1, d.size(), fp);
	}
};

bool EffectRecorder::Begin(int32_t squareMaxCount,
						   Effekseer::Tool::EffectRendererParameter config,
						   Vector2I screenSize,
						   std::shared_ptr<Effekseer::Tool::GraphicsDevice> graphicsDevice,
						   std::shared_ptr<Effekseer::Tool::EffectSetting> setting,
						   const RecordingParameter& recordingParameter,
						   Effekseer::Tool::Vector2I imageSize,
						   bool isSRGBMode,
						   Effekseer::Tool::ViewerEffectBehavior behavior,
						   Effekseer::Tool::PostEffectParameter postEffectParameter,
						   std::shared_ptr<Effekseer::Tool::Effect> effect)
{
	graphicsDevice_ = graphicsDevice;
	recordingParameter_ = recordingParameter;
	int recScale = Effekseer::Max(1, recordingParameter.Scale);
	imageSize_ = Effekseer::Tool::Vector2I(imageSize.X * recScale, imageSize.Y * recScale);

	if (recordingParameter_.Transparence == TransparenceType::Generate2)
	{
		recordingParameter2_ = recordingParameter_;
		auto path = recordingParameter_.GetPath();

		char pathWOE[256];
		char16_t path_[256];
		Effekseer::ConvertUtf16ToUtf8(pathWOE, 256, path);

		auto ss = std::stringstream();
		ss << pathWOE << "_add";

		Effekseer::ConvertUtf8ToUtf16(path_, 256, ss.str().c_str());
		recordingParameter2_.SetPath(path_);
	}

	if (recordingParameter_.RecordingMode == RecordingModeType::Sprite)
	{
		recorderCallback = std::make_shared<RecorderCallbackSprite>(recordingParameter_, imageSize_);

		if (recordingParameter_.Transparence == TransparenceType::Generate2)
		{
			recorderCallback2 = std::make_shared<RecorderCallbackSprite>(recordingParameter2_, imageSize_);
		}
	}
	else if (recordingParameter_.RecordingMode == RecordingModeType::SpriteSheet)
	{
		recorderCallback = std::make_shared<RecorderCallbackSpriteSheet>(recordingParameter_, imageSize_);

		if (recordingParameter_.Transparence == TransparenceType::Generate2)
		{
			recorderCallback2 = std::make_shared<RecorderCallbackSpriteSheet>(recordingParameter2_, imageSize_);
		}
	}
	else if (recordingParameter_.RecordingMode == RecordingModeType::Gif)
	{
		recorderCallback = std::make_shared<RecorderCallbackGif>(recordingParameter_, imageSize_);

		if (recordingParameter_.Transparence == TransparenceType::Generate2)
		{
			recorderCallback2 = std::make_shared<RecorderCallbackGif>(recordingParameter2_, imageSize_);
		}
	}
	else if (recordingParameter_.RecordingMode == RecordingModeType::Avi)
	{
		recorderCallback = std::make_shared<RecorderCallbackAvi>(recordingParameter_, imageSize_);

		if (recordingParameter_.Transparence == TransparenceType::Generate2)
		{
			recorderCallback2 = std::make_shared<RecorderCallbackAvi>(recordingParameter2_, imageSize_);
		}
	}
	else if (recordingParameter_.RecordingMode == RecordingModeType::H264)
	{
#ifdef _WIN32
		recorderCallback = std::make_shared<RecorderCallbackH264>(recordingParameter_, imageSize_);

		if (recordingParameter_.Transparence == TransparenceType::Generate2)
		{
			recorderCallback2 = std::make_shared<RecorderCallbackH264>(recordingParameter2_, imageSize_);
		}
#else
		return false;
#endif
	}

	if (recordingParameter_.Transparence == TransparenceType::Generate2)
	{
		iteratorCount = 9;
	}

	if (!recorderCallback->OnBeginRecord())
	{
		return false;
	}

	if (recorderCallback2 != nullptr && !recorderCallback2->OnBeginRecord())
	{
		return false;
	}

	generator_ = std::make_shared<Effekseer::Tool::EffectRenderer>();

	if (!generator_->Initialize(graphicsDevice_, nullptr, setting, squareMaxCount, isSRGBMode))
	{
		return false;
	}

	renderTarget_ = Effekseer::Tool::RenderImage::Create(graphicsDevice_);
	renderTarget_->Resize(imageSize_.X, imageSize_.Y);
	generator_->ResizeScreen(imageSize_);

	::Effekseer::Matrix44 mat;
	mat.Values[0][0] = (float)screenSize.X / (float)imageSize.X;
	mat.Values[1][1] = (float)screenSize.Y / (float)imageSize.Y;
	::Effekseer::Matrix44::Mul(config.ProjectionMatrix.Value, config.ProjectionMatrix.Value, mat);

	if (recordingParameter_.Transparence == TransparenceType::Original)
	{
		config.BackgroundColor = Color(0, 0, 0, 0);
	}

	generator_->SetParameter(config);
	generator_->SetEffect(effect);

	generator_->SetBehavior(behavior);

	generator_->SetPostEffectParameter(postEffectParameter);

	generator_->PlayEffect();
	generator_->Update(recordingParameter_.OffsetFrame);

	return true;
}

bool EffectRecorder::Step(int frames)
{
	for (int32_t i = 0; i < frames; i++)
	{
		if (IsCompleted())
		{
			break;
		}

		std::vector<std::vector<Effekseer::Color>> pixelss;
		pixelss.resize(iteratorCount);

		for (int32_t loop = 0; loop < iteratorCount; loop++)
		{
			auto colorValue = Effekseer::Clamp(32 * loop, 255, 0);

			auto config = generator_->GetParameter();
			if (recordingParameter_.Transparence == TransparenceType::None)
			{
				// not change
			}
			else if (recordingParameter_.Transparence == TransparenceType::Original)
			{
				config.BackgroundColor = Effekseer::Color(0, 0, 0, 0);
			}
			else
			{
				config.BackgroundColor = Effekseer::Color(colorValue, colorValue, colorValue, 255);
			}
			generator_->SetParameter(config);

			generator_->Render(renderTarget_);
			graphicsDevice_->GetGraphics()->SaveTexture(renderTarget_->GetTexture(), pixelss[loop]);

			auto generateAlpha = recordingParameter_.Transparence == TransparenceType::Generate;
			auto removeAlpha = recordingParameter_.Transparence == TransparenceType::None;

			auto& pixels = pixelss[loop];

			auto f2b = [](float v) -> uint8_t {
				auto v_ = v * 255;
				if (v_ > 255)
					v_ = 255;
				if (v_ < 0)
					v_ = 0;
				return static_cast<uint8_t>(v_);
			};

			auto b2f = [](uint8_t v) -> float {
				auto v_ = (float)v / 255.0f;
				return v_;
			};

			for (int32_t i = 0; i < imageSize_.X * imageSize_.Y; i++)
			{
				if (generateAlpha)
				{
					auto rf = b2f(pixels[i].R);
					auto gf = b2f(pixels[i].G);
					auto bf = b2f(pixels[i].B);
					auto oaf = b2f(pixels[i].A);

					rf = rf * oaf;
					gf = gf * oaf;
					bf = bf * oaf;

					auto af = rf;
					af = Effekseer::Max(af, gf);
					af = Effekseer::Max(af, bf);

					if (af > 0.0f)
					{
						pixels[i].R = f2b(rf / af);
						pixels[i].G = f2b(gf / af);
						pixels[i].B = f2b(bf / af);
					}

					pixels[i].A = f2b(af);
				}

				if (removeAlpha)
				{
					pixels[i].A = 255;
				}
			}
		}

		generator_->Update(recordingParameter_.Freq);

		if (recordingParameter_.Transparence == TransparenceType::Generate2)
		{
			std::vector<std::vector<Effekseer::Color>> pixels_out;
			pixels_out.resize(2);
			GenerateExportedImageWithBlendAndAdd(pixels_out[0], pixels_out[1], pixelss);

			recorderCallback->OnEndFrameRecord(recordedCount, pixels_out[0]);

			if (recorderCallback2 != nullptr)
			{
				recorderCallback2->OnEndFrameRecord(recordedCount, pixels_out[1]);
			}
		}
		else
		{
			recorderCallback->OnEndFrameRecord(recordedCount, pixelss[0]);
		}

		recordedCount++;
	}

	return true;
}

bool EffectRecorder::End()
{
	generator_->ResetEffect();

	recorderCallback->OnEndRecord();

	if (recorderCallback2 != nullptr)
	{
		recorderCallback2->OnEndRecord();
	}

	generator_ = nullptr;

	return true;
}

bool EffectRecorder::IsCompleted() const
{
	return recordedCount >= recordingParameter_.Count;
}

float EffectRecorder::GetProgress() const
{
	return static_cast<float>(recordedCount) / static_cast<float>(recordingParameter_.Count);
}

} // namespace Tool
} // namespace Effekseer