/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StringCodecs/SHA256.h>

#include <cassert>
#include <algorithm>
#include <string.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

using namespace Swift;

// Copied & adapted from LibTomCrypt, by Tom St Denis, tomstdenis@gmail.com, http://libtom.org
// Substituted some macros by the platform-independent (slower) variants

#include <stdlib.h>

#define CRYPT_OK 0
#define CRYPT_INVALID_ARG -1
#define LTC_ARGCHK assert

#define ROR(x, y) ( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | ((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)
#define RORc(x, y) ( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | ((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)

#define LOAD32H(x, y)														 \
		 { x = ((unsigned long)((y)[0] & 255)<<24) | \
					 ((unsigned long)((y)[1] & 255)<<16) | \
					 ((unsigned long)((y)[2] & 255)<<8)  | \
					 ((unsigned long)((y)[3] & 255)); }

#define STORE32H(x, y)																																		 \
		 { (y)[0] = (unsigned char)(((x)>>24)&255); (y)[1] = (unsigned char)(((x)>>16)&255);	 \
			 (y)[2] = (unsigned char)(((x)>>8)&255); (y)[3] = (unsigned char)((x)&255); }

#define STORE64H(x, y)																																		 \
	 { (y)[0] = (unsigned char)(((x)>>56)&255); (y)[1] = (unsigned char)(((x)>>48)&255);		 \
		 (y)[2] = (unsigned char)(((x)>>40)&255); (y)[3] = (unsigned char)(((x)>>32)&255);		 \
		 (y)[4] = (unsigned char)(((x)>>24)&255); (y)[5] = (unsigned char)(((x)>>16)&255);		 \
		 (y)[6] = (unsigned char)(((x)>>8)&255); (y)[7] = (unsigned char)((x)&255); }

/* a simple macro for making hash "process" functions */
#define HASH_PROCESS(func_name, compress_name, state_var, block_size)												\
int func_name (State * md, const unsigned char *in, unsigned int inlen)								\
{																																														\
		unsigned long n;																																				\
		int						err;																																			\
		LTC_ARGCHK(md != NULL);																																	\
		LTC_ARGCHK(in != NULL || inlen == 0);																																	\
		if (md-> curlen > sizeof(md->buf)) {														 \
			 return CRYPT_INVALID_ARG;																														\
		}																																												\
		while (inlen > 0) {																																			\
				if (md->curlen == 0 && inlen >= block_size) {														\
					 if ((err = compress_name (md, (unsigned char *)in)) != CRYPT_OK) {								\
							return err;																																		\
					 }																																								\
					 md-> length += block_size * 8;																				 \
					 in							+= block_size;																										\
					 inlen					-= block_size;																										\
				} else {																																						\
					 n = std::min(inlen, (block_size - md-> curlen));														\
					 memcpy(md-> buf + md-> curlen, in, (size_t)n);							 \
					 md-> curlen += n;																										 \
					 in							+= n;																															\
					 inlen					-= n;																															\
					 if (md-> curlen == block_size) {																			 \
							if ((err = compress_name (md, md-> buf)) != CRYPT_OK) {						 \
								 return err;																																\
							}																																							\
							md-> length += 8*block_size;																			 \
							md-> curlen = 0;																									 \
					 }																																								\
			 }																																										\
		}																																												\
		return CRYPT_OK;																																				\
}

#ifdef LTC_SMALL_CODE
/* the K array */
static const boost::uint32_t K[64] = {
		0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
		0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
		0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
		0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
		0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
		0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
		0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
		0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
		0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
		0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
		0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
		0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
		0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};
#endif

/* Various logical functions */
#define Ch(x,y,z)				(z ^ (x & (y ^ z)))
#define Maj(x,y,z)			(((x | y) & z) | (x & y)) 
#define S(x, n)					RORc((x),(n))
#define R(x, n)					(((x)&0xFFFFFFFFUL)>>(n))
#define Sigma0(x)				(S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)				(S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)				(S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)				(S(x, 17) ^ S(x, 19) ^ R(x, 10))

