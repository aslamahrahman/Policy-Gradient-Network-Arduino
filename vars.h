/*BEGIN CONTROLS STUFF-----------------------------------------*/
#define LED_BUILTIN 2
/*END CONTROLS STUFF--------------------------------------------*/

/*BEGIN TRAINING STUFF---------------------------------------*/
const int pin_training = 36;
const int pin_training_indicator = 22;
/*END TRAINING STUFF-----------------------------------------*/
  
/*BEGIN ULTRASONIC STUFF--------------------------------*/
const int pin_trigger = 25;
const int pin_echo = 26;
const int timeout = 30000;
/*END ULTRASONIC STUFF----------------------------------*/

/*BEGIN MOTOR STUFF-------------------------------------*/
//define motor variables
int motor1_pwm = 0;
int motor2_pwm = 0;
int motor1_dir1 = 0;
int motor1_dir2 = 0;
int motor2_dir1 = 0;
int motor2_dir2 = 0;

const int pin_motor1_pwm = 23;
const int pin_motor2_pwm = 22;
const int pin_motor1_dir1 = 19;
const int pin_motor1_dir2 = 18;
const int pin_motor2_dir1 = 17;
const int pin_motor2_dir2 = 16;

//constant speed for stage 1
const int drive_speed = 100;

//define thresholds to remove noise from pot readings
const int drive_thresh = 50;
const int drive_thresh_forward = drive_thresh;
const int drive_thresh_backward = -drive_thresh;
const int turn_thresh = 25;
const int dir_thresh_right = turn_thresh;
const int dir_thresh_left = -turn_thresh;

//direction pins values, caster wheel on left, motor 2 nearer to you, forward is anti-clockwise rotation, backward is clockwise rotation
const byte bw_dir1 = LOW;
const byte bw_dir2 = HIGH;
const byte fw_dir1 = HIGH;
const byte fw_dir2 = LOW;
const int is_turn_linear = 1;
/*END MOTOR STUFF---------------------------------------*/

/*BEGIN LEDC STUFF----------------------------------------*/
const int freq = 600;
const int led1_channel = 0;
const int led2_channel = 1;
const int resolution = 8;
/*END LEDC STUFF-------------------------------------------*/

/*BEGIN Q LEARNING STUFF------------------------------*/
PG_MODEL *agent;
const int num_inputs = 1;
const int num_actions = 5;
int actions[num_actions] = {-255, -200, 0, 200, 255};
float action_cdf[num_actions];
int delay_millis = 200;
int16_t goal = 15;
float learning_rate = 1.0e-5f;
float discount_rate = 0.9f; 
float rmsprop_beta_1 = 0.1f;
float reward_amplifier = 1.0f;

const int16_t layers[] = {num_inputs, 2, num_actions};
const int16_t *layers_p = layers;

int16_t **state_now, **state_next;
float reward;
long episode_counter = 0, training_counter = 0;
long max_training = 100;
volatile bool is_learning = false;
float execution_reward = 0.0f;
/*END Q LEARNING STUFF--------------------------------*/
