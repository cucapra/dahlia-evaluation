#include "viterbi.h"

#pragma SDS data copy(obs[0:N_OBS])
#pragma SDS data zero_copy(path[0:N_OBS])
void viterbi( tok_t obs[N_OBS], prob_t init[N_STATES], prob_t transition[N_STATES*N_STATES], prob_t emission[N_STATES*N_TOKENS], state_t path[N_OBS] );
