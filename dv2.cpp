#include "dv2.h"

constexpr const uint8_t vertexShaderData[] = {
     68,  88,  66,  67,  32, 228, 
    217, 167,   8,  78, 245, 245, 
    210, 217, 153,  99,  97, 159, 
     18,  54,   1,   0,   0,   0, 
    196,   4,   0,   0,   6,   0, 
      0,   0,  56,   0,   0,   0, 
     84,   1,   0,   0, 196,   2, 
      0,   0,  64,   3,   0,   0, 
     24,   4,   0,   0, 108,   4, 
      0,   0,  65, 111, 110,  57, 
     20,   1,   0,   0,  20,   1, 
      0,   0,   0,   2, 254, 255, 
    224,   0,   0,   0,  52,   0, 
      0,   0,   1,   0,  36,   0, 
      0,   0,  48,   0,   0,   0, 
     48,   0,   0,   0,  36,   0, 
      1,   0,  48,   0,   0,   0, 
      0,   0,   6,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   2, 254, 255, 
     81,   0,   0,   5,   7,   0, 
     15, 160,   0,   0, 128,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     31,   0,   0,   2,   5,   0, 
      0, 128,   0,   0,  15, 144, 
     31,   0,   0,   2,   5,   0, 
      1, 128,   1,   0,  15, 144, 
      4,   0,   0,   4,   0,   0, 
      7, 128,   1,   0, 196, 144, 
      7,   0, 208, 160,   7,   0, 
    197, 160,   8,   0,   0,   3, 
      0,   0,   1, 224,   0,   0, 
    228, 128,   5,   0, 244, 160, 
      8,   0,   0,   3,   0,   0, 
      2, 224,   0,   0, 228, 128, 
      6,   0, 244, 160,   4,   0, 
      0,   4,   0,   0,   7, 128, 
      0,   0, 196, 144,   7,   0, 
    208, 160,   7,   0, 197, 160, 
      8,   0,   0,   3,   0,   0, 
      4, 192,   0,   0, 228, 128, 
      3,   0, 244, 160,   8,   0, 
      0,   3,   1,   0,   1, 128, 
      0,   0, 228, 128,   1,   0, 
    244, 160,   8,   0,   0,   3, 
      1,   0,   2, 128,   0,   0, 
    228, 128,   2,   0, 244, 160, 
      8,   0,   0,   3,   0,   0, 
      1, 128,   0,   0, 228, 128, 
      4,   0, 244, 160,   4,   0, 
      0,   4,   0,   0,   3, 192, 
      0,   0,   0, 128,   0,   0, 
    228, 160,   1,   0, 228, 128, 
      1,   0,   0,   2,   0,   0, 
      8, 192,   0,   0,   0, 128, 
    255, 255,   0,   0,  83,  72, 
     68,  82, 104,   1,   0,   0, 
     64,   0,   1,   0,  90,   0, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   0,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   1,   0,   0,   0, 
    101,   0,   0,   3,  50,  32, 
     16,   0,   0,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 104,   0, 
      0,   2,   1,   0,   0,   0, 
     54,   0,   0,   5,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  16,   0, 
      0,   8,  18,  32,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     70, 131,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     16,   0,   0,   8,  34,  32, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70, 131,  32,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  54,   0,   0,   5, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     16,   0,   0,   8,  18,  32, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70, 131,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  16,   0,   0,   8, 
     34,  32,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  70, 131, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  16,   0, 
      0,   8,  66,  32,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     70, 131,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     16,   0,   0,   8, 130,  32, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70, 131,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0,  11,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  82,  68, 
     69,  70, 208,   0,   0,   0, 
      1,   0,   0,   0,  68,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,   0,   1,   0,   0, 
    167,   0,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     67,  66, 117, 102,   0, 171, 
    171, 171,  60,   0,   0,   0, 
      2,   0,   0,   0,  92,   0, 
      0,   0, 128,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 140,   0,   0,   0, 
      0,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
    144,   0,   0,   0,   0,   0, 
      0,   0, 160,   0,   0,   0, 
     64,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
    144,   0,   0,   0,   0,   0, 
      0,   0, 109, 116, 120,   0, 
      3,   0,   3,   0,   4,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 116, 101, 
    120, 109, 116, 120,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  49,  48, 
     46,  49,   0, 171,  73,  83, 
     71,  78,  76,   0,   0,   0, 
      2,   0,   0,   0,   8,   0, 
      0,   0,  56,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   3, 
      0,   0,  65,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   3,   3, 
      0,   0,  80, 111, 115, 105, 
    116, 105, 111, 110,   0,  84, 
    101, 120,  67, 111, 111, 114, 
    100,   0, 171, 171,  79,  83, 
     71,  78,  80,   0,   0,   0, 
      2,   0,   0,   0,   8,   0, 
      0,   0,  56,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,  12, 
      0,   0,  65,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  15,   0, 
      0,   0,  84, 101, 120,  67, 
    111, 111, 114, 100,   0,  83, 
     86,  95,  80, 111, 115, 105, 
    116, 105, 111, 110,   0, 171, 
    171, 171
};

