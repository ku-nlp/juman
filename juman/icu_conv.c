#include "icu_conv.h"

void init_ucnv() {
	UErrorCode cnv_status = U_ZERO_ERROR;

	_utf8Conv = ucnv_open("UTF8", &cnv_status);
	if(U_FAILURE(cnv_status)) {
		fprintf(stderr, "The UTF-8 character encoding converter failed to initialise. Aborting.\n");
		ucnv_close(_utf8Conv);
	}
	_currLocaleConv = ucnv_open(NULL, &cnv_status);
	if(U_FAILURE(cnv_status)) {
		fprintf(stderr, "The character encoding converter for the current system locale (%s) failed to initialise. Aborting.\n", ucnv_getDefaultName());
		ucnv_close(_currLocaleConv);
	}
}

void close_ucnv() {
	ucnv_close(_utf8Conv);
	ucnv_close(_currLocaleConv);
}

char* toUtf8Charenc(const UChar* ustr) {
	return toCharenc(ustr, _utf8Conv);
}

char* toCurrLocaleCharenc(const UChar* ustr) {
	return toCharenc(ustr, _currLocaleConv);
}

char* toCharenc(const UChar* ustr, UConverter* const conv) {
	UErrorCode conv_status = U_ZERO_ERROR;
	int cstr_len = UCNV_GET_MAX_BYTES_FOR_STRING(u_strlen(ustr), ucnv_getMaxCharSize(conv));
	char* cstr = malloc(cstr_len + sizeof(char));
	ucnv_fromUChars(conv, cstr, cstr_len, ustr, u_strlen(ustr), &conv_status);
	if (U_FAILURE(conv_status)) {
		fprintf(stderr, "toCharenc() fail\n");
	}
	return cstr;
}

UChar* fromUtf8Charenc(const char* cstr) {
	return fromCharenc(cstr, _utf8Conv);
}

UChar* fromCurrLocaleCharenc(const char* cstr) {
	return fromCharenc(cstr, _currLocaleConv);
}

UChar* fromCharenc(const char* cstr, UConverter* const conv) {
	UErrorCode conv_status = U_ZERO_ERROR;
	int32_t max_char_length = strlen(cstr);
	int32_t conv_len;

	UChar* new_ustr = malloc((max_char_length + 1) * sizeof(UChar));
	conv_len = ucnv_toUChars(conv, new_ustr, ((max_char_length + 1) * sizeof(UChar)), cstr, max_char_length, &conv_status);
	if (U_FAILURE(conv_status)) {
		fprintf(stderr, "fromCharenc() fail\n");
	}
	return new_ustr;
}
