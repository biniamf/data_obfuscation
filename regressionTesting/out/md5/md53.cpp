# 1 "md52.cpp"
# 1 "<command-line>"
# 1 "md52.cpp"
# 55 "md52.cpp"
# 1 "/usr/include/string.h" 1 3 4
# 27 "/usr/include/string.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 324 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/predefs.h" 1 3 4
# 325 "/usr/include/features.h" 2 3 4
# 357 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 1 3 4
# 378 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 379 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 358 "/usr/include/features.h" 2 3 4
# 389 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 1 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 5 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 2 3 4




# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs-64.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 2 3 4
# 390 "/usr/include/features.h" 2 3 4
# 28 "/usr/include/string.h" 2 3 4

extern "C" {




# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.7/include/stddef.h" 1 3 4
# 213 "/usr/lib/gcc/x86_64-linux-gnu/4.7/include/stddef.h" 3 4
typedef long unsigned int size_t;
# 35 "/usr/include/string.h" 2 3 4









extern void *memcpy (void *__restrict __dest,
       __const void *__restrict __src, size_t __n)
     throw () __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, __const void *__src, size_t __n)
     throw () __attribute__ ((__nonnull__ (1, 2)));






extern void *memccpy (void *__restrict __dest, __const void *__restrict __src,
        int __c, size_t __n)
     throw () __attribute__ ((__nonnull__ (1, 2)));





extern void *memset (void *__s, int __c, size_t __n) throw () __attribute__ ((__nonnull__ (1)));


extern int memcmp (__const void *__s1, __const void *__s2, size_t __n)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));



