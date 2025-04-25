#include "Func.h"

void main()
{
  


    Player player, computer;

    srand(time(NULL)); //for pc ship placement
    resetBoard(&computer.board);
    resetBoard(&player.board);
    manualShipPlacement(&player, defaultShips);


	printf("Press ENTER to continue...\n");
	getchar(); getchar(); // double getchar to handle newline after scanf


    randomShipPlacement(&computer, defaultShips);
    gameLoop(&player, &computer);
   


}

