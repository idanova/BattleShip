#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>   // For toupper()
#include <time.h>    // For random seeding
#include "Structs.h" // Include the header file for structs


void resetBoard(gameBoard* pboard) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			pboard->ships[i][j] = '.'; // Empty sea
			pboard->tracking[i][j] = '.'; // No shots taken yet
		}
	}
}

void drawBoard(gameBoard* pboard)
{
    // Print column headers
    printf("   ");
    for (int j = 0; j < SIZE; j++)
        printf(" %c  ", 'A' + j);
    printf("\n");

    for (int i = 0; i < SIZE; i++)
    {
        // Row number
        printf("%2d ", i + 1);

        for (int j = 0; j < SIZE; j++)
        {
            if(pboard->ships[i][j]=='S')
                printf(GREEN " %c" RESET, pboard->ships[i][j]);
            else
                printf(" %c", pboard->ships[i][j]);

            if (j < SIZE - 1)
                printf(" |");
        }
        printf("\n");

        // Print separator (no trailing `+`)
        if (i < SIZE - 1)
        {
            printf("   ");
            for (int j = 0; j < SIZE; j++)
            {
                printf("---");
                if (j < SIZE - 1)
                    printf("+");
            }
            printf("\n");
        }
    }
    printf("\n\n\n");
}

int canPlaceShip(gameBoard* board, int x, int y, int size, char direction) {
    if (direction == 'H') {
        if (y + size > SIZE) return 0;
        for (int i = 0; i < size; i++)
            if (board->ships[x][y + i] != '.') return 0;
    }
    else if (direction == 'V') {
        if (x + size > SIZE) return 0;
        for (int i = 0; i < size; i++)
            if (board->ships[x + i][y] != '.') return 0;
    }
    else {
        return 0;
    }
    return 1;
}

void placeShipOnBoard(gameBoard* board, int x, int y, int size, char direction) {
    for (int i = 0; i < size; i++) {
        if (direction == 'H')
            board->ships[x][y + i] = 'S';
        else
            board->ships[x + i][y] = 'S';
    }
}

void activateVision(Player* computer, Player* player) {
    if (visionUsesLeft <= 0) {
        printf("No vision uses left!\n");
        return;
    }

    int x;
    char colChar;
    printf("Enter location to activate vision (row 1-10, column A-J): ");
    scanf(" %d %c", &x, &colChar);
    x -= 1;
    int y = toupper(colChar) - 'A';

    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        printf("Invalid coordinates!\n");
        return;
    }

    visionCenter.row = x;
    visionCenter.col = y;
    visionActive = 1;
    visionUsesLeft--;

    printf("Vision activated at %d%c for 1 turn!\n", x + 1, 'A' + y);
	printf("Press ENTER to continue...\n");
	getchar(); getchar(); // double getchar to handle newline after scanf
	system("cls");
	
}

void manualShipPlacement(Player* player, ship* defaultShips) {
    
    system("cls");
    drawBoard(&player->board.ships);
   
    for (int s = 0; s < NUM_SHIPS; s++) {
        int x, y;
        char dir, colChar;
        int placed = 0;

        while (!placed) {
            
            printf("Place your %s (size %d)\n", defaultShips[s].name, defaultShips[s].size);
            printf("Enter row (1-10), column (A-J), direction (H/V): ");

            scanf(" %d %c %c", &x, &colChar, &dir);

            x -= 1;
            y = toupper(colChar) - 'A';
            dir = toupper(dir);

            if (x >= 0 && x < SIZE && y >= 0 && y < SIZE &&
                canPlaceShip(&player->board.ships, x, y, defaultShips[s].size, dir)) {

                placeShipOnBoard(&player->board.ships, x, y, defaultShips[s].size, dir);
                player->data[s] = defaultShips[s];
                placed = 1;

                
            }
            else {
                printf("Invalid placement. Try again.\n\n");
                printf("Press ENTER to continue\n");
                getchar(); getchar();
            }
            system("cls");
            drawBoard(&player->board.ships);
        }
    }
}

void randomShipPlacement(Player* player, ship* defaultShips) {
    
    for (int s = 0; s < NUM_SHIPS; s++) {
        int placed = 0;

        while (!placed) {
            int x = rand() % SIZE;
            int y = rand() % SIZE;
            char dir = (rand() % 2 == 0) ? 'H' : 'V';

            if (canPlaceShip(&player->board.ships, x, y, defaultShips[s].size, dir)) {
                placeShipOnBoard(&player->board.ships, x, y, defaultShips[s].size, dir);
                player->data[s] = defaultShips[s];
                placed = 1;
            }
        }
    }
}

