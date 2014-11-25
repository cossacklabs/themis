static void huppy_bug(void)
{
  i = 1;  /*Unrecognized identifier: i*/
}

#info "LcLint ignores all code that follows this line!"

void unhuppy_bug()
{
  i = "Asdf";
}
