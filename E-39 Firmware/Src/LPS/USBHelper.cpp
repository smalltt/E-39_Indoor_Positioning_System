#include "USBHelper.h"

void InitUSB() {

  TM_USB_VCP_Init();
}

void UsbPutc(uint8_t val) {

  TM_USB_VCP_Putc(val);
}

void UsbPutArrayViaDMA(uint8_t *data, int32_t size) {

  TM_USB_VCP_Send(data, size);
}

void UsbPuts(const char string[]) {

  for (int i = 0; string[i] != 0; i++)
    TM_USB_VCP_Putc(string[i]);
}

void UsbPutc32(uint32_t val) {

  TM_USB_VCP_Putc(Convert8(val, 0));
  TM_USB_VCP_Putc(Convert8(val, 8));
  TM_USB_VCP_Putc(Convert8(val, 16));
  TM_USB_VCP_Putc(Convert8(val, 24));
}

void UsbPuti(uint64_t n) {

  int  i;
  int  sign;
  char s[10];

  if ((sign = n) < 0)  /* record sign */
    n = -n;          /* make n positive */

  i = 0;

  do {                       /* generate digits in reverse order */

    s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);   /* delete it */

  if (sign < 0)
    s[i++] = '-';

  s[i] = '\0';

  reverse(s);

  UsbPuts(s);
}
