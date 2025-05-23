
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <random>

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
	uint8_t diceRolls[3];
	uint8_t score; // max possible score for each player is 162
	uint8_t idxOnAdd[6];
	uint8_t idxOnRemove[6];
} triplet_data_t;

typedef struct {
	uint8_t triplets[2][3]; // [player][move]
	int16_t score;
} game_board_t;

typedef struct {
	uint8_t triplets[2]; // [player]
	int16_t score;
} rollback_data_t;

const std::string DICE_SYMBOLS("-123456");
const uint8_t INVALID_MOVE = 255;
const uint8_t TRIPLETS_COUNT = 84;

const triplet_data_t TRIPLETS_DATA[TRIPLETS_COUNT] = {
	{ { 0, 0, 0 },  0, {  1,  2,  3,  4,  5,  6 }, {  0,  0,  0,  0,  0,  0 } }, //  0
	{ { 1, 0, 0 },  1, {  7,  8,  9, 10, 11, 12 }, {  0,  1,  1,  1,  1,  1 } }, //  1
	{ { 2, 0, 0 },  2, {  8, 13, 14, 15, 16, 17 }, {  2,  0,  2,  2,  2,  2 } }, //  2
	{ { 3, 0, 0 },  3, {  9, 14, 18, 19, 20, 21 }, {  3,  3,  0,  3,  3,  3 } }, //  3
	{ { 4, 0, 0 },  4, { 10, 15, 19, 22, 23, 24 }, {  4,  4,  4,  0,  4,  4 } }, //  4
	{ { 5, 0, 0 },  5, { 11, 16, 20, 23, 25, 26 }, {  5,  5,  5,  5,  0,  5 } }, //  5
	{ { 6, 0, 0 },  6, { 12, 17, 21, 24, 26, 27 }, {  6,  6,  6,  6,  6,  0 } }, //  6
	{ { 1, 1, 0 },  4, { 28, 29, 30, 31, 32, 33 }, {  0,  7,  7,  7,  7,  7 } }, //  7
	{ { 2, 1, 0 },  3, { 29, 34, 35, 36, 37, 38 }, {  2,  1,  8,  8,  8,  8 } }, //  8
	{ { 3, 1, 0 },  4, { 30, 35, 39, 40, 41, 42 }, {  3,  9,  1,  9,  9,  9 } }, //  9
	{ { 4, 1, 0 },  5, { 31, 36, 40, 43, 44, 45 }, {  4, 10, 10,  1, 10, 10 } }, // 10
	{ { 5, 1, 0 },  6, { 32, 37, 41, 44, 46, 47 }, {  5, 11, 11, 11,  1, 11 } }, // 11
	{ { 6, 1, 0 },  7, { 33, 38, 42, 45, 47, 48 }, {  6, 12, 12, 12, 12,  1 } }, // 12
	{ { 2, 2, 0 },  8, { 34, 49, 50, 51, 52, 53 }, { 13,  0, 13, 13, 13, 13 } }, // 13
	{ { 3, 2, 0 },  5, { 35, 50, 54, 55, 56, 57 }, { 14,  3,  2, 14, 14, 14 } }, // 14
	{ { 4, 2, 0 },  6, { 36, 51, 55, 58, 59, 60 }, { 15,  4, 15,  2, 15, 15 } }, // 15
	{ { 5, 2, 0 },  7, { 37, 52, 56, 59, 61, 62 }, { 16,  5, 16, 16,  2, 16 } }, // 16
	{ { 6, 2, 0 },  8, { 38, 53, 57, 60, 62, 63 }, { 17,  6, 17, 17, 17,  2 } }, // 17
	{ { 3, 3, 0 }, 12, { 39, 54, 64, 65, 66, 67 }, { 18, 18,  0, 18, 18, 18 } }, // 18
	{ { 4, 3, 0 },  7, { 40, 55, 65, 68, 69, 70 }, { 19, 19,  4,  3, 19, 19 } }, // 19
	{ { 5, 3, 0 },  8, { 41, 56, 66, 69, 71, 72 }, { 20, 20,  5, 20,  3, 20 } }, // 20
	{ { 6, 3, 0 },  9, { 42, 57, 67, 70, 72, 73 }, { 21, 21,  6, 21, 21,  3 } }, // 21
	{ { 4, 4, 0 }, 16, { 43, 58, 68, 74, 75, 76 }, { 22, 22, 22,  0, 22, 22 } }, // 22
	{ { 5, 4, 0 },  9, { 44, 59, 69, 75, 77, 78 }, { 23, 23, 23,  5,  4, 23 } }, // 23
	{ { 6, 4, 0 }, 10, { 45, 60, 70, 76, 78, 79 }, { 24, 24, 24,  6, 24,  4 } }, // 24
	{ { 5, 5, 0 }, 20, { 46, 61, 71, 77, 80, 81 }, { 25, 25, 25, 25,  0, 25 } }, // 25
	{ { 6, 5, 0 }, 11, { 47, 62, 72, 78, 81, 82 }, { 26, 26, 26, 26,  6,  5 } }, // 26
	{ { 6, 6, 0 }, 24, { 48, 63, 73, 79, 82, 83 }, { 27, 27, 27, 27, 27,  0 } }, // 27
	{ { 1, 1, 1 },  9, { 28, 28, 28, 28, 28, 28 }, {  0, 28, 28, 28, 28, 28 } }, // 28
	{ { 2, 1, 1 },  6, { 29, 29, 29, 29, 29, 29 }, {  2,  7, 29, 29, 29, 29 } }, // 29
	{ { 3, 1, 1 },  7, { 30, 30, 30, 30, 30, 30 }, {  3, 30,  7, 30, 30, 30 } }, // 30
	{ { 4, 1, 1 },  8, { 31, 31, 31, 31, 31, 31 }, {  4, 31, 31,  7, 31, 31 } }, // 31
	{ { 5, 1, 1 },  9, { 32, 32, 32, 32, 32, 32 }, {  5, 32, 32, 32,  7, 32 } }, // 32
	{ { 6, 1, 1 }, 10, { 33, 33, 33, 33, 33, 33 }, {  6, 33, 33, 33, 33,  7 } }, // 33
	{ { 2, 2, 1 },  9, { 34, 34, 34, 34, 34, 34 }, { 13,  1, 34, 34, 34, 34 } }, // 34
	{ { 3, 2, 1 },  6, { 35, 35, 35, 35, 35, 35 }, { 14,  9,  8, 35, 35, 35 } }, // 35
	{ { 4, 2, 1 },  7, { 36, 36, 36, 36, 36, 36 }, { 15, 10, 36,  8, 36, 36 } }, // 36
	{ { 5, 2, 1 },  8, { 37, 37, 37, 37, 37, 37 }, { 16, 11, 37, 37,  8, 37 } }, // 37
	{ { 6, 2, 1 },  9, { 38, 38, 38, 38, 38, 38 }, { 17, 12, 38, 38, 38,  8 } }, // 38
	{ { 3, 3, 1 }, 13, { 39, 39, 39, 39, 39, 39 }, { 18, 39,  1, 39, 39, 39 } }, // 39
	{ { 4, 3, 1 },  8, { 40, 40, 40, 40, 40, 40 }, { 19, 40, 10,  9, 40, 40 } }, // 40
	{ { 5, 3, 1 },  9, { 41, 41, 41, 41, 41, 41 }, { 20, 41, 11, 41,  9, 41 } }, // 41
	{ { 6, 3, 1 }, 10, { 42, 42, 42, 42, 42, 42 }, { 21, 42, 12, 42, 42,  9 } }, // 42
	{ { 4, 4, 1 }, 17, { 43, 43, 43, 43, 43, 43 }, { 22, 43, 43,  1, 43, 43 } }, // 43
	{ { 5, 4, 1 }, 10, { 44, 44, 44, 44, 44, 44 }, { 23, 44, 44, 11, 10, 44 } }, // 44
	{ { 6, 4, 1 }, 11, { 45, 45, 45, 45, 45, 45 }, { 24, 45, 45, 12, 45, 10 } }, // 45
	{ { 5, 5, 1 }, 21, { 46, 46, 46, 46, 46, 46 }, { 25, 46, 46, 46,  1, 46 } }, // 46
	{ { 6, 5, 1 }, 12, { 47, 47, 47, 47, 47, 47 }, { 26, 47, 47, 47, 12, 11 } }, // 47
	{ { 6, 6, 1 }, 25, { 48, 48, 48, 48, 48, 48 }, { 27, 48, 48, 48, 48,  1 } }, // 48
	{ { 2, 2, 2 }, 18, { 49, 49, 49, 49, 49, 49 }, { 49,  0, 49, 49, 49, 49 } }, // 49
	{ { 3, 2, 2 }, 11, { 50, 50, 50, 50, 50, 50 }, { 50,  3, 13, 50, 50, 50 } }, // 50
	{ { 4, 2, 2 }, 12, { 51, 51, 51, 51, 51, 51 }, { 51,  4, 51, 13, 51, 51 } }, // 51
	{ { 5, 2, 2 }, 13, { 52, 52, 52, 52, 52, 52 }, { 52,  5, 52, 52, 13, 52 } }, // 52
	{ { 6, 2, 2 }, 14, { 53, 53, 53, 53, 53, 53 }, { 53,  6, 53, 53, 53, 13 } }, // 53
	{ { 3, 3, 2 }, 14, { 54, 54, 54, 54, 54, 54 }, { 54, 18,  2, 54, 54, 54 } }, // 54
	{ { 4, 3, 2 },  9, { 55, 55, 55, 55, 55, 55 }, { 55, 19, 15, 14, 55, 55 } }, // 55
	{ { 5, 3, 2 }, 10, { 56, 56, 56, 56, 56, 56 }, { 56, 20, 16, 56, 14, 56 } }, // 56
	{ { 6, 3, 2 }, 11, { 57, 57, 57, 57, 57, 57 }, { 57, 21, 17, 57, 57, 14 } }, // 57
	{ { 4, 4, 2 }, 18, { 58, 58, 58, 58, 58, 58 }, { 58, 22, 58,  2, 58, 58 } }, // 58
	{ { 5, 4, 2 }, 11, { 59, 59, 59, 59, 59, 59 }, { 59, 23, 59, 16, 15, 59 } }, // 59
	{ { 6, 4, 2 }, 12, { 60, 60, 60, 60, 60, 60 }, { 60, 24, 60, 17, 60, 15 } }, // 60
	{ { 5, 5, 2 }, 22, { 61, 61, 61, 61, 61, 61 }, { 61, 25, 61, 61,  2, 61 } }, // 61
	{ { 6, 5, 2 }, 13, { 62, 62, 62, 62, 62, 62 }, { 62, 26, 62, 62, 17, 16 } }, // 62
	{ { 6, 6, 2 }, 26, { 63, 63, 63, 63, 63, 63 }, { 63, 27, 63, 63, 63,  2 } }, // 63
	{ { 3, 3, 3 }, 27, { 64, 64, 64, 64, 64, 64 }, { 64, 64,  0, 64, 64, 64 } }, // 64
	{ { 4, 3, 3 }, 16, { 65, 65, 65, 65, 65, 65 }, { 65, 65,  4, 18, 65, 65 } }, // 65
	{ { 5, 3, 3 }, 17, { 66, 66, 66, 66, 66, 66 }, { 66, 66,  5, 66, 18, 66 } }, // 66
	{ { 6, 3, 3 }, 18, { 67, 67, 67, 67, 67, 67 }, { 67, 67,  6, 67, 67, 18 } }, // 67
	{ { 4, 4, 3 }, 19, { 68, 68, 68, 68, 68, 68 }, { 68, 68, 22,  3, 68, 68 } }, // 68
	{ { 5, 4, 3 }, 12, { 69, 69, 69, 69, 69, 69 }, { 69, 69, 23, 20, 19, 69 } }, // 69
	{ { 6, 4, 3 }, 13, { 70, 70, 70, 70, 70, 70 }, { 70, 70, 24, 21, 70, 19 } }, // 70
	{ { 5, 5, 3 }, 23, { 71, 71, 71, 71, 71, 71 }, { 71, 71, 25, 71,  3, 71 } }, // 71
	{ { 6, 5, 3 }, 14, { 72, 72, 72, 72, 72, 72 }, { 72, 72, 26, 72, 21, 20 } }, // 72
	{ { 6, 6, 3 }, 27, { 73, 73, 73, 73, 73, 73 }, { 73, 73, 27, 73, 73,  3 } }, // 73
	{ { 4, 4, 4 }, 36, { 74, 74, 74, 74, 74, 74 }, { 74, 74, 74,  0, 74, 74 } }, // 74
	{ { 5, 4, 4 }, 21, { 75, 75, 75, 75, 75, 75 }, { 75, 75, 75,  5, 22, 75 } }, // 75
	{ { 6, 4, 4 }, 22, { 76, 76, 76, 76, 76, 76 }, { 76, 76, 76,  6, 76, 22 } }, // 76
	{ { 5, 5, 4 }, 24, { 77, 77, 77, 77, 77, 77 }, { 77, 77, 77, 25,  4, 77 } }, // 77
	{ { 6, 5, 4 }, 15, { 78, 78, 78, 78, 78, 78 }, { 78, 78, 78, 26, 24, 23 } }, // 78
	{ { 6, 6, 4 }, 28, { 79, 79, 79, 79, 79, 79 }, { 79, 79, 79, 27, 79,  4 } }, // 79
	{ { 5, 5, 5 }, 45, { 80, 80, 80, 80, 80, 80 }, { 80, 80, 80, 80,  0, 80 } }, // 80
	{ { 6, 5, 5 }, 26, { 81, 81, 81, 81, 81, 81 }, { 81, 81, 81, 81,  6, 25 } }, // 81
	{ { 6, 6, 5 }, 29, { 82, 82, 82, 82, 82, 82 }, { 82, 82, 82, 82, 27,  5 } }, // 82
	{ { 6, 6, 6 }, 54, { 83, 83, 83, 83, 83, 83 }, { 83, 83, 83, 83, 83,  0 } }  // 83
};