extern "C++"
{
extern void *memchr (void *__s, int __c, size_t __n)
      throw () __asm ("memchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern __const void *memchr (__const void *__s, int __c, size_t __n)
      throw () __asm ("memchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 93 "/usr/include/string.h" 3 4
}










extern "C++" void *rawmemchr (void *__s, int __c)
     throw () __asm ("rawmemchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern "C++" __const void *rawmemchr (__const void *__s, int __c)
     throw () __asm ("rawmemchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));







extern "C++" void *memrchr (void *__s, int __c, size_t __n)
      throw () __asm ("memrchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern "C++" __const void *memrchr (__const void *__s, int __c, size_t __n)
      throw () __asm ("memrchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));









extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
     throw () __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     throw () __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, __const char *__restrict __src)
     throw () __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, __const char *__restrict __src,
        size_t __n) throw () __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (__const char *__s1, __const char *__s2)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (__const char *__s1, __const char *__s2, size_t __n)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (__const char *__s1, __const char *__s2)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         __const char *__restrict __src, size_t __n)
     throw () __attribute__ ((__nonnull__ (2)));






# 1 "/usr/include/xlocale.h" 1 3 4
# 28 "/usr/include/xlocale.h" 3 4
typedef struct __locale_struct
{

  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
} *__locale_t;


typedef __locale_t locale_t;
# 163 "/usr/include/string.h" 2 3 4


extern int strcoll_l (__const char *__s1, __const char *__s2, __locale_t __l)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern size_t strxfrm_l (char *__dest, __const char *__src, size_t __n,
    __locale_t __l) throw () __attribute__ ((__nonnull__ (2, 4)));





extern char *strdup (__const char *__s)
     throw () __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (__const char *__string, size_t __n)
     throw () __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 210 "/usr/include/string.h" 3 4



extern "C++"
{
extern char *strchr (char *__s, int __c)
     throw () __asm ("strchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern __const char *strchr (__const char *__s, int __c)
     throw () __asm ("strchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 233 "/usr/include/string.h" 3 4
}






extern "C++"
{
extern char *strrchr (char *__s, int __c)
     throw () __asm ("strrchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern __const char *strrchr (__const char *__s, int __c)
     throw () __asm ("strrchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 260 "/usr/include/string.h" 3 4
}










extern "C++" char *strchrnul (char *__s, int __c)
     throw () __asm ("strchrnul") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern "C++" __const char *strchrnul (__const char *__s, int __c)
     throw () __asm ("strchrnul") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));









extern size_t strcspn (__const char *__s, __const char *__reject)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (__const char *__s, __const char *__accept)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern "C++"
{
extern char *strpbrk (char *__s, __const char *__accept)
     throw () __asm ("strpbrk") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
extern __const char *strpbrk (__const char *__s, __const char *__accept)
     throw () __asm ("strpbrk") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 312 "/usr/include/string.h" 3 4
}






extern "C++"
{
extern char *strstr (char *__haystack, __const char *__needle)
     throw () __asm ("strstr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
extern __const char *strstr (__const char *__haystack,
        __const char *__needle)
     throw () __asm ("strstr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 340 "/usr/include/string.h" 3 4
}







extern char *strtok (char *__restrict __s, __const char *__restrict __delim)
     throw () __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    __const char *__restrict __delim,
    char **__restrict __save_ptr)
     throw () __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, __const char *__restrict __delim,
         char **__restrict __save_ptr)
     throw () __attribute__ ((__nonnull__ (2, 3)));





extern "C++" char *strcasestr (char *__haystack, __const char *__needle)
     throw () __asm ("strcasestr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
extern "C++" __const char *strcasestr (__const char *__haystack,
           __const char *__needle)
     throw () __asm ("strcasestr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 382 "/usr/include/string.h" 3 4
extern void *memmem (__const void *__haystack, size_t __haystacklen,
       __const void *__needle, size_t __needlelen)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 3)));



extern void *__mempcpy (void *__restrict __dest,
   __const void *__restrict __src, size_t __n)
     throw () __attribute__ ((__nonnull__ (1, 2)));
extern void *mempcpy (void *__restrict __dest,
        __const void *__restrict __src, size_t __n)
     throw () __attribute__ ((__nonnull__ (1, 2)));





extern size_t strlen (__const char *__s)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern size_t strnlen (__const char *__string, size_t __maxlen)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern char *strerror (int __errnum) throw ();

# 438 "/usr/include/string.h" 3 4
extern char *strerror_r (int __errnum, char *__buf, size_t __buflen)
     throw () __attribute__ ((__nonnull__ (2)));





extern char *strerror_l (int __errnum, __locale_t __l) throw ();





extern void __bzero (void *__s, size_t __n) throw () __attribute__ ((__nonnull__ (1)));



extern void bcopy (__const void *__src, void *__dest, size_t __n)
     throw () __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) throw () __attribute__ ((__nonnull__ (1)));


extern int bcmp (__const void *__s1, __const void *__s2, size_t __n)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));



extern "C++"
{
extern char *index (char *__s, int __c)
     throw () __asm ("index") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern __const char *index (__const char *__s, int __c)
     throw () __asm ("index") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 487 "/usr/include/string.h" 3 4
}







extern "C++"
{
extern char *rindex (char *__s, int __c)
     throw () __asm ("rindex") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern __const char *rindex (__const char *__s, int __c)
     throw () __asm ("rindex") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 515 "/usr/include/string.h" 3 4
}







extern int ffs (int __i) throw () __attribute__ ((__const__));




extern int ffsl (long int __l) throw () __attribute__ ((__const__));

__extension__ extern int ffsll (long long int __ll)
     throw () __attribute__ ((__const__));




extern int strcasecmp (__const char *__s1, __const char *__s2)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));





extern int strcasecmp_l (__const char *__s1, __const char *__s2,
    __locale_t __loc)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern int strncasecmp_l (__const char *__s1, __const char *__s2,
     size_t __n, __locale_t __loc)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 4)));





extern char *strsep (char **__restrict __stringp,
       __const char *__restrict __delim)
     throw () __attribute__ ((__nonnull__ (1, 2)));




extern char *strsignal (int __sig) throw ();


extern char *__stpcpy (char *__restrict __dest, __const char *__restrict __src)
     throw () __attribute__ ((__nonnull__ (1, 2)));