#ifdef LTC_CLEAN_STACK
int SHA256::_compress(State * md, unsigned char *buf)
#else
int  SHA256::compress(State * md, unsigned char *buf)
#endif
{
		boost::uint32_t S[8], W[64], t0, t1;
#ifdef LTC_SMALL_CODE
		boost::uint32_t t;
#endif
		int i;

		/* copy state into S */
		for (i = 0; i < 8; i++) {
				S[i] = md->state[i];
		}

		/* copy the state into 512-bits into W[0..15] */
		for (i = 0; i < 16; i++) {
				LOAD32H(W[i], buf + (4*i));
		}

		/* fill W[16..63] */
		for (i = 16; i < 64; i++) {
				W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
		}				 

		/* Compress */
#ifdef LTC_SMALL_CODE		
#define RND(a,b,c,d,e,f,g,h,i)												 \
		 t0 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i];	 \
		 t1 = Sigma0(a) + Maj(a, b, c);										 \
		 d += t0;																					 \
		 h	= t0 + t1;

		 for (i = 0; i < 64; ++i) {
				 RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i);
				 t = S[7]; S[7] = S[6]; S[6] = S[5]; S[5] = S[4]; 
				 S[4] = S[3]; S[3] = S[2]; S[2] = S[1]; S[1] = S[0]; S[0] = t;
		 }	
#else 
#define RND(a,b,c,d,e,f,g,h,i,ki)										 \
		 t0 = h + Sigma1(e) + Ch(e, f, g) + ki + W[i];	 \
		 t1 = Sigma0(a) + Maj(a, b, c);									 \
		 d += t0;																				 \
		 h	= t0 + t1;

		RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],0,0x428a2f98);
		RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],1,0x71374491);
		RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],2,0xb5c0fbcf);
		RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],3,0xe9b5dba5);
		RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],4,0x3956c25b);
		RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],5,0x59f111f1);
		RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],6,0x923f82a4);
		RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],7,0xab1c5ed5);
		RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],8,0xd807aa98);
		RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],9,0x12835b01);
		RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],10,0x243185be);
		RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],11,0x550c7dc3);
		RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],12,0x72be5d74);
		RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],13,0x80deb1fe);
		RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],14,0x9bdc06a7);
		RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],15,0xc19bf174);
		RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],16,0xe49b69c1);
		RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],17,0xefbe4786);
		RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],18,0x0fc19dc6);
		RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],19,0x240ca1cc);
		RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],20,0x2de92c6f);
		RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],21,0x4a7484aa);
		RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],22,0x5cb0a9dc);
		RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],23,0x76f988da);
		RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],24,0x983e5152);
		RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],25,0xa831c66d);
		RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],26,0xb00327c8);
		RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],27,0xbf597fc7);
		RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],28,0xc6e00bf3);
		RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],29,0xd5a79147);
		RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],30,0x06ca6351);
		RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],31,0x14292967);
		RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],32,0x27b70a85);
		RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],33,0x2e1b2138);
		RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],34,0x4d2c6dfc);
		RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],35,0x53380d13);
		RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],36,0x650a7354);
		RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],37,0x766a0abb);
		RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],38,0x81c2c92e);
		RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],39,0x92722c85);
		RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],40,0xa2bfe8a1);
		RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],41,0xa81a664b);
		RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],42,0xc24b8b70);
		RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],43,0xc76c51a3);
		RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],44,0xd192e819);
		RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],45,0xd6990624);
		RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],46,0xf40e3585);
		RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],47,0x106aa070);
		RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],48,0x19a4c116);
		RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],49,0x1e376c08);
		RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],50,0x2748774c);
		RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],51,0x34b0bcb5);
		RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],52,0x391c0cb3);
		RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],53,0x4ed8aa4a);
		RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],54,0x5b9cca4f);
		RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],55,0x682e6ff3);
		RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],56,0x748f82ee);
		RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],57,0x78a5636f);
		RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],58,0x84c87814);
		RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],59,0x8cc70208);
		RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],60,0x90befffa);
		RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],61,0xa4506ceb);
		RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],62,0xbef9a3f7);
		RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],63,0xc67178f2);

