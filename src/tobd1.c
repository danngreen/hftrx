/*
 * Бортовой компьютер для протокола Toyota OBD1
 */

#include "hardware.h"
#include "keyboard.h"
#include "formats.h"
#include "gpio.h"

#if CTLSTYLE_TOBD1

#include "display/ssd1326.h"
#include <stdbool.h>
#include <string.h>

#define MY_HIGH  1 //0    // I have inverted the Eng line using an Opto-Coupler, if yours isn't then reverse these low & high defines.
#define MY_LOW   0 //1
#define TOBD1_MAX_BYTES  24
#define OBD_INJ 1 //Injector pulse width (INJ)
#define OBD_IGN 2 //Ignition timing angle (IGN)
#define OBD_IAC 3 //Idle Air Control (IAC)
#define OBD_RPM 4 //Engine speed (RPM)
#define OBD_MAP 5 //Manifold Absolute Pressure (MAP)
#define OBD_ECT 6 //Engine Coolant Temperature (ECT)
#define OBD_TPS 7 // Throttle Position Sensor (TPS)
#define OBD_SPD 8 //Speed (SPD)
#define OBD_OXSENS 9

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

volatile uint_fast16_t tobd1_FailBit = 0;
volatile uint_fast8_t tobd1_ID, tobd1_NumBytes;
uint_fast8_t tobd1_Data[TOBD1_MAX_BYTES];
static uint_fast32_t tobd1_timer = 0;

enum {
	graph_count = 25,
	screen_count = 2,
	KBCH_DN = KBD_CODE_1,
	KBCH_UP = KBD_CODE_2
};

enum {
	SCREEN_SPEED,
	SCREEN_DIAG

};

float getOBDdata(uint_fast8_t OBDdataIDX);
void tobd1_graph(uint_fast8_t val);

static void tobd1_tim4_IRQHandler(void)
{
	 TIM4->SR &= ~TIM_SR_UIF;
	 tobd1_timer ++;
}

void tobd1_initialize(void)
{
	ssd1326_init();
	TOBD1_DATA_INITIALIZE();

	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->PSC = CPU_FREQ / 2000 - 1;
	TIM4->ARR = 1;
	TIM4->DIER |= TIM_DIER_UIE;
	arm_hardware_set_handler_system(TIM4_IRQn, tobd1_tim4_IRQHandler);
	TIM4->CR1 |= TIM_CR1_CEN;
}

void display_screen(uint_fast8_t index)
{
	float tmp;
	char buf[20];
	switch(index)
	{
	case SCREEN_SPEED:
	{
		// Скорость
		tmp = getOBDdata(OBD_SPD);
		local_snprintf_P(buf, ARRAY_SIZE(buf), "Speed %.0f", tmp);
		ssd1326_DrawString_big(0, buf, 0);
		tobd1_graph(tmp);
	}
		break;

	case SCREEN_DIAG:
	{
		// температура
		tmp = getOBDdata(OBD_ECT);
		local_snprintf_P(buf, ARRAY_SIZE(buf), "Temp: %.0f", tmp);
		ssd1326_DrawString_small(0, 0, buf, 0);

		// Обороты
		tmp = getOBDdata(OBD_RPM);
		local_snprintf_P(buf, ARRAY_SIZE(buf), "RPM:  %.0f", tmp);
		ssd1326_DrawString_small(0, 1, buf, 0);

		// Впрыск
		tmp = getOBDdata(OBD_INJ);
		local_snprintf_P(buf, ARRAY_SIZE(buf), "INJ:  %.1f", tmp);
		ssd1326_DrawString_small(10, 0, buf, 0);

		// Throttle
		tmp = getOBDdata(OBD_TPS);
		local_snprintf_P(buf, ARRAY_SIZE(buf), "TPS:  %.0f", tmp);
		ssd1326_DrawString_small(10, 1, buf, 0);
	}
		break;

	default:
		break;
	}
}

void tobd1_main_step(void)
{
	static uint_fast8_t screen_index = 0, update = 1;
	uint_fast8_t kbch = KBD_CODE_MAX;
	kbd_scan(& kbch);

	switch(kbch)
	{
	case KBCH_DN:

		screen_index = screen_index ? screen_index : screen_count;
		screen_index --;
		update = 1;

		break;

	case KBCH_UP:

		screen_index ++;
		screen_index = (screen_index >= screen_count) ? 0 : screen_index;
		update = 1;

		break;

	default:
		break;
	}

	if (tobd1_timer % 1000 == 0)
	{
		update = 1;
//		TP();
	}

	if (update)
	{
		update = 0;
		display_screen(screen_index);
		ssd1326_send_framebuffer();
//		TP();
	}
}

