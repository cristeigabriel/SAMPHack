#include "pch.h"
#include "Draw.h"

CRectangle::CRectangle(const D3DRECT rect, D3DCOLOR color)
{
	m_rect = rect;
	m_color = color;
}

void CRectangle::Draw(IDirect3DDevice9* pDevice)
{
	pDevice->Clear(1, &m_rect, D3DCLEAR_TARGET, m_color, 0.f, 0);
}

CLine::CLine(POINT start, POINT end, D3DCOLOR color)
{
	m_start = start;
	m_end = end;
	m_color = color;
}

struct D3DTLVERTEX
{	
	float m_fX;	
	float m_fY;	
	float m_fZ;	
	float m_fRHW;	
	D3DCOLOR m_color;	
};

void CLine::Draw(IDirect3DDevice9* pDevice)
{
	D3DTLVERTEX vertex[2] =
	{
		{(float)(m_start.x), (float)(m_start.y), 0.f, 1.f, m_color},
		{(float)(m_end.x), (float)(m_end.y), 0.f, 1.f, m_color},
	};

	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->SetTexture(0, nullptr);
	pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 2, &vertex[0], sizeof(D3DTLVERTEX));
}

CLines::CLines(std::vector<Line>&& lines, D3DCOLOR color)
{
	m_lines = std::move(lines);
	m_color = color;
}

void CLines::Draw(IDirect3DDevice9* pDevice)
{
	if (m_lines.empty()) return;

	std::vector<D3DTLVERTEX> vertex;
	for (auto [start, end] : m_lines)
	{
		vertex.emplace_back((float)(start.x), (float)(start.y), 0.f, 1.f, m_color);
		vertex.emplace_back((float)(end.x), (float)(end.y), 0.f, 1.f, m_color);
	}

	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->SetTexture(0, nullptr);
	pDevice->DrawPrimitiveUP(D3DPT_LINELIST, vertex.size(), &vertex[0], sizeof(D3DTLVERTEX));
}

CLineStrip::CLineStrip(std::vector<POINT>&& points, D3DCOLOR color)
{
	m_points = std::move(points);
	m_color = color;
}

void CLineStrip::Draw(IDirect3DDevice9* pDevice)
{
	std::vector<D3DTLVERTEX> vertex;
	for (POINT point : m_points)
		vertex.emplace_back((float)(point.x), (float)(point.y), 0.f, 1.f, m_color);

	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->SetTexture(0, nullptr);
	pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, vertex.size(), &vertex[0], sizeof(D3DTLVERTEX));
}

static std::map<FontStyle, LPD3DXFONT> g_fontsMap;
struct Text
{
	std::string_view text;
	const FontStyle& style;

	constexpr bool operator==(const Text& otherText)
	{
		return text == otherText.text
			&& style == otherText.style;
	}

	constexpr bool operator<(const Text& otherText) const
	{
		return text < otherText.text
			&& style < otherText.style;
	}
};

static std::map<Text, LPD3DXSPRITE> g_spritesMap;

void DrawTextFinal(IDirect3DDevice9* pDevice, const Text& text, POINT point, D3DCOLOR color)
{
	LPD3DXFONT* ppFont;
	if (!g_fontsMap.contains(text.style))
	{
		ppFont = &(g_fontsMap[text.style]);
		HRESULT hr = D3DXCreateFontA(pDevice, text.style.size, 0, text.style.weight, 0, text.style.italic, DEFAULT_CHARSET,
			text.style.precision, text.style.quality, text.style.pitch, text.style.font.data(), ppFont);

		if (!SUCCEEDED(hr))
			__debugbreak();
	}
	else ppFont = &(g_fontsMap[text.style]);

	LPD3DXSPRITE& pSprite = g_spritesMap[text];
	if (!pSprite)
		D3DXCreateSprite(pDevice, &pSprite);

	RECT rect = { (LONG)point.x, (LONG)point.y, 0, 0 };
	(*ppFont)->DrawTextA(nullptr, text.text.data(), text.text.size(), &rect, DT_NOCLIP, color);
}

CText::CText(POINT point, std::string&& text, FontStyle style, D3DCOLOR color)
{
	m_point = point;
	m_text = std::move(text);
	m_style = style;
	m_color = color;
}

void CText::Draw(IDirect3DDevice9* pDevice)
{
	Text text =
	{
		std::string_view{m_text},
		m_style
	};

	DrawTextFinal(pDevice, text, m_point, m_color);
}

CQueue::CQueue()
{
}

std::shared_mutex& CQueue::GetMutex()
{
	return m_mutex;
}

void CQueue::Push(IDrawable* pDrawable)
{
	m_drawables.push_front(std::move(std::unique_ptr<IDrawable>(pDrawable)));
}

void CQueue::Clear()
{
	m_drawables.clear();
}

CDrawing::CDrawing()
{
}

void CDrawing::PushDrawables(std::function<void(CDrawing*)>&& fn)
{
	std::unique_lock _(m_queue.GetMutex());

	m_queue.Clear();

	fn(this);
}

void CDrawing::DrawQueue(IDirect3DDevice9* pDevice)
{
	std::unique_lock _(m_queue.GetMutex());

	for (auto&& pDrawable : m_queue)
		pDrawable->Draw(pDevice);
}

CQueue& CDrawing::GetQueue()
{
	return m_queue;
}

CDrawing g_drawing{};

