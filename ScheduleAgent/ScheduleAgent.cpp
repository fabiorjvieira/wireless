/*
 * SchedulerAgent.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: fabiorjvieira
 *
 *      Must do:
 *         Create only events in the past
 *            Get the last week of the month instead of the first
 *            Go backwards when moving between weeks of months
 *         See more closely the rand() versus (int)
 *
Debug/ScheduleAgent d 5     h 10 m 0 r 3 x 60 y 20 z 1 a 3 A 10 b 90 B 10 c c1 g 0 > 3eventos_todoQUINTOdia_as10h00_de60em60minutos_durante20minutos_1vez.output
Debug/ScheduleAgent d 5     h 10 m 0 r 3 x 60 y 20 z 1 a 3 A 10 b 90 B 10 c c1 g 1 > 3eventos_todoQUINTOdia_as10h00_de60em60minutos_durante20minutos_1vez_agrupado.output

Debug/ScheduleAgent w 2     h 13 m 5 r 3 x 60 y 50 z 5 a 3 A 10 b 90 B 10 c c1 g 0 > 3eventos_todaTERCAfeira_as01h05_de60em60minutos_durante50minutos_5vezes.output
Debug/ScheduleAgent w 2     h 13 m 5 r 3 x 60 y 50 z 5 a 3 A 10 b 90 B 10 c c1 g 1 > 3eventos_todaTERCAfeira_as01h05_de60em60minutos_durante50minutos_5vezes_agrupado.output

Debug/ScheduleAgent w 2 n 2 h 14 m 5 r 3 x 90 y 75 z 3 a 3 A 10 b 90 B 10 c c1 g 0 > 5eventos_toda3aQUARTAfeira_as14h00_de90em90minutos_durante75minutos_3vezes.output
Debug/ScheduleAgent w 2 n 2 h 14 m 5 r 3 x 90 y 75 z 3 a 3 A 10 b 90 B 10 c c1 g 1 > 5eventos_toda3aQUARTAfeira_as14h00_de90em90minutos_durante75minutos_3vezes_agrupado.output
 */

#include <iostream>
#include <algorithm>
#include <ctime>

