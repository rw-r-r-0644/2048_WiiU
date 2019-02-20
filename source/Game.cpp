#include "Game.h"
#include <ctime>
#include <utility> // move
#include <algorithm> // fill, for_each
#include "Render.h"

Game::Game()
{
	m_cells = nullptr;
	m_copyrightX = 0;
	m_copyrightY = 0;
}

Game::~Game()
{

}

void Game::init(int size)
{
	Font font;
	font.init("Bell.ttf", 18);
	m_copyrightTexture.loadFromLatinText(&font, "(copyleft) kbridge@github 2016", BLACK);
	font.free();

	m_size = size;

	int len = size * size;
	m_cells = new Tile*[len];
	std::fill(m_cells, m_cells + len, nullptr);

	m_random_engine.seed(static_cast<unsigned long>(time(nullptr)));

	m_gameOver = false;

	m_score = 0;
	m_bestScore = 100;
	m_curScoreBoard = new ScoreBoard("SCORE", m_score);
	m_bestScoreBoard = new ScoreBoard("BEST", m_bestScore);
	m_tileBoard = new TileBoard(this);

	addRandomTile();
	addRandomTile();
}

void Game::quit()
{
	forEachTile([] (Tile *tile) { delete tile; });

	if(m_cells)
		delete[] m_cells;

	delete m_curScoreBoard;
	delete m_bestScoreBoard;
	delete m_tileBoard;

	m_copyrightTexture.free();
}

void Game::restart()
{
	forEachTile([] (Tile *tile) { delete tile; });
	std::fill(m_cells, m_cells + m_size*m_size, nullptr);

	m_gameOver = false;
	m_score = 0;

	addRandomTile();
	addRandomTile();
}

std::vector<int> Game::getAvailableCells()
{
	std::vector<int> availableCells;
	for(int i = 0; i < m_size * m_size; i++)
		if(m_cells[i] == nullptr)
			availableCells.push_back(i);

	return std::move(availableCells);
}

void Game::addRandomTile()
{
	auto availableCells = getAvailableCells();
	if(availableCells.empty())
		return;

	std::uniform_int_distribution<int> u1(0, availableCells.size() - 1);
	auto cell = availableCells[u1(m_random_engine)];

	std::bernoulli_distribution u2(0.9);
	int value = u2(m_random_engine) ? 2 : 4;

	Tile *tile = new Tile(cell / m_size, cell % m_size, value);
	tile->attachAnimation(tileAppearAnimation);
	m_cells[cell] = tile;
}

void Game::forEachTile(std::function<void (Tile *)> callback)
{
	for(int i = 0; i < m_size * m_size; i++)
		if(m_cells[i] != nullptr)
			callback(m_cells[i]);
}

void debugPrintGrids(Game *g)
{
	for(int row = 0; row < g->m_size; row++) {
		for(int col = 0; col < g->m_size; col++) {
			Tile *t = g->m_cells[row * g->m_size + col];
			printf(" %d", t ? t->value() : 0);
		}
		putchar('\n');
	}
}

void Game::fill(Dir dir, int a, int b, int *pRow, int *pCol)
{
	if(dir % 2 == 1)
		b = m_size - 1 - b;

	if(dir / 2 == 1)
		*pRow = a, *pCol = b;
	else
		*pRow = b, *pCol = a;
}

