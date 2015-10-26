struct controller_t{
  int stepPin;
  int dirPin;
  int enablePin;
  int timeElapsed;
  int state;
  float last;
};

struct controller_t controllers[3];
unsigned long LAST_TIME = micros();

float t = 0;
float M[3][2] = {
  { 0, 1 },
  { cos(-PI / 6), sin(-PI / 6) },
  { cos(7 * PI / 6), sin(7 * PI / 6) }, 
};

struct controller_t initController(int stepPin, int dirPin, int enablePin)
{
   controller_t res = {
     .stepPin     = stepPin,
     .dirPin      = dirPin,
     .enablePin   = enablePin,
     .timeElapsed = 0,
     .state       = 0,
     .last        = 0,
   };
   
   pinMode(res.stepPin,   OUTPUT);
   pinMode(res.dirPin,    OUTPUT);
   pinMode(res.enablePin, OUTPUT);
   
   return res;
}

void stepInDirection(struct controller_t* controller, int dir)
{
   controller->state = !controller->state;
     
   digitalWrite(controller->dirPin, dir > 0 ? HIGH : LOW);   
   digitalWrite(controller->stepPin, controller->state ? HIGH : LOW);
}

void setup() {
  controllers[0] = initController(3, 4, 5);
  controllers[1] = initController(6, 8, 9);
  controllers[2] = initController(10, 11, 12 );  
}

float dot(float* a, float* b)
{
  return a[0] * b[0] + a[1] * b[1];
}

void loop() {
  float T[2] = { sin(t), cos(t) };
  unsigned long now = micros();
  unsigned long elapsed = now - LAST_TIME;
    
  for(int i = 3; i--;){
    float d = dot(M[i], T);
    int isNegative = d < 0;

    digitalWrite(controllers[i].enablePin, isNegative ? HIGH : LOW); 

    if(isNegative) continue;
    
    if(controllers[i].timeElapsed > (1 - d) * 800 + 400 ){
      stepInDirection(controllers + i, ((d - controllers[i].last) > 0) * 2 - 1);
      controllers[i].timeElapsed = 0;
      controllers[i].last = d;
    }
    
    controllers[i].timeElapsed += elapsed;

  }
  
  t += (elapsed / 1000000.0f);
  
  LAST_TIME = now;
}