///////////////////////////////////////////////////////////////////////////////////////////////////

void initBoard(game_board_t *board) {
	memset(board, 0, sizeof(game_board_t));
}

void displayBoard(game_board_t *board) {
	for (uint8_t player = 0; player < 2; player++) {
		std::cout << "Player " << (int)(player + 1) << ": ";
		for (uint8_t move = 0; move < 3; move++) {
			const triplet_data_t *triplet = &TRIPLETS_DATA[board->triplets[player][move]];
			std::cout << "[" <<
				DICE_SYMBOLS.at(triplet->diceRolls[0]) << " " <<
				DICE_SYMBOLS.at(triplet->diceRolls[1]) << " " <<
				DICE_SYMBOLS.at(triplet->diceRolls[2]) << "] ";
		}
		std::cout << std::endl;
	}
	std::cout << "Score: " << (int)(board->score) << std::endl;
}

bool isMoveValid(game_board_t *board, uint8_t player, uint8_t move) {
	return (0 == TRIPLETS_DATA[board->triplets[player][move]].diceRolls[2]);
}

bool moreMovesPossible(game_board_t *board, uint8_t player) {
	bool result = false;
	result |= isMoveValid(board, player, 0);
	result |= isMoveValid(board, player, 1);
	result |= isMoveValid(board, player, 2);
	return result;
}

