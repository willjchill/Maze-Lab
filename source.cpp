/*	Code created by William Desueza 
	Tuesday Lab, Section TX
	Nicholas Alavrez
*/ 

#include "library.h"

// size of tile in GUI
const int size = 20;

struct maze {
	string line[100]; 
	int row, col; 
	int win_pos[2]; 
};

// pos properties:
// pos[0] = row pos, pos[1] = col pos 
struct player {
	int pos[2];
	char type;	// contains char that defines player (ex: #, E, e) 
	int health;	
	bool isAutomatic; 
	bool hasTreasure;
	char retrace[10000];
	int retraceIndex;
	maze memory; // only for automatic mode
};

const int pCount = 3;	// how many players (main player + enemies)
player Players[pCount];	// note: Players[0] = main player, everyone else enemy

// reads maze from text file
void read_maze(maze& m, string grid) {
	ifstream fin;
	fin.open(grid);
	for(int i = 0; i < 100; i++) {
		getline(fin, m.line[i]);
		if(fin.fail())
			break;
	}
	m.row = 11; 
	m.col = m.line[0].length();
}

// draws a tile (t) for each line of the maze
void draw_tile(char t, int i, int j) {
	// offset prevents from drawing over grid
	// note: all images are 19x19 pixel to prevent drawing over grid
	const int offset = 1;
	if(t == '#') {
		fill_rectangle(j * size + offset, i * size + offset, size - offset, size - offset, make_color_int(187, 191, 191));
		image * wall_icon = image_from_file("wall.bmp");
		draw_image(wall_icon, j * size + offset, i * size + offset);
	}
	else if(t == 'E') {		// enemy 1: reaper, uses greedy_input
		fill_rectangle(j * size + offset, i * size + offset, size - offset, size - offset, make_color_int(201, 99, 114));
		image * robot_icon = image_from_file("enemy.bmp");
		draw_image(robot_icon, j * size + offset, i * size + offset);
	}
	else if(t == 'e') {		// enemy 2: slime, uses random_input
		fill_rectangle(j * size + offset, i * size + offset, size - offset, size - offset, make_color_int(201, 99, 114));
		image * robot_icon = image_from_file("enemy2.bmp");
		draw_image(robot_icon, j * size + offset, i * size + offset);
	}
	else if(t == '+') {		// main player
		fill_rectangle(j * size + offset, i * size + offset, size - offset, size - offset, make_color_int(51, 153, 196));
		image * robot_icon = image_from_file("robot.bmp");
		draw_image(robot_icon, j * size + offset, i * size + offset);
	}
	else if(t == '$') {		// treasure
		fill_rectangle(j * size + offset, i * size + offset, size - offset, size - offset, make_color_int(132, 189, 94));
		image * gold_icon = image_from_file("gold.bmp");
		draw_image(gold_icon, j * size + offset, i * size + offset);
	}
	else if(t == 'W') {		// successful path after win
		fill_rectangle(j * size + offset, i * size + offset, size - offset, size - offset, make_color_int(230, 191, 64));
	}
}

// prints maze to console
void print_maze(maze m) {
	for(int i = 0; i < m.row; i++) {
		for(int j = 0; j < m.col; j++) {
			if(m.line[i][j] == '#') 
				cout << '@';
			else if(m.line[i][j] == '~')
				cout << ' ';
			else if(m.line[i][j] == '+') 
				cout << '+';
			else if(m.line[i][j] == '$') 
				cout << '$';
			else if(m.line[i][j] == 'E')
				cout << 'E';
			else if(m.line[i][j] == 'e')
				cout << 'e';
		}
		cout << endl;
	}
}

void draw_grid(maze& m) {
	for(int i = 1; i < m.row; i++) {
		move_to(0, i * size);
		set_pen_color(make_color_int(0, 0, 0));
		draw_to(m.col * size, i * size);
	}
	for(int j = 1; j < m.col; j++) {
		move_to(j * size, 0);
		set_pen_color(make_color_int(0, 0, 0));
		draw_to(j * size, m.row * size);
	}
}