float getOBDdata(uint_fast8_t OBDdataIDX) {
	float returnValue;
	switch (OBDdataIDX) {
	case 0:// UNKNOWN
		returnValue = tobd1_Data[0];
		break;
	case OBD_INJ: //  Время впрыска форсунок  =X*0.125 (мс)
		returnValue = tobd1_Data[OBD_INJ] * 0.125; //Время впрыска форсунок
		break;
	case OBD_IGN: // Угол опережения зажигания X*0.47-30 (град)
		returnValue = tobd1_Data[OBD_IGN] * 0.47 - 30;
		break;
	case OBD_IAC: //  Состояние клапана ХХ Для разных типов КХХ разные формулы: X/255*100 (%)
		//  X (шаг)
		returnValue = tobd1_Data[OBD_IAC] * 0.39215; ///optimize divide
		break;
	case OBD_RPM: //Частота вращения коленвала X*25(об/мин)
		returnValue = tobd1_Data[OBD_RPM] * 25;
		break;
	case OBD_MAP: //Расходомер воздуха (MAP/MAF)
		//  X*0.6515 (кПа)
		//  X*4.886 (мм.ртут.столба)
		//  X*0.97 (кПа) (для турбомоторов)
		//  X*7.732 (мм.рт.ст) (для турбомоторов)
		//  (гр/сек) (данная формула для MAF так и не найдена)
		//  X/255*5 (Вольт) (напряжение на расходомере)
		returnValue = tobd1_Data[OBD_MAP] * 4.886;
		break;
	case OBD_ECT: // Температура двигателя (ECT)
		// В зависимости от величины Х разные формулы:
		// 0..14:          =(Х-5)*2-60
		// 15..38:        =(Х-15)*0.83-40
		// 39..81:        =(Х-39)*0.47-20
		// 82..134:      =(Х-82)*0.38
		// 135..179:    =(Х-135)*0.44+20
		// 180..209:    =(Х-180)*0.67+40
		// 210..227:    =(Х-210)*1.11+60
		// 228..236:    =(Х-228)*2.11+80
		// 237..242:    =(Х-237)*3.83+99
		// 243..255:    =(Х-243)*9.8+122
		// Температура в градусах цельсия.
		if (tobd1_Data[OBD_ECT] >= 243)
			returnValue = ((float)(tobd1_Data[OBD_ECT] - 243) * 9.8) + 122;
		else if (tobd1_Data[OBD_ECT] >= 237)
			returnValue = ((float)(tobd1_Data[OBD_ECT] - 237) * 3.83) + 99;
		else if (tobd1_Data[OBD_ECT] >= 228)
			returnValue = ((float)(tobd1_Data[OBD_ECT] - 228) * 2.11) + 80.0;
		else if (tobd1_Data[OBD_ECT] >= 210)
			returnValue = ((float)(tobd1_Data[OBD_ECT] - 210) * 1.11) + 60.0;
		else if (tobd1_Data[OBD_ECT] >= 180)
			returnValue = ((float)(tobd1_Data[OBD_ECT] - 180) * 0.67) + 40.0;
		else if (tobd1_Data[OBD_ECT] >= 135)
			returnValue = ((float)(tobd1_Data[OBD_ECT] - 135) * 0.44) + 20.0;
		else if (tobd1_Data[OBD_ECT] >= 82)
			returnValue = ((float)(tobd1_Data[OBD_ECT] - 82) * 0.38);
		else if (tobd1_Data[OBD_ECT] >= 39)
			returnValue = ((float)(tobd1_Data[OBD_ECT] - 39) * 0.47) - 20.0;
		else if (tobd1_Data[OBD_ECT] >= 15)
			returnValue = ((float)(tobd1_Data[OBD_ECT] - 15) * 0.83) - 40.0;
		else
			returnValue = ((float)(0 - 5) * 2.0) - 60.0;
		break;
	case OBD_TPS: // Положение дроссельной заслонки
		// X/2(градусы)
		// X/1.8(%)
		returnValue = tobd1_Data[OBD_TPS] / 1.8;
		break;
	case OBD_SPD: // Скорость автомобиля (км/час)
		returnValue = tobd1_Data[OBD_SPD];
		break;
		//  Коррекция для рядных/ коррекция первой половины
	case OBD_OXSENS:
		returnValue = (float)tobd1_Data[OBD_OXSENS] * 0.01953125;
		break;

		//  читаем Байты флагов побитно
	case 11:
		returnValue = bitRead(tobd1_Data[11], 0);  //  Переобогащение после запуска 1-Вкл
		break;
	case 12:
		returnValue = bitRead(tobd1_Data[11], 1); //Холодный двигатель 1-Да
		break;
	case 13:
		returnValue = bitRead(tobd1_Data[11], 4); //Детонация 1-Да
		break;
	case 14:
		returnValue = bitRead(tobd1_Data[11], 5); //Обратная связь по лямбда зонду 1-Да
		break;
	case 15:
		returnValue = bitRead(tobd1_Data[11], 6); //Дополнительное обогащение 1-Да
		break;
	case 16:
		returnValue = bitRead(tobd1_Data[12], 0); //Стартер 1-Да
		break;
	case 17:
		returnValue = bitRead(tobd1_Data[12], 1); //Признак ХХ (Дроссельная заслонка) 1-Да(Закрыта)
		break;
	case 18:
		returnValue = bitRead(tobd1_Data[12], 2); //Кондиционер 1-Да
		break;
	case 19:
		returnValue = bitRead(tobd1_Data[12], 3); //Нейтраль 1-Да
		break;
	case 20:
		returnValue = bitRead(tobd1_Data[12], 4); //Смесь  первой половины 1-Богатая, 0-Бедная
		break;
	default: // DEFAULT CASE (in no match to number)
		// send "error" value
		returnValue =  9999.99;
	} // end switch
	// send value back
	return returnValue;
} // end void getOBDdata

