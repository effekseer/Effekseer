#include "EffekseerTool.Sound.h"

namespace EffekseerTool
{

Sound::Sound()
	: m_sound(nullptr)
	, m_manager(nullptr)
{
}

Sound::~Sound()
{
	if (m_sound != nullptr)
	{
		m_sound.Reset();
	}

	if (m_manager)
	{
		m_manager->Release();
	}
}

bool Sound::Initialize()
{
	m_manager = osm::Manager::Create();
	if (m_manager == nullptr)
	{
		return false;
	}

	if (!m_manager->Initialize())
	{
		return false;
	}

	m_sound = EffekseerSound::Sound::Create(m_manager);
	if (m_sound == nullptr)
	{
		return false;
	}

	return true;
}

void Sound::SetListener(const Effekseer::Vector3D& pos, const Effekseer::Vector3D& at, const Effekseer::Vector3D& up)
{
	if (m_sound != nullptr)
	{
		m_sound->SetListener(pos, at, up);
	}
}

} // namespace EffekseerTool
