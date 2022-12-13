# arduino_ws2812
Random color led controller for WS2812 chips in serial. Quick and dirty code.

Этот проект предназначен для организации увеселительного процесса при помощи ленты с независимыми RGB-светодиодами на базе чипа WS2812

В основе аппаратной части используется Arduino Nano либо любая другая совместимая плата, поскольку ресурсов используется минимум - ограничений нет.
Питание схемы осуществляется от источника постоянного напряжения 12 вольт, для питания лент с WS2812 необходим импульсный преобразователь на 5 вольт и если лента длинная желательно запитать несколькими преобразователями каждые 2...5 метров локально, иначе падение напряжения на ленте при максимаьной яркости по мере удаления от точки подключения источника питания приведёт к заметному падению яркости.
Обратите внимание, что не все ленты имеют одинаковую последовательность цветов! При соединении вместе разных лент необходимо адаптировать переход на другую цветовую схему посреди цепочки программно! В коде такой переход сделан с 50-го элемента, если у вас лента однородная - уберите эту часть кода.

Подключение к плате. 

Вход ленты необходимо подключить к 13-му порту платы ардуино. В случае необходимости(например - суеверности) можно использовать любой другой удобный но указать его в скетче в первых строках.

Потенциометры крайними выводами подключаются к +5В выводу платы ардуино и выводу GND(общий провод).

Подвижный контакт потенциометра "Яркость" подключается к выводу A0.
Подвижный контакт потенциометра "Скорость движения" подключается к выводу A1.
Подвижный контакт потенциометра "Скорость изменения" подключается к выводу A2.

Номера выводов можно программно переназначить, при необходимости, что необходимо указать в первых строках скетча.
Так же не забудьте указать максимальное количество элементов ленты в константе count_led.

Приятного праздника!

https://youtu.be/qs_YFxL3LCI