extern char *stpcpy (char *__restrict __dest, __const char *__restrict __src)
     throw () __attribute__ ((__nonnull__ (1, 2)));



extern char *__stpncpy (char *__restrict __dest,
   __const char *__restrict __src, size_t __n)
     throw () __attribute__ ((__nonnull__ (1, 2)));
extern char *stpncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     throw () __attribute__ ((__nonnull__ (1, 2)));




extern int strverscmp (__const char *__s1, __const char *__s2)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strfry (char *__string) throw () __attribute__ ((__nonnull__ (1)));


extern void *memfrob (void *__s, size_t __n) throw () __attribute__ ((__nonnull__ (1)));







extern "C++" char *basename (char *__filename)
     throw () __asm ("basename") __attribute__ ((__nonnull__ (1)));
extern "C++" __const char *basename (__const char *__filename)
     throw () __asm ("basename") __attribute__ ((__nonnull__ (1)));
# 646 "/usr/include/string.h" 3 4
}
# 56 "md52.cpp" 2
# 1 "md5.h" 1
# 63 "md5.h"
typedef unsigned char md5_byte_t;
typedef unsigned int md5_word_t;


typedef struct md5_state_s {
    md5_word_t count[2];
    md5_word_t abcd[4];
    md5_byte_t buf[64];
} md5_state_t;


extern "C"
{



void md5_init(md5_state_t *pms);


int md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);


