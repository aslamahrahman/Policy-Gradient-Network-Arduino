#include "deep_policy.h"

#pragma once

Utilities utlq;
Matrix matq;

#define DEBUG false

void PG_MODEL::allocate_params() {
  int i=0;

  for(i=0; i<max_episodes; i++) {
    this->mem[i].state_now = utlq.allocate_2D_int16_t(this->layers[0], 1);
    this->mem[i].action = utlq.allocate_2D_float(this->layers[num_layers-1], 1);
    this->mem[i].reward = 0.0f;
  }

  // Main variables
  this->x_float = utlq.allocate_2D_float(this->layers[0], 1);
  for(i=0; i<num_layers-1; i++) {
    this->weights[i] = utlq.allocate_2D_float(this->layers[i], this->layers[i+1]);
    this->biases[i] = utlq.allocate_2D_float(this->layers[i+1], 1);
    this->xl[i] = utlq.allocate_2D_float(this->layers[i], 1);
    this->z[i] = utlq.allocate_2D_float(this->layers[i+1], 1);
  }

  this->y = utlq.allocate_2D_float(this->layers[num_layers-1], 1);
  this->score = utlq.allocate_1D_float(max_episodes);

  switch(this->op_algo) {
    case RMSPROP:
      this->rmsprop_v = utlq.allocate_2D_float(this->layers[num_layers-2], this->layers[num_layers-1]);
      break;
      
    case ADAM:
      this->adam_v = utlq.allocate_2D_float(this->layers[num_layers-2], this->layers[num_layers-1]);
      this->adam_s = utlq.allocate_2D_float(this->layers[num_layers-2], this->layers[num_layers-1]);
      break;
  }

  // Container variables
  for(i=0; i<num_layers; i++) {
    this->fp_H[i] = utlq.allocate_2D_float(this->layers[i], 1);
  }
  for(i=0; i<num_layers-1; i++) {
    this->fp_w_T[i] = utlq.allocate_2D_float(this->layers[i+1], this->layers[i]);
    this->fp_mat1[i] = utlq.allocate_2D_float(this->layers[i+1], 1);
    
    this->bp_grad_layer[i] = utlq.allocate_2D_float(this->layers[i+1], 1);
    this->bp_w_delta[i] = utlq.allocate_2D_float(this->layers[i+1], 1);
    this->bp_del[i] = utlq.allocate_2D_float(this->layers[i+1], 1);
    this->bp_del_T[i] = utlq.allocate_2D_float(1, this->layers[i+1]);
    this->bp_gradient[i] = utlq.allocate_2D_float(this->layers[i], this->layers[i+1]);
    this->bp_gradient_update_weights[i] = utlq.allocate_2D_float(this->layers[i], this->layers[i+1]);
    this->bp_gradient_update_biases[i] = utlq.allocate_2D_float(this->layers[i+1], 1);
  }

  return;
}

void PG_MODEL::xavier_init() {
  int l, i, j;

  for(l=0; l<num_layers-1; l++) {
    for(i=0; i<this->layers[l]; i++) {
      for(j=0; j<this->layers[l+1]; j++) {
        float buf = sqrtf(6.0f/(this->layers[l] + this->layers[l+1]));
        this->weights[l][i][j] = -buf + 2.0f*buf*utlq.random_between_vals(this->layers[l], this->layers[l+1]);
      }
    }
    for(i=0; i<this->layers[l+1]; i++) {
      for(j=0; j<1; j++) {
        this->biases[l][i][j] = 0.0f;
      }
    }
  }
  return;
}

void PG_MODEL::forward_pass(int16_t **x) {
  this->x_float = matq.int_to_float(this->x_float, x, this->layers[0], 1);
  this->fp_H[0] = matq.copy_to_existing(this->fp_H[0], this->x_float, this->layers[0], 1);
  
  int l;

  switch(this->activation_func) {
    case TANH:
      for(l=0; l<num_layers-2; l++) {
        this->xl[l] = matq.copy_to_existing(this->xl[l], this->fp_H[l], this->layers[l], 1);
        this->fp_w_T[l] = matq.transpose(this->fp_w_T[l], this->weights[l], this->layers[l], this->layers[l+1]);
        this->fp_mat1[l] = matq.multiply_mat(this->fp_mat1[l], this->fp_w_T[l], this->layers[l+1], this->layers[l], this->fp_H[l], this->layers[l], 1);
        this->z[l] = matq.add_mat(this->z[l], this->fp_mat1[l], this->biases[l], this->layers[l+1], 1);
        this->fp_H[l+1] = matq.tanh_mat(this->fp_H[l+1], this->z[l], this->layers[l+1], 1);
      }
      break;
    case RELU:
      for(l=0; l<num_layers-2; l++) {
        this->xl[l] = matq.copy_to_existing(this->xl[l], this->fp_H[l], this->layers[l], 1);
        this->fp_w_T[l] = matq.transpose(this->fp_w_T[l], this->weights[l], this->layers[l], this->layers[l+1]);
        this->fp_mat1[l] = matq.multiply_mat(this->fp_mat1[l], this->fp_w_T[l], this->layers[l+1], this->layers[l], this->fp_H[l], this->layers[l], 1);
        this->z[l] = matq.add_mat(this->z[l], this->fp_mat1[l], this->biases[l], this->layers[l+1], 1);
        this->fp_H[l+1] = matq.relu_mat(this->fp_H[l+1], this->z[l], this->layers[l+1], 1);
      }
      break;
  }

  // Softmax layer
  this->xl[l] = matq.copy_to_existing(this->xl[l], this->fp_H[l], this->layers[l], 1);
  this->fp_w_T[l] = matq.transpose(this->fp_w_T[l], this->weights[l], this->layers[l], this->layers[l+1]);
  this->fp_mat1[l] = matq.multiply_mat(this->fp_mat1[l], this->fp_w_T[l], this->layers[l+1], this->layers[l], this->fp_H[l], this->layers[l], 1);
  this->z[l] = matq.add_mat(this->z[l], this->fp_mat1[l], this->biases[l], this->layers[l+1], 1);
  this->fp_H[l+1] = matq.softmax_mat(this->fp_H[l+1], this->z[l], this->layers[l+1], 1);
  this->y = matq.copy_to_existing(this->y, this->fp_H[l+1], this->layers[l+1], 1);

  return;
}