int main(int nargs, char ** args)
{
   bool success = true, grouped = true;
   char * isNumber, * channel = NULL;
   char buffer[25];
   std::tm * now = new tm;
   std::time_t initialBeginning, finalBeginning, randomicBeginning;
   int randomicEveryMinutes, randomicDuringMinutes, randomicIntervalRepetition;
   char randomicRate0, randomicRate1 = 0;

   char monthDay =-1, weekDay =-1, monthWeekDay =-1,
      hour =-1, minute =-1,
      rate0 =-1, rate1 =0,
      ocurrenceProbability =100;
   int repeat =-1, everyMinutes =-1, duringMinutes =-1, intervalRepetition =-1;
   int precisionRepeat =0, precisionMinute =0;
   int precisionEveryMinutes =0, precisionDuringMinutes =0, precisionIntervalRepetition =0, precisionRate0 =0, precisionRate1 =0;


   //loading variables with command line parameters
   if (nargs >1)
   {
      if (args[1][0] == '?')
      {
         std::cout << "Usage: ScheduleAgent d 1 h 13 m 54 r 10 x 20 y 15" << std::endl;
         std::cout << std::endl;
         std::cout << "The beginning of the event:" << std::endl;
         std::cout << "d day of the month                 (1, 2, 3, ... , 31)" << std::endl;
         std::cout << "w day of the week                  (0 = Sunday, 1 = Monday, ... , 6 = Saturday)" << std::endl;
         std::cout << "n weekday of the month             (if w = 1, n = 0 first Monday of the month, ... , n = 4 fifth Monday)" << std::endl;
         std::cout << std::endl;
         std::cout << "The hour and the minute of the event:" << std::endl;
         std::cout << "h hour in 24H of the event beginning" << std::endl;
         std::cout << "m minute of the event beginning" << std::endl;
         std::cout << "M precision of m                   (m 10 M 4 means a 4% of random variation on the value around m,  10*0.96 <= m <= 10*1.04)" << std::endl;
         std::cout << std::endl;
         std::cout << "The frequency of the event:" << std::endl;
         std::cout << "p probability to trigger the event (0 to 100)" << std::endl;
         std::cout << "r how many times the event will be trigger" << std::endl;
         std::cout << "R precision of r" << std::endl;
         std::cout << std::endl;
         std::cout << "The frequency interval of the event:" << std::endl;
         std::cout << "x the event repeat every x minutes" << std::endl;
         std::cout << "X precision of x" << std::endl;
         std::cout << "y event duration in minutes" << std::endl;
         std::cout << "Y precision of y" << std::endl;
         std::cout << "z the event repeat every x minutes for z times" << std::endl;
         std::cout << "Z precision of z" << std::endl;
         std::cout << std::endl;
         std::cout << "Wireless parameters:" << std::endl;
         std::cout << "c the channel identification       (typically c0 or c1 ...)" << std::endl;
         std::cout << "a the rate percentage available    (0 to 100)" << std::endl;
         std::cout << "A the precision of a" << std::endl;
         std::cout << "b the rate out of the event        (0 to 100)" << std::endl;
         std::cout << "B the precision of b" << std::endl;
         std::cout << "g group events? (1 = true, false otherwise)" << std::endl;
         std::cout << std::endl;
         std::cout << "Note that:" << std::endl;
         std::cout << "n have to be used with w" << std::endl;
         std::cout << "Missing precision parameters means = 0" << std::endl;
         std::cout << "Missing probability parameter means = 100" << std::endl;
         return 0;
      }

      for (int argument =1; argument +1< nargs; argument +=2)
      {
         switch (args[argument][0])
         {
         case 'g':
            grouped = (std::strtol(args[argument +1], &isNumber, 10) == 1);
            if (*isNumber !=0) {argument--;grouped = true;}
            break;
         case 'c':
            channel = args[argument +1];
            break;
         case 'a':
            rate0 = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;rate0 =-1;}
            break;
         case 'A':
            precisionRate0 = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;precisionRate0 =0;};
            break;
         case 'b':
            rate1 = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;rate0 =-1;}
            break;
         case 'B':
            precisionRate1 = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;precisionRate1 =0;};
            break;
         case 'd':
            monthDay = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;monthDay =-1;}
            break;
         case 'w':
            weekDay = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;weekDay =-1;};
            break;
         case 'n':
            monthWeekDay = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;monthWeekDay =-1;};
            break;
         case 'h':
            hour = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;hour =-1;};
            break;
         case 'm':
            minute = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;minute =-1;};
            break;
         case 'M':
            precisionMinute = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;precisionMinute =0;};
            break;
         case 'p':
            ocurrenceProbability = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;ocurrenceProbability =100;};
            break;
         case 'r':
            repeat = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;repeat =-1;};
            break;
         case 'R':
            precisionRepeat = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;precisionRepeat =0;};
            break;
         case 'x':
            everyMinutes = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;everyMinutes =-1;};
            break;
         case 'X':
            precisionEveryMinutes = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;precisionEveryMinutes =0;};
            break;
         case 'y':
            duringMinutes = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;duringMinutes =-1;};
            break;
         case 'Y':
            precisionDuringMinutes = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;precisionDuringMinutes =0;};
            break;
         case 'z':
            intervalRepetition = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;intervalRepetition =-1;};
            break;
         case 'Z':
            precisionIntervalRepetition = std::strtol(args[argument +1], &isNumber, 10);
            if (*isNumber !=0) {argument--;precisionIntervalRepetition =0;};
            break;
         default:
            std::cout << "Option not recognized!" << std::endl;
            std::cout << "Try ?" << std::endl;
            return 0;
            break;
         }
      }
   }
   else
   {
      std::cout << "Usage: ScheduleAgent d 1 h 13 m 54 r 10 x 20 y 15" << std::endl;
      std::cout << "Try ?" << std::endl;
      return 0;
   }

   //validating parameter values
   if (rate0 ==-1 or channel == NULL)
   {
      std::cout << "Must be set (a and c)" << std::endl;
      success = false;
   }
   if (monthDay ==-1 and weekDay ==-1)
   {
      std::cout << "One must be set (d or w)" << std::endl;
      success = false;
   }
   else if (monthDay >31 or weekDay >6 or monthWeekDay >4)
   {
      std::cout << "Out of bounds (d <= 31 and w <= 6 and n <= 4)" << std::endl;
      success = false;
   }
   else if (monthDay !=-1 and weekDay !=-1)
   {
      std::cout << "Choose only one (d or w)" << std::endl;
      success = false;
   }

   if (hour ==-1 or minute ==-1)
   {
      std::cout << "Must be set (h and m)" << std::endl;
      success = false;
   }
   else if (hour >23 or minute >59)
   {
      std::cout << "Out of bounds (h <= 23 and m <= 59)" << std::endl;
      success = false;
   }

   if (everyMinutes ==-1 or duringMinutes ==-1)
   {
      std::cout << "Must be set (x and y)" << std::endl;
      success = false;
   }
   else if (everyMinutes < duringMinutes)
   {
      std::cout << "Out of bounds (x > y)" << std::endl;
      success = false;
   }

   if (repeat ==-1)
   {
      std::cout << "Must be set (r > 0)" << std::endl;
      success = false;
   }
   if (intervalRepetition ==-1)
   {
      std::cout << "Must be set (z > 0)" << std::endl;
      success = false;
   }
   if (everyMinutes*(1-((float)precisionEveryMinutes/100)) < duringMinutes*(1+((float)precisionDuringMinutes/100)) or rate0*(1+((float)precisionRate0/100)) > 100 or rate1*(1+((float)precisionRate1/100)) > 100)
   {
      std::cout << "Precisions must not generate an out of bounds (x*[1-(X/100)] > y*[1+(Y/100)] and a*[1+(A/100)] <= 100 and b*[1+(B/100)] <= 100)" << std::endl;
      success = false;
   }
   if (rate0*(1-((float)precisionRate0/100)) <= 0 and rate1*(1-((float)precisionRate1/100)) <= 0 and intervalRepetition*(1-((float)precisionIntervalRepetition/100)) <= 0 and repeat*(1-((float)precisionRepeat/100)) <= 0 and everyMinutes*(1-((float)precisionEveryMinutes/100)) <= 0 and duringMinutes*(1-((float)precisionDuringMinutes/100)) <=0)
   {
      std::cout << "Precisions must not generate non-positive values (a*[1-(A/100)] >= 0 and b*[1-(B/100)] >= 0 and z*[1-(Z/100)] >= 0 and r*[1-(R/100)] >= 0 and x*[1-(X/100)] >= 0 and y*[1-(Y/100)]) >= 0" << std::endl;
      success = false;
   }
   if (ocurrenceProbability >100 or rate0 > 100 or rate1 > 100)
   {
      std::cout << "Probability/rate must not exceed 100 (p <= 100 and a <=100 and b <= 100)" << std::endl;
      success = false;
   }

   if (!success) return success;

   //obtaining the requested date/time
   //calculate the difference in seconds between the current day and the requested date/time
   std::time(&initialBeginning);

   now->tm_year = std::localtime(&initialBeginning)->tm_year;
   now->tm_mon = std::localtime(&initialBeginning)->tm_mon;
   now->tm_mday = std::localtime(&initialBeginning)->tm_mday;
   now->tm_hour = std::localtime(&initialBeginning)->tm_hour;
   now->tm_min = std::localtime(&initialBeginning)->tm_min;
   now->tm_wday = std::localtime(&initialBeginning)->tm_wday;

   initialBeginning += (hour - now->tm_hour)*60*60 + (minute - now->tm_min)*60;
   if (monthDay !=-1) initialBeginning += (monthDay - now->tm_mday)*24*60*60;
   else //if (weekDay !=-1)
   {
      //searching for first weekDay of the month
      //the others shall be +1*7*24*60*60, +2*7*24*60*60, +3*7*24*60*60 and +4*7*24*60*60 (the last may step one month)
      initialBeginning += (weekDay - now->tm_wday)*24*60*60;
      //special cases: January (0) and December (11)
      if (std::localtime(&initialBeginning)->tm_mon == 0 and now->tm_mon == 11)
      {
         do
            initialBeginning -= 7*24*60*60;
         while (std::localtime(&initialBeginning)->tm_mon != 10);
         initialBeginning += 7*24*60*60;
      }
      else if (std::localtime(&initialBeginning)->tm_mon == 11 and now->tm_mon == 0)
         initialBeginning += 7*24*60*60;
      else if (std::localtime(&initialBeginning)->tm_mon >= now->tm_mon )
      {
         do
            initialBeginning -= 7*24*60*60;
         while (std::localtime(&initialBeginning)->tm_mon == now->tm_mon);
         initialBeginning += 7*24*60*60;
      }
      else if (std::localtime(&initialBeginning)->tm_mon < now->tm_mon)
         initialBeginning += 7*24*60*60;

      //stepping to the monthWeekDay_th week without step two months
      if (monthWeekDay != -1)
      {
         initialBeginning += monthWeekDay*7*24*60*60;
         if (std::localtime(&initialBeginning)->tm_mon > now->tm_mon)
            initialBeginning -= 7*24*60*60;
      }
   }

   //the requested beginning
   if (monthDay != -1)
   {
      std::cout << "%Every " << std::localtime(&initialBeginning)->tm_mday << " day of the month" << std::endl;
      now->tm_mday = std::localtime(&initialBeginning)->tm_mday;
   }
   else if (weekDay !=-1 and monthWeekDay == -1)
   {
      std::cout << "%Every " << std::localtime(&initialBeginning)->tm_wday << " (Sunday=0, ...) of the month" << std::endl;
   }
   else
   {
      std::cout << "%Every the " << (int)monthWeekDay +1 << "_th " << std::localtime(&initialBeginning)->tm_wday << " (Sunday=0, ...) of the month" << std::endl;
   }

   //calculating the interval defined by precisionMinute
   finalBeginning = initialBeginning + 60*minute*(float)precisionMinute/100;
   initialBeginning -=  60*minute*(float)precisionMinute/100;

   //the requested random beginning
   std::strftime(buffer, 25, "%Y-%m-%d %H:%M", std::localtime(&initialBeginning));
   std::cout << "%Beginning between " << buffer;
   std::strftime(buffer, 25, "%Y-%m-%d %H:%M", std::localtime(&finalBeginning));
   std::cout << " and " << buffer << std::endl;
   std::cout << "%With " << (int)ocurrenceProbability << "% probability of occurrence" << std::endl;
   std::cout << "%Repeating " << (int)(repeat*(1-((float)precisionRepeat/100))) << " <= days <= " << (int)(repeat*(1+((float)precisionRepeat/100))) << std::endl;
   std::cout << "%Every " << (int)(everyMinutes*(1-((float)precisionEveryMinutes/100))) << " <= minutes <= " << (int)(everyMinutes*(1+((float)precisionEveryMinutes/100))) << std::endl;
   std::cout << "%During " << (int)(duringMinutes*(1-((float)precisionDuringMinutes/100))) << " <= minutes <= " << (int)(duringMinutes*(1+((float)precisionDuringMinutes/100))) << std::endl;
   std::cout << "%For " << (int)(intervalRepetition*(1-((float)precisionIntervalRepetition/100))) << " <= repeat <= " << (int)(intervalRepetition*(1+((float)precisionIntervalRepetition/100))) << " times" << std::endl;
   std::cout << "%Channel " << channel << " "<< (int)(rate0*(1-((float)precisionRate0/100))) << "% <= rate <= " << (int)(rate0*(1+((float)precisionRate0/100))) << "%" << std::endl;
   std::cout << "%Channel " << channel << " out of the event "<< (int)(rate1*(1-((float)precisionRate1/100))) << "% <= rate <= " << (int)(rate1*(1+((float)precisionRate1/100))) << "%" << std::endl;
   std::cout << std::endl;

   //aleatory seed
   std::srand (time(NULL));

   //Occurrence probability
   if (ocurrenceProbability < std::rand() % 101) return 0;

   //Randomly choose repeat (repeat*(1-((float)precisionRepeat/100)) <= repeat <= repeat*(1+((float)precisionRepeat/100))) times:
   //   the real beginning of the event between initialBegin and finalBegin
   //   the interval between events defined by precisionEveryMinutes and repeated intervalRepetition defined by precisionIntervalRepetition
   //   the duration of the event defined by precisionDuringMinutes
   if (precisionRepeat > 0)
      repeat = (std::rand() % (int)((repeat*(1+((float)precisionRepeat/100))) - (repeat*(1-((float)precisionRepeat/100))) +1)) + repeat*(1-((float)precisionRepeat/100));
   for (int repetition =0; repetition < repeat; repetition++)
   {
      if (finalBeginning - initialBeginning > 0)
         randomicBeginning = (std::rand() % (finalBeginning - initialBeginning)) + initialBeginning;
      else randomicBeginning = initialBeginning;
      if (precisionDuringMinutes > 0)
         randomicDuringMinutes = std::rand() % (int)(duringMinutes*(1+((float)precisionDuringMinutes/100)) - duringMinutes*(1-((float)precisionDuringMinutes/100)) +1) + duringMinutes*(1-((float)precisionDuringMinutes/100));
      else randomicDuringMinutes = duringMinutes;
      if (precisionEveryMinutes > 0)
         randomicEveryMinutes = std::rand() % (int)(everyMinutes*(1+((float)precisionEveryMinutes/100)) - everyMinutes*(1-((float)precisionEveryMinutes/100)) +1) + everyMinutes*(1-((float)precisionEveryMinutes/100));
      else randomicEveryMinutes = everyMinutes;

      //if the requested date/time is a day of the month, step one month (deal with month size variation: 28, 29,30 and 31 days)
      if (monthDay != -1)
      {
         initialBeginning += 31*24*60*60;
         finalBeginning += 31*24*60*60;
         if (now->tm_mday != std::localtime(&initialBeginning)->tm_mday)
         {
            finalBeginning += (now->tm_mday - std::localtime(&initialBeginning)->tm_mday)*24*60*60;
            initialBeginning += (now->tm_mday - std::localtime(&initialBeginning)->tm_mday)*24*60*60;
         }
      }
      //if the requested date/time is a day of the week, step one week
      else if (weekDay !=-1 and monthWeekDay == -1)
      {
         initialBeginning += 7*24*60*60;
         finalBeginning += 7*24*60*60;
      }
      //if the requested date/time is the monthWeekDay_th weekDay of the month, step 4 weeks (begins every second Monday of the month)
      else
      {
         //stepping to the next month
         now->tm_mon = std::localtime(&initialBeginning)->tm_mon;
         do
         {
            initialBeginning += 7*24*60*60;
            finalBeginning += 7*24*60*60;
         }
         while (std::localtime(&initialBeginning)->tm_mon == now->tm_mon);

         //stepping to the monthWeekDay_th week without step two months
         now->tm_mon = std::localtime(&initialBeginning)->tm_mon;
         initialBeginning += monthWeekDay*7*24*60*60;
         finalBeginning += monthWeekDay*7*24*60*60;
         if (std::localtime(&initialBeginning)->tm_mon != now->tm_mon)
         {
            initialBeginning -= 7*24*60*60;
            finalBeginning -= 7*24*60*60;
         }
      }

      //output the event taking into account the interval, duration and intervalRepetition
      if (precisionIntervalRepetition > 0)
         randomicIntervalRepetition = std::rand() % (int)(intervalRepetition*(1+((float)precisionIntervalRepetition/100)) - intervalRepetition*(1-((float)precisionIntervalRepetition/100)) +1) + intervalRepetition*(1-((float)precisionIntervalRepetition/100));
      else randomicIntervalRepetition = intervalRepetition;
      for (int intervalRepeat =0; intervalRepeat < randomicIntervalRepetition; intervalRepeat++)
      {
         if (grouped) std::cout << "example(channel("<< channel << ")";
         for (int duration =0; duration < randomicDuringMinutes; duration++)
         {
            if (precisionRate0 > 0)
               randomicRate0 = std::rand() % (int)(rate0*(1+((float)precisionRate0/100)) - rate0*(1-((float)precisionRate0/100)) +1) + rate0*(1-((float)precisionRate0/100));
            else randomicRate0 = rate0;
            std::strftime(buffer, 25, "%d,%m,%Y", std::localtime(&randomicBeginning));
            if (grouped) std::cout << ",";
            std::cout << std::endl << "date_time_rate_available(channel(" << channel << "),date(" << buffer << "),";
            std::strftime(buffer, 25, "%H,%M", std::localtime(&randomicBeginning));
            std::cout << "time(" << buffer << "),rate(" << (int)randomicRate0 << "))";
            //stepping to the next minute
            randomicBeginning += 60;
         }
         if (grouped) std::cout << std::endl << ")." << std::endl;

         //completing the interval with rate1
         if (randomicDuringMinutes < randomicEveryMinutes)
         {
            if (grouped) std::cout << "example(channel("<< channel << ")";
            for (int duration = randomicDuringMinutes; duration < randomicEveryMinutes; duration++)
            {
               if (precisionRate1 > 0)
                  randomicRate1 = std::rand() % (int)(rate1*(1+((float)precisionRate1/100)) - rate1*(1-((float)precisionRate1/100)) +1) + rate1*(1-((float)precisionRate1/100));
               else randomicRate1 = rate1;
               std::strftime(buffer, 25, "%d,%m,%Y", std::localtime(&randomicBeginning));
               if (grouped) std::cout << ",";
               std::cout << std::endl << "date_time_rate_available(channel(" << channel << "),date(" << buffer << "),";
               std::strftime(buffer, 25, "%H,%M", std::localtime(&randomicBeginning));
               std::cout << "time(" << buffer << "),rate(" << (int)randomicRate1 << "))";
               //stepping to the next minute
               randomicBeginning += 60;
            }
            if (grouped) std::cout << std::endl << ")." << std::endl;
         }

         //stepping to the next interval
         //randomicBeginning += randomicEveryMinutes*60;
      }
      std::cout << std::endl;
   }

   return success;
}
