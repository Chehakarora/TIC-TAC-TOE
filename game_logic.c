#include <stdio.h>
#include <string.h>

#define MAX_USERS 200
#define MAX_NAME_LENGTH 51
#define MAX_PASSWORD_LENGTH 51

typedef struct {
    char username[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int gamesPlayed;
    int wins;
    int losses;
    int draws;
} User;

/* GLOBALS */
User users[MAX_USERS];
int userCount = 0;
int loggedInIndex = -1;

/* FILE HANDLING */
void ReadUserData() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return;

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

void SaveUserData() {
    FILE *fp = fopen("users.txt", "w");
    if (!fp) return;

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

/* AUTH */
int RegisterUser(const char *username, const char *password) {
    if (strlen(username) == 0 || strlen(password) == 0)
        return 0;

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0)
            return 0;
    }

    if (userCount >= MAX_USERS)
        return 0;

    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    users[userCount].gamesPlayed = 0;
    users[userCount].wins = 0;
    users[userCount].losses = 0;
    users[userCount].draws = 0;

    userCount++;
    return 1;   // SAVE ON EXIT
}

int LoginUser(const char *username, const char *password) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            loggedInIndex = i;
            return 1;
        }
    }
    return 0;
}

/* GAME LOGIC */
void ResetBoard(char board[3][3]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';
}

int CheckWinner(char board[3][3]) {
    for (int i = 0; i < 3; i++)
        if (board[i][0] == board[i][1] &&
            board[i][1] == board[i][2] &&
            board[i][0] != ' ')
            return board[i][0] == 'X' ? 1 : 2;

    for (int i = 0; i < 3; i++)
        if (board[0][i] == board[1][i] &&
            board[1][i] == board[2][i] &&
            board[0][i] != ' ')
            return board[0][i] == 'X' ? 1 : 2;

    if (board[0][0] == board[1][1] &&
        board[1][1] == board[2][2] &&
        board[0][0] != ' ')
        return board[0][0] == 'X' ? 1 : 2;

    if (board[0][2] == board[1][1] &&
        board[1][1] == board[2][0] &&
        board[0][2] != ' ')
        return board[0][2] == 'X' ? 1 : 2;

    return 0;
}

int IfGameDraw(char board[3][3]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == ' ')
                return 0;
    return 1;
}

void UpdateUserData(int result) {
    if (loggedInIndex == -1) return;

    users[loggedInIndex].gamesPlayed++;
    if (result == 1) users[loggedInIndex].wins++;
    else if (result == 0) users[loggedInIndex].losses++;
    else users[loggedInIndex].draws++;
}
