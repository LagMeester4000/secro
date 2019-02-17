#pragma once
#include <vector>
#include <array>
#include <SFML/Network.hpp>

template<typename T>
sf::Packet& operator<<(sf::Packet& packet, const std::vector<T>& vec)
{
	sf::Uint32 size = (sf::Uint32)vec.size();
	packet << size;
	for (auto& it : vec)
	{
		packet << it;
	}
	return packet;
}

template<typename T>
sf::Packet& operator>>(sf::Packet& packet, std::vector<T>& vec)
{
	sf::Uint32 size;
	packet >> size;
	for (sf::Uint32 i; i < size; ++i)
	{
		T it;
		packet >> it;
		vec.push_back(it);
	}
	return packet;
}



template<typename T, size_t S>
sf::Packet& operator<<(sf::Packet& packet, const std::array<T, S>& arr)
{
	for (auto& it : arr)
	{
		packet << it;
	}
	return packet;
}

template<typename T, size_t S>
sf::Packet& operator>>(sf::Packet& packet, const std::array<T, S>& arr)
{
	for (size_t i; i < S; ++i)
	{
		T it;
		packet >> it;
		arr[i] = it;
	}
	return packet;
}