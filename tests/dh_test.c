/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 */
#include <tomcrypt_test.h>

#ifdef LTC_MDH

#ifdef DH4096
#define KEYSIZE 4096
#else
#define KEYSIZE 2048
#endif

static int _prime_test(void)
{
   void *p, *g, *tmp;
   int x, err, primality;

   if ((err = mp_init_multi(&p, &g, &tmp, NULL)) != CRYPT_OK)               { goto error; }

   for (x = 0; ltc_dh_sets[x].size != 0; x++) {
      if ((err = mp_read_radix(g, ltc_dh_sets[x].base, 16)) != CRYPT_OK)    { goto error; }
      if ((err = mp_read_radix(p, ltc_dh_sets[x].prime, 16)) != CRYPT_OK)   { goto error; }

      /* ensure p is prime */
      if ((err = mp_prime_is_prime(p, 8, &primality)) != CRYPT_OK)          { goto done; }
      if (primality != LTC_MP_YES ) {
         err = CRYPT_FAIL_TESTVECTOR;
         goto done;
      }

      if ((err = mp_sub_d(p, 1, tmp)) != CRYPT_OK)                          { goto error; }
      if ((err = mp_div_2(tmp, tmp)) != CRYPT_OK)                           { goto error; }

      /* ensure (p-1)/2 is prime */
      if ((err = mp_prime_is_prime(tmp, 8, &primality)) != CRYPT_OK)        { goto done; }
      if (primality == 0) {
         err = CRYPT_FAIL_TESTVECTOR;
         goto done;
      }

      /* now see if g^((p-1)/2) mod p is in fact 1 */
      if ((err = mp_exptmod(g, tmp, p, tmp)) != CRYPT_OK)                   { goto error; }
      if (mp_cmp_d(tmp, 1)) {
         err = CRYPT_FAIL_TESTVECTOR;
         goto done;
      }
   }
   err = CRYPT_OK;
error:
done:
   mp_clear_multi(tmp, g, p, NULL);
   return err;
}

