
#ifndef JUMAN_ICU_CONV_H
#define JUMAN_ICU_CONV_H

#include	<stdio.h>

#ifndef UCNV_H
#include "unicode/ucnv.h"
#endif

#ifndef USTRING_H
#include "unicode/ustring.h"
#endif

UConverter* _utf8Conv;
UConverter* _currLocaleConv;

void init_ucnv(void);
void close_ucnv(void);
char* toUtf8Charenc(const UChar* ustr);
char* toCurrLocaleCharenc(const UChar* ustr);
char* toCharenc(const UChar* ustr, UConverter* const conv);
UChar* fromUtf8Charenc(const char* cstr);
UChar* fromCurrLocaleCharenc(const char* cstr);
UChar* fromCharenc(const char* cstr, UConverter* const conv);

#endif //JUMAN_ICU_CONV_H
