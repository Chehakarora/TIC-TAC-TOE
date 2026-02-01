#include <gtk/gtk.h>
#include "game_logic.h"

/* GLOBALS */
GtkWidget *window, *stack;
GtkWidget *username_entry, *password_entry, *status_label;
GtkWidget *buttons[3][3];
GtkWidget *score_value_labels[5];

char board[3][3];
char currentPlayer = 'X';

/* SAVE ON EXIT */
void on_app_close(GtkWidget *widget, gpointer data) {
    SaveUserData();
    gtk_main_quit();
}

/* HELPERS */
GtkWidget* colored_label(const char *text, const char *color, int size) {
    GtkWidget *label = gtk_label_new(NULL);
    char markup[256];
    sprintf(markup,
            "<span foreground='%s' font='Sans Bold %d'>%s</span>",
            color, size, text);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    return label;
}

GtkWidget* bordered(GtkWidget *child) {
    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_add(GTK_CONTAINER(frame), child);
    return frame;
}

/* LOGIN */
void on_login_clicked(GtkButton *btn, gpointer data) {
    const char *u = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *p = gtk_entry_get_text(GTK_ENTRY(password_entry));

    if (LoginUser(u, p)) {
        ResetBoard(board);
        currentPlayer = 'X';
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "game");
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "Invalid credentials");
    }
}

void on_register_clicked(GtkButton *btn, gpointer data) {
    const char *u = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *p = gtk_entry_get_text(GTK_ENTRY(password_entry));

    if (RegisterUser(u, p))
        gtk_label_set_text(GTK_LABEL(status_label), "Registered successfully!");
    else
        gtk_label_set_text(GTK_LABEL(status_label), "User already exists!");
}

/* GAME */
void set_button_symbol(GtkWidget *btn, char s) {
    GtkWidget *lbl = gtk_bin_get_child(GTK_BIN(btn));
    char m[64];

    if (s == 'X')
        sprintf(m, "<span foreground='red' font='Sans Bold 28'>X</span>");
    else
        sprintf(m, "<span foreground='blue' font='Sans Bold 28'>O</span>");

    gtk_label_set_markup(GTK_LABEL(lbl), m);
}

void on_cell_clicked(GtkButton *btn, gpointer data) {
    int idx = GPOINTER_TO_INT(data);
    int r = idx / 3, c = idx % 3;

    if (board[r][c] != ' ') return;

    board[r][c] = currentPlayer;
    set_button_symbol(GTK_WIDGET(btn), currentPlayer);

    int win = CheckWinner(board);
    if (win || IfGameDraw(board)) {

        if (win == 1) UpdateUserData(1);
        else if (win == 2) UpdateUserData(0);
        else UpdateUserData(2);

        gtk_label_set_text(GTK_LABEL(score_value_labels[0]),
                           users[loggedInIndex].username);

        char buf[16];
        sprintf(buf, "%d", users[loggedInIndex].gamesPlayed);
        gtk_label_set_text(GTK_LABEL(score_value_labels[1]), buf);
        sprintf(buf, "%d", users[loggedInIndex].wins);
        gtk_label_set_text(GTK_LABEL(score_value_labels[2]), buf);
        sprintf(buf, "%d", users[loggedInIndex].losses);
        gtk_label_set_text(GTK_LABEL(score_value_labels[3]), buf);
        sprintf(buf, "%d", users[loggedInIndex].draws);
        gtk_label_set_text(GTK_LABEL(score_value_labels[4]), buf);

        gtk_stack_set_visible_child_name(GTK_STACK(stack), "score");
        return;
    }

    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
}

void on_play_again(GtkButton *btn, gpointer data) {
    ResetBoard(board);
    currentPlayer = 'X';

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            gtk_label_set_text(GTK_LABEL(
                gtk_bin_get_child(GTK_BIN(buttons[i][j]))), " ");

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "game");
}

/* UI PAGES */
GtkWidget* build_title() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(box), colored_label("TIC", "red", 28), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), colored_label("TAC", "blue", 28), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), colored_label("TOE", "red", 28), FALSE, FALSE, 0);
    return box;
}

GtkWidget* login_page() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width(GTK_CONTAINER(box), 30);

    username_entry = gtk_entry_new();
    password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Username");
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);

    GtkWidget *login = gtk_button_new_with_label("Login");
    GtkWidget *reg = gtk_button_new_with_label("Register");
    status_label = gtk_label_new("");

    g_signal_connect(login, "clicked", G_CALLBACK(on_login_clicked), NULL);
    g_signal_connect(reg, "clicked", G_CALLBACK(on_register_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(box), build_title(), FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box), bordered(username_entry), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), bordered(password_entry), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), bordered(login), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), bordered(reg), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), status_label, FALSE, FALSE, 0);

    return box;
}

GtkWidget* game_page() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(box), 20);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            buttons[i][j] = gtk_button_new();
            GtkWidget *l = gtk_label_new(" ");
            gtk_container_add(GTK_CONTAINER(buttons[i][j]), l);
            gtk_widget_set_size_request(buttons[i][j], 80, 80);
            gtk_grid_attach(GTK_GRID(grid), buttons[i][j], j, i, 1, 1);
            g_signal_connect(buttons[i][j], "clicked",
                G_CALLBACK(on_cell_clicked), GINT_TO_POINTER(i * 3 + j));
        }

    gtk_box_pack_start(GTK_BOX(box), build_title(), FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), bordered(grid), TRUE, TRUE, 0);
    return box;
}

GtkWidget* score_page() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(box), 30);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

    const char *t[] = {"Player", "Games", "Wins", "Losses", "Draws"};
    for (int i = 0; i < 5; i++) {
        GtkWidget *k = gtk_label_new(t[i]);
        score_value_labels[i] = gtk_label_new("-");
        gtk_grid_attach(GTK_GRID(grid), k, 0, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), score_value_labels[i], 1, i, 1, 1);
    }

    GtkWidget *again = gtk_button_new_with_label("Play Again");
    g_signal_connect(again, "clicked", G_CALLBACK(on_play_again), NULL);

    gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), again, FALSE, FALSE, 10);
    return box;
}

/* MAIN */
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    ReadUserData();

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Tic Tac Toe");
    gtk_window_set_default_size(GTK_WINDOW(window), 350, 450);
    g_signal_connect(window, "destroy", G_CALLBACK(on_app_close), NULL);

    stack = gtk_stack_new();
    gtk_stack_add_named(GTK_STACK(stack), login_page(), "login");
    gtk_stack_add_named(GTK_STACK(stack), game_page(), "game");
    gtk_stack_add_named(GTK_STACK(stack), score_page(), "score");

    gtk_container_add(GTK_CONTAINER(window), stack);
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "login");

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
