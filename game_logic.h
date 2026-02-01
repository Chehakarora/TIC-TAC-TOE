#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#define MAX_USERS 200

typedef struct {
    char username[51];
    char password[51];
    int gamesPlayed;
    int wins;
    int losses;
    int draws;
} User;

extern User users[MAX_USERS];
extern int loggedInIndex;

/* FUNCTIONS */
void ReadUserData();
void SaveUserData();
int RegisterUser(const char *username, const char *password);
int LoginUser(const char *username, const char *password);

void ResetBoard(char board[3][3]);
int CheckWinner(char board[3][3]);
int IfGameDraw(char board[3][3]);
void UpdateUserData(int result);

#endif
