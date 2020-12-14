/*
  標準入力から2進列を受け取って、4wordsごとに改行の入った16進列を標準出力に吐きます。
  入力文字列の途中に改行などの余計な文字を入れないでください。
  god_fibで生成した機械語に対し、cat binary | ./this > hex.txt とかどうでしょう。
*/

#include <stdio.h>
#include <stdlib.h>

void print_hex(int h){
  if(h<10) printf("%d", h);
  else printf("%c", (char)(h+55));
}

int main(){
  int input = 0;
  while(input>=0){
    for(size_t i=0; i<8; i++){
      int h = 0;
      for(size_t j=0; j<4; j++){
        input = getchar();
        if(input<0) return 0;
        if(input==49) h = h+(8>>j);
      }
      print_hex(h);
    }
    printf("\n");
  }
}