void PG_MODEL::back_propagate(int episode) {
  int l = num_layers-2;

  this->bp_grad_layer[l] = matq.grad_softmax_mat(this->bp_grad_layer[l], this->z[l], this->layers[l+1], 1);
  this->bp_del[l] = matq.divide_element_mat(this->bp_del[l], this->bp_grad_layer[l], this->y, this->layers[l+1], 1);  
  this->bp_del_T[l] = matq.transpose(this->bp_del_T[l], this->bp_del[l], this->layers[l+1], 1);
  this->bp_gradient[l] = matq.multiply_mat(this->bp_gradient[l], this->xl[l], this->layers[l], 1, this->bp_del_T[l], 1, this->layers[l+1]);   

  switch(this->op_algo) {
    case SGD:
      this->bp_gradient_update_weights[l] = matq.multiply_scalar(this->bp_gradient_update_weights[l], this->bp_gradient[l],
                                                        (this->learning_rate*this->score[episode]), this->layers[l], this->layers[l+1]);
      this->bp_gradient_update_biases[l] = matq.multiply_scalar(this->bp_gradient_update_biases[l], this->bp_del[l], 
                                                        (this->learning_rate*this->score[episode]), this->layers[l+1], 1);
      break;
      
    case RMSPROP:      
      this->bp_gradient_update_weights[l] = matq.multiply_scalar(this->bp_gradient_update_weights[l], this->bp_gradient[l],
                                                        (this->learning_rate*this->score[episode]), this->layers[l], this->layers[l+1]);
      this->bp_gradient_update_biases[l] = matq.multiply_scalar(this->bp_gradient_update_biases[l], this->bp_del[l], 
                                                        (this->learning_rate*this->score[episode]), this->layers[l+1], 1);
      for(int i=0; i<l; i++) {
        for(int j=0; j<l+1; j++) {
          this->rmsprop_v[i][j] = this->op_beta_1*this->rmsprop_v[i][j] + (1.0f - this->op_beta_1)*this->bp_gradient[l][i][j]*this->bp_gradient[l][i][j];
          this->bp_gradient_update_weights[l][i][j] /= sqrtf(this->rmsprop_v[i][j]);
        }
      }
      break;

    case ADAM:
      this->bp_gradient_update_weights[l] = matq.multiply_scalar(this->bp_gradient_update_weights[l], this->bp_gradient[l],
                                                        (this->learning_rate*this->score[episode]), this->layers[l], this->layers[l+1]);
      this->bp_gradient_update_biases[l] = matq.multiply_scalar(this->bp_gradient_update_biases[l], this->bp_del[l], 
                                                        (this->learning_rate*this->score[episode]), this->layers[l+1], 1);
      for(int i=0; i<l; i++) {
        for(int j=0; j<l+1; j++) {
          this->adam_v[i][j] = this->op_beta_1*this->adam_v[i][j] - (1.0f - this->op_beta_1)*this->bp_gradient[l][i][j];
          this->adam_s[i][j] = this->op_beta_2*this->adam_s[i][j] - (1.0f - this->op_beta_2)*this->bp_gradient[l][i][j]*this->bp_gradient[l][i][j] + matq.jitter;
          this->bp_gradient_update_weights[l][i][j] *= this->adam_v[i][j];
          this->bp_gradient_update_weights[l][i][j] /= this->adam_s[i][j];
        }
      }
      break;
  }
  
  this->weights[l] = matq.add_mat(this->weights[l], this->weights[l], this->bp_gradient_update_weights[l], this->layers[l], this->layers[l+1]);
  this->biases[l] = matq.add_mat(this->biases[l], this->biases[l], this->bp_gradient_update_biases[l], this->layers[l+1], 1);

  #if DEBUG
    Serial.println("Gradients: ");
//    matq.print_mat(this->bp_grad_layer[l], this->layers[l+1], 1);
//    matq.print_mat(this->bp_del[l], this->layers[l+1], 1);
//    matq.print_mat(this->bp_gradient[l], this->layers[l], this->layers[l+1]);
//    matq.print_mat(this->bp_gradient_update_weights[l], this->layers[l], this->layers[l+1]);
  #endif

  switch(this->activation_func) {
    case TANH:
      for(l=num_layers-3; l>=0; l--) {
        this->bp_grad_layer[l] = matq.grad_tanh_mat(this->bp_grad_layer[l], this->z[l], this->layers[l+1], 1);
        this->bp_w_delta[l] = matq.multiply_mat(this->bp_w_delta[l], this->weights[l+1], this->layers[l+1], this->layers[l+2], this->bp_del[l+1], this->layers[l+2], 1);
        this->bp_del[l] = matq.multiply_element_mat(this->bp_del[l], this->bp_w_delta[l], this->bp_grad_layer[l], this->layers[l+1], 1);
        this->bp_del_T[l] = matq.transpose(this->bp_del_T[l], this->bp_del[l], this->layers[l+1], 1);
        this->bp_gradient[l] = matq.multiply_mat(this->bp_gradient[l], this->xl[l], this->layers[l], 1, this->bp_del_T[l], 1, this->layers[l+1]);
        this->bp_gradient_update_weights[l] = matq.multiply_scalar(this->bp_gradient_update_weights[l], this->bp_gradient[l], this->learning_rate, this->layers[l], this->layers[l+1]);
        this->bp_gradient_update_biases[l] = matq.multiply_scalar(this->bp_gradient_update_biases[l], this->bp_del[l], this->learning_rate, this->layers[l+1], 1);
        this->weights[l] = matq.add_mat(this->weights[l], this->weights[l], this->bp_gradient_update_weights[l], this->layers[l], this->layers[l+1]);
        this->biases[l] = matq.add_mat(this->biases[l], this->biases[l], this->bp_gradient_update_biases[l], this->layers[l+1], 1);
      }
      break;
    case RELU:
      for(l=num_layers-3; l>=0; l--) {
        this->bp_grad_layer[l] = matq.grad_relu_mat(this->bp_grad_layer[l], this->z[l], this->layers[l+1], 1);
        this->bp_w_delta[l] = matq.multiply_mat(this->bp_w_delta[l], this->weights[l+1], this->layers[l+1], this->layers[l+2], this->bp_del[l+1], this->layers[l+2], 1);
        this->bp_del[l] = matq.multiply_element_mat(this->bp_del[l], this->bp_w_delta[l], this->bp_grad_layer[l], this->layers[l+1], 1);
        this->bp_del_T[l] = matq.transpose(this->bp_del_T[l], this->bp_del[l], this->layers[l+1], 1);
        this->bp_gradient[l] = matq.multiply_mat(this->bp_gradient[l], this->xl[l], this->layers[l], 1, this->bp_del_T[l], 1, this->layers[l+1]);
        this->bp_gradient_update_weights[l] = matq.multiply_scalar(this->bp_gradient_update_weights[l], this->bp_gradient[l], this->learning_rate, this->layers[l], this->layers[l+1]);
        this->bp_gradient_update_biases[l] = matq.multiply_scalar(this->bp_gradient_update_biases[l], this->bp_del[l], this->learning_rate, this->layers[l+1], 1);
        this->weights[l] = matq.add_mat(this->weights[l], this->weights[l], this->bp_gradient_update_weights[l], this->layers[l], this->layers[l+1]);
        this->biases[l] = matq.add_mat(this->biases[l], this->biases[l], this->bp_gradient_update_biases[l], this->layers[l+1], 1);
      }
      break;
  }
  
  return;
}

