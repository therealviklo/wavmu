#pragma once
#include <vector>
#include "win.h"
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <wincodec.h>
#include "winerror.h"
#include <Shlwapi.h>

using Microsoft::WRL::ComPtr;

class D2DFactory
{
	friend class RenderTarget;
	friend class PolygonGeometry;
private:
	ComPtr<ID2D1Factory> factory;
public:
	D2DFactory();
};

class WICFactory
{
	friend class Bitmap;
private:
	ComPtr<IWICImagingFactory> factory;
public:
	WICFactory();
};

class DWFactory
{
	friend class Font;
private:
	ComPtr<IDWriteFactory> factory;
public:
	DWFactory();
};

class RenderTarget
{
	friend class Bitmap;
	friend class SolidBrush;
private:
	HWND hWnd;
	ComPtr<ID2D1HwndRenderTarget> rt;
	unsigned long long ver;

	void createRenderTarget(D2DFactory& d2dfac);
public:
	RenderTarget(HWND hWnd, D2DFactory& d2dfac);

	constexpr void beginDraw() noexcept { rt->BeginDraw(); }
	// Returnerar false om rendertargeten återskapades
	constexpr bool endDraw(D2DFactory& d2dfac)
	{
		HRESULT hr;
		if (FAILED(hr = rt->EndDraw()))
		{
			if (hr == D2DERR_RECREATE_TARGET)
			{
				createRenderTarget(d2dfac);
				return false;
			}
			else
			{
				throw WinError(L"Direct2D drawing error", hr);
			}
		}
		return true;
	}

	constexpr void resize(unsigned int w, unsigned int h)
	{
		rt->Resize(D2D1::SizeU(w, h));
	}

	void drawBitmap(Bitmap& bitmap, float x, float y, float w, float h, float alpha = 1.0f) noexcept;
	void drawBitmap(Bitmap& bitmap, D2D1_RECT_F dest, D2D1_RECT_F src, float alpha = 1.0f) noexcept;
	void clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) noexcept
	{
		rt->Clear(D2D1::ColorF(r, g, b, a));
	}
	void drawLine(D2D1_POINT_2F a, D2D1_POINT_2F b, SolidBrush& brush, float strokeWidth) noexcept;
	void drawRectangle(const D2D1_RECT_F& rect, SolidBrush& brush) noexcept;
	void outlineRectangle(const D2D1_RECT_F& rect, SolidBrush& brush, float strokeWidth) noexcept;
	void drawPolygon(PolygonGeometry& pg, D2D1_POINT_2F origin, SolidBrush& brush) noexcept;
	void drawText(const std::wstring& str, Font& font, const D2D1_RECT_F& rect, SolidBrush& brush, bool clip = true);
};

class Bitmap
{
	friend class RenderTarget;
private:
	unsigned long long ver;
	ComPtr<IWICFormatConverter> fmtCnv;
	ComPtr<ID2D1Bitmap> bmp;
	unsigned w;
	unsigned h;

	void recreateIfOutdated(RenderTarget& rt);
	void initialiseFromDecoder(IWICBitmapDecoder* decoder, WICFactory& wicfac, RenderTarget& rt);
public:
	Bitmap(const wchar_t* filename, WICFactory& wicfac, RenderTarget& rt);
	Bitmap(const char* data, size_t size, WICFactory& wicfac, RenderTarget& rt);

	constexpr unsigned getWidth() const noexcept { return w; }
	constexpr unsigned getHeight() const noexcept { return h; }
};

class SolidBrush
{
	friend class RenderTarget;
private:
	ComPtr<ID2D1SolidColorBrush> brush;
public:
	SolidBrush(D2D1_COLOR_F colour, RenderTarget& rt);
};

class PolygonGeometry
{
	friend class RenderTarget;
private:
	ComPtr<ID2D1PathGeometry> geo;
public:
	PolygonGeometry(D2DFactory& d2dfac, const std::vector<D2D1_POINT_2F>& points);
};

class Font
{
	friend class RenderTarget;
private:
	ComPtr<IDWriteTextFormat> font;
public:
	Font(
		DWFactory& dwfac,
		const wchar_t* fontname,
		DWRITE_FONT_WEIGHT fweight,
		DWRITE_FONT_STYLE fstyle,
		DWRITE_FONT_STRETCH fstretch,
		float size,
		const wchar_t* locale = L"en-us"
	);
};