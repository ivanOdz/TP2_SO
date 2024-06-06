#include "snake.h"
#include "db.h"
#include "sc.h"
typedef struct {
	uint16_t x;
	uint16_t y;
	int8_t dir;

} position;

typedef struct {
	uint16_t head; // Indice de arreglo (derecha)
	uint16_t tail; // Indice de arreglo (izquierda)
	int8_t dir;	   // Direccion
	uint16_t score;

} snake_position;

typedef struct {
	uint8_t lose1 : 1;
	uint8_t lose2 : 1;
	uint8_t grow1 : 1;
	uint8_t grow2 : 1;
	uint8_t players : 2;

} playersFlags;

static void reset_positions();
static uint8_t game_loop(uint8_t players);
static void move_heads(playersFlags *flags, position *nextHead1, position *nextHead2);
static void set_background();
static void set_entities(playersFlags *flags);
static void print_block(uint16_t setX, uint16_t setY, const uint16_t simpleColorTexture[TEXTURE_SIZE_X][TEXTURE_SIZE_Y], int8_t rotation);
static uint8_t check_collision(playersFlags *flags, position *nextHead1, position *nextHead2);
static uint8_t check_positions(playersFlags *flags, position *nextHead1, position *nextHead2);
static uint8_t check_limits(playersFlags *flags, position *nextHead1, position *nextHead2);
static uint8_t get_directions();
static void initScreen(uint8_t players);
void newApple(playersFlags *flags);
void printScore(playersFlags flags);

static position snake1[MAX_SNAKE_SIZE];
static position snake2[MAX_SNAKE_SIZE];
static snake_position snakePos1;
static snake_position snakePos2;
static position apple;
static uint64_t margenEnX;
static uint64_t margenEnY;

void snake(int argc, char **argv) {
	text_format fmt = {0x00000000, 0x00C0C0C0, 3, 0};
	SyscallSetFormat(&fmt);
	SyscallSetTimer(30000);

	uint8_t nextGame = 1;
	int status = 0;
	uint8_t players = 2;
	if (argc >= 2) {
		players = stringToInt(argv[1], strlen((uint8_t *) argv[1]));
		if (players > 2)
			players = 2; // De argumento
		if (players < 1)
			players = 1;
	}
	else
		players = 1;
	while (nextGame) {
		initScreen(players);
		reset_positions();
		game_loop(players);
		putchar(0xd);
		printf("Play Again ? [y/n]      ");
		do {
			nextGame = get_directions();
		} while (nextGame == 0);
		nextGame--;
	}
	SyscallAudio(0, 0, 0);
	putchar('\e');
	exit(status);
}

void reset_positions() {
	snakePos1.head = START_LEN;
	snakePos1.tail = 0;
	snakePos1.dir = 0;
	snakePos2.head = START_LEN;
	snakePos2.tail = 0;
	snakePos2.dir = 0;
	for (int i = 0; i <= START_LEN; i++) {
		snake1[i].x = 0;
		snake1[i].y = i;
		snake1[i].dir = DIR_S;
		snake2[i].x = BLOCKS_X - 1;
		snake2[i].y = BLOCKS_Y - 1 - i;
		snake2[i].dir = DIR_N;
	}
	time_t time;
	SyscallGetRTC(&time);
	srand(time.hora << 16 | time.min << 8 | time.seg);
	apple.x = BLOCKS_X / 2 - 1;
	apple.y = BLOCKS_Y / 2 - 1;
}

