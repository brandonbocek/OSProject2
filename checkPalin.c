#include <stdio.h>
#include <string.h>

int isPalindrome(char s[]){
	int l, i;
	l = strlen(s);
	for(i=0; i<l/2;i++){
		if(s[i]!=s[l-1-i])
			return 0;
	}
	return 1;
}

int main(int argc, char *argv[]){

	char str[20];
	printf("Enter a string: ");
	scanf("%s", &str);
	if(isPalindrome(str))
		printf("Palindrome\n");
	else
		printf("Not a plaindrome\n");
	return 0;
}
