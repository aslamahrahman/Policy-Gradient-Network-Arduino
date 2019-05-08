#include "deep_policy.h"
#include "vars.h"

#pragma once

Matrix mat;
Utilities utl;

#define DEBUG true
#define DEBUG_TIME false
#define DEBUG_SCORE false
#define DEBUG_REWARD true
#define DEBUG_STATE true

void initialize(void);
int16_t get_distance(void);
void get_state(int8_t when);
void try_policy(void);
int sample_action(void);
void move_policy(void);
void learn_policy(void);
void execute_policy(void);
void drive(int16_t velocity);
void turn_right(int16_t velocity, int16_t pot_turn);
void turn_right(int16_t velocity, int16_t pot_turn);

void setup() {
  Serial.begin(115200);
  initialize();

  /*BEGIN LEDC STUFF--------------------------------------*/
  ledcSetup(led1_channel, freq, resolution);
  ledcAttachPin(pin_motor1_pwm, led1_channel);
  ledcSetup(led2_channel, freq, resolution);
  ledcAttachPin(pin_motor2_pwm, led2_channel);
  /*END LEDC STUFF---------------------------------------*/

  /*BEGIN Q LEARNING STUFF------------------------------*/
  state_now = utl.allocate_2D_int16_t(agent->layers[0], 1);
  state_next = utl.allocate_2D_int16_t(agent->layers[0], 1);
  /*END Q LEARNING STUFF------------------------------*/

  Serial.println("Begin!");
  get_state(0);
}

void loop() {
  /*BEGIN Q LEARNING STUFF------------------------------*/
  if(!is_learning) {
    episode_counter++;
    #if DEBUG
      Serial.printf("episode_counter: %d\n", episode_counter);
    #endif
  }

  if(digitalRead(pin_training) == HIGH) {
    if(!is_learning && (episode_counter <= max_episodes)) {
      try_policy();
    }
    else {
      is_learning = true;
      episode_counter = 0;
      training_counter++;

      if(training_counter < max_training) {
        learn_policy();
      }
      else {
        training_counter = 0;
        is_learning = false;
      }
    }
  }
  else {
    execute_policy();
  }
  /*END Q LEARNING STUFF------------------------------*/
}

void initialize() {
  /*BEGIN TRAINING STUFF---------------------------------------*/
  pinMode(pin_training, INPUT);
  pinMode(pin_training_indicator, OUTPUT);
  /*END TRAINING STUFF-----------------------------------------*/
  
  /*BEGIN ULTRASONIC STUFF--------------------------------*/
  pinMode(pin_trigger, OUTPUT);
  pinMode(pin_echo, INPUT);
  /*END ULTRASONIC STUFF----------------------------------*/

  /*BEGIN MOTOR STUFF-------------------------------------*/
  pinMode(pin_motor1_pwm, OUTPUT);
  pinMode(pin_motor2_pwm, OUTPUT);
  pinMode(pin_motor1_dir1, OUTPUT);
  pinMode(pin_motor1_dir2, OUTPUT);
  pinMode(pin_motor2_dir1, OUTPUT);
  pinMode(pin_motor2_dir2, OUTPUT);
  /*END MOTOR STUFF-------------------------------------*/

  /*BEGIN Q LEARNING STUFF------------------------------*/
  agent = new QL_MODEL(QL_MODEL::ACTIVATION::RELU, QL_MODEL::OPTIMIZER::SGD, rmsprop_beta_1, layers_p, num_actions, learning_rate, discount_rate, reward_amplifier);
  /*END Q LEARNING STUFF------------------------------*/
}