uint8_t game_loop(uint8_t players) {
	uint8_t winner = 0;
	uint64_t mNextFrame = SyscallGetTicks();
	playersFlags flags = {0, 0, 0, 0};
	position nextHead1 = {0, 0, 0};
	position nextHead2 = {0, 0, 0};

	flags.players = players;

	set_background();
	set_entities(&flags);

	while (snakePos1.dir == 0 || (snakePos2.dir == 0 && flags.players == 2)) {
		// Esperar a que ambos estén de acuerdo de arrancar
		get_directions();
	}
	SyscallAudio(1, skelliecity, skelliecitylength);
	printf("%c                                    ", 0xD);
	printScore(flags);
	while (1) {
		if (mNextFrame < SyscallGetTicks()) {
			mNextFrame = SyscallGetTicks() + TICKS_PER_FRAME;

			get_directions();
			uint8_t lost = check_positions(&flags, &nextHead1, &nextHead2);
			move_heads(&flags, &nextHead1, &nextHead2);
			uint16_t tempScore1 = (snakePos1.tail > snakePos1.head) ? MAX_SNAKE_SIZE - snakePos1.tail + snakePos1.head - START_LEN : snakePos1.head - snakePos1.tail - START_LEN;
			uint16_t tempScore2 = (snakePos2.tail > snakePos2.head) ? MAX_SNAKE_SIZE - snakePos2.tail + snakePos2.head - START_LEN : snakePos2.head - snakePos2.tail - START_LEN;

			if (tempScore1 != snakePos1.score) {
				snakePos1.score = tempScore1;
				snakePos2.score = tempScore2;
				printScore(flags);
			}

			if (lost) {
				SyscallAudio(0, Dogbass, Dogbasslength); // stop musica
				putchar(0xD);							 // carriage return
				if (flags.lose1 && flags.lose2) {
					winner = 0;
				}
				else if (flags.lose1 && flags.players == 2) {
					winner = 2;
				}
				else if (flags.lose2) {
					winner = 1;
				}
				if (winner && flags.players == 2)
					printf("Player %1d won!    Scores: %5d vs. %5d", winner, snakePos1.score, snakePos2.score);
				else if (!winner && flags.players == 1)
					printf("Game Over!       Score: %5d          ", snakePos1.score);
				else
					printf("It's a draw!                             ");
				mNextFrame = SyscallGetTicks() + TICKS_PER_FRAME * 50;
				while (SyscallGetTicks() < mNextFrame)
					;
				break;
			}
		}
	}

	return winner;
}
void printScore(playersFlags flags) {
	putchar(0xD); // carriage return
	printf("Player 1: %5d ", snakePos1.score);
	if (flags.players == 2) {
		printf("\tPlayer 2: %5d ", snakePos2.score);
	}
}

void set_background() {
	for (uint16_t posX = 0; posX < BLOCKS_X; posX++) {
		for (uint16_t posY = 0; posY < BLOCKS_Y; posY++) {
			print_block(posX, posY, texture_background, DIR_N);
		}
	}
}

void set_entities(playersFlags *flags) {
	print_block(snake1[snakePos1.tail].x, snake1[snakePos1.tail].y, texture_tail_snake1, snake1[snakePos1.tail].dir);
	for (int i = snakePos1.tail + 1; i != snakePos1.head; i = (i + 1) % MAX_SNAKE_SIZE) {
		print_block(snake1[i].x, snake1[i].y, texture_body_snake1, snake1[i].dir);
	}
	print_block(snake1[snakePos1.head].x, snake1[snakePos1.head].y, texture_head_snake1, snake1[snakePos1.head].dir);

	if (flags->players == 2) {
		print_block(snake2[snakePos2.tail].x, snake2[snakePos2.tail].y, texture_tail_snake2, snake1[snakePos2.tail].dir);
		for (int i = snakePos2.tail + 1; i != snakePos2.head; i = (i + 1) % MAX_SNAKE_SIZE) {
			print_block(snake2[i].x, snake2[i].y, texture_body_snake2, snake2[i].dir);
		}
		print_block(snake2[snakePos2.head].x, snake2[snakePos2.head].y, texture_head_snake2, snake2[snakePos2.head].dir);
	}

	print_block(apple.x, apple.y, texture_apple, DIR_N);
}

