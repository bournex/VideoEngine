#ifndef _BASE64_INCLUDE__H__
#define _BASE64_INCLUDE__H__


int Base64Encode(char * base64code, const char * src, int src_len = 0); 
int Base64Decode(char * buf, const char * base64code, int src_len = 0);

#endif