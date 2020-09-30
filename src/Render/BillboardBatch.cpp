// Copyright 2019-2020 the donut authors. See AUTHORS.md

#include "Render/BillboardBatch.h"

#include "Game.h"
#include "P3D/P3D.generated.h"
#include "Render/OpenGL/IndexBuffer.h"
#include "Render/OpenGL/ShaderProgram.h"
#include "Render/OpenGL/VertexBinding.h"
#include "Render/OpenGL/VertexBuffer.h"
#include "Render/Shader.h"
#include "ResourceManager.h"

#include <array>

namespace Donut
{
struct QuadInstance
{
	Vector3 pos;
	Vector2 size;
	Vector4 color;
	Vector2 uv0;
	Vector2 uv1;
	Vector2 uv2;
	Vector2 uv3;
};

static std::array<Vector2, 4> quadVertices = {
    Vector2(-0.5, 0.5),
    Vector2(0.5, 0.5),
    Vector2(0.5, -0.5),
    Vector2(-0.5, -0.5),
};

static std::array<uint32_t, 6> indices = {0, 1, 2, 2, 3, 0};

BillboardBatch::BillboardBatch(const P3D::BillboardQuadGroup& billboardQuadGroup)
{
	static const size_t vertStride = sizeof(Vector2);
	static const size_t instanceStride = sizeof(QuadInstance);

	_numQuads = billboardQuadGroup.GetQuadCount();
	std::vector<QuadInstance> quadInstances;
	quadInstances.reserve(_numQuads);

	for (auto const& billboardQuad : billboardQuadGroup.GetQuads())
	{
		quadInstances.push_back(QuadInstance {
		    billboardQuad->GetTranslation(),
		    Vector2(billboardQuad->GetWidth(), billboardQuad->GetHeight()),
		    P3D::P3DUtil::ConvertColor(billboardQuad->GetColor()),
		    billboardQuad->GetUv0(),
		    billboardQuad->GetUv1(),
		    billboardQuad->GetUv2(),
		    billboardQuad->GetUv3(),
		});
	}

	_vertexBuffer = std::make_shared<GL::VertexBuffer>(quadVertices.data(), quadVertices.size(), vertStride);
	_indexBuffer = std::make_shared<GL::IndexBuffer>(indices.data(), indices.size(), GL_UNSIGNED_INT);
	_instanceBuffer = std::make_shared<GL::VertexBuffer>(quadInstances.data(), _numQuads, instanceStride);

	GL::ArrayElement vertexLayout[] = {
	    GL::ArrayElement(_vertexBuffer.get(), 0, 2, GL::AE_FLOAT, vertStride, 0),
	    GL::ArrayElement(_instanceBuffer.get(), 1, 3, GL::AE_FLOAT, instanceStride, 0 * sizeof(float), 1),
	    GL::ArrayElement(_instanceBuffer.get(), 2, 2, GL::AE_FLOAT, instanceStride, 3 * sizeof(float), 1),
	    GL::ArrayElement(_instanceBuffer.get(), 3, 4, GL::AE_FLOAT, instanceStride, 5 * sizeof(float), 1),
	    GL::ArrayElement(_instanceBuffer.get(), 4, 2, GL::AE_FLOAT, instanceStride, 9 * sizeof(float), 1),
	    GL::ArrayElement(_instanceBuffer.get(), 5, 2, GL::AE_FLOAT, instanceStride, 11 * sizeof(float), 1),
	    GL::ArrayElement(_instanceBuffer.get(), 6, 2, GL::AE_FLOAT, instanceStride, 13 * sizeof(float), 1),
	    GL::ArrayElement(_instanceBuffer.get(), 7, 2, GL::AE_FLOAT, instanceStride, 15 * sizeof(float), 1),
	};

	_vertexBinding = std::make_shared<GL::VertexBinding>();
	_vertexBinding->Create(vertexLayout, 8, *_indexBuffer, GL::ElementType::AE_UINT);

	_shader = billboardQuadGroup.GetShader();
	_zTest = billboardQuadGroup.GetZTest() == 1;
	_zWrite = billboardQuadGroup.GetZWrite() == 1;
}

void BillboardBatch::Draw(GL::ShaderProgram& shader, bool opaque)
{
	auto const& material = Game::GetInstance().GetResourceManager().GetShader(_shader);

	bool trans = (!material->IsAlphaTested() && material->IsTranslucent());

	if (trans && opaque)
	{
		return;
	}

	if (!trans && !opaque)
	{
		return;
	}

	if (!opaque)
	{
		auto blendMode = material->GetBlendMode();

		if (blendMode == BlendMode::Alpha)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else if (blendMode == BlendMode::Additive)
		{
			glBlendFunc(GL_ONE, GL_ONE);
		}
	}

	if (_zTest)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	glDepthMask(_zWrite ? GL_TRUE : GL_FALSE);

	shader.SetUniformValue("alphaMask", material->IsAlphaTested() ? 0.5f : 0.0f);

	_vertexBinding->Bind();

	material->Bind(0);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, _numQuads);
	_vertexBinding->Unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}
} // namespace Donut
