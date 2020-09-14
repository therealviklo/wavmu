#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <d3d11.h>
#include <stdexcept>
#include <wrl.h>
#include <directxmath.h>
#include <wincodec.h>
#include <memory>

using Microsoft::WRL::ComPtr;

/* Länka med:
   - user32
   - d3d11 */

struct Colour
{
	float r;
	float g;
	float b;
	float a;
};

class DV2;

class Texture
{
	friend class DV2;
private:
	UINT width;
	UINT height;
	ComPtr<ID3D11Texture2D> tex;
	ComPtr<ID3D11ShaderResourceView> texView;

	Texture(const wchar_t* filename, ID3D11Device* device, IWICImagingFactory* wicFactory);
public:
	UINT getWidth() const noexcept {return width;}
	UINT getHeight() const noexcept {return height;}
};

class DV2
{
public:
	class Exception : public std::runtime_error
	{
	public:
		Exception(const char* msg) : std::runtime_error(msg) {}
	};
	class NoFullscreenChange : public Exception
	{
	public:
		NoFullscreenChange() : Exception("Unable to change fullscreen state") {}
	};
private:
	float width;
	float height;

	HWND hWnd;

	ComPtr<IDXGISwapChain> swap;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	ComPtr<ID3D11RenderTargetView> target;
	
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> matrixBuffer;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11BlendState> blendState;
	ComPtr<ID3D11SamplerState> samplerState;

    ComPtr<IWICImagingFactory> wicFactory;

	void createSwapChain();
	void destroySwapChain();
public:
	DV2(HWND hWnd);
	~DV2();

	DV2(const DV2&) = delete;
	DV2& operator=(const DV2&) = delete;

	/* Denna funktion återskapar många saker. Det betyder att om denna
	   funktion kastar en exception så går det nog inte att använda
	   DV2-klassen längre, eftersom vissa saker kanske inte är återskapade. */
	void resize();
	/* Ibland kan det bli så att denna funktion misslyckas men att
	   programmet kan fortsätta sin körning. Om det är så kastar
	   funktionen en exception av typen DV2::NoFullscreenChange istället
	   för en av typen DV2::Exception. (Notera att DV2::NoFullscreenChange
	   är en underklass till DV2::Exception.) */
	void setFullscreen(bool on);

	Texture createTexture(const wchar_t* filename);

	void clear();
	void clear(Colour clr);

	void draw(
		Texture& texture,
		float x,
		float y,
		float width,
		float height,
		float srcX,
		float srcY,
		float srcWidth,
		float srcHeight,
		float angle
	);
	void draw(
		Texture& texture,
		float x,
		float y,
		float width,
		float height,
		float srcX,
		float srcY,
		float srcWidth,
		float srcHeight
	) {draw(texture, x, y, width, height, srcX, srcY, srcWidth, srcHeight, 0.0f);}
	void draw(
		Texture& texture,
		float x,
		float y,
		float width,
		float height,
		float angle
	) {draw(texture, x, y, width, height, 0.0f, 0.0f, texture.getWidth(), texture.getHeight(), angle);}
	void draw(
		Texture& texture,
		float x,
		float y,
		float width,
		float height
	) {draw(texture, x, y, width, height, 0.0f, 0.0f, texture.getWidth(), texture.getHeight(), 0.0f);}
	void draw(
		Texture& texture,
		float x,
		float y,
		float srcX,
		float srcY,
		float srcWidth,
		float srcHeight,
		float angle
	) {draw(texture, x, y, texture.getWidth(), texture.getHeight(), 0.0f, 0.0f, texture.getWidth(), texture.getHeight(), 0.0f);}
	void draw(
		Texture& texture,
		float x,
		float y,
		float srcX,
		float srcY,
		float srcWidth,
		float srcHeight
	) {draw(texture, x, y, texture.getWidth(), texture.getHeight(), srcX, srcY, srcWidth, srcHeight, 0.0f);}
	void draw(
		Texture& texture,
		float x,
		float y,
		float angle
	) {draw(texture, x, y, texture.getWidth(), texture.getHeight(), 0.0f, 0.0f, texture.getWidth(), texture.getHeight(), angle);}
	void draw(
		Texture& texture,
		float x,
		float y
	) {draw(texture, x, y, texture.getWidth(), texture.getHeight(), 0.0f, 0.0f, texture.getWidth(), texture.getHeight(), 0.0f);}

	// Synkar med refreshraten.
	void presentSync();
	// 1 -> synka med refreshraten, 2 -> synka med 1/2 av refreshraten, 3 -> 1/3 etc.
	void presentSync(unsigned int syncInterval);
	// Ingen synkning, visar direkt.
	void presentNoSync();

	float clientToDVX(float x) const noexcept
	{
		return (x - width / 2.0f) * 2.0f;
	}
	float DVToClientX(float x) const noexcept
	{
		return (x + width / 2.0f) / 2.0f;
	}
	float clientToDVY(float y) const noexcept
	{
		return (height / 2.0f - y) * 2.0f;
	}
	float DVToClientY(float y) const noexcept
	{
		return (-height / 2.0f - y) / 2.0f;
	}

	static void changeScreenResolution(int width, int height);
};