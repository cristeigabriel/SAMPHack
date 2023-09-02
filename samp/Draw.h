#pragma once

class IDrawable
{
public:
	virtual ~IDrawable() {}

	virtual void Draw(IDirect3DDevice9*) = 0;
};

class CRectangle : public IDrawable
{
public:
	CRectangle(const D3DRECT, D3DCOLOR);

	void Draw(IDirect3DDevice9*) override;

private:
	D3DRECT m_rect;
	D3DCOLOR m_color;
};

class CLine : public IDrawable
{
public:
	CLine(POINT, POINT, D3DCOLOR);

	void Draw(IDirect3DDevice9*) override;

private:
	POINT m_start;
	POINT m_end;
	D3DCOLOR m_color;
};

typedef std::pair<POINT, POINT> Line;

class CLines : public IDrawable
{
public:
	CLines(std::vector<Line>&&, D3DCOLOR);

	void Draw(IDirect3DDevice9*) override;

private:
	std::vector<Line> m_lines;
	D3DCOLOR m_color;
};

struct FontStyle
{
	std::string_view font;
	UINT size;
	UINT weight;
	BOOL italic;
	DWORD precision = OUT_TT_ONLY_PRECIS;
	DWORD quality = ANTIALIASED_QUALITY;
	DWORD pitch = DEFAULT_PITCH;

	constexpr bool operator==(const FontStyle& style) const&
	{
		return font == style.font
			&& size == style.size
			&& weight == style.weight
			&& italic == style.italic
			&& precision == style.precision
			&& quality == style.quality
			&& pitch == style.pitch;
	}

	constexpr bool operator<(const FontStyle& style) const
	{
		return font < style.font;
	}
};

class CText : public IDrawable
{
public:
	CText(POINT, std::string&&, FontStyle, D3DCOLOR);

	void Draw(IDirect3DDevice9*) override;

private:
	POINT m_point;
	std::string m_text;
	FontStyle m_style;
	D3DCOLOR m_color;
};

class CLineStrip : public IDrawable
{
public:
	CLineStrip(std::vector<POINT>&&, D3DCOLOR);

	void Draw(IDirect3DDevice9*) override;

private:
	std::vector<POINT> m_points;
	D3DCOLOR m_color;
};

class CQueue
{
public:
	CQueue();

	std::shared_mutex& GetMutex();

	void Push(IDrawable*);
	void Clear();

	inline decltype(auto) begin()
	{
		return m_drawables.begin();
	}

	inline decltype(auto) end()
	{
		return m_drawables.end();
	}

private:
	std::shared_mutex m_mutex;
	std::deque<std::unique_ptr<IDrawable>> m_drawables;
};

class CDrawing
{
public:
	CDrawing();

	void PushDrawables(std::function<void(CDrawing*)>&&);
	void DrawQueue(IDirect3DDevice9*);

	CQueue& GetQueue();

private:
	CQueue m_queue;
};