void applyMove(game_board_t *board, rollback_data_t *rollbackData, uint8_t player, uint8_t move, uint8_t diceRoll) {
	rollbackData->triplets[0] = board->triplets[0][move];
	rollbackData->triplets[1] = board->triplets[1][move];
	rollbackData->score = board->score;

	int16_t scoreChange = 0;
	scoreChange -= (int16_t)(TRIPLETS_DATA[board->triplets[0][move]].score);
	scoreChange += (int16_t)(TRIPLETS_DATA[board->triplets[1][move]].score);

	const uint8_t opponent = 1 - player;
	board->triplets[player][move] = TRIPLETS_DATA[board->triplets[player][move]].idxOnAdd[diceRoll - 1];
	board->triplets[opponent][move] = TRIPLETS_DATA[board->triplets[opponent][move]].idxOnRemove[diceRoll - 1];

	scoreChange += (int16_t)(TRIPLETS_DATA[board->triplets[0][move]].score);
	scoreChange -= (int16_t)(TRIPLETS_DATA[board->triplets[1][move]].score);
	
	board->score += scoreChange;
}

void rollbackMove(game_board_t *board, rollback_data_t *rollbackData, uint8_t move) {
	board->triplets[0][move] = rollbackData->triplets[0];
	board->triplets[1][move] = rollbackData->triplets[1];
	board->score = rollbackData->score;
}

