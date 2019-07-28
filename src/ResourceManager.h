#pragma once

#include <P3D/p3d.generated.h>
#include <Render/OpenGL/Texture2D.h>
#include <Render/Font.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Donut
{
	class Shader
	{
	public:

		Shader(const P3D::Shader&);

		std::string GetTextureParam() const { return _textureParams.at("TEX"); }

	private:

		std::map<std::string, std::string> _textureParams;
	};

class ResourceManager
{
  public:
	ResourceManager();

	void AddTexture(const std::string& name, std::unique_ptr<GL::Texture2D> texture);
	void AddShader(const std::string& name, std::unique_ptr<Shader> shader);
	void AddFont(const std::string& name, std::unique_ptr<Font> font);

	const GL::Texture2D& GetTexture(const std::string& name) const;
	const GL::Texture2D& GetShaderTexture(const std::string& name) const;
	const Font* GetFont(const std::string& name) const;

	const std::map<std::string, std::unique_ptr<GL::Texture2D>>& GetTextures() const { return _textures; }
	const std::map<std::string, std::unique_ptr<Shader>>& GetShaders() const { return _shaders; }
	const std::map<std::string, std::unique_ptr<Font>>& GetFonts() const { return _fonts; }

  protected:
	std::unique_ptr<GL::Texture2D> _errorTexture;
	std::map<std::string, std::unique_ptr<GL::Texture2D>> _textures;
	std::map<std::string, std::unique_ptr<Shader>> _shaders;
	std::map<std::string, std::unique_ptr<Font>> _fonts;
};

} // namespace Donut