#undef RND		 
		
#endif		 

		/* feedback */
		for (i = 0; i < 8; i++) {
				md->state[i] = md->state[i] + S[i];
		}
		return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int SHA256::compress(State * md, unsigned char *buf)
{
		int err;
		err = SHA256::_compress(md, buf);
		burn_stack(sizeof(boost::uint32_t) * 74);
		return err;
}
#endif

/**
	 Initialize the hash state
	 @param md	 The hash state you wish to initialize
	 @return CRYPT_OK if successful
*/
int SHA256::init(State * md)
{
		LTC_ARGCHK(md != NULL);

		md->curlen = 0;
		md->length = 0;
		md->state[0] = 0x6A09E667UL;
		md->state[1] = 0xBB67AE85UL;
		md->state[2] = 0x3C6EF372UL;
		md->state[3] = 0xA54FF53AUL;
		md->state[4] = 0x510E527FUL;
		md->state[5] = 0x9B05688CUL;
		md->state[6] = 0x1F83D9ABUL;
		md->state[7] = 0x5BE0CD19UL;
		return CRYPT_OK;
}

/**
	 Process a block of memory though the hash
	 @param md		 The hash state
	 @param in		 The data to hash
	 @param inlen  The length of the data (octets)
	 @return CRYPT_OK if successful
*/
HASH_PROCESS(SHA256::process, SHA256::compress, sha256, 64)

/**
	 Terminate the hash to get the digest
	 @param md	The hash state
	 @param out [out] The destination of the hash (32 bytes)
	 @return CRYPT_OK if successful
*/
int SHA256::done(State * md, unsigned char *out)
{
		int i;

		LTC_ARGCHK(md  != NULL);
		LTC_ARGCHK(out != NULL);

		if (md->curlen >= sizeof(md->buf)) {
			 return CRYPT_INVALID_ARG;
		}


		/* increase the length of the message */
		md->length += md->curlen * 8;

		/* append the '1' bit */
		md->buf[md->curlen++] = (unsigned char)0x80;

		/* if the length is currently above 56 bytes we append zeros
		 * then compress.  Then we can fall back to padding zeros and length
		 * encoding like normal.
		 */
		if (md->curlen > 56) {
				while (md->curlen < 64) {
						md->buf[md->curlen++] = (unsigned char)0;
				}
				SHA256::compress(md, md->buf);
				md->curlen = 0;
		}

		/* pad upto 56 bytes of zeroes */
		while (md->curlen < 56) {
				md->buf[md->curlen++] = (unsigned char)0;
		}

		/* store length */
		STORE64H(md->length, md->buf+56);
		SHA256::compress(md, md->buf);

		/* copy output */
		for (i = 0; i < 8; i++) {
				STORE32H(md->state[i], out+(4*i));
		}
#ifdef LTC_CLEAN_STACK
		zeromem(md, sizeof(State));
#endif
		return CRYPT_OK;
}

// End copied code

namespace Swift {

SHA256::SHA256() {
	init(&state);
}

SHA256& SHA256::update(const std::vector<unsigned char>& input) {
	std::vector<unsigned char> inputCopy(input);
	process(&state, (boost::uint8_t*) vecptr(inputCopy), inputCopy.size());
	return *this;
}

std::vector<unsigned char> SHA256::getHash() const {
	std::vector<unsigned char> digest;
	digest.resize(256/8);
	State contextCopy(state);
	done(&contextCopy, (boost::uint8_t*) vecptr(digest));
	return digest;
}

template<typename Container>
ByteArray SHA256::getHashInternal(const Container& input) {
	State context;
	init(&context);

	Container inputCopy(input);
	process(&context, (boost::uint8_t*) vecptr(inputCopy), inputCopy.size());

	ByteArray digest;
	digest.resize(256/8);
	done(&context, (boost::uint8_t*) vecptr(digest));

	return digest;
}

ByteArray SHA256::getHash(const ByteArray& input) {
	return getHashInternal(input);
}

ByteArray SHA256::getHash(const SafeByteArray& input) {
	return getHashInternal(input);
}

}
