#include "main.h"
/**
* print_int - prints a number of digit
*
* @n: number to print
*
* Return: number of digits written
*/
int print_int(long n)
{
	int count = 0;
	char c;
	char sign = '-';
	if (n < 0)
	{
		count += write(1, &sign, 1);
		n = -n;
	}
	if (n > 9)
	{
		count += print_int(n / 10);
	}
	n = n % 10;
	c = n + '0';
	count += write(1, &c, 1);
	return (count);
}


/**
 * print_char - prints a single character
 * @args: va_list holding the char to print
 *
 * Return: number of characters written
 */
int print_char(va_list args)
{
	char c;

	c = va_arg(args, int);
	return (write(1, &c, 1));
}

/**
 * print_string - prints a string
 * @args: va_list holding the string to print
 *
 * Return: number of characters written
 */
int print_string(va_list args)
{
	char *str;
	int len;

	str = va_arg(args, char *);
	if (str == NULL)
		str = "(null)";

	len = 0;
	while (str[len] != '\0')
		len++;

	return (write(1, str, len));
}

/**
 * print_percent - prints a percent sign
 * @args: unused, kept so all handlers share the same signature
 *
 * Return: number of characters written
 */
int print_percent(va_list args)
{
	(void)args;
	return (write(1, "%", 1));
}

/**
 * print_binary - prints the binary representation of an unsigned int
 * @args: va_list holding the unsigned int to print
 *
 * Return: number of characters written
 */
int print_binary(va_list args)
{
	unsigned int n;
	unsigned int mask;
	int count;
	int started;

	n = va_arg(args, unsigned int);
	count = 0;
	started = 0;
	mask = 1U << 31;

	while (mask > 0)
	{
		if (n & mask)
		{
			count += write(1, "1", 1);
			started = 1;
		}
		else if (started)
		{
			count += write(1, "0", 1);
		}
		mask >>= 1;
	}

	if (!started)
		count += write(1, "0", 1);

	return (count);
}

/**
 * _printf - produces output according to a format, like printf
 * @format: string with the text and conversion specifiers to print
 *
 * Return: the number of characters printed
 */
int _printf(const char *format, ...)
{
	va_list args;
	int i;
	int count;

	if (format == NULL)
		return (-1);

	va_start(args, format);
	count = 0;
	i = 0;

	while (format[i] != '\0')
	{
		if (format[i] == '%')
		{
			i++;
			if (format[i] == 'd' || format[i] == 'i')
			    count+= print_int(va_arg(args, int));
			else if (format[i] == 'c')
				count += print_char(args);
			else if (format[i] == 's')
				count += print_string(args);
			else if (format[i] == '%')
				count += print_percent(args);
			else if (format[i] == 'b')
				count += print_binary(args);
			else if (format[i] == '\0')
			{
				va_end(args);
				return (-1);
			}
			else
			{
				/* unknown specifier, just print it as-is */
				count += write(1, "%", 1);
				count += write(1, &format[i], 1);
			}
		}
		else
		{
			count += write(1, &format[i], 1);
		}
		i++;
	}

	va_end(args);
	return (count);
}
