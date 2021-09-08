#ifndef WEB_H
#define WEB_H

#include "stdio.h"
#include "stdlib.h"

#define WEB_MAX_QUERY_STRING 1024
//maximum permissible size of post data is 10 MB;
//TODO(GI): What is the right maximum size to use?
#define WEB_MAX_POST_DATA 10485760

#define TEC_MOST_SIG_BIT 128
#define TEC_2ND_MOST_SIG_BIT 64

#define web_print_header() printf("Content-Type: text/html; charset=utf-8\r\n\r\n")

#define web_print_relocate_header(X) printf("Status:308 Permanent Redirect\nLocation: %s\r\n\r\n", X)

/*web_get_query_string:
	a query string typically looks something like this:
	var1=abc&var2=def&var3=1%2B1
	this function will retrieve the query string
	it will split the query string where there is a '&'
	it will convert the escaped symbols e.g. %2B -> '+'
	this does modify the original string, make a copy if needed
*/
char** web_get_query_string();

/*web_get_post_string:
	this does the same thing as web_get_query_string
	but gets data sent with POST method
*/
char** web_get_post_string();

/*web_get_from_query_string:
	returns a string which contains the value of variable var_name
	this requires you first used web_get_query_string()
	returns NULL if var_name was not found
	char *qss: this is the output of web_get_query_string
	char *var_name: name of variable you are seeking the value of
*/
char* web_get_from_query_string(char **qss, char *var_name);

/*tec_buf_begins:
	returns 1 if the buffer begins with string str
	returns 0 in all other cases, including errors and str being longer than buffer
*/
int tec_buf_begins(char *buffer, char *str);

/*tec_string_length:
	returns the length of a string in bytes
	check tec_string_utf8_length to know the number of codepoints
	unlike strlen, this function does not segfault when you give it a NULL pointer
	instead it returns zero because, well, you gave it an empty string ;-)
*/
int tec_string_length(char *str);

/*tec_string_find_char:
	returns index of first instance of a character in a string
	returns -1 if not found
*/
int tec_string_find_char(char *s, char to_find);

/*tec_string_shift:
	removes an ascii char or unicode codepoint at front of string
	assumes a valid utf8 string
*/
void tec_string_shift(char *str);

/*tec_string_split:

	splits a string where char c does occur
	num will give you the number of times char c did occur in the string
	num is also index of last string in returned char**
	CAUTION: there is one more string than occurences of char c, this might include empty strings

	the string str will be modified so make sure to make a copy if needed

	note that some character pointers may be NULL pointers
	if 2 or more characters c are right next to each other

	free the returned char ** when done

	returns NULL in case of error
*/
char** tec_string_split(char *str, char c, int *num);

/*tec_string_to_int:
	converts a string to an integer
	string may not contain anything in front of number except '-' or '+'
	does not safeguard against overflow
*/
int tec_string_to_int(char *s);

/*tec_char_is_white_space:
	returns 1 if c is a white space character (e.g. space)
	returns 0 otherwise
	assumes 7 bit ascii character
	there are more white space characters within unicode
	they are not so commonly used and could not all be considered in just an 8 bit character
*/
int			tec_char_is_white_space(char c);

void w_process_query_string(char *qs);


/////////////////////////////////////////////////////////////////////////////////////
//Implementation below
/////////////////////////////////////////////////////////////////////////////////////

char** web_get_query_string(){

	char *qs = getenv("QUERY_STRING");
	if(!qs)
		return NULL;
	if(tec_string_length(qs) > WEB_MAX_QUERY_STRING){
		return NULL;
	}

	int i = 0;
	int num = 0;
	char **qss = tec_string_split(qs, '&', &num);

	while(i <= num){
		w_process_query_string(qss[i]);
		i += 1;
	}

	return qss;

}//web_get_query_string*/

char** web_get_post_string(){

	char *cont_len = getenv("CONTENT_LENGTH");
	if(!cont_len){
		return NULL;
	}
	int len = tec_string_to_int(cont_len);
	if(len > WEB_MAX_POST_DATA){
		return NULL;
	}
	len += 1;

	char *buffer = (char *) malloc(sizeof(char) * len );
	fread(buffer, sizeof(char), len, stdin);

	int i = 0;
	int num = 0;
	char **qss = tec_string_split(buffer, '&', &num);
	while(i <= num){
		w_process_query_string(qss[i]);
		i += 1;
	}

	return qss;

}//web_get_post_string*/

char* web_get_from_query_string(char **qss, char *var_name){

	if(!qss)
		return NULL;

	int i = 0;
	while(qss[i]){
		if(tec_buf_begins(qss[i], var_name)){
			char *str = qss[i];
			str += tec_string_length(var_name);
			str += 1;       //skip over '='
			return str;
		}
		i += 1;
	}

	return NULL;

}//web_get_from_query_string*/

