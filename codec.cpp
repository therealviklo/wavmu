#include "codec.h"

namespace
{
	struct BoundCursor
	{
		const unsigned char* cur;
		const unsigned char* end;

		BoundCursor(const unsigned char* data, size_t size) noexcept :
			cur(data),
			end(data + size) {}
		
		unsigned char get()
		{
			if (cur == end) throw WRE(L"Unexpected end of file");
			return *cur++;
		}
	};

	bool readAndCheckId(BoundCursor& cur, const char* id)
	{
		const size_t sl = strlen(id);
		bool ret = true;
		for (size_t i = 0; i < sl; i++)
		{
			const char b = cur.get();
			if (b != id[i]) ret = false;
		}
		return ret;
	};

	template <std::unsigned_integral T>
	T read(BoundCursor& cur)
	{
		T i = 0;
		for (uint8_t i = 0; i < sizeof(T); i++)
		{
			const T b = cur.get();
			i += (b << (i * 8));
		}
		return i;
	};

	void skip(BoundCursor& cur, size_t numBytes)
	{
		for (size_t i = 0; i < numBytes; i++)
			cur.get();
	};
}

namespace Decoder
{
	Wave wave(const unsigned char* data, size_t size)
	{
		BoundCursor cur(data, size);

		if (!readAndCheckId(cur, "RIFF")) throw WRE(L"Unsupported format");
		read<uint32_t>(cur); // Bryr mig inte om denna men den måste läsas.
		if (!readAndCheckId(cur, "WAVE")) throw WRE(L"Unsupported format");

		// Skippar till "fmt "-headern.
		while (!readAndCheckId(cur, "fmt ")) skip(cur, read<uint32_t>(cur));

		Wave::Info info;
		// Har redan läst in id:t.
		if (read<uint32_t>(cur) != 16) throw WRE(L"Unsupported format"); // Storleken
		if (read<uint16_t>(cur) != 1) throw WRE(L"Unsupported format"); // Formatet (ska vara PCM)
		info.channels = read<uint16_t>(cur);
		if (info.channels != 2 && info.channels != 1) throw WRE(L"Unsupported number of channels");
		info.sampleRate = read<uint32_t>(cur);
		read<uint32_t>(cur); // byteRate
		read<uint16_t>(cur); // blockAlign
		uint32_t bitsPerSample = read<uint16_t>(cur);
		if (bitsPerSample != 8 &&
			bitsPerSample != 16 &&
			bitsPerSample != 24 &&
			bitsPerSample != 32) throw WRE(L"Unsupported bit depth");

		auto readSample = [&]() -> Sample {
			uint32_t n = 0;
			for (size_t i = 0; i < bitsPerSample; i += 8)
			{
				const uint32_t b = cur.get();
				n += (b << i);
			}

			if (n & (1 << (bitsPerSample - 1)))
			{
				for (size_t i = bitsPerSample; i < 32; i += 8)
				{
					n += (0xff << i);
				}
			}

			return n;
		};
		auto readSample8bit = [&]() -> Sample { // Om bitdjupet är 8 bitar så blir det lite speciellt.
			uint32_t n = cur.get();
			if (n & (1 << 7))
			{
				for (size_t i = 8; i < 32; i += 8)
				{
					n += (0xff << i);
				}
			}

			n += 0x7fffffff;

			return n;
		};

		// Skippar till "data"-headern.
		while (!readAndCheckId(cur, "data")) skip(cur, read<uint32_t>(cur));

		// Har redan läst in id:t.
		const size_t numSamples = read<uint32_t>(cur) / (bitsPerSample / 8);
		std::vector<Sample> sdata(numSamples);
		if (bitsPerSample == 8)
		{
			for (size_t i = 0; i < numSamples; i++)
				sdata[i] = readSample8bit();
		}
		else
		{
			for (size_t i = 0; i < numSamples; i++)
				sdata[i] = readSample();
		}
		
		return Wave(std::move(sdata), info);
	}
}

Wave decodeFile(const char* file)
{
	auto tolc = [](const std::string& str) -> std::string {
		std::string cpy = str;
		std::transform(
			str.begin(),
			str.end(),
			cpy.begin(),
			[](unsigned char c) -> unsigned char { return std::tolower(c); }
		);
		return cpy;
	};
	const std::string ext = tolc(std::filesystem::path(file).extension().string());
	if (ext == "wav" ||
		ext == "wave")
	{
		const std::vector<unsigned char> data = readfile(file);
		return Decoder::wave(data.data(), data.size());
	}
	throw WRE(L"Unknown file type");
}