double negamax(game_board_t *board, uint8_t player, uint8_t diceRoll, uint8_t depth, uint8_t &bestMove) {
	const int16_t direction = (player == 0) ? +1 : -1;

	if (depth == 0) {
		return (double)(direction * board->score);
	}

	bestMove = INVALID_MOVE;
	double bestScore = 0.0;

	for (uint8_t move = 0; move < 3; move++) {
		if (isMoveValid(board, player, move)) {
			rollback_data_t rollbackData;
			applyMove(board, &rollbackData, player, move, diceRoll);

			double score = 0.0;
			if (moreMovesPossible(board, player)) {
				for (uint8_t nextDiceRoll = 1; nextDiceRoll < 7; nextDiceRoll++) {
					uint8_t ignored; // opponent's best move
					score += -negamax(board, 1 - player, nextDiceRoll, depth - 1, ignored);
				}
				score /= 6.0;
			}
			else {
				score = (double)(direction * board->score);
			}

			if ((score > bestScore) || (bestMove == INVALID_MOVE)) {
				bestScore = score;
				bestMove = move;
			}

			rollbackMove(board, &rollbackData, move);
		}
	}

	return bestScore;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void gameLoop() {
	std::random_device rngDev;
	std::mt19937 rng(rngDev());
	std::uniform_int_distribution<std::mt19937::result_type> U_01(0, 1);
	std::uniform_int_distribution<std::mt19937::result_type> U_16(1, 6);

	game_board_t board;
	rollback_data_t rollbackData;
	uint8_t player = (uint8_t)(U_01(rng));

	initBoard(&board);

	std::cout << "Starting new game" << std::endl;

	bool done = false;
	uint16_t turnsCount = 0;

	while (!done) {
		uint8_t diceRoll = (uint8_t)(U_16(rng));

		displayBoard(&board);
		std::cout << "Dice roll: " << (int)diceRoll << std::endl;

		uint8_t move = INVALID_MOVE;
		if (player == 0) {
			int userInput;

			std::cout << "Your turn. Enter move (1-3): ";
			std::cin >> userInput;

			move = (uint8_t)(userInput - 1);
		}
		else {
			std::cout << "AI turn. Move: ";

			uint8_t bestMove = INVALID_MOVE;
			double negamaxValue = negamax(&board, player, diceRoll, 7, bestMove);

			std::cout << (int)(bestMove + 1) << " (negamax=" << negamaxValue << ")" << std::endl;

			move = bestMove;
		}

		if (isMoveValid(&board, player, move)) {
			applyMove(&board, &rollbackData, player, move, diceRoll);
			done = !moreMovesPossible(&board, player);
			player = 1 - player;
			turnsCount++;
		}
		else {
			std::cout << "Invalid move" << std::endl;
		}

		std::cout << std::endl;
	}

	std::cout << "Game over. Turns count: " << (int)turnsCount << std::endl;
	displayBoard(&board);
	std::cout << std::endl;
}

int main() {
	while (true) {
		gameLoop();
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
