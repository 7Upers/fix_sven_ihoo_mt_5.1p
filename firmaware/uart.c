#include <stdio.h>
#include <avr/io.h>
#include <string.h>

#define ASCII_VALID_START 0x20
#define ASCII_VALID_END 0x7E

//prototipes
int uart_getchar(FILE *stream);
int uart_putchar(char c, FILE *stream);

FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int uart_getchar(FILE *stream)
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

int uart_putchar(char c, FILE *stream)
{
	while (!(UCSR0A & (1<<UDRE0))); /* Wait until transmission ready. */
	UDR0 = c;
	return 0;
}

void uart_init(unsigned int baud)
{
	unsigned int ubrr = F_CPU/16/baud - 1;

	//set speed
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)(ubrr);

	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	// Set frame format: 8data, 1stop bit - default

	// Redefine STD streams
	stdin = &uart_io;
	stdout = &uart_io;
}

char* uart_getline(char* buff, int len)
{
		/*
		//usage
			char* buff = (char*) calloc(1,5);
    	uart_getline(buff, 5);
		*/
    int buff_index = 0;
    char in = 0;

    /* Clear the buffer before using it */
    memset(buff, 0, len);

    /* Keep getting input until we get the enter key */
    while (buff_index < len && (in != '\n' && in != '\r'))
    {
        in = getchar();

        /* Handle characters that can be displayed */
        if (in >= ASCII_VALID_START && in <= ASCII_VALID_END)
        {
            /* Add them to the input buffer */
            buff[buff_index] = in;
            buff_index++;

            /* Print them to the screen so the user can see what they are typing */
            putc(in, stdout);
        }

        /* Handle Backspace (Win/Lin) or Del (Mac) */
        if (in == 0x7F || in == 0x08)
        {
            /* Remove the deleted character from the input buffer */
            if (buff_index > 0) { buff_index--; }
            buff[buff_index] = 0;
        }
    }

    putc('\n', stdout);

    return buff;
}
