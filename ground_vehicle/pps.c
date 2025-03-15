#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/timex.h>

#include <pigpio.h>

/*

pps.c
2020-09-18
Public Domain

gcc -o pps pps.c -lpigpio

sudo ./pps

*/

#define GPIO    4   /* gpio for output pulse */
#define LEVEL   1   /* pulse high or low */
#define PULSE 200   /* pulse length in microseconds */
#define SECONDS 1   /* pulse every second */
#define SLACK  5000 /* slack period to correct time */

#define MILLION 1000000

static int g_gpio    = GPIO;
static int g_plevel  = LEVEL;
static int g_plength = PULSE;
static int g_seconds = SECONDS;

static int g_interval;

static uint32_t *g_slackA;

void fatal(char *fmt, ...)
{
   char buf[256];
   va_list ap;

   va_start(ap, fmt);
   vsnprintf(buf, sizeof(buf), fmt, ap);
   va_end(ap);

   fprintf(stderr, "%s\n", buf);

   exit(EXIT_FAILURE);
}

void usage()
{
   fprintf(stderr, "\n" \
      "Usage: sudo ./pps [OPTION] ...\n"\
      "   -g 0-31   gpio (%d)\n"\
      "   -l 0,1    pulse level (%d)\n"\
      "   -m 1-5000 pulse micros (%d)\n"\
      "   -s 1-60   interval seconds (%d)\n"\
      "EXAMPLE\n"\
      "sudo ./pps -g 23 -s 5\n"\
      "  Generate pulse every 5 seconds on gpio 23.\n"\
   "\n", GPIO, LEVEL, PULSE, SECONDS);
}

static void initOpts(int argc, char *argv[])
{
   int i, opt;

   while ((opt = getopt(argc, argv, "g:l:m:s:")) != -1)
   {
      i = -1;

      switch (opt)
      {
         case 'g':
            i = atoi(optarg);
            if ((i >= 0) && (i <= 31)) g_gpio = i;
            else fatal("invalid -g option (%d)", i);
            break;

         case 'l':
            i = atoi(optarg);
            if ((i == 0) || (i == 1)) g_plevel = i;
            else fatal("invalid -l option (%d)", i);
            break;

         case 'm':
            i = atoi(optarg);
            if ((i > 0) && (i<=5000)) g_plength = i;
            else fatal("invalid -m option (%d)", i);
            break;

         case 's':
            i = atoi(optarg);
            if ((i > 0) && (i<=60)) g_seconds = i;
            else fatal("invalid -s option (%d)", i);
            break;

         default: /* '?' */
            usage();
            exit(EXIT_FAILURE);
        }
    }
}

void callback(int gpio, int level, uint32_t tick)
{
   static int inited = 0, drift = 0, count = 0;

   int i;
   int slack; /* how many microseconds for slack pulse */
   int offby; /* microseconds off from 0 */
   uint32_t stamp_micro, stamp_tick;
   uint32_t pulse_tick, now_tick;
   uint32_t tick1, tick2, tick_diff;
   uint32_t nextPulse, nextPulseTick, delay, fixed;
   struct timespec tp;

   if (level == g_plevel)
   {
      /*
         Seconds boundary has arrived.

         Make several attempts at finding the relationship between the
         system tick and the clock microsecond.

         Do so by bracketing the call to the clock with calls to get
         the system tick.

         Escape the loop early if the difference between the two
         system ticks is zero (can't do any better).
      */

      pulse_tick = rawWaveGetIn(0); /* tick read at pulse start */
      now_tick = gpioTick();        /* just for interest, to get an idea
                                       of scheduling delays */

      tick_diff = 10000000;

      for (i=0; i<10; i++)
      {
         tick1 = gpioTick();

         clock_gettime(CLOCK_REALTIME, &tp);

         tick2 = gpioTick();

         if ((tick2 - tick1) < tick_diff)
         {
            tick_diff = tick2 - tick1;

            stamp_tick  = tick1;

            stamp_micro = ((tp.tv_sec % g_seconds) * MILLION) +
                          ((tp.tv_nsec+500) / 1000);

            if (tick_diff == 0) break;
         }
      }

      /*
      */

      if (inited)
      {
         /* correct if early */
         if (stamp_micro > (g_interval / 2)) stamp_micro -= g_interval;
         offby  = stamp_micro - (stamp_tick - pulse_tick);
         drift += offby/2; /* correct drift, bit of lag */
      }
      else
      {
         offby = 0;
         drift = 0;
      }

      nextPulse = g_interval - stamp_micro;
      nextPulseTick = stamp_tick + nextPulse - drift;

      delay = nextPulseTick - pulse_tick;

      fixed = g_interval - SLACK;
      slack = delay - fixed;

      if (slack < 0) slack += g_interval;
      if (!slack) slack = 1;
      *g_slackA = (slack * 4);

      if (inited)
      {
         printf("%8d %5d %5d %5d %5d\n",
            count++, drift, offby, now_tick - pulse_tick, slack);
      }
      else
      {
         printf("#  count drift offby sched slack\n");
         inited = 1;
      }
   }
}

int main(int argc, char *argv[])
{
   int off;
   int wave_id;
   rawWave_t wave[3];
   rawWaveInfo_t winf;

   initOpts(argc, argv);

   g_interval = g_seconds * MILLION;

   off = g_interval - (g_plength + SLACK);

   printf("# gpio=%d, level=%d slack=%dus, off=%dus\n",
      g_gpio, g_plevel, SLACK, off);

   if (gpioInitialise()<0) return -1;

   gpioSetAlertFunc(g_gpio, callback);     /* set pps callback */

   gpioSetMode(g_gpio, PI_OUTPUT);

   if (g_plevel) /* pulse is high */
   {
      wave[0].gpioOn  = 0;
      wave[0].gpioOff = (1<<g_gpio);
      wave[0].usDelay = SLACK;
      wave[0].flags   = 0;

      wave[1].gpioOn = (1<<g_gpio);
      wave[1].gpioOff = 0;
      wave[1].usDelay = g_plength;
      wave[1].flags   = WAVE_FLAG_TICK;    /* read tick at start of pulse */

      wave[2].gpioOn  = 0;
      wave[2].gpioOff = (1<<g_gpio);
      wave[2].usDelay = off;
      wave[2].flags   = 0;
   }
   else        /* pulse is low */
   {
      wave[0].gpioOn  = (1<<g_gpio);
      wave[0].gpioOff = 0;
      wave[0].usDelay = SLACK;
      wave[0].flags   = 0;

      wave[1].gpioOn  = 0;
      wave[1].gpioOff = (1<<g_gpio);
      wave[1].usDelay = g_plength;
      wave[1].flags   = WAVE_FLAG_TICK;    /* read tick at start of pulse */

      wave[2].gpioOn  = (1<<g_gpio);
      wave[2].gpioOff = 0;
      wave[2].usDelay = off;
      wave[2].flags   = 0;
   }

   gpioWaveClear();            /* clear all waveforms */

   rawWaveAddGeneric(3, wave); /* add data to waveform */

   wave_id = gpioWaveCreate(); /* create waveform from added data */

   if (wave_id >= 0)
   {
      gpioWaveTxSend(wave_id, PI_WAVE_MODE_REPEAT);

      winf = rawWaveInfo(wave_id);
      /* get address of slack length */
      g_slackA  = &(rawWaveCBAdr(winf.botCB+2)->length);

      while (1) sleep(1);
   }

   gpioTerminate();
}