/*BEGIN ULTRASONIC STUFF--------------------------------*/
int16_t get_distance() {
  int16_t duration, cm = 0;
  int16_t num_samples = 20;
  for(int16_t i=0; i<num_samples; i++) {
    digitalWrite(pin_trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(pin_trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin_trigger, LOW);
    delayMicroseconds(2);
    duration = pulseIn(pin_echo, HIGH);
    cm += (duration/29)/2;
  }
  cm = cm/num_samples;

  if(cm < 1)
    cm = 1;
  
  #if DEBUG_STATE
    Serial.printf("Distance: %d\n", cm);
  #endif

  
  return cm;
}

void get_state(int8_t when) {
  int16_t dist = get_distance();

  if(when == 0) {
    if(dist <= 400) {
      state_now[0][0] = dist;
    }
    else {
      state_now[0][0] = 400;
    }
  }
  else {
    if(dist <= 400) {
      state_next[0][0] = dist;
    }
    else {
      state_next[0][0] = 400;
    }
  }
  return;
}
/*END ULTRASONIC STUFF----------------------------------*/

/*BEGIN POLICY GRADIENT STUFF*/
void try_policy(void) {
  #if DEBUG
    Serial.println("Trying policy");
  #endif
  
  get_state(0);
  agent->predict(state_now);

  // Sample
  int action_idx = sample_action();
  agent->action_taken[episode_counter-1] = action_idx;
  
  drive(actions[action_idx]);
  delay(delay_millis);
  stop_now();
  
  get_state(1);
  reward = 0.0f;
  if(state_next[0][0] >= goal) {
    for(int i=0; i<agent->layers[0]; i++) {
//      reward += float(-state_next[i][0] + state_now[i][0])*(state_next[i][0] - goal)*agent->reward_amplifier;
        reward += float(-state_next[i][0] + state_now[i][0])*agent->reward_amplifier;
    }
  }
  else {
    for(int i=0; i<agent->layers[0]; i++) {
//      reward += float(state_next[i][0] - state_now[i][0])*(goal - state_next[i][0])*agent->reward_amplifier;
        reward += float(state_next[i][0] - state_now[i][0])*agent->reward_amplifier;
    }
  }
  agent->insert_episodes(state_now, agent->y, reward, episode_counter-1);

  #if DEBUG_REWARD
    Serial.printf("Reward: %.6e\n", reward);
  #endif

  return;
}

int sample_action() {
  int max_iter = 5;
  for(int i=0; i<max_iter; i++) {
    float rp = float(random(0,100))/100.0f;
    int r = random(0, agent->layers[num_layers-1]);
    if(agent->y[r][0] < rp) {
      #if DEBUG
        Serial.printf("Sampled: %d\n", r);
      #endif
      return r;
    }
  }
  return random(0, agent->layers[num_layers-1]);
}

void learn_policy() {
  #if DEBUG
    Serial.println("Learning policy");
  #endif
  
  int e, j;

  agent->discount_and_normalize_rewards();
  agent->current_expected_score = 0.0f;
  for(e=0; e<max_episodes; e++) {
    agent->forward_pass(agent->mem[e].state_now);
    agent->back_propagate(e);
    for(j=0; j<1; j++) {
      agent->current_expected_score += agent->y[agent->action_taken[e]][j]*agent->score[e];
    }
  }

  #if DEBUG_SCORE
    Serial.printf("Expected score %.6e\n", agent->current_expected_score /= max_episodes);
  #endif

  return;
}

void execute_policy() {
  #if DEBUG
    Serial.println("Executing policy");
  #endif
  
  get_state(0);
  agent->predict(state_now);

  // Sample
  int action_idx = sample_action();
  
  drive(actions[action_idx]);
  delay(delay_millis);
  stop_now();

  if(state_next[0][0] >= goal) {
    for(int i=0; i<agent->layers[0]; i++) {
      execution_reward += float(-state_next[i][0] + state_now[i][0])*agent->reward_amplifier;
    }
  }
  else {
    for(int i=0; i<agent->layers[0]; i++) {
      execution_reward += float(state_next[i][0] - state_now[i][0])*agent->reward_amplifier;
    }
  }

  #if DEBUG_REWARD
    Serial.printf("Execution cumulative reward: %.6e\n", execution_reward);
  #endif

  return;
}
/*END POLICY GRADIENT STUFF*/

/*BEGIN MOTOR STUFF-------------------------------------*/
void drive(int16_t velocity) {
  if (velocity > drive_thresh_forward) {
    //go forward
    motor1_pwm = abs(velocity);
    motor2_pwm = abs(velocity);

    digitalWrite(pin_motor1_dir1, fw_dir1);
    digitalWrite(pin_motor1_dir2, fw_dir2);
    ledcWrite(led1_channel, motor1_pwm);

    digitalWrite(pin_motor2_dir1, fw_dir1);
    digitalWrite(pin_motor2_dir2, fw_dir2);
    ledcWrite(led2_channel, motor2_pwm);
  }
  else if (velocity < drive_thresh_backward) {
    //go backward
    motor1_pwm = abs(velocity);
    motor2_pwm = abs(velocity);

    digitalWrite(pin_motor1_dir1, bw_dir1);
    digitalWrite(pin_motor1_dir2, bw_dir2);
    ledcWrite(led1_channel, motor1_pwm);

    digitalWrite(pin_motor2_dir1, bw_dir1);
    digitalWrite(pin_motor2_dir2, bw_dir2);
    ledcWrite(led2_channel, motor2_pwm);
  }
  else {
    //stay right there
    motor1_pwm = 0;
    motor2_pwm = 0;
    ledcWrite(led1_channel, motor1_pwm);
    ledcWrite(led2_channel, motor2_pwm);
  }
}

void stop_now(void) {
  motor1_pwm = 0;
  motor2_pwm = 0;
  ledcWrite(led1_channel, motor1_pwm);
  ledcWrite(led2_channel, motor2_pwm);
}

void turn_left(int16_t velocity, int16_t pot_turn) {
  if (velocity > drive_thresh_forward) {
    //go forward left
    motor1_pwm = abs(velocity);
    motor2_pwm = is_turn_linear*(abs(velocity)*pot_turn/(3*turn_thresh) + 4*abs(velocity)/3);

    digitalWrite(pin_motor1_dir1, fw_dir1);
    digitalWrite(pin_motor1_dir2, fw_dir2);
    ledcWrite(led1_channel, motor1_pwm);

    digitalWrite(pin_motor2_dir1, fw_dir1);
    digitalWrite(pin_motor2_dir2, fw_dir2);
    ledcWrite(led2_channel, motor2_pwm);
  }
  else if (velocity < drive_thresh_backward) {
    //go backward left
    motor1_pwm = abs(velocity);
    motor2_pwm = is_turn_linear*(abs(velocity)*pot_turn/(3*turn_thresh) + 4*abs(velocity)/3);

    digitalWrite(pin_motor1_dir1, bw_dir1);
    digitalWrite(pin_motor1_dir2, bw_dir2);
    ledcWrite(led1_channel, motor1_pwm);

    digitalWrite(pin_motor2_dir1, bw_dir1);
    digitalWrite(pin_motor2_dir2, bw_dir2);
    ledcWrite(led2_channel, motor2_pwm);
  }
  else {
    //stay right there
    motor1_pwm = 0;
    motor2_pwm = 0;
    ledcWrite(led1_channel, motor1_pwm);
    ledcWrite(led2_channel, motor2_pwm);
  }
}

void turn_right(int16_t velocity, int16_t pot_turn) {
  if (velocity > drive_thresh_forward) {
    //go forward right
    motor1_pwm = is_turn_linear*(-abs(velocity)*pot_turn/(3*turn_thresh) + 4*abs(velocity)/3);
    motor2_pwm = abs(velocity);

    digitalWrite(pin_motor1_dir1, fw_dir1);
    digitalWrite(pin_motor1_dir2, fw_dir2);
    ledcWrite(led1_channel, motor1_pwm);

    digitalWrite(pin_motor2_dir1, fw_dir1);
    digitalWrite(pin_motor2_dir2, fw_dir2);
    ledcWrite(led2_channel, motor2_pwm);
  }
  else if (velocity < drive_thresh_backward) {
    //go backward right
    motor1_pwm = is_turn_linear*(-abs(velocity)*pot_turn/(3*turn_thresh) + 4*abs(velocity)/3);
    motor2_pwm = abs(velocity);
    
    digitalWrite(pin_motor1_dir1, bw_dir1);
    digitalWrite(pin_motor1_dir2, bw_dir2);
    ledcWrite(led1_channel, motor1_pwm);

    digitalWrite(pin_motor2_dir1, bw_dir1);
    digitalWrite(pin_motor2_dir2, bw_dir2);
    ledcWrite(led2_channel, motor2_pwm);
  }
  else {
    //stay right there
    motor1_pwm = 0;
    motor2_pwm = 0;
    ledcWrite(led1_channel, motor1_pwm);
    ledcWrite(led2_channel, motor2_pwm);
  }
}
/*END MOTOR STUFF---------------------------------------*/