constexpr const uint8_t pixelShaderData[] =
{
     68,  88,  66,  67, 135, 208, 
    249, 177, 136,  47, 163, 101, 
     41, 211,  31,   5, 136, 116, 
    252, 134,   1,   0,   0,   0, 
    136,   2,   0,   0,   6,   0, 
      0,   0,  56,   0,   0,   0, 
    164,   0,   0,   0,  16,   1, 
      0,   0, 140,   1,   0,   0, 
     32,   2,   0,   0,  84,   2, 
      0,   0,  65, 111, 110,  57, 
    100,   0,   0,   0, 100,   0, 
      0,   0,   0,   2, 255, 255, 
     60,   0,   0,   0,  40,   0, 
      0,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     40,   0,   1,   0,  36,   0, 
      0,   0,  40,   0,   0,   0, 
      0,   0,   1,   2, 255, 255, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   0,   0,   3, 176, 
     31,   0,   0,   2,   0,   0, 
      0, 144,   0,   8,  15, 160, 
     66,   0,   0,   3,   0,   0, 
     15, 128,   0,   0, 228, 176, 
      0,   8, 228, 160,   1,   0, 
      0,   2,   0,   8,  15, 128, 
      0,   0, 228, 128, 255, 255, 
      0,   0,  83,  72,  68,  82, 
    100,   0,   0,   0,  64,   0, 
      0,   0,  25,   0,   0,   0, 
     90,   0,   0,   3,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   0,   0,   0,   0, 
     85,  85,   0,   0,  98,  16, 
      0,   3,  50,  16,  16,   0, 
      0,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      0,   0,   0,   0,  69,   0, 
      0,   9, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  16, 
     16,   0,   0,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    116,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     82,  68,  69,  70, 140,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  28,   0,   0,   0, 
      0,   4, 255, 255,   0,   1, 
      0,   0,  99,   0,   0,   0, 
     92,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  95,   0,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     12,   0,   0,   0, 115, 115, 
      0, 116, 101, 120,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  49,  48, 
     46,  49,   0, 171,  73,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   3, 
      0,   0,  84, 101, 120,  67, 
    111, 111, 114, 100,   0, 171, 
    171, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  97, 114, 
    103, 101, 116,   0, 171, 171
};