void print_block(uint16_t setX, uint16_t setY, const uint16_t texture[TEXTURE_SIZE_X][TEXTURE_SIZE_Y], int8_t rotation) {
	uint32_t transform[PIXELS_BLOCK_X][PIXELS_BLOCK_Y];
	uint8_t transformations = 0;
	switch (rotation) {
		case DIR_N:
			// xi e yi incrementan
			break;
		case DIR_S:
			// xi y yi decrementan
			transformations = 2;
			break;
		case DIR_W:
			// xi decrementa y yi aumenta (swap)
			transformations = 1;
			break;
		case DIR_E:
			// xi aumenta y yi decrementa (swap)
			transformations = 3;
			break;
	}
	for (uint16_t fullPosX = 0; fullPosX < PIXELS_BLOCK_X; fullPosX++) {
		// veo si el indice x esta incrementando o decrementando
		uint16_t texturex = ((transformations & 1) ^ (transformations >> 1)) ? (TEXTURE_SIZE_X - 1) - (fullPosX / TEXTURE_COMPACT_Y) : fullPosX / TEXTURE_COMPACT_X;
		for (uint16_t fullPosY = 0; fullPosY < PIXELS_BLOCK_Y; fullPosY++) {
			// veo si el indice y esta incrementando o decrementando
			uint16_t texturey = (transformations & 2) ? (TEXTURE_SIZE_Y - 1) - (fullPosY / TEXTURE_COMPACT_Y) : fullPosY / TEXTURE_COMPACT_Y;

			uint16_t pixel = (transformations & 1) ? texture[texturey][texturex] : texture[texturex][texturey];
			uint32_t fullColorPixel = (pixel & 0xF) << 4 | ((pixel & 0xF0) << 8) | ((pixel & 0xF00) << 12) | ((pixel & 0xF000) << 16);
			transform[fullPosX][fullPosY] = fullColorPixel;
		}
	}
	draw_type draw = {setX * PIXELS_BLOCK_X + margenEnX, setY * PIXELS_BLOCK_X + margenEnY, PIXELS_BLOCK_X, PIXELS_BLOCK_Y, 0, 0, (uint32_t *) transform};
	SyscallPutBlock(&draw);
}

void initScreen(uint8_t players) {
	putchar('\e');
	printf("%s player mode (awaiting input)", (players == 2) ? "Dual" : "Single");
	// Dibujo el tablero
	uint64_t anchoTablero = BLOCKS_X * PIXELS_BLOCK_X;
	uint64_t altoTablero = BLOCKS_Y * PIXELS_BLOCK_Y;
	margenEnX = (1024 - anchoTablero) / 2;
	margenEnY = PIXELS_BLOCK_Y * 5;
	draw_type border = {margenEnX, margenEnY, anchoTablero, altoTablero, 2, 0xc9c9c9, 0};
	SyscallPutBlock(&border);
}
uint8_t check_collision(playersFlags *flags, position *nextHead1, position *nextHead2) {
	uint16_t cont = flags->grow1 ? snakePos1.tail : (snakePos1.tail + 1) & 0x1FF;
	if (nextHead1->x == nextHead2->x && nextHead1->y == nextHead2->y) {
		flags->lose1 = 1;
		flags->lose2 = 1;
		return 1;
	}
	do {
		if (nextHead1->x == snake1[cont].x && nextHead1->y == snake1[cont].y) {
			flags->lose1 = 1;
			break;
		} // Colisiona Snake 1 con sigo misma
		else if (flags->players == 2 && nextHead2->x == snake1[cont].x && nextHead2->y == snake1[cont].y) {
			flags->lose2 = 1;
			break;
		} // Colisiona Snake 2 con Snake 1

		cont++;
		cont &= 0x1FF;

	} while (cont != ((snakePos1.head + 1) & 0x1FF));

	if (flags->players == 2) {
		cont = flags->grow2 ? snakePos2.tail : (snakePos2.tail + 1) & 0x1FF;

		do {
			if (nextHead2->x == snake2[cont].x && nextHead2->y == snake2[cont].y) {
				flags->lose2 = 1;
				break;
			} // Colisiona Snake 2 con sigo misma
			else if (nextHead1->x == snake2[cont].x && nextHead1->y == snake2[cont].y) {
				flags->lose1 = 1;
				break;
			} // Colisiona Snake 1 con Snake 2

			cont++;
			cont &= 0x1FF;

		} while (cont != ((snakePos2.head + 1) & 0x1FF));
	}

	return (uint8_t) (flags->lose1 || flags->lose2);
}