static int _dhparam_test(void)
{
   dh_key k;
   unsigned char buf[1024];
   /* generated by: openssl dhparam -outform der -out dhparam.der 2048 */
   unsigned char dhparam_der[] = {
      0x30, 0x82, 0x01, 0x08, 0x02, 0x82, 0x01, 0x01, 0x00, 0xae, 0xfe, 0x78, 0xce, 0x80, 0xd5, 0xd7,
      0x8e, 0xcc, 0x4f, 0x0c, 0x1b, 0xb0, 0x95, 0x10, 0xe1, 0x41, 0x15, 0x53, 0x4d, 0x0e, 0x68, 0xb0,
      0xf8, 0x5a, 0x41, 0x0e, 0x65, 0x2f, 0x9f, 0xac, 0x9c, 0x30, 0xb0, 0x76, 0xec, 0x02, 0xe9, 0x43,
      0x55, 0x08, 0xb4, 0x20, 0x60, 0xd9, 0x52, 0xda, 0x2d, 0xab, 0x9a, 0xba, 0xe6, 0xcf, 0x11, 0xa7,
      0x00, 0x44, 0xc2, 0x5e, 0xd1, 0xba, 0x9b, 0xaa, 0xfe, 0x03, 0xdd, 0xdc, 0xef, 0x41, 0x89, 0x9c,
      0xac, 0x64, 0x13, 0xd9, 0x6a, 0x8a, 0x55, 0xa0, 0x5b, 0xff, 0x12, 0x92, 0x37, 0x52, 0x6a, 0x91,
      0xa4, 0x6e, 0x9e, 0x61, 0xb7, 0xfe, 0xb0, 0x17, 0x8e, 0x67, 0x0f, 0x88, 0x46, 0xa7, 0x9e, 0xb1,
      0xdb, 0x68, 0x77, 0x70, 0xb5, 0x77, 0xf2, 0x7e, 0x33, 0xb1, 0x3e, 0x10, 0xc4, 0x63, 0x36, 0xd0,
      0x13, 0x27, 0xd3, 0x29, 0xc3, 0xb6, 0x5d, 0xf6, 0x5d, 0xa7, 0xd8, 0x25, 0x5c, 0x0b, 0x65, 0x99,
      0xfa, 0xf9, 0x5f, 0x1d, 0xee, 0xd1, 0x86, 0x64, 0x7c, 0x44, 0xcb, 0xa0, 0x12, 0x52, 0x4c, 0xd4,
      0x46, 0x81, 0xae, 0x07, 0xdb, 0xc7, 0x13, 0x29, 0xce, 0x9b, 0xcf, 0x1c, 0x06, 0xd2, 0x0f, 0x2d,
      0xbb, 0x12, 0x33, 0xb9, 0xb1, 0x0f, 0x67, 0x5d, 0x3f, 0x0c, 0xe4, 0xfa, 0x67, 0x26, 0xe2, 0x89,
      0xa2, 0xd5, 0x66, 0x29, 0x1c, 0xe2, 0x8e, 0xbb, 0x7b, 0xcb, 0xcc, 0x70, 0x7e, 0x4f, 0x0e, 0xd3,
      0x5d, 0x64, 0x64, 0x1b, 0x27, 0xbb, 0xda, 0xa9, 0x08, 0x2b, 0x62, 0xd4, 0xca, 0xc3, 0x3a, 0x23,
      0x39, 0x58, 0x57, 0xaf, 0x7b, 0x8b, 0x0c, 0x5b, 0x2e, 0xfc, 0x42, 0x57, 0x59, 0x39, 0x2e, 0x6d,
      0x39, 0x97, 0xdb, 0x5b, 0x5c, 0xb9, 0x59, 0x71, 0x42, 0xf3, 0xcd, 0xea, 0xda, 0x86, 0x54, 0x86,
      0x61, 0x8d, 0x93, 0x66, 0xc7, 0x65, 0xd1, 0x98, 0xcb, 0x02, 0x01, 0x02
   };
   /* text dump: openssl dh -inform DER -in dhparam.der -text
      DH Parameters: (2048 bit)
          prime:
              00:ae:fe:78:ce:80:d5:d7:8e:cc:4f:0c:1b:b0:95:
              10:e1:41:15:53:4d:0e:68:b0:f8:5a:41:0e:65:2f:
              9f:ac:9c:30:b0:76:ec:02:e9:43:55:08:b4:20:60:
              d9:52:da:2d:ab:9a:ba:e6:cf:11:a7:00:44:c2:5e:
              d1:ba:9b:aa:fe:03:dd:dc:ef:41:89:9c:ac:64:13:
              d9:6a:8a:55:a0:5b:ff:12:92:37:52:6a:91:a4:6e:
              9e:61:b7:fe:b0:17:8e:67:0f:88:46:a7:9e:b1:db:
              68:77:70:b5:77:f2:7e:33:b1:3e:10:c4:63:36:d0:
              13:27:d3:29:c3:b6:5d:f6:5d:a7:d8:25:5c:0b:65:
              99:fa:f9:5f:1d:ee:d1:86:64:7c:44:cb:a0:12:52:
              4c:d4:46:81:ae:07:db:c7:13:29:ce:9b:cf:1c:06:
              d2:0f:2d:bb:12:33:b9:b1:0f:67:5d:3f:0c:e4:fa:
              67:26:e2:89:a2:d5:66:29:1c:e2:8e:bb:7b:cb:cc:
              70:7e:4f:0e:d3:5d:64:64:1b:27:bb:da:a9:08:2b:
              62:d4:ca:c3:3a:23:39:58:57:af:7b:8b:0c:5b:2e:
              fc:42:57:59:39:2e:6d:39:97:db:5b:5c:b9:59:71:
              42:f3:cd:ea:da:86:54:86:61:8d:93:66:c7:65:d1:
              98:cb
          generator: 2 (0x2)
   */
   unsigned char prime[] = {
            0xae, 0xfe, 0x78, 0xce, 0x80, 0xd5, 0xd7, 0x8e, 0xcc, 0x4f, 0x0c, 0x1b, 0xb0, 0x95,
      0x10, 0xe1, 0x41, 0x15, 0x53, 0x4d, 0x0e, 0x68, 0xb0, 0xf8, 0x5a, 0x41, 0x0e, 0x65, 0x2f,
      0x9f, 0xac, 0x9c, 0x30, 0xb0, 0x76, 0xec, 0x02, 0xe9, 0x43, 0x55, 0x08, 0xb4, 0x20, 0x60,
      0xd9, 0x52, 0xda, 0x2d, 0xab, 0x9a, 0xba, 0xe6, 0xcf, 0x11, 0xa7, 0x00, 0x44, 0xc2, 0x5e,
      0xd1, 0xba, 0x9b, 0xaa, 0xfe, 0x03, 0xdd, 0xdc, 0xef, 0x41, 0x89, 0x9c, 0xac, 0x64, 0x13,
      0xd9, 0x6a, 0x8a, 0x55, 0xa0, 0x5b, 0xff, 0x12, 0x92, 0x37, 0x52, 0x6a, 0x91, 0xa4, 0x6e,
      0x9e, 0x61, 0xb7, 0xfe, 0xb0, 0x17, 0x8e, 0x67, 0x0f, 0x88, 0x46, 0xa7, 0x9e, 0xb1, 0xdb,
      0x68, 0x77, 0x70, 0xb5, 0x77, 0xf2, 0x7e, 0x33, 0xb1, 0x3e, 0x10, 0xc4, 0x63, 0x36, 0xd0,
      0x13, 0x27, 0xd3, 0x29, 0xc3, 0xb6, 0x5d, 0xf6, 0x5d, 0xa7, 0xd8, 0x25, 0x5c, 0x0b, 0x65,
      0x99, 0xfa, 0xf9, 0x5f, 0x1d, 0xee, 0xd1, 0x86, 0x64, 0x7c, 0x44, 0xcb, 0xa0, 0x12, 0x52,
      0x4c, 0xd4, 0x46, 0x81, 0xae, 0x07, 0xdb, 0xc7, 0x13, 0x29, 0xce, 0x9b, 0xcf, 0x1c, 0x06,
      0xd2, 0x0f, 0x2d, 0xbb, 0x12, 0x33, 0xb9, 0xb1, 0x0f, 0x67, 0x5d, 0x3f, 0x0c, 0xe4, 0xfa,
      0x67, 0x26, 0xe2, 0x89, 0xa2, 0xd5, 0x66, 0x29, 0x1c, 0xe2, 0x8e, 0xbb, 0x7b, 0xcb, 0xcc,
      0x70, 0x7e, 0x4f, 0x0e, 0xd3, 0x5d, 0x64, 0x64, 0x1b, 0x27, 0xbb, 0xda, 0xa9, 0x08, 0x2b,
      0x62, 0xd4, 0xca, 0xc3, 0x3a, 0x23, 0x39, 0x58, 0x57, 0xaf, 0x7b, 0x8b, 0x0c, 0x5b, 0x2e,
      0xfc, 0x42, 0x57, 0x59, 0x39, 0x2e, 0x6d, 0x39, 0x97, 0xdb, 0x5b, 0x5c, 0xb9, 0x59, 0x71,
      0x42, 0xf3, 0xcd, 0xea, 0xda, 0x86, 0x54, 0x86, 0x61, 0x8d, 0x93, 0x66, 0xc7, 0x65, 0xd1,
      0x98, 0xcb
   };

   DO(dh_make_key_dhparam(&yarrow_prng, find_prng ("yarrow"), dhparam_der, sizeof(dhparam_der), &k));
   if (mp_unsigned_bin_size(k.prime) > sizeof(buf)) {
      printf("dhparam_test: short buf\n");
      dh_free(&k);
      return CRYPT_ERROR;
   }
   DO(mp_to_unsigned_bin(k.prime, buf));
   if (compare_testvector(buf, sizeof(prime), prime, sizeof(prime), "dhparam_test", 1)) {
      printf("dhparam_test: prime mismatch\n");
      dh_free(&k);
      return CRYPT_ERROR;
   }
   if (mp_cmp_d(k.base, 2) != LTC_MP_EQ) {
      printf("dhparam_test: base mismatch\n");
      dh_free(&k);
      return CRYPT_ERROR;
   }
   dh_free(&k);
   return CRYPT_OK;
}