void Game::reduce(const std::vector<int> &mapping, bool *pMoved, int *pDeltaScore)
{
	bool moved = false;
	int n = -1; // fill pointer
	int first = -1; // search pointer
	bool prevMerged = false;

	for(;;) {
		while(++first < m_size)
			if(m_cells[mapping[first]] != nullptr)
				break;

		if(first == m_size)
			break;

		if(!prevMerged && n >= 0 && m_cells[mapping[n]] != nullptr
			&& m_cells[mapping[n]]->value() == m_cells[mapping[first]]->value())
		{
			Tile *a = m_cells[mapping[n]];
			Tile *b = m_cells[mapping[first]];

			// add move animation to B
			b->planMovement(Vector(mapping[n] / m_size, mapping[n] % m_size));

			// pop the merged tile
			Tile *m = new Tile(a->row(), a->col(), a->value() * 2);
			m->setMergedFrom(a, b);
			m->attachAnimation(tilePopAnimation);
			m_cells[mapping[n]] = m;

			// remove A, B
			m_cells[mapping[first]] = nullptr;
			prevMerged = true;
			moved = true;

			// add score
			*pDeltaScore += m->value();
		}
		else
		{
			if(++n < first)
			{
				Tile *t = m_cells[mapping[first]];
				t->planMovement(Vector(mapping[n] / m_size, mapping[n] % m_size));
				m_cells[mapping[first]] = nullptr;
				m_cells[mapping[n]] = t;
				moved = true;
			}
			prevMerged = false;
		}
	}
	*pMoved = moved;
}

void Game::move(Dir dir)
{
	if(m_gameOver)
		return;

	// [FIXED] ensure their animation have ended
	forEachTile([] (Tile *tile) { tile->clearMergedFrom(); });

	bool moved = false;
	int deltaScore = 0;

	std::vector<int> mapping;

	for(int i = 0; i < m_size; i++) {
		mapping.clear();
		for(int j = 0; j < m_size; j++) {
			int row, col;
			fill(dir, i, j, &row, &col);
			mapping.push_back(row * m_size + col);
		}
		bool m;
		reduce(mapping, &m, &deltaScore);
		moved |= m;
	}

	if(moved) {
		if(deltaScore > 0) {
			m_score += deltaScore;
			m_scoreAdditions.push_back(new ScoreAddition(deltaScore));
			if(m_score > m_bestScore)
				m_bestScore = m_score;
		}

		addRandomTile();

		if(!movesAvailable())
			m_gameOver = true;
	}
}

bool Game::movesAvailable()
{
	auto begin = m_cells;
	auto end = m_cells + m_size * m_size;
	if(std::find(begin, end, nullptr) != end)
		return true;

	static int vectors[4][2] =
	{
		{ -1,  0 }, // up
		{  1,  0 }, // down
		{  0, -1 }, // left
		{  0,  1 }, // right
	};
	auto withInBounds = [this] (int row, int col) {
		return row >= 0 && col >= 0 && row < m_size && col < m_size;
	};
	for(int i = 0; i < m_size * m_size; i++) {
		int row = i / m_size;
		int col = i % m_size;
		for(int j = 0; j < 4; j++) {
			int row2 = row + vectors[j][0];
			int col2 = col + vectors[j][1];
			if(!withInBounds(row2, col2))
				continue;
			if(m_cells[i]->value() == m_cells[row2 * m_size + col2]->value())
				return true;
		}
	}

	return false;
}

void Game::render()
{
	m_copyrightTexture.render(m_copyrightX, m_copyrightY);

	m_bestScoreBoard->render();
	m_curScoreBoard->render();

	std::for_each(m_scoreAdditions.begin(), m_scoreAdditions.end(), [&] (ScoreAddition *a) {
		a->render(
			m_curScoreBoard->x() + (m_curScoreBoard->width() - a->width()) / 2,
			m_curScoreBoard->y() + m_curScoreBoard->height() - a->height() - 2);
	});

	m_tileBoard->render();
}

void Game::update(int delta_ms)
{
	auto animateTile = [=] (Tile *t) { t->update(delta_ms); };
	forEachTile(animateTile);

	// [FIXME] more elegant solution?
	std::vector<ScoreAddition *> toDelete;
	std::for_each(m_scoreAdditions.begin(), m_scoreAdditions.end(), [&] (ScoreAddition *a) {
		a->update(delta_ms);
		if(!a->alive())
			toDelete.push_back(a);
	});

	auto newEnd = std::remove_if(m_scoreAdditions.begin(), m_scoreAdditions.end(), [] (ScoreAddition *a) {
		return !a->alive();
	});
	m_scoreAdditions.erase(newEnd, m_scoreAdditions.end());

	std::for_each(toDelete.begin(), toDelete.end(), [] (ScoreAddition *a) {
		delete a;
	});
}
