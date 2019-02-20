#ifndef Tile_h
#define Tile_h

#include "Animation.h"
#include <memory>
#include "Texture.h"

extern void Tile_LoadMetaData();
extern void Tile_UnloadMetaData();

extern std::shared_ptr<Animation> tileAppearAnimation;
extern std::shared_ptr<Animation> tilePopAnimation;
extern std::shared_ptr<Animation> Tile_makeMoveAnimation(int row1, int col1, int row2, int col2);

struct Vector
{
	int row, col;
	Vector() : row(0), col(0) { }
	Vector(int row, int col) : row(row), col(col) { }
	
	Vector addNew(Vector o) {
		return Vector(row + o.row, col + o.col);
	}

	bool equalsTo(Vector o) {
		return row == o.row && col == o.col;
	}
};

class Tile : public AnimationTarget
{
public:
	explicit Tile(int row, int col, int value);
	void render(int x, int y);
	void update(int delta_ms);
	virtual void setProperty(int propertyID, double value) override;
	void attachAnimation(std::shared_ptr<Animation> animation);
	void planMovement(const Vector &v);

	int row() { return m_row; }
	int col() { return m_col; }
	int value() { return m_value; }

	bool hasMergedFrom() { return m_mergedFrom1 != nullptr; }
	void clearMergedFrom() {
		delete m_mergedFrom1;
		delete m_mergedFrom2;
		m_mergedFrom1 = m_mergedFrom2 = nullptr;
	}
	void setMergedFrom(Tile *a, Tile *b) {
		m_mergedFrom1 = a;
		m_mergedFrom2 = b;
	}

private:
	int m_x;
	int m_y;
	int m_row;
	int m_col;
	int m_value;
	double m_opacity;
	double m_scale;
	std::shared_ptr<AnimationExecutor> m_animationExecutor;
	Texture m_texture;
	Tile *m_mergedFrom1, *m_mergedFrom2;

	friend void renderTile(Tile *tile, int x, int y);
	friend void updateTile(Tile *tile, int delta_ms);
};

#endif

