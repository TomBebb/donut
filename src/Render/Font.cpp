// Copyright 2019-2020 the donut authors. See AUTHORS.md

#include "Render/Font.h"

#include <P3D/P3D.generated.h>
#include <P3D/P3DChunk.h>
#include <Render/Texture.h>

namespace Donut
{
Font::Font(P3D::TextureFont& fontP3D)
{
	_height = fontP3D.GetHeight();

	for (const auto& texture : fontP3D.GetTextures()) { _textures.push_back(std::make_shared<Texture>(*texture)); }

	for (const auto& glyph : fontP3D.GetGlyphs())
	{
		_glyphs.insert({glyph.id, Glyph {glyph.textureId, glyph.bottomLeftX, glyph.bottomLeftY, glyph.topRightX,
		                                 glyph.topRightY, glyph.leftBearing, glyph.rightBearing, glyph.width, glyph.advance}});
	}
}

bool Font::TryGetGlyph(int32_t id, Font::Glyph& glyph) const
{
	if (_glyphs.find(id) != _glyphs.end())
	{
		glyph = _glyphs.at(id);
		return true;
	}

	return false;
}
} // namespace Donut
