#pragma once

namespace pacman {

template <typename T>
class ServiceLocator
{
public:
	static void SetService(T* p_service)
	{
		ms_service = p_service;
	};
	static T* GetService()
	{
		return ms_service;
	}
private:
	static T* ms_service;
};

template <typename T>
T* ServiceLocator<T>::ms_service = nullptr;

}; // namespace pacman
