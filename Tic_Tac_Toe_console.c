#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 200
#define MAX_NAME_LENGTH 51
#define MAX_PASSWORD_LENGTH 51

// USER INFORMATION STRUCTURE
typedef struct {
    char username[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int gamesPlayed;
    int wins;
    int losses;
    int draws;
} User;

// GLOBAL VARIABLES USED THROUGHOUT THE PROGRAM
User users[MAX_USERS];
int userCount = 0;
int loggedInIndex = -1;

// FUNCTION DECLARATION AND PROTOTYPE
void ReadUserData();
void SaveUserData();
int RegisterUser();
int LoginUser();
void MainMenu();
void BeginGame();
void DisplayBoard(char board[3][3]);
int CheckWinner(char board[3][3]);
int IfGameDraw(char board[3][3]);
void ResetBoard(char board[3][3]);
int IfValidMove(char board[3][3], int move);
void UpdateUserData(int result); // 1=win, 0=loss, 2=draw

// FUNCTIONS RELATED TO USER REGISTRATION, LOGIC AND DATA HANDLING

// Reads user data from file at the start of the program
void ReadUserData() {
    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL) return;

    userCount = 0;

    while (userCount < MAX_USERS &&
           fscanf(fp, "%s %s %d %d %d %d",
                  users[userCount].username,
                  users[userCount].password,
                  &users[userCount].gamesPlayed,
                  &users[userCount].wins,
                  &users[userCount].losses,
                  &users[userCount].draws) == 6) {

        userCount++;
    }

    fclose(fp);
}
// Saves updated user data back to file
void SaveUserData() {
    FILE *fp = fopen("users.txt", "w");
    for (int i = 0; i < userCount; i++) {
        fprintf(fp, "%s %s %d %d %d %d\n",
                users[i].username,
                users[i].password,
                users[i].gamesPlayed,
                users[i].wins,
                users[i].losses,
                users[i].draws);
    }
    fclose(fp);
}
// Handles new user registration and checks for duplicates
int RegisterUser() {
    char username[MAX_NAME_LENGTH], password[MAX_PASSWORD_LENGTH];
    printf("\n--- REGISTER ---\n");
    printf("Enter username: ");
    scanf("%50s", username);

    // check if already registered
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Username already exists!\n");
            return 0;
        }
    }
    // check user limit
    if (userCount >= MAX_USERS) {
        printf("User limit reached!\n");
        return 0;
    }

    printf("Enter password: ");
    scanf("%50s", password);

    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    users[userCount].gamesPlayed = 0;
    users[userCount].wins = 0;
    users[userCount].losses = 0;
    users[userCount].draws = 0;
    userCount++;

    SaveUserData();
    printf("Registration successful! You can now login and play!\n");
    return 1;
}
// Verifies user credentials and logs the user in
int LoginUser() {
    char username[MAX_NAME_LENGTH], password[MAX_PASSWORD_LENGTH];
    printf("\n--- LOGIN ---\n");
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            loggedInIndex = i;
            printf("\nWelcome back, %s! Ready to play?\n", username);
            return 1;
        }
    }
    printf("Invalid credentials! Please try again.\n");
    return 0;
}

// FUNCTIONS RELATED TO THE TIC TAC TOE BOARD AND GAME LOGIC

// Resets the game board with numbers 1 to 9
void ResetBoard(char board[3][3]) {
    int num = 1;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = '0' + num++;
}
// Displays the current state of the Tic Tac Toe board
void DisplayBoard(char board[3][3]) {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        printf(" ");
        for (int j = 0; j < 3; j++) {
            printf("%c", board[i][j]);
            if (j < 2) printf(" | ");
        }
        printf("\n");
        if (i < 2) printf("---|---|---\n");
    }
    printf("\n");
}
// Checks whether the move entered by the player is valid
int IfValidMove(char board[3][3], int move) {
    if (move < 1 || move > 9) return 0;
    int row = (move - 1) / 3;
    int col = (move - 1) % 3;
    if (board[row][col] == 'X' || board[row][col] == 'O')
        return 0;
    return 1;
}
// Checks all possible winning combinations
// Returns 1 for X win, 2 for O win, 0 otherwise
int CheckWinner(char board[3][3]) { 
    // Rows 
    for (int i = 0; i < 3; i++) 
    if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) 
    return board[i][0] == 'X' ? 1 : 2; 
    // Columns 
    for (int i = 0; i < 3; i++) 
    if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) 
    return board[0][i] == 'X' ? 1 : 2; 
    // Diagonals 
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) 
    return board[0][0] == 'X' ? 1 : 2; 
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) 
    return board[0][2] == 'X' ? 1 : 2;
    return 0; 
    }
