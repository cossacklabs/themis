                void NotGoodfunc(char *str)
                {
                    char buffer[256];
                    strncat(buffer, str, sizeof(buffer) - 1);
                    return;
                }
