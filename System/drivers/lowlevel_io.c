unsigned char   port_byte_in(unsigned short port)
{
    /* Функция-обертка над assembly, читающая 1 байт из параметра port */
    /* unsigned short port: адрес регистра какого-либо девайса, из которого */
    /* мы что-то прочтем. */

    /* Используется другой синтаксис ассембли (GAS). Обратите внимание, что */
    /* выражение "mov dest, src" в GAS мы запишем как "mov src, dest", т.е. */
    /* "in dx, al" означает прочитать содержимое порта (адрес которого */
    /* находится в DX) и положить в AL. */
    /* Символ % означает регистр, а т.к. % - escape symbol, то мы */
    /* пишем еще один %. */
    /* Перемещаем результат в регистр AL т.к. размер AL == 1 байт */
    unsigned char result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	/* разберем только что вызванную функцию: */
	/* "in %%dx, %%al"		- Прочитать содержимое порта и положить это в AL */
	/* : "=a" (result)		- Положить значение AL в переменную result */
	/* : "d" (port)			- Загрузить port в регистр EDX (extended DX: 32b) */
    return (result);		/* Возвращаем прочитанное содержимое из port */
}


void    port_byte_out(unsigned short port, unsigned char data)
{
    /* Функция-обертка над assembly, пишущая data (1 байт) в port */
    /* unsigned short port: адрес регистра девайса, в который что-то запишем */
    /* unsigned char data: 1 байт какой-то информации (например, символ) */
	__asm__("out %%al, %%dx" : : "a" (data), "d" (port));
	/* разберем только что вызванную функцию: */
	/* "out %%al, %%dx"		- Записать data в port */
	/* : : "a" (data)		- Загрузить data в регистр EAX */
	/* : "d" (port)			- Загрузить port в регистр EDX */
}


unsigned char   port_word_in(unsigned short port)
{
    /* Функция-обертка над assembly, читающая 2 байта из параметра port */
    /* Перемещаем результат в регистр AX т.к. размер AX == 2 байта */
    unsigned short result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return (result);
}


void port_word_out(unsigned short port, unsigned short data)
{
    /* Функция-обертка над assembly, пишущая data (2 байта, т.е. word) в port */
    __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}