Texture::Texture(const wchar_t* filename, ID3D11Device* device, IWICImagingFactory* wicFactory)
{
	using Exception = DV2::Exception;

	ComPtr<IWICBitmapDecoder> decoder;
	if (FAILED(
		wicFactory->CreateDecoderFromFilename(
			filename,
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder
		)
	)) throw Exception("Failed to create decoder for image file");

	ComPtr<IWICBitmapFrameDecode> frame;
	if (FAILED(
		decoder->GetFrame(0, &frame)
	)) throw Exception("Failed to get image frame");

	WICPixelFormatGUID pixelFormat;
	if (FAILED(
		frame->GetPixelFormat(&pixelFormat)
	)) throw Exception("Failed to get pixel format");

	size_t rowPitch = 0;
	size_t bufSize = 0;
	std::unique_ptr<uint8_t[]> buf;
	if (memcmp(&pixelFormat, &GUID_WICPixelFormat32bppBGRA, sizeof(WICPixelFormatGUID)) != 0)
	{
		// Måste konvertera formatet.

		ComPtr<IWICFormatConverter> formatConverter;
		if (FAILED(wicFactory->CreateFormatConverter(&formatConverter))) throw Exception("Failed to create WIC format converter");

		if (FAILED(
			formatConverter->Initialize(
				frame.Get(),
				GUID_WICPixelFormat32bppBGRA,
				WICBitmapDitherTypeNone,
				nullptr,
				0.0,
				WICBitmapPaletteTypeCustom
			)
		)) throw Exception("Failed to initialise WIC format converter");

		if (FAILED(formatConverter->GetSize(&width, &height))) throw Exception("Failed to get image size");

		WICPixelFormatGUID newPixelFormat;
		if (FAILED(formatConverter->GetPixelFormat(&newPixelFormat))) throw Exception("Failed to get pixel format");

		if (memcmp(&newPixelFormat, &GUID_WICPixelFormat32bppBGRA, sizeof(WICPixelFormatGUID)) != 0)
			throw Exception("Failed to convert image format"); // Detta borde aldrig hända.

		ComPtr<IWICComponentInfo> compinfo;
		if (FAILED(wicFactory->CreateComponentInfo(newPixelFormat, &compinfo))) throw Exception("Failed to get pixel format info");

		ComPtr<IWICPixelFormatInfo> pfi;
		if (FAILED(
			compinfo->QueryInterface(
				__uuidof(IWICPixelFormatInfo),
				static_cast<void**>(&pfi)
			)
		)) throw Exception("Failed to get pixel format info");

		UINT bpp;
		if (FAILED(pfi->GetBitsPerPixel(&bpp))) throw Exception("Failed to get bits per pixel");

		rowPitch = (width * bpp + 7) / 8;
		bufSize = rowPitch * height;
		buf = std::make_unique<uint8_t[]>(bufSize);

		if (FAILED(formatConverter->CopyPixels(nullptr, rowPitch, bufSize, buf.get()))) throw Exception("Failed to copy pixels");
	}
	else
	{
		// Ingen konvertering behövs.
		if (FAILED(frame->GetSize(&width, &height))) throw Exception("Failed to get image size");

		ComPtr<IWICComponentInfo> compinfo;
		if (FAILED(wicFactory->CreateComponentInfo(pixelFormat, &compinfo))) throw Exception("Failed to get pixel format info");
		
		ComPtr<IWICPixelFormatInfo> pfi;
		if (FAILED(
			compinfo->QueryInterface(
				__uuidof(IWICPixelFormatInfo),
				static_cast<void**>(&pfi)
			)
		)) throw Exception("Failed to get pixel format info");

		UINT bpp;
		if (FAILED(pfi->GetBitsPerPixel(&bpp))) throw Exception("Failed to get bits per pixel");

		rowPitch = (width * bpp + 7) / 8;
		bufSize = rowPitch * height;
		buf = std::make_unique<uint8_t[]>(bufSize);

		if (FAILED(frame->CopyPixels(nullptr, rowPitch, bufSize, buf.get()))) throw Exception("Failed to copy pixels");
	}

	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.MipLevels = 1;
	D3D11_SUBRESOURCE_DATA texSd{};
	texSd.pSysMem = buf.get();
	texSd.SysMemPitch = static_cast<UINT>(rowPitch);
	if (static_cast<decltype(rowPitch)>(texSd.SysMemPitch) != rowPitch) throw Exception("Size error");
	texSd.SysMemSlicePitch = static_cast<UINT>(bufSize);
	if (static_cast<decltype(bufSize)>(texSd.SysMemSlicePitch) != bufSize) throw Exception("Size error");
	if (FAILED(
		device->CreateTexture2D(
			&texDesc,
			&texSd,
			&tex
		)
	)) throw Exception("Failed to create texture");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
	srvd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	if (FAILED(device->CreateShaderResourceView(tex.Get(), &srvd, &texView)))
		throw Exception("Failed to create texture view");
}