void tobd1_interrupt_handler(void)
{
	static uint_fast8_t ID, EData[TOBD1_MAX_BYTES];
  static bool InPacket = false;
  static unsigned long StartMS;
  static uint_fast16_t BitCount;
  int state = (TOBD1_DATA_PORT & TOBD1_DATA_PIN) != 0;

  if (InPacket == false)  {
    if (state == MY_HIGH)   {
      StartMS = tobd1_timer;
    }   else   { // else  if (state == MY_HIGH)
      if ((tobd1_timer - StartMS) > (15 * 8))   {
        StartMS = tobd1_timer;
        InPacket = true;
        BitCount = 0;
      } // end if  ((millis() - StartMS) > (15 * 8))
    } // end if  (state == MY_HIGH)
  }  else   { // else  if (InPacket == false)
    uint_fast16_t bits = ((tobd1_timer - StartMS) + 1 ) / 8; // The +1 is to cope with slight time errors
    StartMS = tobd1_timer;
    // process bits
    while (bits > 0)  {
      if (BitCount < 4)  {
        if (BitCount == 0)
          ID = 0;
        ID >>= 1;
        if (state == MY_LOW)  // inverse state as we are detecting the change!
          ID |= 0x08;
      }   else    { // else    if (BitCount < 4)
        uint_fast16_t bitpos = (BitCount - 4) % 11;
        uint_fast16_t bytepos = (BitCount - 4) / 11;
        if (bitpos == 0)      {
          // Start bit, should be LOW
          if ((BitCount > 4) && (state != MY_HIGH))  { // inverse state as we are detecting the change!
            tobd1_FailBit = BitCount;
            InPacket = false;
            break;
          } // end if ((BitCount > 4) && (state != MY_HIGH))
        }  else if (bitpos < 9)  { //else TO  if (bitpos == 0)
          EData[bytepos] >>= 1;
          if (state == MY_LOW)  // inverse state as we are detecting the change!
            EData[bytepos] |= 0x80;
        } else { // else if (bitpos == 0)
          // Stop bits, should be HIGH
          if (state != MY_LOW)  { // inverse state as we are detecting the change!
            tobd1_FailBit = BitCount;
            InPacket = false;
            break;
          } // end if (state != MY_LOW)
          if ( (bitpos == 10) && ((bits > 1) || (bytepos == (TOBD1_MAX_BYTES - 1))) ) {
            tobd1_NumBytes = 0;
            tobd1_ID = ID;
            for (uint_fast16_t i = 0; i <= bytepos; i++)
              tobd1_Data[i] = EData[i];
            tobd1_NumBytes = 1;
            if (bits >= 16)  // Stop bits of last byte were 1's so detect preamble for next packet
              BitCount = 0;
            else  {
              tobd1_FailBit = BitCount;
              InPacket = false;
            }
            break;
          }
        }
      }
      ++ BitCount;
      -- bits;
    } // end while
  } // end (InPacket == false)
}

void tobd1_graph(uint_fast8_t val)
{
	static uint_fast8_t graph[graph_count] = { 0 };
	static uint_fast16_t ccc = 0;

	for (uint_fast8_t i = graph_count - 1; i >= 1; i --)
	{
		graph[i] = graph[i - 1];
	}
	graph[0] = ++ ccc;

	if (ccc > 31)
		ccc = 0;

	uint_fast8_t i, j, x0 = 160;
	uint_fast16_t x1;

	for(i = 0; i < graph_count; i ++)
	{
		for (j = 0; j <= 2; j ++)
		{
			x1 = x0 + 4 * (i - 1) + j;
			ssd1326_draw_line(x1, 31, x1, 31 - graph[i], (i + 5) / 2);
			ssd1326_draw_line(x1, 31 - graph[i], x1, 0, 0);
		}
	}
}


#endif /* CTLSTYLE_TOBD1 */
