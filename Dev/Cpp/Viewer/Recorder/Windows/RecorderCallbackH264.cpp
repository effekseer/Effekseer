#include "RecorderCallbackH264.h"

#include <Windows.h>

#include <mfapi.h>

#include <mfidl.h>

#include <Mfreadwrite.h>

#include <mferror.h>

#include <string>
#include <vector>

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")

namespace Effekseer
{
namespace Tool
{

class VideoWriter
{
private:
	IMFSinkWriter* sinkWriter_ = nullptr;
	DWORD streamIndex_ = 0;
	int32_t width_ = 0;
	int32_t height_ = 0;
	int32_t framerate_ = 0;
	uint64_t timestamp_ = 0;
	bool isValid_ = false;

public:
	VideoWriter()
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (SUCCEEDED(hr))
		{
			hr = MFStartup(MF_VERSION);
			if (SUCCEEDED(hr))
			{
				isValid_ = true;
			}
		}
	}

	int32_t GetBufferSize() const
	{
		return width_ * height_ * 4;
	}

	bool Start(const std::u16string& path, int width, int height, int framerate, int frameCount)
	{
		if (!isValid_)
		{
			return false;
		}

		IMFMediaType* pMediaTypeOut = nullptr;
		IMFMediaType* pMediaTypeIn = nullptr;
		int bitrate = 8000000;

		HRESULT hr = MFCreateSinkWriterFromURL(reinterpret_cast<const wchar_t*>(path.c_str()), NULL, NULL, &sinkWriter_);

		if (SUCCEEDED(hr))
		{
			hr = MFCreateMediaType(&pMediaTypeOut);
		}
		if (SUCCEEDED(hr))
		{
			hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		}
		if (SUCCEEDED(hr))
		{
			hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
		}
		if (SUCCEEDED(hr))
		{
			hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, bitrate);
		}
		if (SUCCEEDED(hr))
		{
			hr = pMediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
		}
		if (SUCCEEDED(hr))
		{
			hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, width, height);
		}
		if (SUCCEEDED(hr))
		{
			hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, framerate, 1);
		}
		if (SUCCEEDED(hr))
		{
			hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
		}
		if (SUCCEEDED(hr))
		{
			hr = sinkWriter_->AddStream(pMediaTypeOut, &streamIndex_);
		}

		if (SUCCEEDED(hr))
		{
			hr = MFCreateMediaType(&pMediaTypeIn);
		}
		if (SUCCEEDED(hr))
		{
			hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		}
		if (SUCCEEDED(hr))
		{
			hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
		}
		if (SUCCEEDED(hr))
		{
			hr = pMediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
		}
		if (SUCCEEDED(hr))
		{
			hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, width, height);
		}
		if (SUCCEEDED(hr))
		{
			hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, framerate, 1);
		}
		if (SUCCEEDED(hr))
		{
			hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
		}
		if (SUCCEEDED(hr))
		{
			hr = sinkWriter_->SetInputMediaType(streamIndex_, pMediaTypeIn, NULL);
		}

		if (SUCCEEDED(hr))
		{
			hr = sinkWriter_->BeginWriting();
		}

		SafeRelease(pMediaTypeOut);
		SafeRelease(pMediaTypeIn);

		if (SUCCEEDED(hr))
		{
			width_ = width;
			height_ = height;
			framerate_ = framerate;
			timestamp_ = 0;
			return true;
		}
		else
		{
			SafeRelease(sinkWriter_);
			streamIndex_ = 0;
			return false;
		}
	}

	bool Write(const std::vector<uint8_t> buffer)
	{
		if (sinkWriter_ == nullptr)
		{
			return false;
		}

		if (buffer.size() != GetBufferSize())
		{
			return false;
		}

		IMFSample* sample = NULL;
		IMFMediaBuffer* videoBuffer = NULL;

		const LONG widthSize = 4 * width_;
		const DWORD bufferSize = widthSize * height_;

		BYTE* internalData = NULL;

		HRESULT hr = MFCreateMemoryBuffer(bufferSize, &videoBuffer);

		if (SUCCEEDED(hr))
		{
			hr = videoBuffer->Lock(&internalData, NULL, NULL);
		}
		if (SUCCEEDED(hr))
		{
			hr = MFCopyImage(
				internalData,
				widthSize,
				(BYTE*)buffer.data(),
				widthSize,
				widthSize,
				height_);
		}
		if (videoBuffer)
		{
			videoBuffer->Unlock();
		}

		if (SUCCEEDED(hr))
		{
			hr = videoBuffer->SetCurrentLength(bufferSize);
		}

		if (SUCCEEDED(hr))
		{
			hr = MFCreateSample(&sample);
		}
		if (SUCCEEDED(hr))
		{
			hr = sample->AddBuffer(videoBuffer);
		}

		if (SUCCEEDED(hr))
		{
			hr = sample->SetSampleTime(timestamp_);
		}
		if (SUCCEEDED(hr))
		{
			hr = sample->SetSampleDuration(10 * 1000 * 1000 / framerate_);
		}

		if (SUCCEEDED(hr))
		{
			hr = sinkWriter_->WriteSample(streamIndex_, sample);
		}

		timestamp_ += 10 * 1000 * 1000 / framerate_;

		SafeRelease(sample);
		SafeRelease(videoBuffer);
		return SUCCEEDED(hr);
	}

	void End()
	{
		if (sinkWriter_ != nullptr)
		{
			sinkWriter_->Finalize();
			SafeRelease(sinkWriter_);
		}
	}

	~VideoWriter()
	{
		End();

		if (isValid_)
		{
			MFShutdown();
			CoUninitialize();
		}
	}
};

bool RecorderCallbackH264::OnBeginRecord()
{
	auto path = std::u16string(recordingParameter_.GetPath()) + std::u16string(recordingParameter_.GetExt());
	videoWriter_ = std::make_shared<VideoWriter>();
	if (!videoWriter_->Start(path, imageSize_.X, imageSize_.Y, (int32_t)(60.0f / (float)recordingParameter_.Freq), recordingParameter_.Count))
	{
		return false;
	}

	buffer_.resize(videoWriter_->GetBufferSize());

	return true;
}

void RecorderCallbackH264::OnEndRecord()
{
	videoWriter_.reset();
}

void RecorderCallbackH264::OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels)
{
	for (int y = 0; y < imageSize_.Y; y++)
	{
		for (int x = 0; x < imageSize_.X; x++)
		{
			const auto pixel = pixels[x + y * imageSize_.X];

			auto p = reinterpret_cast<Effekseer::Color*>(buffer_.data());
			auto c = pixels[x + y * imageSize_.X];
			std::swap(c.R, c.B);
			p[x + (imageSize_.Y - 1 - y) * imageSize_.X + 0] = c;
		}
	}

	videoWriter_->Write(buffer_);
}

} // namespace Tool
} // namespace Effekseer