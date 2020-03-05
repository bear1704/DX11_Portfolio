#include "PTextureManager.h"

PTextureManager::~PTextureManager()
{
}

PTextureManager::PTextureManager()
{
}

bool PTextureManager::Init()
{
	return false;
}

bool PTextureManager::Frame()
{
	return false;
}

bool PTextureManager::Render()
{
	return false;
}

bool PTextureManager::Release()
{
	return false;
}

void PTextureManager::LoadTextureFromScript(multibyte_string filepath, ID3D11Device* current_device)
{
	//아즥 1회제한 안걸어놓음

	std::vector<std::pair<string, string>> ret_parse;
	std::string str;
	str.assign(filepath.begin(), filepath.end());
	parser.XmlParse(str, &ret_parse);

	for (auto iter = ret_parse.begin(); iter != ret_parse.end(); iter++)
	{

		if (iter->second.compare("Texture") == 0)
		{
			std::wstring texture_name;
			std::wstring tex_path;
			DX::PTex_uv4 uv4;
			PTexture* texture = nullptr;
			float width = 0;
			float height = 0;

			while (true)
			{
				iter++;
				if (iter->first == "tex_name")
					texture_name.assign(iter->second.begin(), iter->second.end());
				else if (iter->first == "tex_path")
					tex_path.assign(iter->second.begin(), iter->second.end());
				else if (iter->first == "image_width")
					width = std::stof(iter->second);
				else if (iter->first == "image_height")
					height = std::stof(iter->second);
				else if (iter->first == "uv_ltop")
				{

					std::vector<std::string> uv_string = parser.SplitString(iter->second, ',');

					DX::PTex_uv left_top = ImageCoordinateToTexCoordinate(std::stof(uv_string[0]), std::stof(uv_string[1]),
						width, height);

					iter++;

					uv_string = parser.SplitString(iter->second, ',');
					DX::PTex_uv right_bottom = ImageCoordinateToTexCoordinate(std::stof(uv_string[0]), std::stof(uv_string[1]),
						width, height);
					
					uv4.u[0] = left_top.u;
					uv4.v[0] = left_top.v;
					uv4.u[1] = right_bottom.u;
					uv4.v[1] = left_top.v;
					uv4.u[2] = right_bottom.u;
					uv4.v[2] = right_bottom.v;
					uv4.u[3] = left_top.u;
					uv4.v[3] = right_bottom.v;
					
				}
				else if (iter->first == "END")
					break;
			}

			texture = new PTexture();
			texture->set_uv_coord(uv4);
			texture->SetImageSize(width, height);

			ID3D11ShaderResourceView** view = texture->shader_res_view_double_ptr();
			
			
			HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
				current_device, tex_path.c_str(), NULL, NULL, view, NULL
			);

			if (FAILED(hr))
				assert(false); //여기 오류뜸

			texture_list_.insert(make_pair(texture_name, texture));
		}
	}
}

PTexture* PTextureManager::LoadTextureWithoutScript(TextureInfo info, ID3D11Device* current_device)
{
	if (info.tex_path.empty()) return nullptr;

	TCHAR Drive[MAX_PATH] = { 0, };
	TCHAR Dir[MAX_PATH] = { 0, };
	TCHAR Name[MAX_PATH] = { 0, };
	TCHAR Ext[MAX_PATH] = { 0, };

	_tsplitpath_s(info.tex_path.c_str(), Drive, Dir, Name, Ext);

	std::wstring str(Name);
	
	if (texture_list_.find(str) != texture_list_.end()) //중복임
	{
		return nullptr;
	}

	PTexture* texture = new PTexture;
	if (info.width > 0 || info.height > 0)
	{
		DX::PTex_uv4 uv4;
		//std::vector<std::string>&& ltop = parser.SplitString(info.uv_ltop, ',');
		std::vector<std::string> ltop = parser.SplitString(info.uv_ltop, ',');
		DX::PTex_uv left_top = ImageCoordinateToTexCoordinate(std::stof(ltop[0]), std::stof(ltop[1]),
			info.width, info.height);

		std::vector<std::string>&& rbottom = parser.SplitString(info.uv_rbottom, ','); //가능?
		DX::PTex_uv right_bottom = ImageCoordinateToTexCoordinate(std::stof(rbottom[0]), std::stof(rbottom[1]),
			info.width, info.height);

		uv4.u[0] = left_top.u;     uv4.v[0] = left_top.v;
		uv4.u[1] = right_bottom.u; uv4.v[1] = left_top.v;
		uv4.u[2] = right_bottom.u; uv4.v[2] = right_bottom.v;
		uv4.u[3] = left_top.u;     uv4.v[3] = right_bottom.v;
	
		texture->set_uv_coord(uv4);
		texture->SetImageSize(info.width, info.height);
	}
	else
	{
		DX::PTex_uv4 uv4;
		uv4.u[0] = 0.0f;	uv4.v[0] = 0.0f;
		uv4.u[1] = 1.0f;	uv4.v[1] = 0.0f;
		uv4.u[2] = 1.0f;	uv4.v[2] = 1.0f;
		uv4.u[3] = 0.0f;	uv4.v[3] = 1.0f;
		texture->set_uv_coord(uv4);
		
	}
	ID3D11ShaderResourceView** view = texture->shader_res_view_double_ptr();

	std::wstring tex_name = Name;
	tex_name += Ext;

	texture->set_tex_name(tex_name);
	texture->set_tex_path(info.tex_path);

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		current_device, info.tex_path.c_str(), NULL, NULL, view, NULL
	);

	

	if (FAILED(hr))
		assert(false); //여기 오류뜸

	texture_list_.insert(make_pair(tex_name, texture));

	return texture;

}

PTexture* PTextureManager::GetTextureFromMap(std::wstring key)
{
	auto iter = texture_list_.find(key);
	if (iter != texture_list_.end())
	{
		PTexture* data = (*iter).second;
		return data;
	}

	assert(false);
	return nullptr;
}


DX::PTex_uv PTextureManager::ImageCoordinateToTexCoordinate(float x, float y, float image_width, float image_height)
{
	DX::PTex_uv uv;
	ZeroMemory(&uv, sizeof(DX::PTex_uv));
	uv.u = x / image_width;
	uv.v = y / image_height;

	return uv;
}
