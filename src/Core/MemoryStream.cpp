// Copyright 2019-2020 the donut authors. See AUTHORS.md

#include "Core/MemoryStream.h"

namespace Donut
{

MemoryStream::MemoryStream(const std::vector<uint8_t>& data)
{
	_data = data;
	_position = _data.begin();
}

void MemoryStream::ReadBytes(uint8_t* dest, std::size_t length)
{
	// check cur + length doesn't exceed size

	// copy the data
	std::memcpy(dest, &(*_position), length);

	// advance our current position iterator
	std::advance(_position, length);
}

std::string MemoryStream::ReadString(std::size_t length)
{
	char* str = new char[length];
	ReadBytes(reinterpret_cast<uint8_t*>(str), length);

	// Bullshit null terminate
	uint32_t l = 0;
	for (; l < length; ++l)
	{
		if (str[l] == 0)
			break;
	}

	std::string ret(str, l);
	delete[] str;

	return ret;
}

std::string MemoryStream::ReadLPString()
{
	const auto length = Read<uint8_t>();

	// empty string
	if (length == 0)
		return std::string();

	return ReadString(length);
}

void MemoryStream::Seek(std::size_t position, SeekMode mode)
{
	switch (mode)
	{
	case SeekMode::Begin: _position = std::next(_data.begin(), position); break;
	case SeekMode::Current: std::advance(_position, position); break;
	case SeekMode::End: _position = std::next(_data.end(), -static_cast<int32_t>(position)); break;
	}
}

} // namespace Donut
