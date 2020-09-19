#include "wav.h"

Wave::Wave(const char* file)
{
	std::ifstream fs(file, std::ios_base::in | std::ios_base::binary);
	auto readAndCheckId = [&](const char* id) -> bool {
		const size_t sl = strlen(id);
		for (size_t i = 0; i < sl; i++)
		{
			const int b = fs.get();
			if (b == EOF) throw Exception("Unexpected end of file");
			if (b != id[i]) return false;
		}
		return true;
	};
	auto readU32 = [&]() -> uint32_t {
		uint32_t i = 0;
		for (uint8_t i = 0; i < sizeof(uint32_t); i++)
		{
			const int b = fs.get();
			if (b == EOF) throw Exception("Unexpected end of file");
			i += (b << (i * 8));
		}
		return i;
	};
	auto readU16 = [&]() -> uint16_t {
		uint16_t i = 0;
		for (uint8_t i = 0; i < sizeof(uint16_t); i++)
		{
			const int b = fs.get();
			if (b == EOF) throw Exception("Unexpected end of file");
			i += (b << (i * 8));
		}
		return i;
	};
	auto skip = [&](size_t numBytes) -> void {
		for (size_t i = 0; i < numBytes; i++)
			if (fs.get() == EOF) throw Exception("Unexpected end of file");
	};

	if (!readAndCheckId("RIFF")) throw Exception("Unsupported format");
	readU32(); // Bryr mig inte om denna men den måste läsas.
	if (!readAndCheckId("WAVE")) throw Exception("Unsupported format");

	// Skippar till "fmt "-headern.
	while (!readAndCheckId("fmt ")) skip(readU32());

	// Har redan läst in id:t.
	if (readU32() != 16) throw Exception("Unsupported format"); // Storleken
	if (readU16() != 1) throw Exception("Unsupported format"); // Formatet (ska vara PCM)
	info.channels = readU16();
	if (info.channels != 2 && info.channels != 1) throw Exception("Unsupported number of channels");
	info.sampleRate = readU32();
	uint32_t byteRate = readU32();
	uint32_t blockAlign = readU16();
	uint32_t bitsPerSample = readU16();
	if (bitsPerSample != 8 &&
		bitsPerSample != 16 &&
		bitsPerSample != 24 &&
		bitsPerSample != 32) throw Exception("Unsupported bit depth");

	auto readSample = [&]() -> Sample {
		uint32_t n = 0;
		for (size_t i = 0; i < bitsPerSample; i += 8)
		{
			const int b = fs.get();
			if (b == EOF) throw Exception("Unexpected end of file");
			n += (b << i);
		}

		if (n & (1 << (bitsPerSample - 1)))
		{
			for (size_t i = bitsPerSample; i < 32; i += 8)
			{
				n += (0xff << i);
			}
		}

		return *(int32_t*)&n;
	};
	auto readSample8bit = [&]() -> Sample { // Om bitdjupet är 8 bitar så blir det lite speciellt.
		uint32_t n = fs.get();
		if (n == EOF) throw Exception("Unexpected end of file");

		if (n & (1 << 7))
		{
			for (size_t i = 8; i < 32; i += 8)
			{
				n += (0xff << i);
			}
		}

		n += 0x7fffffff;

		return *(int32_t*)&n;
	};

	// Skippar till "data"-headern.
	while (!readAndCheckId("data")) skip(readU32());

	// Har redan läst in id:t.
	const size_t numSamples = readU32() / (bitsPerSample / 8);
	data = std::vector<Sample>(numSamples);
	if (bitsPerSample == 8)
	{
		for (size_t i = 0; i < numSamples; i++)
			data[i] = readSample8bit();
	}
	else
	{
		for (size_t i = 0; i < numSamples; i++)
			data[i] = readSample();
	}
}

Sample Wave::at(uint32_t sampleNum, uint32_t sampleRate, uint16_t channel, double pitchShift) const
{
	const double samplePos = fmod(sampleNum / (double)sampleRate / pitchShift * info.sampleRate, data.size() / info.channels);
	return std::lerp(
		data.at(floor(samplePos) * info.channels + channel),
		data.at(floor(samplePos) * info.channels + channel + 1),
		fmod(samplePos, 1.0)
	);
}