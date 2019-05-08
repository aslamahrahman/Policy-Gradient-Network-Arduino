/*
 * DESIGN OF Q LEARNING FRAMEWORK
 * STATE = DISCRETE DISTANCE RANGE VARYING FROM 0 TO 400 CM, IN STEPS OF 50 CM
 * SO OVERALL 8+1 STATES
 * MAINTAIN A DISTANCE OF 50-100 CM
 * ACTION = GO FORWARD, BACKWARD, STOP
 * SO OVERALL 3 ACTIONS
 * Q = {{0,0,0}, 0-50
 *      {0,0,0}, 51-100
 *      {0,0,0}, 101-150
 *      {0,0,0}, 151-200
 *      {0,0,0}, 201-250
 *      {0,0,0}, 251-300
 *      {0,0,0}, 301-350
 *      {0,0,0}, 351-400
 *      {0,0,0}} >400
 * R = {{-1, 0, 1000}, CAN NOT GO FORWARD IF DISTANCE IS ALREADY LESS THAN 50 CM
 *      {-1, -1, 0}, STOP WHEN IN POSITION IE 50-100 CM DISTANCE
 *      {0, -1, 1000}, TAKE THE RIGHT STEP TO GET IN POSITION
 *      {0, 0, 0},
 *      {0, 0, 0},
 *      {0, 0, 0},
 *      {0, 0, 0},
 *      {0, 0, 0},
 *      {0, -1, 0}} CAN NOT GO BACKWARD IF DISTANCE IS ALREADY MORE THAN 400 CM
 * GOAL = REACH STATE[1]
*/

/*
 * x      : 1x1
 * y      : 3x1
 * xl[0]  : 1x1 
 * xl[1]  : 2x1
 * z[0]   : 2x1
 * z[1]   : 3x1
 * w[0]   : 1x2
 * w[1]   : 2x3
 * b[0]   : 2x1
 * b[1]   : 3x1
 */