void move_heads(playersFlags *flags, position *nextHead1, position *nextHead2) {
	print_block(snake1[snakePos1.head].x, snake1[snakePos1.head].y, texture_background, snake1[snakePos1.head].dir);
	print_block(snake1[snakePos1.head].x, snake1[snakePos1.head].y, texture_body_snake1, snake1[snakePos1.head].dir);
	if (snake1[snakePos1.head].dir != nextHead1->dir) {
		print_block(snake1[snakePos1.head].x, snake1[snakePos1.head].y, texture_background, snake1[snakePos1.head].dir);
		if ((snake1[snakePos1.head].dir == DIR_N && nextHead1->dir == DIR_E) ||
			(snake1[snakePos1.head].dir == DIR_E && nextHead1->dir == DIR_S) ||
			(snake1[snakePos1.head].dir == DIR_S && nextHead1->dir == DIR_W) ||
			(snake1[snakePos1.head].dir == DIR_W && nextHead1->dir == DIR_N)) {
			print_block(snake1[snakePos1.head].x, snake1[snakePos1.head].y, texture_twist1_snake1, snake1[snakePos1.head].dir);
		}
		else {
			print_block(snake1[snakePos1.head].x, snake1[snakePos1.head].y, texture_twist2_snake1, snake1[snakePos1.head].dir);
		}
	}
	snakePos1.head++;
	snakePos1.head &= 0x1FF;
	if (!flags->grow1) {
		print_block(snake1[snakePos1.tail].x, snake1[snakePos1.tail].y, texture_background, DIR_N);
		snakePos1.tail++;
		snakePos1.tail &= 0x1FF;
		print_block(snake1[snakePos1.tail].x, snake1[snakePos1.tail].y, texture_background, snake1[snakePos1.tail].dir);
		print_block(snake1[snakePos1.tail].x, snake1[snakePos1.tail].y, texture_tail_snake1, snake1[(snakePos1.tail + 1) & 0x1FF].dir);
	}

	snake1[snakePos1.head].x = nextHead1->x;
	snake1[snakePos1.head].y = nextHead1->y;
	snake1[snakePos1.head].dir = nextHead1->dir;
	print_block(nextHead1->x, nextHead1->y, texture_head_snake1, nextHead1->dir);

	if (flags->players == 2) {
		print_block(snake2[snakePos2.head].x, snake2[snakePos2.head].y, texture_background, snake2[snakePos2.head].dir);
		print_block(snake2[snakePos2.head].x, snake2[snakePos2.head].y, texture_body_snake2, snake2[snakePos2.head].dir);
		if (snake2[snakePos2.head].dir != nextHead2->dir) {
			print_block(snake2[snakePos2.head].x, snake2[snakePos2.head].y, texture_background, snake2[snakePos2.head].dir);
			if ((snake2[snakePos2.head].dir == DIR_N && nextHead2->dir == DIR_E) ||
				(snake2[snakePos2.head].dir == DIR_E && nextHead2->dir == DIR_S) ||
				(snake2[snakePos2.head].dir == DIR_S && nextHead2->dir == DIR_W) ||
				(snake2[snakePos2.head].dir == DIR_W && nextHead2->dir == DIR_N)) {
				print_block(snake2[snakePos2.head].x, snake2[snakePos2.head].y, texture_twist1_snake2, snake2[snakePos2.head].dir);
			}
			else {
				print_block(snake2[snakePos2.head].x, snake2[snakePos2.head].y, texture_twist2_snake2, snake2[snakePos2.head].dir);
			}
		}
		snakePos2.head++;
		snakePos2.head &= 0x2FF;
		if (!flags->grow2) {
			print_block(snake2[snakePos2.tail].x, snake2[snakePos2.tail].y, texture_background, DIR_N);
			snakePos2.tail++;
			snakePos2.tail &= 0x2FF;
			print_block(snake2[snakePos2.tail].x, snake2[snakePos2.tail].y, texture_background, snake2[snakePos2.tail].dir);
			print_block(snake2[snakePos2.tail].x, snake2[snakePos2.tail].y, texture_tail_snake2, snake2[(snakePos2.tail + 1) & 0x1FF].dir);
		}

		snake2[snakePos2.head].x = nextHead2->x;
		snake2[snakePos2.head].y = nextHead2->y;
		snake2[snakePos2.head].dir = nextHead2->dir;
		print_block(nextHead2->x, nextHead2->y, texture_head_snake2, nextHead2->dir);
	}
	if (flags->grow1 | flags->grow2)
		newApple(flags);

	flags->grow1 = 0;
	flags->grow2 = 0;
}

