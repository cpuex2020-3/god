#include <stdio.h>
#include <stdlib.h>

signed char eqlstr(char *a, char *b){
  for (size_t i=0; ; i++){
    if(a[i]!=b[i]){
      return -1;
    }
    else if(a[i]=='\0'){
      break;
    }
  }
  return 0;
}