void DV2::createSwapChain()
{
	ComPtr<ID3D11Resource> backBuffer;
	if (FAILED(swap->GetBuffer(
		0,
		__uuidof(ID3D11Resource),
		&backBuffer
	))) throw Exception("Failed to get back buffer");

	if (FAILED(device->CreateRenderTargetView(
		backBuffer.Get(),
		nullptr,
		&target
	))) throw Exception("Failed to create render target view");
	
	struct Vertex
	{
		float x;
		float y;
		float u;
		float v;
	};
	const Vertex vertices[] = {
		{-1.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 1.0f, 0.0f},
		{1.0f, -1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, -1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, 0.0f, 1.0f},
	};

	D3D11_BUFFER_DESC bd{};
	bd.ByteWidth = sizeof(vertices);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA srd{};
	srd.pSysMem = vertices;
	if (FAILED(device->CreateBuffer(&bd, &srd, &vertexBuffer))) throw Exception("Failed to create vertex buffer");
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	const struct {
		float mtx[4][4] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		float texmtx[4][4] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	} mtcs;
	D3D11_BUFFER_DESC mbd{};
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mbd.ByteWidth = sizeof(mtcs);
	D3D11_SUBRESOURCE_DATA msd{};
	msd.pSysMem = &mtcs;
	if (FAILED(
		device->CreateBuffer(
			&mbd,
			&msd,
			&matrixBuffer
		)
	)) throw Exception("Failed to create matrix buffer");
	context->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());

	if (FAILED(
		device->CreatePixelShader(
			pixelShaderData,
			sizeof(pixelShaderData),
			nullptr,
			&pixelShader
		)
	)) throw Exception("Failed to create pixel shader");
	context->PSSetShader(pixelShader.Get(), nullptr, 0);

	if (FAILED(
		device->CreateVertexShader(
			vertexShaderData,
			sizeof(vertexShaderData),
			nullptr,
			&vertexShader
		)
	)) throw Exception("Failed to create vertex shader");
	context->VSSetShader(vertexShader.Get(), nullptr, 0);

	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	if (FAILED(
		device->CreateInputLayout(
			ied,
			2,
			vertexShaderData,
			sizeof(vertexShaderData),
			&inputLayout
		)
	)) throw Exception("Failed to create input layout");
	context->IASetInputLayout(inputLayout.Get());

	context->OMSetRenderTargets(1, target.GetAddressOf(), nullptr);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	RECT wndSize;
	if (!GetClientRect(hWnd, &wndSize)) throw Exception("Failed to get window size");
	this->width = wndSize.right;
	this->height = wndSize.bottom;
	
	D3D11_VIEWPORT vp{};
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	context->RSSetViewports(1, &vp);

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	if (FAILED(device->CreateSamplerState(&samplerDesc, &samplerState))) throw Exception("Failed to create sampler state");

	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	D3D11_BLEND_DESC bsd{};
	bsd.RenderTarget[0].BlendEnable = TRUE;
	bsd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bsd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bsd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	bsd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bsd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bsd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	if (FAILED(device->CreateBlendState(&bsd, &blendState))) throw Exception("Failed to create blend state");

	float blendFactor[4] = {0, 0, 0, 0};
	context->OMSetBlendState(blendState.Get(), blendFactor, 0xffffffff);
}

void DV2::destroySwapChain()
{
	samplerState.Reset();
	blendState.Reset();
	inputLayout.Reset();
	vertexShader.Reset();
	pixelShader.Reset();
	matrixBuffer.Reset();
	vertexBuffer.Reset();

	target.Reset();
}

DV2::DV2(HWND hWnd)
	: hWnd(hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&swap,
		&device,
		nullptr,
		&context
	))) throw Exception("Failed to initialise Direct3D 11");

	createSwapChain();

    if (FAILED(
        CoCreateInstance(
            CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			__uuidof(IWICImagingFactory),
			&wicFactory
        )
    )) throw Exception("Failed to create WIC factory");
}

DV2::~DV2()
{
	try
	{
		swap->SetFullscreenState(false, nullptr);
	} catch (...) {}
}

