#include "d2d.h"

D2DFactory::D2DFactory()
{
	HRESULT hr = 0;

	if (FAILED(hr = D2D1CreateFactory<ID2D1Factory>(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&factory
	))) throw WinError(L"Failed to create Direct2D factory", hr);
}

WICFactory::WICFactory()
{
	HRESULT hr = 0;

	if (FAILED(hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWICImagingFactory),
		&factory
	))) throw WinError(L"Failed to create WIC factory", hr);
}

void RenderTarget::createRenderTarget(D2DFactory& d2dfac)
{
	HRESULT hr = 0;

	ver++;

	RECT rc;
	if (GetClientRect(hWnd, &rc) == 0) throw WinError(L"Failed to get window client area");

	if (FAILED(hr = d2dfac.factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_SOFTWARE,
			{DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE}
		),
		D2D1::HwndRenderTargetProperties(
			hWnd,
			D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top)
		),
		&rt
	))) throw WinError(L"Failed to create hWnd render target", hr);

	rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
}

void RenderTarget::drawBitmap(Bitmap& bitmap, float x, float y, float w, float h, float alpha) noexcept
{
	bitmap.recreateIfOutdated(*this);
	rt->DrawBitmap(
		bitmap.bmp.Get(),
		D2D1::RectF(x, y, x + w, y + h),
		alpha,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		nullptr
	);
}

void RenderTarget::drawBitmap(Bitmap& bitmap, D2D1_RECT_F dest, D2D1_RECT_F src, float alpha) noexcept
{
	bitmap.recreateIfOutdated(*this);
	rt->DrawBitmap(
		bitmap.bmp.Get(),
		dest,
		alpha,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		src
	);
}

void RenderTarget::drawLine(D2D1_POINT_2F a, D2D1_POINT_2F b, SolidBrush& brush, float strokeWidth) noexcept
{
	rt->DrawLine(
		a,
		b,
		brush.brush.Get(),
		strokeWidth,
		nullptr
	);	
}

void RenderTarget::drawRectangle(const D2D1_RECT_F& rect, SolidBrush& brush) noexcept
{
	rt->FillRectangle(rect, brush.brush.Get());
}

void RenderTarget::outlineRectangle(const D2D1_RECT_F& rect, SolidBrush& brush, float strokeWidth) noexcept
{
	rt->DrawRectangle(rect, brush.brush.Get(), strokeWidth);
}

void RenderTarget::drawPolygon(PolygonGeometry& pg, D2D1_POINT_2F origin, SolidBrush& brush) noexcept
{
	D2D1_MATRIX_3X2_F trans;
	rt->GetTransform(&trans);
	rt->SetTransform(D2D1::Matrix3x2F::Translation(origin.x, origin.y) * trans);
	rt->FillGeometry(pg.geo.Get(), brush.brush.Get());
	rt->SetTransform(trans);
}

RenderTarget::RenderTarget(HWND hWnd, D2DFactory& d2dfac) :
	hWnd(hWnd),
	ver(0)
{
	createRenderTarget(d2dfac);
}

void Bitmap::recreateIfOutdated(RenderTarget& rt)
{
	if (ver != rt.ver)
	{
		hrthrow(rt.rt->CreateBitmapFromWicBitmap(
					fmtCnv.Get(),
					&bmp
				),
				L"Failed to create Direct2D bitmap");
		ver = rt.ver;
	}
}

void Bitmap::initialiseFromDecoder(IWICBitmapDecoder* decoder, WICFactory& wicfac, RenderTarget& rt)
{
	HRESULT hr = 0;

	ComPtr<IWICBitmapFrameDecode> bmpDcd;
	if (FAILED(hr = decoder->GetFrame(
		0,
		&bmpDcd
	))) throw WinError(L"Failed to decode image", hr);

	if (FAILED(hr = wicfac.factory->CreateFormatConverter(&fmtCnv)))
		throw WinError(L"Failed to create format converter", hr);

	if (FAILED(hr = fmtCnv->Initialize(
		bmpDcd.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0F,
		WICBitmapPaletteTypeCustom
	))) throw WinError(L"Failed to initalise format converter", hr);
	
	if (FAILED(hr = rt.rt->CreateBitmapFromWicBitmap(
		fmtCnv.Get(),
		&bmp
	))) throw WinError(L"Failed to create Direct2D bitmap", hr);

	const auto size = bmp->GetPixelSize();
	w = size.width;
	h = size.height;
}

Bitmap::Bitmap(const wchar_t* filename, WICFactory& wicfac, RenderTarget& rt) :
	ver(rt.ver)
{
	HRESULT hr = 0;

	ComPtr<IWICBitmapDecoder> decoder;
	if (FAILED(hr = wicfac.factory->CreateDecoderFromFilename(
		filename,
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&decoder
	))) throw WinError(L"Failed to create WIC image decoder", hr);

	initialiseFromDecoder(decoder.Get(), wicfac, rt);
}

Bitmap::Bitmap(const char* data, size_t size, WICFactory& wicfac, RenderTarget& rt)
{
	HRESULT hr = 0;

	const ComPtr<IStream> memstream(SHCreateMemStream((const BYTE*)data, size));
	if (!memstream) throw NoResWinError(L"Failed to create memory stream");

	ComPtr<IWICBitmapDecoder> decoder;
	if (FAILED(hr = wicfac.factory->CreateDecoderFromStream(
		memstream.Get(),
		nullptr,
		WICDecodeMetadataCacheOnDemand,
		&decoder
	))) throw WinError(L"Failed to create WIC image decoder", hr);

	initialiseFromDecoder(decoder.Get(), wicfac, rt);
}

SolidBrush::SolidBrush(D2D1_COLOR_F colour, RenderTarget& rt)
{
	hrthrow(rt.rt->CreateSolidColorBrush(
				colour,
				&brush
			),
			L"Failed to create solid colour brush");
}

PolygonGeometry::PolygonGeometry(D2DFactory& d2dfac, const std::vector<D2D1_POINT_2F>& points)
{
	if (!points.size())
		throw WRE(L"Empty polygon");
	hrthrow(d2dfac.factory->CreatePathGeometry(&geo),
			L"Failed to create path geometry");
	ComPtr<ID2D1GeometrySink> sink;
	hrthrow(geo->Open(&sink),
			L"Failed to open geometry sink");
	sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
	for (size_t i = 1; i < points.size(); i++)
	{
		sink->AddLine(points[i]);
	}
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	hrthrow(sink->Close(),
			L"Failed to close figure");
}