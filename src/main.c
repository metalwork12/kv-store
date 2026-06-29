#include "hashtable.h"
#include <stdio.h>





int main(){
    HashTable* table = createHashTable();
    char* key = "key";
    char* value = "Value";

    set(table, key, value);
    char* got_value = get(table, "hi");
    printf("%s \n", got_value);


    return 0;
}