void DV2::resize()
{
	destroySwapChain();
	context->ClearState();
	if (FAILED(swap->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0))) throw Exception("Failed to resize buffers");
	createSwapChain();
}

void DV2::setFullscreen(bool on)
{
	HRESULT hr = swap->SetFullscreenState(on, nullptr);
	if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE || hr == DXGI_STATUS_MODE_CHANGE_IN_PROGRESS)
	{
		throw NoFullscreenChange();
	}
	else if (FAILED(hr))
	{
		throw Exception("Failed to set fullscreen state");
	}
}

Texture DV2::createTexture(const wchar_t* filename)
{
	return Texture(filename, device.Get(), wicFactory.Get());
}

void DV2::clear()
{
	const float clrArr[] = {0.0f, 0.0f, 0.0f, 1.0f};
	context->ClearRenderTargetView(target.Get(), clrArr);
}

void DV2::clear(Colour clr)
{
	const float clrArr[] = {clr.r, clr.g, clr.b, clr.a};
	context->ClearRenderTargetView(target.Get(), clrArr);
}

void DV2::draw(
	Texture& texture,
	float x,
	float y,
	float width,
	float height,
	float srcX,
	float srcY,
	float srcWidth,
	float srcHeight,
	float angle)
{
	context->PSSetShaderResources(0, 1, texture.texView.GetAddressOf());

	struct {
		DirectX::XMMATRIX mtx;
		DirectX::XMMATRIX texmtx;
	} mtcs;

	mtcs.mtx = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixScaling(width, height, 1.0f) *
		DirectX::XMMatrixRotationZ(angle) *
		DirectX::XMMatrixTranslation(x, y, 0.0f) *
		DirectX::XMMatrixScaling(1.0f / this->width, 1.0f / this->height, 1.0f)
	);
	mtcs.texmtx = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixScaling(texture.width / srcWidth, texture.height / srcHeight, 1.0f) *
		DirectX::XMMatrixTranslation(srcX / texture.width, srcY / texture.height, 0.0f)
	);
	D3D11_MAPPED_SUBRESOURCE msr;
	if (FAILED(context->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr))) throw Exception("Failed to map buffer");
	memcpy(msr.pData, &mtcs, sizeof(mtcs));
	context->Unmap(matrixBuffer.Get(), 0);

	context->Draw(6, 0);
}

void DV2::presentSync()
{
	swap->Present(1, 0);
}

void DV2::presentSync(unsigned int syncInterval)
{
	swap->Present(syncInterval, 0);
}

void DV2::presentNoSync()
{
	swap->Present(0, 0);
}

void DV2::changeScreenResolution(int width, int height)
{
	DEVMODEW devMode{};
	devMode.dmSize = sizeof(devMode);
	devMode.dmPelsWidth = width;
	devMode.dmPelsHeight = height;
	devMode.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;
	LONG res = ChangeDisplaySettingsW(&devMode, CDS_FULLSCREEN);
	switch (res)
	{
		case DISP_CHANGE_BADDUALVIEW:
		{
			throw Exception("Could not change the screen resolution because the system is DualView capable.");
		}
		break;
		case DISP_CHANGE_BADFLAGS:
		{
			throw Exception("Could not change the screen resolution because the flags were invalid.");
		}
		break;
		case DISP_CHANGE_BADMODE:
		{
			throw Exception("Could not change the screen resolution because the screen resolution is not supported.");
		}
		break;
		case DISP_CHANGE_BADPARAM:
		{
			throw Exception("Could not change the screen resolution because the parameters were invalid.");
		}
		break;
		case DISP_CHANGE_FAILED:
		{
			throw Exception("Could not change the screen resolution because the display driver failed the graphics mode.");
		}
		break;
		case DISP_CHANGE_NOTUPDATED:
		{
			throw Exception("Could not change the screen resolution because the settings could not be written to the registry.");
		}
		break;
		case DISP_CHANGE_RESTART:
		{
			throw Exception("The computer must be restarted for the change to go into effect.");
		}
		break;
		default:
		break;
	}
}