void newApple(playersFlags *flags) {
	uint8_t collision = 0;
	SyscallAudio(0x2, sfx1, sfx1length);

	do {
		collision = 0;
		apple.x = randBetween(0, BLOCKS_X);
		apple.y = randBetween(0, BLOCKS_Y);
		uint16_t cont = snakePos1.tail;
		do {
			if (apple.x == snake1[cont].x && apple.y == snake1[cont].y) {
				collision = 1;
				break;
			} // Manzana sobre snake 1
			cont++;
			cont &= 0x1FF;
		} while (cont != ((snakePos1.head + 1) & 0x1FF));
		if (flags->players == 2 && !collision) {
			cont = snakePos2.tail;
			do {
				if (apple.x == snake2[cont].x && apple.y == snake2[cont].y) {
					collision = 1;
					break;
				} // Manzana sobre snake 2
				cont++;
				cont &= 0x1FF;
			} while (cont != ((snakePos2.head + 1) & 0x1FF));
		}
	} while (collision);
	print_block(apple.x, apple.y, texture_apple, DIR_N);
}

uint8_t check_positions(playersFlags *flags, position *nextHead1, position *nextHead2) {
	nextHead1->x = snake1[snakePos1.head].x + snakePos1.dir / DIR_E;
	nextHead1->y = snake1[snakePos1.head].y + snakePos1.dir % DIR_E;
	nextHead1->dir = snakePos1.dir;

	if (flags->players == 2) {
		nextHead2->x = snake2[snakePos2.head].x + snakePos2.dir / DIR_E;
		nextHead2->y = snake2[snakePos2.head].y + snakePos2.dir % DIR_E;
		nextHead2->dir = snakePos2.dir;
	}

	if (check_limits(flags, nextHead1, nextHead2))
		return 1;

	if (check_collision(flags, nextHead1, nextHead2))
		return 1;

	if (nextHead1->x == apple.x && nextHead1->y == apple.y) {
		flags->grow1 = 1;
	}
	if (flags->players == 2 && nextHead2->x == apple.x && nextHead2->y == apple.y) {
		flags->grow2 = 1 * (flags->players == 2);
	}

	return 0;
}

uint8_t check_limits(playersFlags *flags, position *nextHead1, position *nextHead2) {
	if (nextHead1->x >= BLOCKS_X || nextHead1->y >= BLOCKS_Y) {
		flags->lose1 = 1;
	}

	if (flags->players == 2 && (nextHead2->x >= BLOCKS_X || nextHead2->y >= BLOCKS_Y)) {
		flags->lose2 = 1;
	}

	return (uint8_t) (flags->lose1 || flags->lose2);
}

uint8_t get_directions() {
	static uint8_t keyboard_buffer[256] = {0};
	uint64_t read = SyscallRead(STD_IN, keyboard_buffer, 254);
	uint8_t newDir1 = 0;
	uint8_t newDir2 = 0;
	if (keyboard_buffer[0] != 0) {
		for (uint8_t i = 0; i < read - 1; i++) {
			switch (keyboard_buffer[i]) {
				case 'w':
				case 'W':
					if (snake1[snakePos1.head].dir != DIR_S)
						newDir1 = DIR_N;
					break;
				case 'a':
				case 'A':
					if (snake1[snakePos1.head].dir != DIR_E)
						newDir1 = DIR_W;
					break;
				case 's':
				case 'S':
					if (snake1[snakePos1.head].dir != DIR_N)
						newDir1 = DIR_S;
					break;
				case 'd':
				case 'D':
					if (snake1[snakePos1.head].dir != DIR_W)
						newDir1 = DIR_E;
					break;
				case 'i':
				case 'I':
					if (snake2[snakePos2.head].dir != DIR_S)
						newDir2 = DIR_N;
					break;
				case 'j':
				case 'J':
					if (snake2[snakePos2.head].dir != DIR_E)
						newDir2 = DIR_W;
					break;
				case 'k':
				case 'K':
					if (snake2[snakePos2.head].dir != DIR_N)
						newDir2 = DIR_S;
					break;
				case 'l':
				case 'L':
					if (snake2[snakePos2.head].dir != DIR_W)
						newDir2 = DIR_E;
					break;
				case 'y':
				case 'Y':
					return 2;
				case 'n':
				case 'N':
					return 1;
			}
		}
	}
	if (newDir1)
		snakePos1.dir = newDir1;
	if (newDir2)
		snakePos2.dir = newDir2;
	return 0;
}