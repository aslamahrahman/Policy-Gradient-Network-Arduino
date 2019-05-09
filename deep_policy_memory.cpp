#include "deep_policy.h"

#pragma once

Utilities utlqm;
Matrix matqm;

#define DEBUG false

void PG_MODEL::insert_episodes(int16_t **state_now, float **action, float reward, int episode_counter) {
  matqm.copy_to_existing(this->mem[episode_counter].state_now, state_now, this->layers[0], 1);
  matqm.copy_to_existing(this->mem[episode_counter].action, action, this->layers[num_layers-1], 1);
  this->mem[episode_counter].reward = reward;
  
  return;
}

void PG_MODEL::discount_and_normalize_rewards() {
  int e, j;
  
  float cumulative = 0.0f;
  float max_score = 0.0f;
  for(e=max_episodes-1; e>=0; e--) {
    cumulative = cumulative*this->discount_rate + this->mem[e].reward;
    this->score[e] =  cumulative;
    if(abs(this->score[e]) > max_score) {
      max_score = abs(this->score[e]);
    }
  }

  // Rescale
  for(e=max_episodes-1; e>=0; e--) {
    this->score[e] /=  max_score;
  }

  #if DEBUG
    Serial.println("Score:");
    matqm.print_mat(this->score, max_episodes);
  #endif

//  float mean = 0.0f;
//  for(e=max_episodes-1; e>=0; e--) {
//    mean += this->score[e];
//  }
//  mean /= max_episodes;
//
//  float std = 0.0f;
//  for(e=max_episodes-1; e>=0; e--) {
//    std += (this->score[e] - mean)*(this->score[e] - mean);
//  }
//  std /= max_episodes;
//  std = sqrtf(std);
//
//  for(e=max_episodes-1; e>=0; e--) {
//    this->score[e] =  (this->score[e] - mean)/std;
//  }
  
  return;
}