int tec_buf_begins(char *buffer, char *str){

	if(!buffer)
		return 0;
	if(!str)
		return 0;

	while(*str && *buffer == *str){
		str += 1;
		buffer += 1;
	}

	if(*str){
		return 0;
	}

	return 1;

}//tec_buf_begins*/

int tec_string_length(char *str){

	//TODO(GI): go through string one WORD at a time and check for empty characters in the word

	if(!str)
		return 0;
	if(!*str)
		return 0;

	int len = 0;
	while(*str){
		len += 1;
		str += 1;
	}

	return len;

}//tec_string_length*/

int tec_string_find_char(char *s, char to_find){

	if(!s)
		return -1;

	int i = 0;
	while(s[i] && to_find != s[i]){
		i += 1;
	}

	if(s[i]){
		return i;
	}else{
		return -1;
	}

}//tec_string_find_char*/

void tec_string_shift(char *str){

	if(!str)
		return;

	int len = tec_string_length(str);
	int i = 1;
	int j = 1;

	if( (str[i-j] & TEC_MOST_SIG_BIT) && (str[i-j] & TEC_2ND_MOST_SIG_BIT) ){
		j += 1;
		i += 1;
		while( (str[i] & TEC_MOST_SIG_BIT) && !(str[i] & TEC_2ND_MOST_SIG_BIT) ){
			j += 1;
			i += 1;
		}
	}

	while(i < len){
		str[ i - j ] = str[i];
		i += 1;
	}
	str[i - j] = '\0';

}//tec_string_shift*/

char** tec_string_split(char *str, char c, int *num){

	if(!str)
		return NULL;
	if(!c)
		return NULL;

	// we start by assuming that there will be no more than 32 instances of c
	int original_limit = 32;
	int limit = original_limit;
	char **table[26];
	int table_index = 0;
	int tmp_num = 0;
	*num = 0;

	char **res = (char **) malloc(sizeof(char *) * limit);
	if(!res){
		return NULL;
	}
	table[table_index] = res;

	res[0] = str;
	while(*str){
		if(*str == c){
			tmp_num += 1;
			*num += 1;
			if(tmp_num == limit){
				limit *= 2;
				table_index += 1;
				res = (char **) malloc(sizeof(char *) * limit);
				if(!res){
					return NULL;
				}
				table[table_index] = res;
				tmp_num = 0;
			}
			*str = '\0';
			str += 1;
			if(*str){
				res[tmp_num] = str;
			}else{
				//Note(GI) this deals with case where char c is last character in the string
				res[tmp_num] = NULL;
			}
		}else{
			str += 1;
		}
	}

	if(*num < original_limit){
		res[(*num)+1] = NULL;
		return res;
	}

	char **real_res = (char **) malloc(sizeof(char *) * (*num + 2));
	int ti = 0;
	int n = 0;
	int n2 = 0;
	limit = original_limit;
	while(ti <= table_index){

		res = table[ti];
		n2 = 0;
		while(n2 < limit && n <= *num){
			real_res[n] = res[n2];
			n2 += 1;
			n += 1;
		}

		free(res);
		limit *= 2;
		ti += 1;

	}
	real_res[(*num) + 1] = NULL;

	return real_res;

}//tec_string_split*/

int tec_string_to_int(char *s){

	if(!s)
		return 0;

	int sign = 1;
	int result = 0;

	while(tec_char_is_white_space(*s)){
		s += 1;
	}
	if(*s == '-'){
		sign = -1;
		s += 1;
	}else{
		if(*s == '+'){
			s += 1;
		}
	}

	while(*s){
		if(*s > '9' || *s < '0'){
			return result * sign;
		}
		result *= 10;
		result += *s - '0';
		s += 1;
	}

	return result * sign;

}//tec_string_to_int*/

int tec_char_is_white_space(char c){

	if(c == 32 || c == 9 || c == 10 || c == 11 || c == 12 || c == 13)
		return 1;
	return 0;

}//tec_char_is_white_space*/

void w_process_query_string(char *qs){

	//first replacing a '+' with a space
	char *tmp = qs;
	int n = tec_string_find_char(tmp, '+');
	while( n != -1 ){
		tmp += n;
		*tmp = ' ';
		n = tec_string_find_char(tmp, '+');
	}

	int i = 0;
	tmp = qs;
	while(qs[i]){
		while(qs[i] && qs[i] != '%'){
			i += 1;
		}
		if(qs[i] == '%'){
			tmp = &(qs[i]);
			int hex = 0;
			i += 1;
			int j = 1;
			while(j >= 0){
				if(qs[i] >= 'A' && qs[i] <= 'F'){
					hex += (qs[i] - 'A') + 10;
				}
				if(qs[i] >= '0' && qs[i] <= '9'){
					hex += (qs[i] - '0');
				}
				if(j){
					hex *= 16;
				}
				j -= 1;
				i += 1;
			}
			tmp[0] = hex;
			tmp += 1;
			tec_string_shift(tmp);
			tec_string_shift(tmp);
			i -= 2;
		}
	}

}//w_process_query_string*/

#endif