void PG_MODEL::predict(int16_t **x) {
  this->forward_pass(x);
  return;
}

void PG_MODEL::free_model() {
  delete(this->y); this->y = NULL;

  int i;
  for(i=0; i<num_layers-1; i++) {
    delete(this->weights[i]); this->weights[i] = NULL;
    delete(this->biases[i]); this->biases[i] = NULL;
    delete(this->xl[i]); this->xl[i] = NULL;
    delete(this->z[i]); this->z[i] = NULL;
  }
  
  for(i=0; i<num_layers; i++) {
    delete(this->fp_H[i]); this->fp_H[i] = NULL;
  }
  for(i=0; i<num_layers-1; i++) {
    delete(this->fp_w_T[i]); this->fp_w_T[i] = NULL;
    delete(this->fp_mat1[i]); this->fp_mat1[i] = NULL;
    
    delete(this->bp_grad_layer[i]); this->bp_grad_layer[i] = NULL;
    delete(this->bp_w_delta[i]); this->bp_w_delta[i] = NULL;
    delete(this->bp_del[i]); this->bp_del[i] = NULL;
    delete(this->bp_del_T[i]); this->bp_del_T[i] = NULL;
    delete(this->bp_gradient[i]); this->bp_gradient[i] = NULL;
    delete(this->bp_gradient_update_weights[i]); this->bp_gradient_update_weights[i] = NULL;
    delete(this->bp_gradient_update_biases[i]); bp_gradient_update_biases[i] = NULL;
  }

  return;
}
