#include <stdio.h>

#include "lib/trans.h"
#include "lib/common.h"
#include "lib/trans.h"

int main(){
 // tts("hi");
 char translation[256]; 
 trans(translation, "hi"); 
 printf("%s\n", translation);
}