// draws maze to GUI
void draw_maze(player p[], maze& m) {
	int enemyCount = 1;
	for(int i = 0; i < m.row; i++) {
		for(int j = 0; j < m.col; j++) {
			if(m.line[i][j] == '#') {
				draw_tile('#', i, j);
			}
			else if(m.line[i][j] == '+') {
				draw_tile('+', i, j);
				p[0].pos[0] = i;
				p[0].pos[1] = j;
				p[0].type = '#';
			}
			else if(m.line[i][j] == '$') {
				draw_tile('$', i, j);
				m.win_pos[0] = i;
				m.win_pos[1] = j;
			}
			else if(m.line[i][j] == 'E') {
				draw_tile('E', i, j);
				p[enemyCount].pos[0] = i;
				p[enemyCount].pos[1] = j;
				p[enemyCount].type = 'E';
				enemyCount += 1; 
			}
			else if(m.line[i][j] == 'e') {
				draw_tile('e', i, j);
				p[enemyCount].pos[0] = i;
				p[enemyCount].pos[1] = j;
				p[enemyCount].type = 'e';
				enemyCount += 1; 
			}
		}
	}
}

// only draws the players
// makes the maze a lot more efficient and run better :)
void draw_players(player p[], maze& m) {
	int enemyCount = 1;
	for(int i = 0; i < m.row; i++) {
		for(int j = 0; j < m.col; j++) {
			if(m.line[i][j] == '+') {
				draw_tile('+', i, j);
				p[0].pos[0] = i;
				p[0].pos[1] = j;
			}
			else if(m.line[i][j] == 'E') {
				draw_tile('E', i, j);
				p[enemyCount].pos[0] = i;
				p[enemyCount].pos[1] = j;
				enemyCount += 1; 
			}
			else if(m.line[i][j] == 'e') {
				draw_tile('e', i, j);
				p[enemyCount].pos[0] = i;
				p[enemyCount].pos[1] = j;
				enemyCount += 1; 
			}
		}
	}
}

// clears player position from specified tile 
void clear_player(player p) {
	fill_rectangle(p.pos[1] * size + 1, p.pos[0] * size + 1, size - 1, size - 1, make_color_int(255, 255, 255));
}

void clear_window(maze m) {
	fill_rectangle(0, 0, m.col * size, m.row * size, make_color_int(255, 255, 255));
}

// swaps player position with new positions i and j in maze
void swap(maze& m, player& p, int i, int j) {
	char temp = m.line[i][j];
	m.line[i][j] = m.line[p.pos[0]][p.pos[1]];
	m.line[p.pos[0]][p.pos[1]] = temp;
	p.pos[0] = i;
	p.pos[1] = j;
}

// detects if there is a wall at position
bool isWall(maze m, int i, int j) {
	if(m.line[i][j] == '#')
		return true;
	return false; 
}

// checks if player and enemy collides (i.e. a "hit") 
bool check_Hit(maze m, player& p, int i, int j) {
	for(int n = 0; n < pCount; n++) {
		if(Players[n].pos[0] == i && Players[n].pos[1] == j) {
			p.health = 0;
			Players[n].health = 0;
			return true; 
		}
	}
	return false;
}

// i and j are new positions, checks to see if 
// player gets to treasure if they update position
bool check_Treasure(maze m, player& p, int i, int j) {
	if(i == m.win_pos[0] && j == m.win_pos[1]) {
		p.hasTreasure = true; 
		return true;
	}
	return false;
}

// updates maze based on player's new positions (i, j) 
void update_maze(maze& m, player& p, int i, int j) {
	if(!check_Hit(m, p, i, j) && !check_Treasure(m, p, i, j)) {
		clear_player(p);
		swap(m, p, i, j);
		// only draws players because it makes updating maze more efficient!! 
		draw_players(Players, m);	
	}
}