void drawBoardsSideBySide(gameBoard* left, gameBoard* right, const char* leftLabel, const char* rightLabel, int hideRightShips) {
    char ch;
    // Print board titles
    printf("                   %-40s       %-40s\n\n", leftLabel, rightLabel);

    // Column headers
    printf("   ");
    for (int j = 0; j < SIZE; j++)
        printf(" %c  ", 'A' + j);
    printf("       ");
    for (int j = 0; j < SIZE; j++)
        printf(" %c  ", 'A' + j);
    printf("\n");
    
    // Row-by-row display
    for (int i = 0; i < SIZE; i++) {
        // Left board row
        printf("%2d ", i + 1);
        for (int j = 0; j < SIZE; j++) {
            ch = left->ships[i][j];
            // Color coding
            if (ch == 'X')
                printf(RED " %c " RESET, ch);         // Hit
            else if (ch == 'O')
                printf(BLUE " %c " RESET, ch);        // Miss
            else if (ch == 'S' && !hideRightShips)
                printf(GREEN " %c " RESET, ch);       // Ship (if visible)
            else
                printf(" %c ", ch);                   // Default
            if (j < SIZE - 1) printf("|");
        }

        // Spacing between boards
        printf("     ");

        // Right board row
        printf("%2d ", i + 1);
        for (int j = 0; j < SIZE; j++) {
            ch = right->ships[i][j];  // enemy’s ship layout (for vision)

            if (visionActive) {
                int dx = i - visionCenter.row;
                int dy = j - visionCenter.col;
                if (dx * dx + dy * dy <= 4) {
                    // Show real ship for vision
                    ch = right->ships[i][j];
                }
                else {
                    // Show result of shots: hit (X), miss (O), or hidden
                    ch = right->tracking[i][j];  // player’s record of shots on enemy
                }
            }
            else {
                ch = right->tracking[i][j];  // Show result of player’s shots
            }

            if (hideRightShips && ch == 'S') ch = '.';
			
            // Color coding
            if (ch == 'X')
                printf(RED " %c " RESET, ch);         // Hit
            else if (ch == 'O')
                printf(BLUE " %c " RESET, ch);        // Miss
            else if (ch == 'S' && !hideRightShips)
                printf(GREEN " %c " RESET, ch);       // Ship (if visible)
            else
                printf(" %c ", ch);                   // Default


            if (j < SIZE - 1) printf("|");
        }
        printf("\n");

        // Separator line
        if (i < SIZE - 1) {
            printf("   ");
            for (int j = 0; j < SIZE; j++) {
                printf("---");
                if (j < SIZE - 1) printf("+");
            }

            printf("     ");

            printf("   ");
            for (int j = 0; j < SIZE; j++) {
                printf("---");
                if (j < SIZE - 1) printf("+");
            }

            printf("\n");
        }
    }
}

int fireAt(gameBoard* targetBoard, gameBoard* trackingBoard, int x, int y) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        printf("Invalid coordinates!\n");
        return -1;
    }


    // Prevent firing at same location twice

    if (trackingBoard) {
        if (trackingBoard->tracking[x][y] == 'X' || trackingBoard->tracking[x][y] == 'O') {
            printf("You already fired at %c%d!\n", y + 'A', x + 1);
            return -1;
        }
    }
    

        if (targetBoard->ships[x][y] == 'S') {
            targetBoard->ships[x][y] = 'X';
            if(trackingBoard) trackingBoard->tracking[x][y] = 'X';
            printf("Hit at %c%d!\n", y + 'A', x + 1);
            return 1;
        }
        else {
            targetBoard->ships[x][y] = 'O';
            if(trackingBoard) trackingBoard->tracking[x][y] = 'O';
            printf("Miss at %c%d.\n", y + 'A', x + 1);
            return 0;
        }
    

        return -1;




}

