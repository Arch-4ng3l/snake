#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

typedef struct {
  int x;
  int y;
} Part;
typedef struct {
  Part part;
  void *next;
} Segment;

int kbhit(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

char getch() {
  char c;
  system("stty raw");
  c = getchar();
  system("stty sane");
  return (c);
}

void draw(Segment player, int map[20][20]) {
  system("clear");
  for (int row = 0; row < 20; row++) {
    if (row == 0 || row == 19) {
      printf("##########");
      printf("##########");
    } else {
      for (int col = 0; col < 20; col++) {
        if (col == 0 || col == 19) {
          printf("#");
        } else {
          if (col == player.part.x && row == player.part.y) {
            printf("O");
          } else if (map[row][col] == 2) {
            printf("X");

          } else if (map[row][col] == 1) {
            printf("+");
          } else {

            printf(" ");
          }
        }
      }
    }
    printf("\n");
  }
}

int logic(Segment *player, int tailLength, int map[20][20], int fruit[2]) {
  if (kbhit()) {
    char in = getch();
    int movX;
    int movY;
    if (in == 'w') {
      player->part.y--;
      movY = -1;
    }
    if (in == 's') {
      player->part.y++;
      movY = 1;
    }
    if (in == 'a') {
      player->part.x--;
      movX = -1;
    }
    if (in == 'd') {
      player->part.x++;
      movX = 1;
    }
    Segment *prev = player;
    Segment *next = prev->next;
    Part cache = prev->part;
    int count = 0;
    while (next != NULL) {
      if (count != 0) {
        cache = next->part;
        next->part = cache;
      } else {
        next->part = prev->part;
      }
      prev = prev->next;
      next = prev->next;
      count++;
    }

    if (player->part.y == fruit[0] && player->part.x == fruit[1]) {
      Segment *new = malloc(sizeof(Segment));
      printf("MALLOC\n");
      new->part = prev->part;
      new->next = NULL;
      fruit[0] = (rand() % 19) + 1;
      fruit[1] = (rand() % 19) + 1;
      if (movX != 0) {
        new->part.x -= movX;
      }
      if (movY != 0) {
        new->part.x -= movY;
      }
      prev->next = new;
      return 1;
    }
  }
  return 0;
}
int main(void) {
  int fruit[2];
  int map[20][20] = {0};
  Segment playerHead;
  playerHead.part.x = 10;
  playerHead.part.y = 10;
  playerHead.next = NULL;

  int gameOver = 0;
  int tailLength = 0;
  fruit[0] = (rand() % 19) + 1;
  fruit[1] = (rand() % 19) + 1;
  while (!gameOver) {

    memset(map, 0, sizeof(map));
    Segment *cur;
    cur = &playerHead;
    int count = 0;
    while (cur != NULL) {
      map[cur->part.y][cur->part.x] = 1;
      count++;
      cur = cur->next;
      printf("%d\n", count);
    }

    map[fruit[0]][fruit[1]] = 2;

    draw(playerHead, map);

    if (logic(&playerHead, tailLength, map, fruit)) {
      tailLength++;
    }
    printf("Score : %d\n", tailLength);
    printf("%d %d\n", playerHead.part.x, playerHead.part.y);
    printf("%d %d\n", fruit[1], fruit[0]);
    printf("%d\n", count);
  }
}