// returns true if the input is valid and updates maze
// returns false if input is invalid and does not update anything
bool check_input(char c, maze& m, player& p) {
	int new_row, new_col;
	if (c == 'l' || c == -91) { 
		new_row = p.pos[0];
		new_col = p.pos[1] - 1;
		if(!isWall(m, new_row, new_col)) {
			update_maze(m, p, new_row, new_col);
			return true;
		}
		else 
			return false;
	}
	else if (c == 'r' || c == -89) { 
		new_row = p.pos[0];
		new_col = p.pos[1] + 1;
		if(!isWall(m, new_row, new_col)) {
			update_maze(m, p, new_row, new_col);
			return true;
		}
		else 
			return false;
	}
	else if (c == 'u' || c == -90) { 
		new_row = p.pos[0] - 1;
		new_col = p.pos[1];
		if(!isWall(m, new_row, new_col)) {
			update_maze(m, p, new_row, new_col);
			return true;
		}
	}
	else if (c == 'd' || c == -88) { 
		new_row = p.pos[0] + 1;
		new_col = p.pos[1];
		if(!isWall(m, new_row, new_col)) {
			update_maze(m, p, new_row, new_col);
			return true;
		}
		else 
			return false;
	}
	else if(c == 'q' || c == 'x') {
		exit(1);
		return true;
	}
	else if(c == 'a') {	// automatic mode :o 
		p.isAutomatic = true;
		return true;
	}
	else if(c == 'b' && p.retraceIndex != 0) {
		char c = p.retrace[p.retraceIndex - 1];
		p.retraceIndex = p.retraceIndex - 1; 
		// doing opposite
		if(c == 'd' || c == -88) {
			new_row = p.pos[0] - 1;
			new_col = p.pos[1];
			update_maze(m, p, new_row, new_col);
		}
		else if(c == 'u' || c == -90) {
			new_row = p.pos[0] + 1;
			new_col = p.pos[1];
			update_maze(m, p, new_row, new_col);
		}
		else if(c == 'r' || c == -89) {
			new_row = p.pos[0];
			new_col = p.pos[1] - 1;
			update_maze(m, p, new_row, new_col);
		}
		else if(c == 'l' || c == -91) {
			new_row = p.pos[0];
			new_col = p.pos[1] + 1;
			update_maze(m, p, new_row, new_col);
		}
		return true;
	}
	return false;
}

void random_input(maze& m, int pIndex) {
	char c; 
	bool validInput = false;
	while(!validInput) {
		c = random_in_range(88, 91) * -1; 
		validInput = check_input(c, m, Players[pIndex]);
	}
}

