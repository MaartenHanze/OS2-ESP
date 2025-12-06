// basic_tasks.ino
// MIT License (see file LICENSE)

// Change the following if you want to use
// different GPIO pins for the three LEDs

#define LED1 12  // GPIO 12
#define LED2 13  // etc.
#define LED3 14

struct s_led {
  byte gpio;           // LED GPIO number
  byte state;          // LED state
  unsigned napms;      // Delay to use (ms)
  TaskHandle_t taskh;  // Task handle
};

static s_led leds[3] = {
  { LED1, 0, 500, 0 },
  { LED2, 0, 200, 0 },
  { LED3, 0, 750, 0 }
};

static void led_task_func(void *argp) {
  s_led *ledp = (s_led *)argp;
  unsigned stack_hwm = 0, temp;

  delay(1000);

  for (;;) {
    digitalWrite(ledp->gpio, ledp->state ^= 1);
    temp = uxTaskGetStackHighWaterMark(nullptr);
    if (!stack_hwm || temp < stack_hwm) {
      stack_hwm = temp;
      printf("Task for gpio %d has stack hwm %u\n",
             ledp->gpio, stack_hwm);
    }
    delay(ledp->napms);
  }
}

void setup() {
  int app_cpu = 0;  // CPU number

  delay(500);  // Pause for serial setup

  app_cpu = xPortGetCoreID();
  printf("app_cpu is %d (%s core)\n",
         app_cpu,
         app_cpu > 0 ? "Dual" : "Single");

  printf("LEDs on gpios: ");
  for (auto &led : leds) {
    pinMode(led.gpio, OUTPUT);
    digitalWrite(led.gpio, LOW);
    xTaskCreatePinnedToCore(
      led_task_func,
      "led_task",
      2048,
      &led,
      1,
      &led.taskh,
      app_cpu);
    printf("%d ", led.gpio);
  }
  putchar('\n');
}

void loop() {
  static int counter = 0;       //deze telt hoeveel seconden er voorbij zijn gegaan sinds de huidige fase is begonnen. static betekent de waarde behouden blijft tussen de verschillende loops
  static int phase = 0;         //deze bepaalt in welke fase van het proces het momenteel zit, case 0 tot en met 3
  delay(1000);                  //elke loop duurt nu 1000ms, 1 seconde
  counter++;                    //verhoogt de counter, telkens met 1000ms

  switch (phase) {
    case 0: //zolang het deze fase/case is, voer dan deze code uit
      if (counter == 10) { //als er 10 seconden voorbij zijn
        vTaskSuspend(leds[0].taskh); //pauzeer, suspend, led0 dus stop lampje 1
        printf("Suspend task op gpio %d\n", leds[0].gpio); //print dat led0 is gepauzeerd, mooi om te zien in de terminal/seriële monitor
        phase = 1; //ga naar case 1
        counter = 0; //reset voor volgende fase 0
      }
      break; //sluit de case correct af

    case 1: //zolang het deze fase/case is, voer dan deze code uit
      if (counter == 3) { //als er 3 seconden voorbij zijn
        vTaskSuspend(leds[1].taskh); //pauzeer, suspend, led1 dus stop lampje 2
        printf("Suspend task op gpio %d\n", leds[1].gpio); //zelfde als case 0, alleen dan voor case 1
        phase = 2; //ga naar case 2
        counter = 0; //reset voor volgende fase 1
      }
      break; //sluit de case correct af

    case 2: //zolang het deze fase/case is, voer dan deze code uit
      if (counter == 3) { //als er 3 seconden voorbij zijn
        vTaskSuspend(leds[2].taskh); //pauzeer, suspend, led2 dus stop lampje 3
        printf("Suspend task op gpio %d\n", leds[2].gpio); //zelfde als case 0, alleen dan voor case 2
        phase = 3; //ga naar case 3
        counter = 0; //reset voor volgende fase 2
      }
      break; //sluit de case correct af

    case 3: //3 seconden na case 2 alles weer 'normaal' laten gaan, dus zoals als voor case 0
      if (counter == 3) { //als er 3 seconden voorbij zijn
        for (int i = 0; i < 3; i++) { //met deze for loop loopt die door alle 3 voorgaande fasen resumed deze
          vTaskResume(leds[i].taskh); //resumed die fasen
          printf("Resume task op gpio %d\n", leds[i].gpio); //print zodat het duidelijk is dat de lampjes weer 'normaal' gaan, dus zoals voor case 0
        }
        phase = 4; //omdat we geen case 4 hebben aangemaakt, gaat de code dan gelijk naar default, deze sluit dan deze loop
      }
      break; //sluit de case correct af

    default:
      //hier geven we niks mee, zodat deze direct de loop stopt
      break; //sluit correct af
  }
}
