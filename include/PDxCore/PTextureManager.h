#pragma once
#include "PTexture.h"
#include "PDxHelper.h"
#include "PParser.h"

struct TextureInfo
{
	std::wstring tex_name;
	std::wstring tex_path;
	float width;
	float height;
	std::string uv_ltop;
	std::string uv_rbottom;

	TextureInfo()
	{
		tex_name = L"";
		tex_path = L"";
		width = 0.0f;
		height = 0.0f;
		uv_ltop = "";
		uv_rbottom = "";
	}
};


class PTextureManager : public PSingleton<PTextureManager>
{
	friend class PSingleton<PTextureManager>;
public:
	~PTextureManager();
private:
	PTextureManager();

	std::map<std::wstring, PTexture*> texture_list_;
	PParser parser;


public:
	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;
	
	void LoadTextureFromScript(multibyte_string filepath, ID3D11Device* current_device);
	PTexture* LoadTextureWithoutScript(TextureInfo info, ID3D11Device* current_device);
	PTexture* GetTextureFromMap(std::wstring key);

	DX::PTex_uv ImageCoordinateToTexCoordinate(float x, float y, float image_width, float image_height);

};