// Checks if the game has ended in a draw
int IfGameDraw(char board[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != 'X' && board[i][j] != 'O') {
                return 0; 
            }
        }
    }
    return 1;   
}


// MAIN GAME FLOW AND ROUND HANDLING

// Controls the complete gameplay including multiple rounds
void BeginGame() {
    int rounds, round = 1, xWins = 0, oWins = 0, draws = 0;
    char board[3][3];
    char currentPlayer = 'X';

    while (1) {
    printf("\nEnter number of rounds: ");

    if (scanf("%d", &rounds) != 1) {
        printf("Invalid input! Please enter a number.\n");
        while (getchar() != '\n');   // remove extra input from buffer
        continue;
    }
    if (rounds <= 0) {
        printf("Number of rounds must be greater than 0.\n");
        continue;
    }
    break; 
     }
    
    printf("\nGame starting... Best of luck to both players!\n");
    
    while (round <= rounds) {
        printf("\n===== ROUND %d =====\n", round);
        ResetBoard(board);
        int winner = 0;

        while (winner == 0 && !IfGameDraw(board)) {
            DisplayBoard(board);
            int move;
            printf("Player %c, enter your move (1-9): ", currentPlayer);
            if (scanf("%d", &move) != 1) {
            printf("Invalid input! Enter a number.\n");
            while(getchar() != '\n');   // remove extra input from buffer
            continue;
            }
            if (!IfValidMove(board, move)) {
                printf("Invalid move! Try again.\n");
                continue;
            }
            int r = (move - 1) / 3;
            int c = (move - 1) % 3;
            board[r][c] = currentPlayer;

            winner = CheckWinner(board);
            if (winner || IfGameDraw(board))
                break;
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }

        DisplayBoard(board);
        if (winner == 1) {
            printf("Player X wins this round!\n");
            xWins++;
        } else if (winner == 2) {
            printf("Player O wins this round!\n");
            oWins++;
        } else {
            printf("It's a draw!\n");
            draws++;
        }

        round++;
        currentPlayer = (round % 2 == 1) ? 'X' : 'O'; // change starting player for next round

    }

    printf("\n     MATCH SUMMARY     \n");
    printf("Player X: %d wins | Player O: %d wins | Draws: %d\n", xWins, oWins, draws);

    if (xWins > oWins) {
        printf(">>> Player X wins the match!\n");
        UpdateUserData(1);
    } else if (oWins > xWins) {
        printf(">>> Player O wins the match!\n");
        UpdateUserData(0);
    } else {
        printf(">>> Match Drawn!\n");
        UpdateUserData(2);
    }
}
// Updates the statistics of the logged-in user after a match
void UpdateUserData(int result) {
    if (loggedInIndex == -1) return;
    users[loggedInIndex].gamesPlayed++;
    if (result == 1) users[loggedInIndex].wins++;
    else if (result == 0) users[loggedInIndex].losses++;
    else users[loggedInIndex].draws++;
    SaveUserData();
}

// MAIN MENU SHOWN TO THE USER

// Displays the main menu and handles user choices
void MainMenu() {
    int choice;
    while (1) {
        printf("\n      TIC TAC TOE      \n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
        printf("Invalid input!\n");
        while(getchar() != '\n');
        continue;
}

        switch (choice) {
            case 1:
                RegisterUser();
                break;
            case 2:
                if (LoginUser()) {
                    printf("\nYour Stats: Games=%d Wins=%d Losses=%d Draws=%d\n",
                           users[loggedInIndex].gamesPlayed,
                           users[loggedInIndex].wins,
                           users[loggedInIndex].losses,
                           users[loggedInIndex].draws);
                    BeginGame();
                }
                break;
            case 3:
                printf("Goodbye!\n");
                return;
            default:
                printf("Invalid choice!\n");
        }
    }
}

int main() {
    ReadUserData();
    MainMenu();
    return 0;
}