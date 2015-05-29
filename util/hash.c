#include "util/hash.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
static inline uint64_t xorshift128plus(xorshift128_state_t* state) { 
	uint64_t s1 = state->s[ 0 ];
	const uint64_t s0 = state->s[ 1 ];
	state->s[ 0 ] = s0;
	s1 ^= s1 << 23; // a
	return ( state->s[ 1 ] = ( s1 ^ s0 ^ ( s1 >> 17 ) ^ ( s0 >> 26 ) ) ) + s0; // b, c
}
static inline uint64_t xorshift1024star(xorshift1024_state_t* state){
  uint64_t s0 = state->s[0];
  int p = (state->index = (state->index+1)&15);;
  uint64_t s1 = state->s[p];
  s1 ^=(s1 << 31);
  s1 ^=(s1 >> 11);
  s0 ^=(s0 >> 30);
  return (state->s[ p ] = s0 ^ s1 ) * 1181783497276652981LL;
}

uint64_t threadrand_128(){
  static __thread xorshift128_state_t  state = {.s={0,0}};
  if(UNLIKELY(!state.s[0]&&!state.s[1])){
    state.s[0] = (uint64_t)&state;
    state.s[1] = get_timestamp();
    xorshift128plus(&state);
  }
  return xorshift128plus(&state);
}
uint64_t threadrand_1024(){
  static __thread xorshift1024_state_t state = {.s={0}};
  static __thread bool initialized = false;
  if(UNLIKELY(!initialized)){
    for(int i = 0; i< 16; i++)
      state.s[i] = threadrand_128();
  }
  return xorshift1024star(&state);
}