Point getSmartShot(Player* player) {
    Point shot;
    int attempts = 0;
	const int MAX_ATTEMPTS = 100; // Arbitrary limit to avoid infinite loop

    while (attempts++ < MAX_ATTEMPTS) {
        if (aiState.mode == 0) {

            // Random hunt mode
            shot.row = rand() % SIZE;
            shot.col = rand() % SIZE;

            if (player->board.ships[shot.row][shot.col] != 'X' &&
                player->board.ships[shot.row][shot.col] != 'O') {
                return shot;
            }
        }

        else if (aiState.mode == 1) {

			// Tracking mode

            int foundDirection = 0;
            Direction selectedDir;

            for (Direction i = UP; i <= LEFT; i++) {
                if (!aiState.tried[i]) {
                    int nx = aiState.origin.row + directions[i].row;
                    int ny = aiState.origin.col + directions[i].col;

                    aiState.tried[i] = 1;

                    if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE &&
                        player->board.ships[nx][ny] != 'X' &&
                        player->board.ships[nx][ny] != 'O') {

                        foundDirection = 1;
                        selectedDir = i;

                        break;
                    }
                }
            }

            if (foundDirection) {
				// Update AI state
                aiState.dirIndex = selectedDir;
                aiState.current.row = aiState.origin.row + directions[selectedDir].row;
                aiState.current.col = aiState.origin.col + directions[selectedDir].col;
                aiState.mode = 2;
                return aiState.current;
            }
            else {
                aiState.mode = 0;
            }
        }


        else if (aiState.mode == 2) {
            //Ship ANNIHILATION mode

            int dx = directions[aiState.dirIndex].row;
            int dy = directions[aiState.dirIndex].col;

            int step = aiState.reverse ? -1 : 1;
            int nx = aiState.current.row + dx * step;
            int ny = aiState.current.col + dy * step;

            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE &&
                player->board.ships[nx][ny] != 'X' && player->board.ships[nx][ny] != 'O') {

                aiState.current.row = nx;
                aiState.current.col = ny;
                return aiState.current;

            }
            else {
                // Direction invalid (already hit or out of bounds)
                if (aiState.reverse) {
                    aiState.mode = 0;
                    aiState.reverse = 0;
                    for (int i = 0; i < 4; i++) aiState.tried[i] = 0;
                }
                else {
                    aiState.reverse = 1;
                    aiState.current = aiState.origin;
                }
                // Loop continues
            }
        }



    }

    // Fallback in case loop failed
    for (int i = 0; i < SIZE * SIZE; i++) {
        int r = i / SIZE;
        int c = i % SIZE;
        if (player->board.ships[r][c] != 'X' && player->board.ships[r][c] != 'O') {
            return (Point) { r, c };
        }
    }

    return (Point) { 0, 0 };
}

int allShipsSunk(gameBoard* board) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board->ships[i][j] == 'S') {
                return 0;  // A ship is still alive
            }
        }
    }
    return 1;  // All ships are sunk
}

void updateAIStateOnHit(Point hit) {
    if (aiState.mode == 0) {
        aiState.origin = hit;
        aiState.current = hit;
        aiState.dirIndex = 0; // start from first direction
        aiState.reverse = 0;
        aiState.mode = 1;

        for (int i = 0; i < 4; i++) {
            aiState.tried[i] = 0;
        }
    }
    // If mode is already 1 or 2, no change needed — tracking continues
}

void updateAIStateOnMiss() {
    if (aiState.mode == 2) {
        if (aiState.reverse) {
            // Already tried both directions → give up
            aiState.mode = 0;
            aiState.reverse = 0;
            for (int i = 0; i < 4; i++) aiState.tried[i] = 0;
        }
        else {
            // First miss — reverse direction
            aiState.reverse = 1;
            aiState.current = aiState.origin;
        }
    }
}

void gameLoop(Player* player, Player* computer) {
    while (1) {
        system("cls");
        drawBoardsSideBySide(&player->board, &computer->board, "Your Board", "Your Shots", 0);

        // --- Vision ---


        if (visionUsesLeft) {
            char choice;
            printf("\nDo you want to use Vision? (%d left) [Y/N]: ", visionUsesLeft);
            scanf(" %c", &choice);
            while (getchar() != '\n');  // flush remaining characters

            choice = toupper(choice);
            if (choice == 'Y') {
                activateVision(computer, player);
            }

        }


		system("cls");
        drawBoardsSideBySide(&player->board, &computer->board, "Your Board", "Your Shots", 0);


        // --- Player's Turn ---

  

        int x, y;
        char colChar;
        int valid = 0;

        while (!valid) {
           
            
            printf("\nYour turn! Enter target (row 1-10 and column A-J): ");
            scanf(" %d %c", &x, &colChar);
            x -= 1;
            y = toupper(colChar) - 'A';

            int result = fireAt(&computer->board, &computer->board, x, y);
            if (result != -1) valid = 1;
        }

        if (allShipsSunk(&computer->board.ships)) {
            printf(" You win! All enemy ships destroyed.\n");
            break;
        }

        // --- Computer's Turn ---
        printf("\nComputer's turn...\n");
        Point compShot = getSmartShot(player);

        int compResult = fireAt(&player->board, NULL, compShot.row, compShot.col);

        if (compResult == 1) {
            updateAIStateOnHit(compShot);
        }
        else if (compResult == 0) {
            updateAIStateOnMiss();
        }


        if (allShipsSunk(&player->board.ships)) {
            printf(" You lost! All your ships are gone.\n");
            break;
        }

        visionActive = 0;
        printf("\nPress Enter to continue...\n");
        getchar(); getchar(); // double getchar to handle newline after scanf
    }
}

