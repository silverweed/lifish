constexpr auto LEVEL_WIDTH = lif::GAME_WIDTH / lif::TILE_SIZE - 2;
constexpr auto LEVEL_HEIGHT = lif::GAME_HEIGHT / lif::TILE_SIZE - 2;

using Grid = char[LEVEL_HEIGHT * LEVEL_WIDTH];

struct Vector2iHash
{
    std::size_t operator()(const sf::Vector2i& s) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(s.x);
        std::size_t h2 = std::hash<int>{}(s.y);
        return h1 ^ (h2 << 1);
    }
};

void dumpBombsToGrid(const lif::BaseLevelManager& blm, Grid grid) {
	memset(grid, '.', LEVEL_HEIGHT * LEVEL_WIDTH);

	blm.getEntities().apply([grid] (const lif::Entity& e) {
		if (dynamic_cast<const lif::Bomb *>(&e)) {
			auto tile = lif::tile(e.getPosition());
			grid[(tile.y - 1) * LEVEL_WIDTH + (tile.x - 1)] = '*';
		}
	});
}

using BombFormation = std::vector<sf::Vector2i>;

std::optional<BombFormation> buildBombFormation(const lif::BaseLevelManager& blm, sf::Vector2i start) {
	Grid grid;
	dumpBombsToGrid(blm, grid);
	for (int row = 0; row < LEVEL_HEIGHT; ++row) {
		for (int col = 0; col < LEVEL_WIDTH; ++col) {
			std::cout << grid[row * LEVEL_WIDTH + col] << " ";
		}
		std::cout << "\n";
	}

	BombFormation formation;
	
	auto* painter = lif::debugPainter;
	std::vector<sf::Vector2i> frontier = { start };
	std::cout << "frontier is " << start.x << ", " << start.y << "\n";
	std::unordered_set<sf::Vector2i, Vector2iHash> visited;
	const auto& entities = blm.getEntities();
	visited.emplace(start);
	formation.push_back(start);
	do {		
		const auto cur = frontier.back();
		frontier.pop_back();
		const auto curTile = cur;
		for (auto dir = (int)lif::Direction::UP; dir < lif::Direction::NONE; ++dir) {
			for (int len = 1; len <= 2; ++len) {
				auto tile = lif::towards<int>(cur, static_cast<lif::Direction>(dir), len);
				if (tile.x < 0 || tile.y < 0) 
					continue;
    		if (visited.find(tile) != visited.end())
    			continue;
				if (grid[tile.y * LEVEL_WIDTH + tile.x] == '*') {
					// add bomb to frontier
      		visited.emplace(tile);
					frontier.push_back(tile);
					formation.push_back(tile);
					std::cout << "adding " << tile.x << ", " << tile.y << "\n";
				}
			}
		}
	} while (!frontier.empty());

	return formation.size() > 1 ? std::make_optional(formation) : std::nullopt;
}