static int _basic_test(void)
{
   unsigned char buf[3][4096];
   unsigned long x, y, z;
   int           size;
   dh_key        usera, userb;

   if (register_prng(&yarrow_desc) == -1) {
      printf("Error registering yarrow PRNG\n");
      return CRYPT_ERROR;
   }
   if (register_hash(&md5_desc) == -1) {
      printf("Error registering md5 hash\n");
      return CRYPT_ERROR;
   }

   /* make up two keys */
   DO(dh_make_key (&yarrow_prng, find_prng ("yarrow"), KEYSIZE/8, &usera));
   DO(dh_make_key (&yarrow_prng, find_prng ("yarrow"), KEYSIZE/8, &userb));

   /* make the shared secret */
   x = KEYSIZE;
   DO(dh_shared_secret (&usera, &userb, buf[0], &x));

   y = KEYSIZE;
   DO(dh_shared_secret (&userb, &usera, buf[1], &y));
   if (y != x) {
      fprintf(stderr, "DH Shared keys are not same size.\n");
      dh_free (&usera);
      dh_free (&userb);
      return CRYPT_ERROR;
   }
   if (memcmp (buf[0], buf[1], x)) {
      fprintf(stderr, "DH Shared keys not same contents.\n");
      dh_free (&usera);
      dh_free (&userb);
      return CRYPT_ERROR;
   }

   /* now export userb */
   y = KEYSIZE;
   DO(dh_export (buf[1], &y, PK_PUBLIC, &userb));
   dh_free (&userb);

   /* import and make the shared secret again */
   DO(dh_import (buf[1], y, &userb));
   z = KEYSIZE;
   DO(dh_shared_secret (&usera, &userb, buf[2], &z));

   dh_free (&usera);
   dh_free (&userb);

   if (z != x) {
      fprintf(stderr, "failed.  Size don't match?\n");
      return CRYPT_ERROR;
   }
   if (memcmp (buf[0], buf[2], x)) {
      fprintf(stderr, "Failed.  Content didn't match.\n");
      return CRYPT_ERROR;
   }

   for (x = 0; ltc_dh_sets[x].size != 0; x++) {
      DO(dh_make_key(&yarrow_prng, find_prng ("yarrow"), ltc_dh_sets[x].size, &usera));
      size = dh_get_groupsize(&usera);
      dh_free(&usera);
      if (size != ltc_dh_sets[x].size) {
         fprintf(stderr, "dh_groupsize mismatch %d %d\n", size, ltc_dh_sets[x].size);
         return CRYPT_ERROR;
      }
   }

   return CRYPT_OK;
}

int dh_test(void)
{
   int fails = 0;
   if (_prime_test() != CRYPT_OK) fails++;
   if (_basic_test() != CRYPT_OK) fails++;
   if (_dhparam_test() != CRYPT_OK) fails++;
   return fails > 0 ? CRYPT_FAIL_TESTVECTOR : CRYPT_OK;
}

#else

int dh_test(void)
{
   return CRYPT_NOP;
}

#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */