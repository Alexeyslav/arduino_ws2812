#include <Adafruit_NeoPixel.h>
#define PIN 13 // номер порта к которому подключена лента ws2812
#define count_led 128   // количество светодиодов 
#define br_pin 0    // номер аналогового входа для потенциометра "Яркость"
#define spd_pin 1   // номер аналогового входа для потенциометра "скорость продвижения по ленте"
#define cspd_pin 2  // номер аналогового входа для потенциометра "скорость изменения цвета"
#define delta_hyst 4  // не реагировать на изменения аналоговой величины выше указанного порога
#define max_random_length 60    // максимальное время на достижение цели в тактах
#define min_random_distance 62  // Минимальное расстояние между цветами на каждом такте.
#define power_budget 600        // Максимальный бюджет мощности на каждый модуль, макс. == 256*3 = 768

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(count_led, PIN, NEO_RGB + NEO_KHZ800); //first number change does distance between colors

word  spdc = 250;        // скорость продвижения пикселей X из 1024 
float Speed_color = 0.5; // скорость изменения компонент цвета в о.е.(0.0 - минимальная, 1.0 - максимальная)

byte R, G, B;       // текущее значение 
byte Rt, Gt, Bt;    // цель которую надо настигнуть
int8_t Rs, Gs, Bs;  // скорость достижения(со знаком)

word ccntr = 0;
word BrValue;
word SpdValue;
word Speed_colort;
word CSpdValue;
word tempval;
byte i;
byte z;
byte randcounter = 0;
byte pixelsoutr[count_led];
byte pixelsoutg[count_led];
byte pixelsoutb[count_led];


void setup() {
  pixels.begin();
  pixels.show(); // Устанавливаем все светодиоды в состояние "Выключено"
  R = 127; Rt = 127;
  G = 127; Gt = 127;
  B = 127; Bt = 127;
  randomSeed(analogRead(3));
}

void loop() {
// Вход управления яркостью
tempval = analogRead(br_pin);  
if (tempval < 16) {tempval = 16;}; // минимальная яркость, иначе совсем не будет различия цветов
if (abs(tempval - BrValue) > delta_hyst) {BrValue = tempval;}; 
delay(1); // дать немного времени на переключение мультиплексора и отработке УВХ АЦП чтобы не цеплял предыдущий канал.

// Скорость движения огней
tempval = analogRead(spd_pin); // Скорость = tempval/(1024*t), где t - период одной итерации. 
                               // Минимальная скорость соответствует периоду T = 1024*t при t = 10мс, T ~ 10 сек на такт.
                               // Диапазон скоростей при t=10мС   0.1Гц - 100Гц
if (tempval < 2) {tempval = 2;}; // минимальная скорость
if (abs(tempval - spdc) > delta_hyst) {spdc = tempval;};
delay(1);

//Скорость движения к цели(размер "шлейфа")
tempval = analogRead(cspd_pin);
if (tempval < 4) {tempval = 4;};
if (abs(tempval - Speed_colort) > delta_hyst) {Speed_colort = tempval; Speed_color = tempval/1024.0;};

pixels.setBrightness( map(BrValue, 0, 1024, 0, 255)); //adjust brightness here
delay(20); // + 2мс на задержках выше = 10мс

ccntr = ccntr + spdc;
 if (ccntr >= 1024) 
  { ccntr = 0;    
    randcounter = randcounter + 1;
    // Сдвинуть пиксели вверх, освободив 0-й 
    for (i = (count_led - 1); i > 0; i = i - 1)
     { 
      if (i == 50)
       {pixelsoutr[i] = pixelsoutg[i-1];  // c 0 по 49 элемент лента с порядком RGB а дальше лента GRB... нужен переход RGB-GRB
        pixelsoutg[i] = pixelsoutr[i-1];
        pixelsoutb[i] = pixelsoutb[i-1];}
      else 
       {pixelsoutr[i] = pixelsoutr[i-1];
        pixelsoutg[i] = pixelsoutg[i-1];
        pixelsoutb[i] = pixelsoutb[i-1];}; 
     }
  
    pixelsoutr[0] =  R;
    pixelsoutg[0] =  G;
    pixelsoutb[0] =  B;
    
    if (((R == Rt) and (G == Gt) and (B == Bt)) or (randcounter > max_random_length))   // Достигли цели... 
     {  // min_random_distance Минимальное расстояние между цветами на каждом такте.
        // power_budget Максимальный бюджет мощности на все светодиоды модуля == 256*3;
       if (random(100) < 80) // светящаяся цель - лишь на 20%, остальные 80% - тёмнота. Сплошной поток света не выразителен, он должен перемежаться с темнотой
       {
       z = power_budget;
       Rt = Rt+random(min(z, 256))+min_random_distance; z = z - Rt;
       Gt = Gt+random(min(z, 256))+min_random_distance; z = z - Gt; 
       Bt = Bt+random(min(z, 256))+min_random_distance; // z = z - Bt;
       }
         else { Rt = 0; Gt = 0; Bt = 0;}
       
       Rs = (Rt - R) * Speed_color; if ((Rs == 0) and (R != Rt)) {Rs = (Rt - R);}; // Задаём новую цель. Если запланированная скорость равна нулю, но значения не равны 
       Gs = (Gt - G) * Speed_color; if ((Gs == 0) and (G != Gt)) {Gs = (Gt - G);}; // это означает что произошло округление. Присвоим скорость равную разнице между значений
       Bs = (Bt - B) * Speed_color; if ((Bs == 0) and (B != Bt)) {Bs = (Bt - B);}; // изменение произойдет за один шаг. 
       randcounter = 0;
       } 
     else  // Цель пока не достигнута, стремимся.... 
     { if (abs(R - Rt) < abs(Rs)) {R = Rt;} else {R = R + Rs;};
       if (abs(G - Gt) < abs(Gs)) {G = Gt;} else {G = G + Gs;};
       if (abs(B - Bt) < abs(Bs)) {B = Bt;} else {B = B + Bs;};
     }
 
   for (i = 0; i <= (count_led - 1); i++)  // Передаём в pixels для вывода в ленту и выводим.
    { pixels.setPixelColor(i, pixels.Color(pixelsoutr[i],pixelsoutg[i],pixelsoutb[i])); }
   pixels.show();
  }
}