double dist(double x1, double y1, double x2, double y2) {
	return(sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

void greedy_input(maze& m, player& p) {
	char c;
	double closestDist = 5000;
	double tempDist;
	char bestMove = 'b';
	bool validInput;
	// note: row = y, col = x
	for(int i = 0; i < 4; i++) {
		if(i == 0 && !isWall(p.memory, p.pos[0], p.pos[1] - 1)) {	// check left
			tempDist = dist(p.pos[1] - 1, p.pos[0], m.win_pos[1], m.win_pos[0]);
			if(closestDist >= tempDist) {
				closestDist = tempDist;
				bestMove = 'l';
			}
		}
		else if(i == 1 && !isWall(p.memory, p.pos[0], p.pos[1] + 1)) { // check right
			tempDist = dist(p.pos[1] + 1, p.pos[0], m.win_pos[1], m.win_pos[0]);
			if(closestDist >= tempDist) {
				closestDist = tempDist;
				bestMove = 'r';
			}
		}
		else if(i == 2 && !isWall(p.memory, p.pos[0] - 1, p.pos[1])) { // check up
			tempDist = dist(p.pos[1], p.pos[0] - 1, m.win_pos[1], m.win_pos[0]);
			if(closestDist >= tempDist) {
				closestDist = tempDist;
				bestMove = 'u';
			}
		}
		else if(i == 3 && !isWall(p.memory, p.pos[0] + 1, p.pos[1])) { // check down
			tempDist = dist(p.pos[1], p.pos[0] + 1, m.win_pos[1], m.win_pos[0]);
			if(closestDist >= tempDist) {
				closestDist = tempDist;
				bestMove = 'd';
			}
		}
	}
	p.memory.line[p.pos[0]][p.pos[1]] = '#';
	if(bestMove != 'b') {
		p.retrace[p.retraceIndex] = bestMove; 
		p.retraceIndex++;
	}
	validInput = check_input(bestMove, m, p);
}

void color_path(maze& m, player& p, const int start_row, const int start_col) {
	int currentPos[2]; 
	currentPos[0] = start_row;
	currentPos[1] = start_col;
	draw_tile('W', currentPos[0], currentPos[1]);	// color starting position
	for(int i = 0; i < p.retraceIndex; i++) {
		if(p.retrace[i] == 'l' || p.retrace[i] == -91) {
			currentPos[1] -= 1; 
			draw_tile('W', currentPos[0], currentPos[1]);
		}
		else if(p.retrace[i] == 'r' || p.retrace[i] == -89) {
			currentPos[1] += 1;
			draw_tile('W', currentPos[0], currentPos[1]);
		}
		else if(p.retrace[i] == 'u' || p.retrace[i] == -90) {
			currentPos[0] -= 1;
			draw_tile('W', currentPos[0], currentPos[1]);
		}
		else if(p.retrace[i] == 'd' || p.retrace[i] == -88) {
			currentPos[0] += 1;
			draw_tile('W', currentPos[0], currentPos[1]);
		}
	}
}

void main() {
	// reads maze 
	maze Maze1; 
	read_maze(Maze1, "maze.txt");
	make_window(Maze1.col * size, Maze1.row * size);
	print_maze(Maze1);
	draw_maze(Players, Maze1);	// note: draw maze initializes players!!
	draw_grid(Maze1);
	const int start_row = Players[0].pos[0];
	const int start_col = Players[0].pos[1];

	Players[0].memory = Maze1; // player's memory of maze when in automatic mode
	Players[0].retraceIndex = 0; // records where on the retrace list the player is on
	Players[0].isAutomatic = false;
	Players[0].health = 1;
	Players[0].hasTreasure = false;

	// set all enemies to automatic
	for(int i = 1; i < pCount; i++)  {
		Players[i].isAutomatic = true;
		if(Players[i].type == 'E') {	// players that are reapers have memory
			Players[i].memory = Maze1;
			Players[i].retraceIndex = 0;
		}
	}

	// checking for inputs
	char c;
	bool validInput = false;

	// game running
	while (true) { 
		// player makes move first
		if(!Players[0].isAutomatic) {
			c = wait_for_key_typed(0.0);
			// note: check_input also updates the maze if input is valid
			validInput = check_input(c, Maze1, Players[0]);
			if(validInput) {
				// keep track of movement inputs
				if(c != 'b' && c != 'a' && c != 'm') {
					Players[0].retrace[Players[0].retraceIndex] = c; 
					Players[0].retraceIndex++;
				}
				// if player makes an input on manual, enemies move
				for(int i = 1; i < pCount; i++) {
					if(Players[i].type == 'e') 
						random_input(Maze1, i);
					else if(Players[i].type == 'E') 
						greedy_input(Maze1, Players[i]);
				}
			}
		}
		else {
			// finding shortest distance for automatic mode (greedy algorithm)
			greedy_input(Maze1, Players[0]);
			c = wait_for_key_typed(1.0);

			// enemies make a move if there's no response
			for(int i = 1; i < pCount; i++) {
				if(Players[i].type == 'e') 
					random_input(Maze1, i);
				else if(Players[i].type == 'E') 
					greedy_input(Maze1, Players[i]);
			}
			if(c == 'm') 
				Players[0].isAutomatic = false;
			else if(c == 'q' || c == 'x')
				exit(1);
		}

		// if player is dead, they lose
		// otherwise, if player gets treasure, they win
		if(Players[0].health == 0) {
			clear_window(Maze1);
			move_to(Maze1.col * size / 2, Maze1.row * size / 2);
			set_pen_color(make_color_int(135, 1, 26));
			write_string("YOU LOSE!");
			break;
		}
		else if(Players[0].hasTreasure) {
			color_path(Maze1, Players[0], start_row, start_col);
			move_to(Maze1.col * size / 2, Maze1.row * size / 2);
			set_pen_color(make_color_int(12, 6, 191));
			write_string("YOU WIN!");
			break;
		}
	}

	// player can still click 'q' or 'x' to exit after game is finished!
	while(true) {
		c = wait_for_key_typed(0.0);
		if(c == 'q' || c == 'x')
			exit(1);
	}
}