void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);


}
# 57 "md52.cpp" 2
# 132 "md52.cpp"
static void
md5_process(md5_state_t *pms, const md5_byte_t *data )
{
    md5_word_t
 a = pms->abcd[0], b = pms->abcd[1],
 c = pms->abcd[2], d = pms->abcd[3];
    md5_word_t t;





    md5_word_t xbuf[16];
    const md5_word_t *X;


    {






 static const int w = 1;

 if (*((const md5_byte_t *)&w))


 {




     if (!((data - (const md5_byte_t *)0) & 3)) {

  X = (const md5_word_t *)data;
     } else {

  memcpy(xbuf, data, 64);
  X = xbuf;
     }
 }


 else


 {




     const md5_byte_t *xp = data;
     int i;


     X = xbuf;



     for (i = 0; i < 16; ++i, xp += 4)
  xbuf[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
 }

    }
# 208 "md52.cpp"
    t = a + ((b & c) | (~b & d)) + X[0] + ((((md5_word_t)~0)) ^ 680876935); a = ((t << (7)) | (t >> (32 - (7)))) + b;
    t = d + ((a & b) | (~a & c)) + X[1] + ((((md5_word_t)~0)) ^ 389564585); d = ((t << (12)) | (t >> (32 - (12)))) + a;
    t = c + ((d & a) | (~d & b)) + X[2] + 606105819; c = ((t << (17)) | (t >> (32 - (17)))) + d;
    t = b + ((c & d) | (~c & a)) + X[3] + ((((md5_word_t)~0)) ^ 1044525329); b = ((t << (22)) | (t >> (32 - (22)))) + c;
    t = a + ((b & c) | (~b & d)) + X[4] + ((((md5_word_t)~0)) ^ 176418896); a = ((t << (7)) | (t >> (32 - (7)))) + b;
    t = d + ((a & b) | (~a & c)) + X[5] + 1200080426; d = ((t << (12)) | (t >> (32 - (12)))) + a;
    t = c + ((d & a) | (~d & b)) + X[6] + ((((md5_word_t)~0)) ^ 1473231340); c = ((t << (17)) | (t >> (32 - (17)))) + d;
    t = b + ((c & d) | (~c & a)) + X[7] + ((((md5_word_t)~0)) ^ 45705982); b = ((t << (22)) | (t >> (32 - (22)))) + c;
    t = a + ((b & c) | (~b & d)) + X[8] + 1770035416; a = ((t << (7)) | (t >> (32 - (7)))) + b;
    t = d + ((a & b) | (~a & c)) + X[9] + ((((md5_word_t)~0)) ^ 1958414416); d = ((t << (12)) | (t >> (32 - (12)))) + a;
    t = c + ((d & a) | (~d & b)) + X[10] + ((((md5_word_t)~0)) ^ 42062); c = ((t << (17)) | (t >> (32 - (17)))) + d;
    t = b + ((c & d) | (~c & a)) + X[11] + ((((md5_word_t)~0)) ^ 1990404161); b = ((t << (22)) | (t >> (32 - (22)))) + c;
    t = a + ((b & c) | (~b & d)) + X[12] + 1804603682; a = ((t << (7)) | (t >> (32 - (7)))) + b;
    t = d + ((a & b) | (~a & c)) + X[13] + ((((md5_word_t)~0)) ^ 40341100); d = ((t << (12)) | (t >> (32 - (12)))) + a;
    t = c + ((d & a) | (~d & b)) + X[14] + ((((md5_word_t)~0)) ^ 1502002289); c = ((t << (17)) | (t >> (32 - (17)))) + d;
    t = b + ((c & d) | (~c & a)) + X[15] + 1236535329; b = ((t << (22)) | (t >> (32 - (22)))) + c;
# 234 "md52.cpp"
    t = a + ((b & d) | (c & ~d)) + X[1] + ((((md5_word_t)~0)) ^ 165796509); a = ((t << (5)) | (t >> (32 - (5)))) + b;
    t = d + ((a & c) | (b & ~c)) + X[6] + ((((md5_word_t)~0)) ^ 1069501631); d = ((t << (9)) | (t >> (32 - (9)))) + a;
    t = c + ((d & b) | (a & ~b)) + X[11] + 643717713; c = ((t << (14)) | (t >> (32 - (14)))) + d;
    t = b + ((c & a) | (d & ~a)) + X[0] + ((((md5_word_t)~0)) ^ 373897301); b = ((t << (20)) | (t >> (32 - (20)))) + c;
    t = a + ((b & d) | (c & ~d)) + X[5] + ((((md5_word_t)~0)) ^ 701558690); a = ((t << (5)) | (t >> (32 - (5)))) + b;
    t = d + ((a & c) | (b & ~c)) + X[10] + 38016083; d = ((t << (9)) | (t >> (32 - (9)))) + a;
    t = c + ((d & b) | (a & ~b)) + X[15] + ((((md5_word_t)~0)) ^ 660478334); c = ((t << (14)) | (t >> (32 - (14)))) + d;
    t = b + ((c & a) | (d & ~a)) + X[4] + ((((md5_word_t)~0)) ^ 405537847); b = ((t << (20)) | (t >> (32 - (20)))) + c;
    t = a + ((b & d) | (c & ~d)) + X[9] + 568446438; a = ((t << (5)) | (t >> (32 - (5)))) + b;
    t = d + ((a & c) | (b & ~c)) + X[14] + ((((md5_word_t)~0)) ^ 1019803689); d = ((t << (9)) | (t >> (32 - (9)))) + a;
    t = c + ((d & b) | (a & ~b)) + X[3] + ((((md5_word_t)~0)) ^ 187363960); c = ((t << (14)) | (t >> (32 - (14)))) + d;
    t = b + ((c & a) | (d & ~a)) + X[8] + 1163531501; b = ((t << (20)) | (t >> (32 - (20)))) + c;
    t = a + ((b & d) | (c & ~d)) + X[13] + ((((md5_word_t)~0)) ^ 1444681466); a = ((t << (5)) | (t >> (32 - (5)))) + b;
    t = d + ((a & c) | (b & ~c)) + X[2] + ((((md5_word_t)~0)) ^ 51403783); d = ((t << (9)) | (t >> (32 - (9)))) + a;
    t = c + ((d & b) | (a & ~b)) + X[7] + 1735328473; c = ((t << (14)) | (t >> (32 - (14)))) + d;
    t = b + ((c & a) | (d & ~a)) + X[12] + ((((md5_word_t)~0)) ^ 1926607733); b = ((t << (20)) | (t >> (32 - (20)))) + c;
# 260 "md52.cpp"
    t = a + (b ^ c ^ d) + X[5] + ((((md5_word_t)~0)) ^ 378557); a = ((t << (4)) | (t >> (32 - (4)))) + b;
    t = d + (a ^ b ^ c) + X[8] + ((((md5_word_t)~0)) ^ 2022574462); d = ((t << (11)) | (t >> (32 - (11)))) + a;
    t = c + (d ^ a ^ b) + X[11] + 1839030562; c = ((t << (16)) | (t >> (32 - (16)))) + d;
    t = b + (c ^ d ^ a) + X[14] + ((((md5_word_t)~0)) ^ 35309555); b = ((t << (23)) | (t >> (32 - (23)))) + c;
    t = a + (b ^ c ^ d) + X[1] + ((((md5_word_t)~0)) ^ 1530992059); a = ((t << (4)) | (t >> (32 - (4)))) + b;
    t = d + (a ^ b ^ c) + X[4] + 1272893353; d = ((t << (11)) | (t >> (32 - (11)))) + a;
    t = c + (d ^ a ^ b) + X[7] + ((((md5_word_t)~0)) ^ 155497631); c = ((t << (16)) | (t >> (32 - (16)))) + d;
    t = b + (c ^ d ^ a) + X[10] + ((((md5_word_t)~0)) ^ 1094730639); b = ((t << (23)) | (t >> (32 - (23)))) + c;
    t = a + (b ^ c ^ d) + X[13] + 681279174; a = ((t << (4)) | (t >> (32 - (4)))) + b;
    t = d + (a ^ b ^ c) + X[0] + ((((md5_word_t)~0)) ^ 358537221); d = ((t << (11)) | (t >> (32 - (11)))) + a;
    t = c + (d ^ a ^ b) + X[3] + ((((md5_word_t)~0)) ^ 722521978); c = ((t << (16)) | (t >> (32 - (16)))) + d;
    t = b + (c ^ d ^ a) + X[6] + 76029189; b = ((t << (23)) | (t >> (32 - (23)))) + c;
    t = a + (b ^ c ^ d) + X[9] + ((((md5_word_t)~0)) ^ 640364486); a = ((t << (4)) | (t >> (32 - (4)))) + b;
    t = d + (a ^ b ^ c) + X[12] + ((((md5_word_t)~0)) ^ 421815834); d = ((t << (11)) | (t >> (32 - (11)))) + a;
    t = c + (d ^ a ^ b) + X[15] + 530742520; c = ((t << (16)) | (t >> (32 - (16)))) + d;
    t = b + (c ^ d ^ a) + X[2] + ((((md5_word_t)~0)) ^ 995338650); b = ((t << (23)) | (t >> (32 - (23)))) + c;
# 286 "md52.cpp"
    t = a + (c ^ (b | ~d)) + X[0] + ((((md5_word_t)~0)) ^ 198630843); a = ((t << (6)) | (t >> (32 - (6)))) + b;
    t = d + (b ^ (a | ~c)) + X[7] + 1126891415; d = ((t << (10)) | (t >> (32 - (10)))) + a;
    t = c + (a ^ (d | ~b)) + X[14] + ((((md5_word_t)~0)) ^ 1416354904); c = ((t << (15)) | (t >> (32 - (15)))) + d;
    t = b + (d ^ (c | ~a)) + X[5] + ((((md5_word_t)~0)) ^ 57434054); b = ((t << (21)) | (t >> (32 - (21)))) + c;
    t = a + (c ^ (b | ~d)) + X[12] + 1700485571; a = ((t << (6)) | (t >> (32 - (6)))) + b;
    t = d + (b ^ (a | ~c)) + X[3] + ((((md5_word_t)~0)) ^ 1894986605); d = ((t << (10)) | (t >> (32 - (10)))) + a;
    t = c + (a ^ (d | ~b)) + X[10] + ((((md5_word_t)~0)) ^ 1051522); c = ((t << (15)) | (t >> (32 - (15)))) + d;
    t = b + (d ^ (c | ~a)) + X[1] + ((((md5_word_t)~0)) ^ 2054922798); b = ((t << (21)) | (t >> (32 - (21)))) + c;
    t = a + (c ^ (b | ~d)) + X[8] + 1873313359; a = ((t << (6)) | (t >> (32 - (6)))) + b;
    t = d + (b ^ (a | ~c)) + X[15] + ((((md5_word_t)~0)) ^ 30611743); d = ((t << (10)) | (t >> (32 - (10)))) + a;
    t = c + (a ^ (d | ~b)) + X[6] + ((((md5_word_t)~0)) ^ 1560198379); c = ((t << (15)) | (t >> (32 - (15)))) + d;
    t = b + (d ^ (c | ~a)) + X[13] + 1309151649; b = ((t << (21)) | (t >> (32 - (21)))) + c;
    t = a + (c ^ (b | ~d)) + X[4] + ((((md5_word_t)~0)) ^ 145523069); a = ((t << (6)) | (t >> (32 - (6)))) + b;
    t = d + (b ^ (a | ~c)) + X[11] + ((((md5_word_t)~0)) ^ 1120210378); d = ((t << (10)) | (t >> (32 - (10)))) + a;
    t = c + (a ^ (d | ~b)) + X[2] + 718787259; c = ((t << (15)) | (t >> (32 - (15)))) + d;
    t = b + (d ^ (c | ~a)) + X[9] + ((((md5_word_t)~0)) ^ 343485550); b = ((t << (21)) | (t >> (32 - (21)))) + c;





    pms->abcd[0] += a;
    pms->abcd[1] += b;
    pms->abcd[2] += c;
    pms->abcd[3] += d;
}

void
md5_init(md5_state_t *pms)
{
    pms->count[0] = pms->count[1] = ;
    pms->abcd[0] = 1732584193;
    pms->abcd[1] = (((md5_word_t)~0)) ^ 271733878;
    pms->abcd[2] = (((md5_word_t)~0)) ^ 1732584193;
    pms->abcd[3] = 271733878;
}

int
md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
{
    const md5_byte_t *p = data;

    int left;
    left = nbytes;

    int offset;
    offset = (pms->count[0] >> 3) & 63;
    md5_word_t nbits = (md5_word_t)(nbytes << 3);

    if (nbytes <= 0)
 return 0;


    pms->count[1] += nbytes >> 29;
    pms->count[0] += nbits;
    if (pms->count[0] < nbits)
 pms->count[1]++;


    Encoded copy;
    if (offset) {

  	int var_8521;
  	if (offset + nbytes > 64) {
  		var_8521 = 64 - offset;
  	}
  	else {
  		var_8521 = nbytes;
  	}
  
  
  copy = Encoded(var_8521);

 memcpy(pms->buf + offset, p, (int)Encoded::DECODE(copy));
 if (offset + (int)Encoded::DECODE(copy) < 64)
     return 0;
 p += (int)Encoded::DECODE(copy);
 left -= (int)Encoded::DECODE(copy);
 md5_process(pms, pms->buf);
    }


    for (; left >= 64; p += 64, left -= 64)
 md5_process(pms, p);


    if (left)
 memcpy(pms->buf, p, left);

    return 1==1 ? (int)Encoded::DECODE(copy) : offset;
}

void
md5_finish(md5_state_t *pms, md5_byte_t digest[16])
{
    static const md5_byte_t pad[64] = {
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    md5_byte_t data[8];
    int i;


    for (i = 0; i < 8; ++i)
 data[i] = i >> 2 >> ((i & 3) << 3);

    md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);

    md5_append(pms, data, 8);
    for (i = 0; i < 16; ++i)
 digest[i] = i >> 2 >> ((i & 3) << 3);
}
