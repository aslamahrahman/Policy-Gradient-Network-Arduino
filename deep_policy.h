#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"
#include "utilities.h"
#include "Arduino.h"

#pragma once

const int num_layers = 3;
const int max_episodes = 5;

class episodes {
  public:
    int16_t **state_now;
    float **action;
    float reward;
};

class PG_MODEL {
  public:
    int num_actions;
    float **y;
    float *score;
    float discounted_rewards[max_episodes];
    int action_taken[max_episodes];
    float current_expected_score;
    int16_t **goal_p;
    float learning_rate;
    float discount_rate; 
    float reward_amplifier;

    /*BEGIN NEURAL NETWORK STUFF----------------------*/
    // Main variables
    const int16_t *layers;
    float **x_float;
    float **xl[num_layers-1];
    float **z[num_layers-1];
    float **weights[num_layers-1];
    float **biases[num_layers-1];
    enum ACTIVATION {
      TANH,
      SIGMOID,
      RELU
    };
    ACTIVATION activation_func;
    enum OPTIMIZER {
      SGD,
      RMSPROP,
      ADAM
    };
    OPTIMIZER op_algo;
    float op_beta_1; float op_beta_2;
    float **rmsprop_v;
    float **adam_v;
    float **adam_s;

    // Container variables
    float **fp_H[num_layers];
    float **fp_w_T[num_layers-1];
    float **fp_mat1[num_layers-1];

    float **log_policy;
    float **bp_grad_layer[num_layers-1];
    float **bp_w_delta[num_layers-1];
    float **bp_del[num_layers-1];
    float **bp_del_T[num_layers-1];
    float **bp_gradient[num_layers-1];
    float **bp_gradient_update_weights[num_layers-1];
    float **bp_gradient_update_biases[num_layers-1];
    /*END NEURAL NETWORK STUFF----------------------*/

    /*BEGIN CONSTRUCTORS & INITIALIZERS--------------*/
    PG_MODEL(PG_MODEL::ACTIVATION a, PG_MODEL::OPTIMIZER algo, const int16_t *layers_p, int num_actions, float learning_rate, float discount_rate, float reward_amplifier) {
      this->activation_func = a;
      this->op_algo = algo;
      this->layers = layers_p;
      this->num_actions = num_actions;
      this->learning_rate = learning_rate;
      this->discount_rate = discount_rate;
      this->reward_amplifier = reward_amplifier;
      this->allocate_params();
      this->xavier_init();
    }
    
    PG_MODEL(PG_MODEL::ACTIVATION a, PG_MODEL::OPTIMIZER algo, float beta_1, const int16_t *layers_p, int num_actions, float learning_rate, float discount_rate, float reward_amplifier) {
      this->activation_func = a;
      this->op_algo = algo;
      this->op_beta_1 = beta_1;
      this->layers = layers_p;
      this->num_actions = num_actions;
      this->learning_rate = learning_rate;
      this->discount_rate = discount_rate;
      this->reward_amplifier = reward_amplifier;
      this->allocate_params();
      this->xavier_init();
    }

    PG_MODEL(PG_MODEL::ACTIVATION a, PG_MODEL::OPTIMIZER algo, float beta_1, float beta_2, const int16_t *layers_p, int num_actions, float learning_rate, float discount_rate, float reward_amplifier) {
      this->activation_func = a;
      this->op_algo = algo;
      this->op_beta_1 = beta_1;
      this->op_beta_2 = beta_2;
      this->layers = layers_p;
      this->num_actions = num_actions;
      this->learning_rate = learning_rate;
      this->discount_rate = discount_rate;
      this->reward_amplifier = reward_amplifier;
      this->allocate_params();
      this->xavier_init();
    }
    /*END CONSTRUCTORS & INITIALIZERS----------------*/

    void free_model(void);

    /*BEGIN NEURAL NETWORK STUFF----------------------*/
    void allocate_params();
    void xavier_init();
    void forward_pass(int16_t **x);
    void back_propagate(int episode);
    void predict(int16_t **x);
    /*END NEURAL NETWORK STUFF----------------------*/

    /*BEGIN EXPERIENCE REPLAY STUFF------------------*/
    episodes mem[max_episodes];
    void insert_episodes(int16_t **state_now, float **action, float reward, int episode_counter);
    void discount_and_normalize_rewards(void);
    /*END EXPERIENCE REPLAY STUFF---------------------*/
};
