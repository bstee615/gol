#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <fstream>

const int NUM_ROWS = 100, NUM_COLS = 100, CELL_SIZE = 5;
typedef bool grid_t[NUM_ROWS][NUM_COLS];

bool grid[NUM_ROWS][NUM_COLS];
bool backgrid[NUM_ROWS][NUM_COLS];

void clear_grid(grid_t grid) {
	for (int row = 0; row < NUM_ROWS; row++) {
		for (int col = 0; col < NUM_COLS; col++) {
			grid[row][col] = false;
		}
	}
}

void copy_grid(grid_t grid, const grid_t other_grid) {
	for (int row = 0; row < NUM_ROWS; row++) {
		for (int col = 0; col < NUM_COLS; col++) {
			grid[row][col] = other_grid[row][col];
		}
	}
}

void seed_grid(grid_t grid) {
	double seed_chance = 1.0 - 1.0 / 5.0;
	for (int row = 0; row < NUM_ROWS; row++) {
		for (int col = 0; col < NUM_COLS; col++) {
			double rand = ((double)std::rand() / (RAND_MAX));
			grid[row][col] = rand > seed_chance;
		}
	}
}

void seed_grid_from_input(grid_t grid) {
	bool should_continue = false, take_input = true;
	do {
		if (take_input) {
			std::cout << "Input cell (row col)" << std::endl;
			int row, col;
			std::cin >> row >> col;

			grid[row][col] = true;
		}

		std::cout << "More cells? (y/n)" << std::endl;
		char c;
		std::cin >> c;
		switch (c) {
		case 'y':
			should_continue = true;
			take_input = true;
			break;
		case 'n':
			should_continue = false;
			break;
		default:
			std::cout << "Invalid input." << std::endl;
			should_continue = true;
			take_input = false;
			break;
		}
	} while (should_continue);
}

void seed_grid_from_file(grid_t grid, char *filepath) {
	auto file = std::ifstream();
	file.open(filepath);
	if (!file) {
		std::cout << "Unable to open " << filepath << ". Press ENTER to quit." << std::endl;

		std::string blank;
		std::getline(std::cin, blank);
		exit(1);
	}

	int row, col;
	while (file >> col >> row) {
		grid[row][col] = true;
	}
}

void seed_square(grid_t grid) {
	grid[1][1] = true;
	grid[1][2] = true;
	grid[2][1] = true;
	grid[2][2] = true;
}

int num_neighbors_in_grid(grid_t grid, int base_row, int base_col) {
	auto out_of_bounds = [](int row, int col) -> bool {
		return row < 0 || row >= NUM_ROWS || col < 0 || col >= NUM_COLS;
	};

	int sum = 0;
	for (int delta_row = -1; delta_row <= 1; delta_row ++) {
		for (int delta_col = -1; delta_col <= 1; delta_col ++) {
			int row = base_row + delta_row, col = base_col + delta_col;

			if (!(delta_row == 0 && delta_col == 0) &&
				!out_of_bounds(row, col)) {
				sum += grid[row][col] ? 1 : 0;
			}
		}
	}

	return sum;
}

void draw_grid(sf::RenderWindow &window, grid_t grid) {
	for (int row = 0; row < NUM_ROWS; row++) {
		for (int col = 0; col < NUM_COLS; col++) {
			sf::Shape *shape;
			if (grid[row][col] == true) {
				shape = new sf::CircleShape(CELL_SIZE / 2);
				shape->setFillColor(sf::Color::Green);
			}
			else {
				shape = new sf::RectangleShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
				shape->setFillColor(sf::Color::Blue);
			}
			shape->setPosition((float)row * (CELL_SIZE + 1), (float)col * (CELL_SIZE + 1));
			window.draw(*shape);
			delete(shape);
		}
	}
}

void iterate_grid_once(grid_t gamegrid, grid_t backgrid) {
	clear_grid(backgrid);

	for (int row = 0; row < NUM_ROWS; row++) {
		for (int col = 0; col < NUM_COLS; col++) {
			int num_neighbors = num_neighbors_in_grid(gamegrid, row, col);
			bool alive = gamegrid[row][col];
			
			if (alive) {
				if (num_neighbors < 2) {
					alive = false;
				}
				if (num_neighbors >  3) {
					alive = false;
				}
			}
			else if (num_neighbors == 3) {
				alive = true;
			}

			backgrid[row][col] = alive;
		}
	}

	copy_grid(gamegrid, backgrid);
}

int main(int argc, char **argv)
{
	grid_t gamegrid;
	grid_t backgrid;

	clear_grid(gamegrid);
	clear_grid(backgrid);

	//seed_grid();
	//seed_square(gamegrid);

	if (argc > 1) {
		seed_grid_from_file(gamegrid, argv[1]);
	}
	else {
		seed_grid_from_input(gamegrid);
	}

	sf::RenderWindow window(sf::VideoMode((CELL_SIZE + 1) * NUM_ROWS, (CELL_SIZE + 1) * NUM_COLS), "Game of Life", sf::Style::Titlebar | sf::Style::Close);

	sf::Clock clock;
	int interation_ms = 50;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		// Iterate
		if (clock.getElapsedTime().asMilliseconds() >= interation_ms) {
			iterate_grid_once(gamegrid, backgrid);
			clock.restart();
		}

		// Draw grid
		draw_grid(window, gamegrid);

		window.display();
	}

	return 0;
}