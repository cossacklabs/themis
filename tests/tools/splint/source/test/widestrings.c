/* Provided by Nelson Beebe */
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int main(void) 
{
  (void)wprintf(L"English:  Hello, world!\n");
  (void)printf(L"English:  Hello, world!\n"); /* error */
  (void)wprintf("English:  Hello, world!\n"); /* error */
  
  (void)wprintf(L"Russian:  Ð^×Ð°Ñ^ÀÐµÐ³Ð¸Ñ^ÁÑ^ÂÑ^ÀÐ¸Ñ^ÀÑ^ÃÐ¹Ñ^ÂÐµÑ^ÁÑ^Ì Ñ^ÁÐµÐ¹Ñ^ÇÐ°Ñ^Á Ð½Ð° Ð^ÔÐµÑ^ÁÑ^ÏÑ^ÂÑ^ÃÑ^Î Ð^ÜÐµÐ¶Ð´Ñ^ÃÐ½Ð°Ñ^ÀÐ¾Ð´Ð½Ñ^ÃÑ^Î Ð^ÚÐ¾Ð½Ñ^ÄÐµÑ^ÀÐµÐ½Ñ^ÆÐ¸Ñ^Î...\n");
  (void)wprintf(L"Greek:    Î£á½² Î³Î½Ï^ÉÏ^Áá½·Î¶Ï^É á¼^ÀÏ^Àá½¸ Ï^Äá½´Î½ Îºá½¹Ï^ÈÎ·...\n");
  (void)wprintf(L"Georgian: á^Ã^Òá^Ã^×á^Ã®á^Ã^Ýá^Ã^Õá^Ã^× á^Ã^Ðá^Ã®á^Ã^Úá^Ã^Ðá^Ã^Õá^Ã^Ô á^Ã^Òá^Ã^Ðá^Ã^Øá^Ã^Ðá^Ã á^Ã^Ýá^Ã^× á^Ã á^Ã^Ôá^Ã^Òá^Ã^Øá^Ã¡á^Ã¢á^Ã á^Ã^Ðá^Ãªá^Ã^Øá^Ã^Ð Unicode-á^Ã^Øá^Ã¡ á^Ã^Ûá^Ã^Ôá^Ã^Ðá^Ã^×á^Ã^Ô á^Ã¡á^Ã^Ðá^Ã^Ôá^Ã á^Ã^×á^Ã^Ðá^Ã¨á^Ã^Ýá^Ã á^Ã^Øá^Ã¡á^Ã^Ý\n");
  return (EXIT_SUCCESS);
}

