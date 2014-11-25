char SOME_STRING[1000] = "kdlsjfklds;fjkls;djflksdjflkdsjfklsdjfkl;sjfklsdjfkl;jsdflk;jsdl;fjsdlkfjsdklf;jds;lfjskdl;fjkls;dfjlkds;jfkl;sjdfk;ldsjklnvklsnbklsdnblkdlsjfklds;fjkls;djflksdjflkdsjfklsdjfkl;sjfklsdjfkl;jsdflk;jsdl;fjsdlkfjsdklf;jds;lfjskdl;fjkls;dfjlkds;jfkl;sjdfk;ldsjklnvklsnbklsdnblkdlsjfklds;fjkls;djflksdjflkdsjfklsdjfkl;sjfklsdjfkl;jsdflk;jsdl;fjsdlkfjsdklf;jds;lfjskdl;fjkls;dfjlkds;jfkl;sjdfk;ldsjklnvklsnbklsdnblkdlsjfklds;fjkls;djflksdjflkdsjfklsdjfkl;sjfklsdjfkl;jsdflk;jsdl;fjsdlkfjsdklf;jds;lfjskdl;fjkls;dfjlkds;jfkl;sjdfk;ldsjklnvklsnbklsdnbl";

void funcGood(char *str)
{
  char buffer[256];

  strncpy (buffer, SOME_STRING, 255);
  buffer[255] = '\0';
  strncat(buffer, str, (sizeof(buffer) - (strlen(buffer)  + 1 ) ) );

  return;
}

